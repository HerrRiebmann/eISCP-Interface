String Title = "";
String ElapsedTime = "";
String Channel = "";

bool UnpackReply(uint8_t *buf) {
  int len = 0;
  int endCounter = 0;
  //Check incoming packet ending (EOF, CR, LF)
  for (len = 0; len < IncomingBufferSize; len++)
    if (buf[len] == packetFooterIncoming[endCounter]) {
      if (endCounter == sizeof(packetFooterIncoming) - 1)
        break;
      else
        endCounter++;
    }
    else if (endCounter > 0)
      endCounter = 0;

  if (len == 0) {
    SetText(incoming, "Lenght is", "zero!");
    return false;
  }

  //packet ending found (EOF, CR, LF)
  if (endCounter != sizeof(packetFooterIncoming) - 1) {
    SetText(incoming, "Package", "incomplete");
    return false;
  }

  //Check packet start (ISCP)
  int start = 0;
  int startCounter = 0;
  for (start = 0; start < IncomingBufferSize; start++)
    if (buf[start] == packetHeader[startCounter]) {
      if (startCounter == 3) //4. Place ISCP
        break;
      else
        startCounter++;
    }
    else if (startCounter > 0)
      startCounter = 0;

  start -= startCounter; //Remove lengh("ISCP")
  //Messages without proper header (Album art cover)
  if(start >= IncomingBufferSize){
    SetText("Ignore Msg!", "Start equals", "Buffersize");    
    ClearIncomingBuffer();
    return false;    
  }

  //Whatever there may come...
  if (len == IncomingBufferSize) {
    SetText(incoming, "Buffer reached");
    return false;
  }

  int headLength = buf[lenghtBytePosHeader + start];//startCounter is set after ISCP
  int msgLength = ((int)buf[lenghtBytePosHigh + start] << 8) + buf[lenghtBytePosLow + start];

  uint8_t cmdLenght = 3;
  char command[cmdLenght];
  char message[msgLength - sizeof(packetFooterIncoming) - cmdLenght + start];
  int startPos = headLength + sizeof(commandPrefix) - 1 + start;
  int endPos = headLength + msgLength - sizeof(packetFooterIncoming) + start;

  if (endPos - startPos <= 0) {
    SetText(incoming, command, "Empty!");
    return false;
  }

  int j = 0;
  for (int i = startPos; i < endPos; i++) {
    if (j >= cmdLenght)
      message[j - cmdLenght] = buf[i];
    else
      command[j] = buf[i];    
    j++;
  }
#if DEBUG
  Serial.print("Cmd: ");
  //ESP32 does not support length
  //Serial.write(command, cmdLenght);
  for (int i = 0; i < cmdLenght; i++)
    Serial.write(command[i]);
  Serial.println();
  Serial.print("Msg: ");
  //Serial.write(message, endPos - startPos - cmdLenght);
  for (int i = 0; i < endPos - startPos - cmdLenght; i++)
    Serial.write(message[i]);
  Serial.println();
#endif DEBUG  
  return ProcessMessage(command, message, endPos - startPos - cmdLenght);
}

