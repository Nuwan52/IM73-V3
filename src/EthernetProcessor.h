#include <Ethernet.h>
#include <EthernetUdp.h>




byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(169,254,189,167);
unsigned int localPort = 80;
char packetBuffer[100];
char ReplyBuffer[] = "this is controller";
EthernetUDP Udp;

extern bool startFlag;
extern int Lid_timer;
extern int Tub_timer;
int ConveryorSpeed(int flag);
void Manual_Overide(int data , bool status);
void TubLidExchnage(int data);


void AnalizeEthernetData(String data) {
  Serial.println(data);

  if (data.equals("SO1_ON")) {

    Serial.println("turing on solonoid 01");
    Manual_Overide(CUP_UPWORD , true);
    
  }
  if (data.equals("SO1_OFF")) {

    Serial.println("turing OFF solonoid 01");
    Manual_Overide(CUP_UPWORD , false);
  }


  if (data.equals("SO2_ON")) {

    Serial.println("turing on solonoid 02");
    Manual_Overide(CUP_PUSHER , true);
    
  }
  if (data.equals("SO2_OFF")) {

    Serial.println("turing OFF solonoid 02");
    Manual_Overide(CUP_PUSHER , false);
    
  }


  if (data.equals("SO3_ON")) {

    Serial.println("turing on solonoid 03");
    Manual_Overide(CUP_LIFTER , true);
    
  }
  if (data.equals("SO3_OFF")) {

    Serial.println("turing OFF solonoid 03");
    Manual_Overide(CUP_LIFTER , false);
    
  }


  if (data.equals("SO4_ON")) {

    Serial.println("turing on solonoid 04");
    Manual_Overide(CUP_GRABBER , true);
    
  }
  if (data.equals("SO4_OFF")) {

    Serial.println("turing OFF solonoid 04");
    Manual_Overide(CUP_GRABBER , false);
    
  }



  if (data.equals("SO5_ON")) {

    Serial.println("turing on solonoid 05");
    Manual_Overide(CUP_PUSHER_TO_CONVEYOR , true);
    
  }
  if (data.equals("SO5_OFF")) {

    Serial.println("turing OFF solonoid 05");
    Manual_Overide(CUP_PUSHER_TO_CONVEYOR , false);
    
  }


  if (data.equals("SO6_ON")) {

    Serial.println("turing on solonoid 06");
    Manual_Overide(CUP_DROO , true);
    
  }
  if (data.equals("SO6_OFF")) {

    Serial.println("turing OFF solonoid 06");
    Manual_Overide(CUP_DROO , false);
    
  }

  if (data.equals("SO7_ON")) {

    Serial.println("turing on solonoid 07");
    Manual_Overide(CUP_PUSHER_TO_CONVEYOR_02 , true);
    
  }
  if (data.equals("SO7_OFF")) {

    Serial.println("turing OFF solonoid 07");
    Manual_Overide(CUP_PUSHER_TO_CONVEYOR_02 , false);
    
  }

  if (data.equals("SO8_ON")) {

    Serial.println("turing on solonoid 08");
    
  }
  if (data.equals("SO8_OFF")) {

    Serial.println("turing OFF solonoid 08");
    
  }


  if (data.equals("CONV_INCRE")) {

    Serial.println("convayer speed increase");
    ("S" + String(ConveryorSpeed(1))).toCharArray(ReplyBuffer, 100);
    
    
  }
  if (data.equals("CONV_DECRE")) {

    Serial.println("convayer speed decrease");
    ("S" + String(ConveryorSpeed(-1))).toCharArray(ReplyBuffer, 100);
    
  }




  if (data.equals("TUB")) {

    Serial.println("runing tub");
    TubLidExchnage(Tub_timer);
    Manual_Overide(PROCESS_SELECTOR , true);
    Manual_Overide(CONVERYOR_LIFTER , false);
  }
  if (data.equals("LID")) {

    Serial.println("runing lid");
    TubLidExchnage(Lid_timer);
    Manual_Overide(PROCESS_SELECTOR , false);
    Manual_Overide(CONVERYOR_LIFTER , true);

  }
  if (data.equals("START")) {

    Serial.println("starting the machine");
    startFlag = true;
    
  
  }
  if (data.equals("STOP")) {
    String data = "STOP";
    startFlag = false;
    
  }
}




void clearCharArray() {
  // Fill the char array with null characters
  for (int i = 0; i < sizeof(packetBuffer); i++) {
    packetBuffer[i] = '\0';
  }
}


void EthernetStart() {
  // starting ethernet
  Ethernet.begin(mac, ip);

  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    while (true) {
      delay(1);  // do nothing, no point running without Ethernet hardware
    }
  }
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Ethernet cable is not connected.");
  }
  Udp.begin(localPort);
  Serial.println("done.");
}


void SendDataBack() {
  Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
  Udp.write(ReplyBuffer);
  Udp.endPacket();
}


void RuningEthernetThread() {
  int packetSize = Udp.parsePacket();
  if (packetSize) {
    Serial.print("Received packet of size ");
    Serial.println(packetSize);
    Serial.print("From ");
    IPAddress remote = Udp.remoteIP();
    for (int i = 0; i < 4; i++) {
      Serial.print(remote[i], DEC);
      if (i < 3) {
        Serial.print(".");
      }
    }
    Serial.print(", port ");
    Serial.println(Udp.remotePort());

    // read the packet into packetBuffer

    Udp.read(packetBuffer, 100);
    Serial.println("Contents:");
    Serial.println(packetBuffer);
    AnalizeEthernetData(packetBuffer);
    clearCharArray();

    SendDataBack();

    
  }
}







