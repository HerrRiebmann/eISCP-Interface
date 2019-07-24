bool ConnectToDevice() {
  clientConnected = client.connect(host, port);  
  return clientConnected;
}

void SendDataToDevice(uint8_t *bts) {
  if (!clientConnected)
    if (!ConnectToDevice())
      return;
  //Write as once. Either the client (Onkyo/Pioneer) will not respond  
  client.write(bts, GetMessageLength());
}

bool WaitForResponse(char* command) {
  if (!clientConnected)
    if (!ConnectToDevice())
      return false;
  unsigned long timeout = millis();
  while (true) {
    if (millis() - timeout > TcpTimeout) {            
      return false;
    }
    if (Read()){ //Read incoming data
      if (strcmp(MessageCmd, command) == 0) //Data is the requested data
        return true;
    }    
  }
  return false;
}

bool Read() {
  if (!clientConnected)
    if (!ConnectToDevice())
      return false;
  while (client.available()) {
    IncomingBuffer[IncomingBufferCounter++] = client.read();
    //Message complete (Linefeed)
    if (IncomingBuffer[IncomingBufferCounter - 1] == 0x0A)
      break;
    if (IncomingBufferCounter >= IncomingBufferSize) {
      //Incoming buffer exeeded! Clear all
      ClearIncomingBuffer();
      return false;
    }
  }
  if (IncomingBufferCounter == 0)
    return false;
  if (UnpackReply(IncomingBuffer)) {
    ClearIncomingBuffer();
    return true;
  }
  return false;
}

void StopConnection() {
  clientConnected = false;
  client.stop();
}

void ClearIncomingBuffer() {
  memset(IncomingBuffer, 0, IncomingBufferSize);
  IncomingBufferCounter = 0;
}