bool ProcessMessage(char* command, char* message, int messageLength) {
  String s = String(command).substring(0, 3);
  MessageInt = 0xFF;
  MessageText = "";
  MessageBool = false;
  s.toCharArray(MessageCmd, sizeof(MessageCmd));
  if (s == "MVL") {
    MessageInt = GetByteFromHexString(String(message[0]) + String(message[1]));
    SetText(incoming, "Volume", String(MessageInt));
    return true;
  }
  if (s == "FLD") {
    MessageText = GetTextFromByte(message, messageLength);    
    Title = MessageText.substring(0, 10);
    if(Channel == "")
      SetText(incoming, Title, ElapsedTime);
    else
      SetText(Channel, Title, ElapsedTime);
    //UpdateLine2Text(MessageText);
    return true;
  }
  //Net/USB File Format Information (64bit)
  if (s == "NFI") {
    //ToDo: Seperate values "/"    
    SetText(incoming, "NFI", GetTextFromByte(message, messageLength));    
  }
  if (s == "NLS") {
    if (message[0] == 'C') { //Cursor Position
      if (message[1] == '-') //No Cursor
        MessageInt = 0xFF;
      else
        MessageInt = message[1] - 48;//0 == char 48

      if (MessageInt == 0xFF)
        SetText(incoming, "NLS", "None");
      else
        SetText(incoming, "NLS", String(MessageInt));
    }
    if (message[0] == 'U') { //List Elements UX-Name (X = Position)
      MessageText = String(message).substring(3, messageLength);
      MessageInt = message[1];
      SetText(incoming, "NLS", MessageText);
    }
    return true;
  }
  if (s == "NLT") { //List Title Info
    MessageInt = GetByteFromHexString(String(message[0]) + String(message[1]));
    switch (MessageInt) {
      case 0x00:
        MessageText = "MusicServer(";
        break;
      case 0x04:
        MessageText = "Pandora";
        break;
      case 0x0A:
        MessageText = "Spotify";
        break;
      case 0x0C:
        MessageText = "Radiko";
        break;
      case 0x0E:
        MessageText = "TuneIn";
        break;
      case 0x11:
        MessageText = "Home Media";
        break;
      case 0x12:
        MessageText = "Deezer";
        break;
      case 0x18:
        MessageText = "Airplay";
        break;
      case 0xF0:
        MessageText = "USB Front";
        break;
      case 0xF1:
        MessageText = "USB Rear";
        break;
      case 0xF2:
        MessageText = "InternetRadio";
        break;
    }
    Channel = MessageText;
    SetText(incoming, "ServiceType", MessageText);
    return true;
  }
  if (s == "NJA") { //Album Art
    switch(message[0]){
      case '0':
        MessageText = "BMP";        
      break;
      case '1':
        MessageText = "JPEG";
      break;
      case '2':
        MessageText = "URL";
      break;
      case 'n':
        MessageText = "No Image";
      break;
    }
    MessageText += ": ";
    switch(message[1]){
      case '0':
        MessageText += "Start";
      break;
      case '1':
        MessageText += "Next";
      break;
      case '2':
        MessageText += "End";
      break;
      case '-':
        MessageText += "No Image";
      break;
    }
    //MessageText = String(message).substring(2, messageLength);
    SetText(incoming, "Album Art", MessageText);
    return true;
  }
  if (s == "NMS") { //Net Menu Status
    MessageText = String(message).substring(0, messageLength);
    SetText(incoming, "Menu State", MessageText);
    return true;
  }
  if (s == "NST") { //Net USB Play Status
    MessageText = String(message).substring(0, messageLength);
    SetText(incoming, "Play State", MessageText);
    return true;
  }
  if (s == "NTM") { //Time Info "--:--:--/--:--:--"
    MessageText = String(message).substring(0, messageLength);    
    ElapsedTime = MessageText.substring(0, 10);
    SetText(Channel, Title, ElapsedTime);
    return true;
  }
  if (s == "NAL" || s == "NTI" || s == "NTR") { //Album Info || Title Name || Track Info
    MessageText = String(message).substring(0, messageLength);
    SetText(incoming, MessageText);
    return true;
  }
  if (s == "PWR") {
    MessageBool = GetByteFromHexString(String(message[0]) + String(message[1]));// 00 | 01
    if (MessageBool)
      SetText(incoming, "Power", "On");
    else
      SetText(incoming, "Power", "Off");
    return true;
  }
  if (s == "AMT") {
    MessageBool = GetByteFromHexString(String(message[0]) + String(message[1]));
    if (MessageBool)
      SetText(incoming, "Mute", "On");
    else
      SetText(incoming, "Mute", "Off");      
    return true;
  }
  if (s == "SLI") {
    MessageInt = GetByteFromHexString(String(message[0]) + String(message[1]));    
    switch(MessageInt){
      case 0x01:
        MessageText = "CBL/SAT";
        break;
      case 0x02:
        MessageText = "GAME";
        break;
      case 0x03:
        MessageText = "AUX";
        break;
      case 0x10:
        MessageText = "BD/DVD";
        break;
      case 0x11:
        MessageText = "STRM BOX";
        break;  
      case 0x12:
        MessageText = "TV";
        break;
      case 0x23:
        MessageText = "CD";
        break;
      case 0x24:
        MessageText = "FM";
        break;
      case 0x26:
        MessageText = "Tuner";
        break;
      case 0x29:
        MessageText = "USB";
        break;
      case 0x2B:
        MessageText = "NET";
        break;
      case 0x2E:
        MessageText = "BT Audio";
        break;
    }
    if(MessageText == "")
      SetText(incoming, "Input Sel.", String(MessageInt));
    else
      SetText(incoming, "Input Sel.", MessageText);
    return true;
  }
  if (s == "UPD") {
    MessageBool = GetByteFromHexString(String(message[0]) + String(message[1]));
    if(MessageBool)
      SetText("Firmware", "Update", "Available");
    else
      SetText(incoming, "No Update");
    return true;
  }
  if (s == "LMD") {
    MessageInt = GetByteFromHexString(String(message[0]) + String(message[1]));
    SetText(incoming, "List. Mode", String(MessageInt));    
    return true;
  }
  if (s == "NLU" || s == "NMS") { //List Info Update || Menu Status 
    SetText(incoming, "Known but", "unsupported");
    return true;
  }

  //Default:  
  ClearIncomingBuffer();
  
  return false;
}

String GetTextFromByte(char* message, int messageLength) {
  String str = "";
  for (int i = 1; i <= messageLength; i++)
    if (i % 2 == 0)
      str += (char)GetByteFromHexString(String(message[i - 2]) + String(message[i - 1]));
  return str;
}

byte GetByteFromHexString(String hexValue) {
  return getVal(hexValue[1]) + (getVal(hexValue[0]) << 4);
}

byte getVal(char c) {
  if (c >= '0' && c <= '9')
    return (byte)(c - '0');
  else
    return (byte)(c - 'A' + 10);
}

