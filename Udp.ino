bool GetUdpAdress() {
  bool success = false;
#if PIONEER
  success = udpSendMessage(udpHost, "!pECNQSTN", port);
#else //Onkyo
  success = udpSendMessage(udpHost, "!xECNQSTN", port);
#endif  
  return success;
}

//******UDP******
bool udpSendMessage(IPAddress ipAddr, String udpMsg, int udpPort) {  
  WiFiUDP udpClientServer;
  // Start UDP client for sending packets
  if (udpClientServer.begin(udpPort) == 0) {
    return false;
  }  
  if (udpClientServer.beginPacket(ipAddr, udpPort) == 0) { // Problem occured!
    udpClientServer.stop();    
#if DEBUG    
    Serial.println("UDP connection failed");
#endif    
    return false;
  }
  //Create Message with Header & Footer
  SetCommand(udpMsg);
  //Bytes to send:
  uint8_t bts[GetMessageLength()];
  CreateCommand(bts);
  int bytesSent = udpClientServer.write(bts,GetMessageLength());    
  if (bytesSent == GetMessageLength()) {
    udpClientServer.endPacket();
    udpClientServer.flush();
    //Receive Response
    int packetSize = udpClientServer.parsePacket();
    unsigned long timeout = millis();
    while (!packetSize) {
      if (millis() - timeout > UdpTimeout) {                
#if DEBUG    
      Serial.println("UdpPackage Timeout");
#endif        
        break;
      }      
      packetSize = udpClientServer.parsePacket();
    }
    if (packetSize) {
      char packetBuffer[255];
      int len = udpClientServer.read(packetBuffer, 255);      
      host = udpClientServer.remoteIP();
    }
      
    udpClientServer.flush();
    udpClientServer.stop();    
    return packetSize > 0;
  } else {
#if DEBUG    
    Serial.println("Failed to send " + udpMsg + ", sent " + String(bytesSent) + " of " + String(udpMsg.length()) + " bytes");
#endif        
    udpClientServer.endPacket();
    udpClientServer.stop();
    return false;
  }
}
