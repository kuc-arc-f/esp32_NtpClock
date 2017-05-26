/*
 SNTP Udp Client, 
 timezone= JST
 next NTP get= 600000us( 600s )
 thanks :Arduino Time library
  http://playground.arduino.cc/code/time
*/
#include <WiFi.h>
#include <WiFiUdp.h>
#include <Time.h>
#include <TimeLib.h>

WiFiUDP udp;
const char* ssid = "your-SSID";
const char* password = "your-PASS";
//
unsigned int localPort = 8888;       // local port to listen for UDP packets
//char timeServer[] = "time.nist.gov"; // time.nist.gov NTP server
const char* timeServer = "ntp.nict.jp";
const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets
// uint32_t mTimerNTP=0;
char mCDate[10+1];
char mCTime[10+1];

// send an NTP request to the time server at the given address
void sendNTPpacket(const char* address)
{
  Serial.print("sendNTPpacket : ");
  Serial.println(address);

  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0]  = 0b11100011;   // LI, Version, Mode
  packetBuffer[1]  = 0;     // Stratum, or type of clock
  packetBuffer[2]  = 6;     // Polling Interval
  packetBuffer[3]  = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12] = 49;
  packetBuffer[13] = 0x4E;
  packetBuffer[14] = 49;
  packetBuffer[15] = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  udp.beginPacket(address, 123); //NTP requests are to port 123
  udp.write(packetBuffer, NTP_PACKET_SIZE);
  udp.endPacket();
}

time_t readNTPpacket() {
  Serial.println("Receive NTP Response");
  udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
  unsigned long secsSince1900 = 0;
  // convert four bytes starting at location 40 to a long integer
  secsSince1900 |= (unsigned long)packetBuffer[40] << 24;
  secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
  secsSince1900 |= (unsigned long)packetBuffer[42] <<  8;
  secsSince1900 |= (unsigned long)packetBuffer[43] <<  0;
  return secsSince1900 - 2208988800UL;
}

time_t getNtpTime()
{
  while (udp.parsePacket() > 0) ; // discard any previously received packets
  Serial.println("Transmit NTP Request");
  sendNTPpacket(timeServer);

  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500) {
    int size = udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      return readNTPpacket();
    }
   }

  Serial.println("No NTP Response :-(");
  return 0; // return 0 if unable to get the time
}

void wait_forCount(int iNum){
    for(int i=0; i< iNum ;i++){
      delay(1000);
      Serial.println("#wait_forCount : " +String(i));
    }
}
//
void init_udpClient() {
//  Serial.begin( 115200 );
 // Serial.begin( 9600 );
  Serial.println("#Start-setup-ssEsp");
  // wifi-start
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  WiFi.begin(ssid, password); 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  //NTP
  wait_forCount(3);
  udp.begin(localPort);
  Serial.println("#End-Setup");
}

String get_mCDate(){  return String(mCDate); }
String get_mCTime(){  return String(mCTime); }
//
void time_display(){
  time_t n = now();
  time_t t;
  char s[20];
  //char cDate[10+1];
  //char cTime[10+1];
  const char* format = "%04d-%02d-%02d %02d:%02d:%02d";
  const char* fmtTime = "%02d:%02d:%02d";
  const char* fmtDate = "%04d-%02d-%02d";
  // JST
  t= n+ (9 * SECS_PER_HOUR);
  sprintf(s, format, year(t), month(t), day(t), hour(t), minute(t), second(t));
  Serial.print("JST : ");
  Serial.println(s);
  sprintf( mCDate, fmtDate, year(t), month(t), day(t) );
  sprintf( mCTime, fmtTime, hour(t), minute(t), second(t));
  // UTC
  t=n;
  sprintf(s, format, year(t), month(t), day(t), hour(t), minute(t), second(t));
  Serial.print("UTC : ");
  Serial.println(s);
}
void exec_syncNtp(){
    wait_forCount(2);
    setSyncProvider(getNtpTime);
    delay(1000); 
Serial.println("#Call-getNtpTime");
Serial.println( String(year())+"-"+ String(month())+"-"+ String(day( ))+" "+String(hour())+":"+String(minute())+":"+String(second())  );       
}
/*
void loop() {
 if(millis() > mTimerNTP){
    wait_forCount(2);
    setSyncProvider(getNtpTime);
    delay(1000); 
Serial.println("#Call-getNtpTime");
Serial.println( String(year())+"-"+ String(month())+"-"+ String(day( ))+" "+String(hour())+":"+String(minute())+":"+String(second())  );       
  }
  time_display();
  delay(1000);  

}


*/

