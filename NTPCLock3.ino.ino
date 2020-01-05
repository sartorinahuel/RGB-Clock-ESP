#include <TimeLib.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Adafruit_NeoPixel.h>

#define PIN 4
#define NUMPIXELS 60

Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, PIN, NEO_GRB + NEO_KHZ800);

const char ssid[] = "Nahuel-11";  //  your network SSID (name)
const char pass[] = "Laprida3579";       // your network password

int brillo=20;
int dia;
int mes;
int horas;
int minutos;
int segundos;
int hArrow=5;
int i;
bool tarde = false;
bool step = false;
int hm;

int buzzer = 5;

// NTP Servers:
static const char ntpServerName[] = "us.pool.ntp.org";

const int timeZone = -3;     // Central European Time

WiFiUDP Udp;
unsigned int localPort = 8888;  // local port to listen for UDP packets

time_t getNtpTime();

void sendNTPpacket(IPAddress &address);

WiFiServer server(80);

void setup()
{
  WiFi.disconnect();
  Serial.begin(9600);
  //inicializamos el STRIP
  strip.begin();
  strip.setBrightness(brillo);
  colorTest();
  pinMode(buzzer, OUTPUT);
  
  //Buzzer test
  digitalWrite(buzzer, 1000);
  delay(20);
  digitalWrite(buzzer, 0);
  
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.print("IP number assigned by DHCP is ");
  Serial.println(WiFi.localIP());
  Serial.println("Starting UDP");
  Udp.begin(localPort);
  Serial.print("Local port: ");
  Serial.println(Udp.localPort());
  Serial.println("waiting for sync");
  setSyncProvider(getNtpTime);
  setSyncInterval(300);

  server.begin();
  Serial.println("Server started");
}

time_t prevDisplay = 0; // when the digital clock was displayed

void loop()
{
  brightness();
  strip.setBrightness(brillo);
  strip.show();
  ledClock();
}

/*-------- NTP code ----------*/

const int NTP_PACKET_SIZE = 48; // NTP time is in the first 48 bytes of message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets

time_t getNtpTime()
{
  IPAddress ntpServerIP; // NTP server's ip address

  while (Udp.parsePacket() > 0) ; // discard any previously received packets
  Serial.println("Transmit NTP Request");
  // get a random server from the pool
  WiFi.hostByName(ntpServerName, ntpServerIP);
  Serial.print(ntpServerName);
  Serial.print(": ");
  Serial.println(ntpServerIP);
  sendNTPpacket(ntpServerIP);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500) {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      Serial.println("Receive NTP Response");
      Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
    }
  }
  Serial.println("No NTP Response :-(");
  return 0; // return 0 if unable to get the time
}

// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress &address)
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12] = 49;
  packetBuffer[13] = 0x4E;
  packetBuffer[14] = 49;
  packetBuffer[15] = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}

void brightness(){
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
 
  // Wait until the client sends some data
  Serial.println("new client");
  while(!client.available()){
    delay(1);
  }
 
  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();
 
  // Match the request
 
  if (request.indexOf("/brillo=muyalto") != -1)  {
    brillo = 150;
  }
  if (request.indexOf("/brillo=alto") != -1)  {
    brillo = 60;
  }
  if (request.indexOf("/brillo=medio") != -1)  {
    brillo = 30;
  }
  if (request.indexOf("/brillo=bajo") != -1)  {
    brillo = 10;
  }
  // Return the response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); //  do not forget this one
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
 
  client.print("el brillo esta: ");
 
  if(brillo == 150) {
    client.print("Muy Alto");
  }
  if(brillo == 60) {
    client.print("Alto");
  }
  if(brillo == 30) {
    client.print("Medio");
  }
  if(brillo == 10) {
    client.print("Bajo");
  }
  client.println("<br><br>");
  client.println("<a href=\"/brillo=muyalto\"\"><button>Muy Alto </button></a>");
  client.println("<a href=\"/brillo=alto\"\"><button>Alto </button></a>");
  client.println("<a href=\"/brillo=medio\"\"><button>Medio </button></a>");
  client.println("<a href=\"/brillo=bajo\"\"><button>Bajo </button></a>");  
  client.println("<br><br>");

  delay(1);
  Serial.println("Client disonnected");
  Serial.println("");
}

void colorTest(){
  for(i=0;i<60;i++){
  strip.setPixelColor(i, 0, 0, 255);
  strip.show();
  delay(5);
  }
  
  delay(500);
  for(i=0;i<60;i++){
  strip.setPixelColor(i, 0, 255, 0);
  strip.show();
  delay(5);
  }
  delay(500);
  for(i=0;i<60;i++){
  strip.setPixelColor(i, 255, 0, 0);
  strip.show();
  delay(5);
  }
  delay(500);
  for(i=0;i<60;i++){
  strip.setPixelColor(i, 0, 0, 0);
  strip.show();
  delay(20);
  }
}

void ledClock(){

  Serial.print(day());
  dia = day();
  Serial.print("/");
  Serial.print(month());
  mes = month();
  Serial.print("/");
  Serial.print(year());
  Serial.println();
  Serial.print(hour());
  horas = hour();
  Serial.print(":");
  Serial.print(minute());
  minutos = minute();
  Serial.print(":");
  segundos = second();
  Serial.println(second());

  if(dia == 31){
    if(mes == 12){
      if(horas == 23){
        if(minutos == 59){
          if(segundos == 50){
            newYear();
          }
        }
      }
    }
  }
  
  step = false;
    
  hArrow = 5;

  if (horas > 12){
    horas = horas - 12;
    tarde = true;
  }
   if (tarde = false){
  strip.setPixelColor(0, 255, 255, 255); //prende LED como PM
    strip.show();
  }else{
  strip.setPixelColor(0, 0, 0, 0); //prende LED como AM
  strip.show();
  }
  
  hm = horas * 5;
  
  for (i=1;i<60;i++){ //probar con i<59
    strip.setPixelColor(i, 0, 0, 0);
    strip.show();
    if (hArrow == i){
      if (hArrow <= hm){
        if (hArrow <= minutos){
          strip.setPixelColor(i, 255, 0, 255);
          strip.show();
          hArrow = hArrow + 5;
          step = true;
        }else{
          strip.setPixelColor(i, 255, 0, 0);
          strip.show();
          hArrow = hArrow + 5;
          step = true;
        }
      }
    }
    if (step == false){
      if (minutos >= i){
        strip.setPixelColor(i, 0, 0, 255);
        strip.show();
      }
    }
   step = false;
   if (segundos==i){
      strip.setPixelColor(i, 0, 255, 0);
      strip.show();
    }
  }
}

void newYear() {
  strip.setBrightness(100);
  strip.show();
  for(int ny=0;ny<10;ny++){
    for(i=0;i<60;i++){
      strip.setPixelColor(i, 255, 255, 255);
      strip.show();
      delay(10);
      }
  digitalWrite(buzzer, 1000);
  delay(20);
  digitalWrite(buzzer, 0);
  delay(380);
  for(i=0;i<60;i++){
      strip.setPixelColor(i, 0, 0, 0);
      strip.show();
      }
   }
  digitalWrite(buzzer, 1000);
  for(int j=0;j<5;j++){
     for(i=0;i<60;i++){
         strip.setPixelColor(i, 255, 255, 255);
         strip.show();
         }
     delay(300);
     for(i=0;i<60;i++){
         strip.setPixelColor(i, 0, 0, 0);
         strip.show();
         }
     delay(300);
  }
  digitalWrite(buzzer, 0);
  setup();
}
