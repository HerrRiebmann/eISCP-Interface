#define PIONEER true //false = Onkyo
#define DEBUG true

//******WIFI******
#if defined(ESP8266)  
  #include <ESP8266WiFi.h>
  #include <WiFiClient.h>
  #include <WiFiUdp.h>
#elif defined(ESP32)  
  #include <WiFi.h>
#endif
WiFiClient client;
bool clientConnected = false;
const char* ssid = "MyWiFi";
const char* password = "Secret :)";

IPAddress host = IPAddress(192, 168, 2, 104);   //Will automatically be set
IPAddress udpHost = IPAddress(192, 168, 2, 255);
const int   port = 60128;

const unsigned int WiFiTimeout = 15000; //15 Sec
const uint16_t TcpTimeout = 5000;   //5 Sec
const uint16_t UdpTimeout = 5000;   //5 Sec

//******ISCP******
static const uint8_t packetHeader[] = {0x49, 0x53, 0x43, 0x50, //ISCP
                                       0x00, 0x00, 0x00, 0x10, //HeaderSize (Bigendian)
                                       0x00, 0x00, 0x00, 0xFF, //DataSize (Bigendian) replace last with length
                                       0x01, 0x00, 0x00, 0x00  //Version + Reserved
                                      };
const uint8_t packetFooterOutgoing[] = {0x0D};//CR
const uint8_t packetFooterIncoming[] = {0x1A, 0x0D, 0x0A}; //EOF, CR, LF
static const uint8_t lenghtBytePosHeader = 7; //Header Length
static const uint8_t lenghtBytePosHigh = 10;  //Message Length 1/2
static const uint8_t lenghtBytePosLow = 11;   //Message Length 2/2
static const char commandPrefix[] = "!1"; //Start Character + Destination Unit (Receiver)

const unsigned int IncomingBufferSize = 300;
unsigned char IncomingBuffer[IncomingBufferSize];
uint8_t IncomingBufferCounter = 0;

const char* incoming = "Incoming";

String MessageText = "";
bool MessageBool = false;
int MessageInt = 0;
char MessageCmd[4];//Null Terminated

void setup() {
  Serial.begin(115200);  
  if (SetupWiFi()) {
    GetUdpAdress();

    if (ConnectToDevice())
      RadioBob();       
  }
}

void loop() {
  //Submit Message via Serial: (f.e. !1PWRQSTN or PWRQSTN)
  if (Serial.available()) {
    String cmdString;
    while (Serial.available()) {
      char c = Serial.read();
      if (c == 0x0D) //CarriageReturn
        SendCommand(cmdString);
      else
        cmdString += c;
    }
  }
  //Constantly read from receiver:
  Read();
}

bool SetupWiFi() {  
  WiFi.begin(ssid, password);
  // Wait for connection
  unsigned long timeout = millis();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);        
#if DEBUG
    Serial.print(".");
#endif DEBUG
    if (millis() - timeout > WiFiTimeout){
      SetText("WiFi", "Timeout!");
      return false;    
    }
  }
  udpHost = WiFi.localIP();  
  //change to broadcast adress
  udpHost[3] = 255;
  return true;
}
