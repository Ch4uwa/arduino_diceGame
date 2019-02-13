#include "SoftwareSerial.h"
#include "WiFiEsp.h"

SoftwareSerial Serial1(6, 7);
WiFiEspClient client;

#define rollPin 2
#define getPin 3
#define wifiEnable 4

// Declare and initialise global arrays for WiFi settings
const char ssid[] = "SSID";
const char pass[] = "PASS";
const char server[] = "SERVER";
const String hostname = "HOSTNAME";
const String email = "email";
const String xid = "ID";
const String uri = "/services/data/" + email + "-" + xid + ".csv";
const String senduri = "/services/sendform.aspx?xdata=" + email + "|"+xid+"|";
const int port = 80;

String line;

// Declare and initialise variable for radio status
int status = WL_IDLE_STATUS;

int die = 0, rollCount=0;
boolean press = false;
int rolls=0;
String dataDie;

void setup()
{
    pinMode(rollPin, INPUT);
    pinMode(getPin, INPUT);
    pinMode(wifiEnable, OUTPUT);

    

    Serial.begin(115200);
    Serial1.begin(9600);

    digitalWrite(wifiEnable,HIGH);
    wifiInit();
}

void loop()
{


    while (digitalRead(rollPin) == HIGH)
    {
        press = true;
    }
    if (press)
    {
        rollDie();
        rollCount = rollCount + 1;
        press = false;
        dataDie = String(die);

        sendData();
        delay(1000);
        
    }
    while (digitalRead(getPin) == HIGH)
    {
        press = true;
    }
    if (press)
    {
        press = false;
        readData();
        delay(1000);
        rollCount=0;
    }
    
}

void rollDie()
{
    randomSeed(analogRead(A0));
    die = random(1, 7);
    
}

void readData()
{
    Serial.println();
    Serial.println("Reading from server...");
    // if you get a connection, report back via serial
    if (client.connect(server, port))
    {
        Serial.println("Connected to server");
        delay(50);
        // Make a HTTP request
        client.println("GET " + uri + " HTTP/1.1");
        client.println("Host: " + hostname);
        client.println("Connection: close");
        client.println();
    }
    /* if there are incoming bytes available
     * from the server, read them and print them */
    while (client.available() == 0)
    {
        delay(5);
    }
    while (client.available())
    {
        line = client.readStringUntil('\n');
        
    }
        Serial.println(line);
        
}

void sendData()
{

    Serial.println();
    Serial.println("Sending to server...");
    // if you get a connection, report back via serial
    if (client.connect(server, port))
    {
        Serial.println("Connected to server");
        delay(50);
        client.println("GET " + senduri + "P1=" + dataDie +"," + "Roll=" + rollCount + " HTTP/1.1");
        client.println("Host: " + hostname);
        client.println("Connection: close");
        client.println();
    }

    while (client.available() == 0)
    {
        delay(5);
    }
    while (client.available())
    {
        char cLine = client.read();
        Serial.write(cLine);
    }
    delay(500);
    if (!client.connected())
    {
        client.stop();
    }
}

void wifiInit()
{
    // Initialize ESP module
    WiFi.init(&Serial1);

    // Check for the presence of the shield
    if (WiFi.status() == WL_NO_SHIELD)
    {
        Serial.println("WiFi shield not present");
        
        // Don't continue
        while (true)
            ;
    }

    // Attempt to connect to WiFi network
    while (status != WL_CONNECTED)
    {
        Serial.print("Attempting to connect to SSID: ");
        Serial.println(ssid);
        
        // Connect to WPA/WPA2 network
        status = WiFi.begin(ssid, pass);
    }

    Serial.println("You're connected to the network");
    
    printWifiStatus();
    Serial.println();
}

void printWifiStatus()
{
    // print the SSID of the network you're attached to
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());
    
    // print your WiFi shield's IP address
    IPAddress ip = WiFi.localIP();
    Serial.print("IP Address: ");
    Serial.println(ip);
    

    // print the received signal strength
    long rssi = WiFi.RSSI();
    Serial.print("Signal strength (RSSI):");
    Serial.print(rssi);
    Serial.println(" dBm");
}
