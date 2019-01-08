char command[20];
void SetCommand(String str) {
  memset(command, 0, sizeof(command));
  if (str[0] != commandPrefix[0]) //Startswith " ! "
    str = commandPrefix + str;
  str.toCharArray(command, sizeof(command));
}

uint8_t GetCommandLength() {
  uint8_t i = 0;
  for (i = 0; i < sizeof(command); i++)
    if (command[i] == 0x00 || command[i] == 0x0D) //Empty or CarriageReturn
      break;
  return i;
}

uint8_t GetCommandLengthWithFooter() {
  return GetCommandLength() + sizeof(packetFooterOutgoing);
}

uint8_t GetMessageLength() {
  return GetCommandLengthWithFooter() + sizeof(packetHeader);
}

void CreateCommand(uint8_t bts[]){  
  //Set Header
  memcpy(bts, packetHeader, sizeof(packetHeader));
  uint8_t commandLenghWithFooter = GetCommandLengthWithFooter();

  //Set length
  bts[lenghtBytePosHigh] = highByte(commandLenghWithFooter);// / 256;
  bts[lenghtBytePosLow]  = lowByte(commandLenghWithFooter); // % 256;

  //Set command
  memcpy(bts + sizeof(packetHeader), command, GetCommandLength());

  //Set Footer
  memcpy(bts + sizeof(packetHeader) + GetCommandLength(), packetFooterOutgoing, sizeof(packetFooterOutgoing));
}

void SendCommand(String str) {
  SetCommand(str);
  //Bytes to send:
  uint8_t bts[GetMessageLength()];
  CreateCommand(bts);

#if DEBUG
  Serial.print("Out: ");
//  for (int i = 0; i < GetMessageLength(); i++)
//    Serial.write(bts[i]);
//  Serial.println();
  Serial.println(command);
#endif
  SendDataToDevice(bts);
}

String getHex(uint8_t data) {
  String str;
  if (data < 0x10)
    str = "0";
  str += String(data, HEX);
  str.toUpperCase();
  return str;
}
