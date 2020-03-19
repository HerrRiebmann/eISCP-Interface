//Start Receiver, navigate to TuneIn-Radio, select specific tuner & Play
void RadioBob() {
  //Check Power Off and turn On:
  SetText("Request", "Power on?");
  SendCommand("!1PWRQSTN");  
  WaitForResponse("PWR");
  if (!MessageBool) {
    SetText("Send", "Power on!");
    SendCommand("!1PWR01");    
  }
  else{
    SetText("Receiver", "Already", "ON!");
    //Ask for current Input:
    SendCommand("!1SLIQSTN");
    return;
  }

  //Check started sucessfully:
  WaitForResponse("PWR");
  if (!MessageBool) {
    SetText("Receiver", "Startup", "Error!");
    return;
  }

  //Disable Net -Images: (Prevent overflowing buffer with images)
  SendCommand("!1NJADIS");

  //Receive: Input, DIM, MUTE, MOT, RAS, PCT, ITV & NDS

  //If "See TV" displayed due to declined userdata transfer to Pioneer
  WaitForResponse("FLD");  
  while(MessageText != "  - See TV -  ") {
    //ToDo: Timeout
    WaitForResponse("FLD");      
  }
  SetText("Send", "Exit", "->See TV");
  SendCommand("!1OSDEXIT");
    
  //Set Input to Net
  SetText("Send", "Input: Net");
  SendCommand("!1SLI2B");  

   WaitForResponse("SLI");
  while(MessageInt != 0x2B){
    WaitForResponse("SLI");    
  } 
     
   //Select List entry No:
   SetText("Send", "ListIndex: 1");
   SendCommand("!1NLSI00001");//TuneIn Radio  
  
  WaitForResponse("FLD");
  while(MessageText.indexOf("My Preset") < 0){
    WaitForResponse("FLD");    
  }
  
  //Set Enter Presets
  SetText("Send", "Enter", "(Preset)");
  SendCommand("!1OSDENTER");
  //WaitForResponse("NLT");
  
  //Check for list entry no and navigate to it directly:  
  WaitForResponse("NLS");
  while(!MessageText.startsWith("RADIO BOB!"))
    WaitForResponse("NLS");
  SetText("Send", String(MessageInt), "(RadioBob)");
  //NetUSB List + L = Line | I = Index + Index 00001-99999
  //Int == 54hex = 6 dec 
  MessageInt -= 47; //-48 but add 1 due to Index starts at 1
  if(MessageInt < 10)
    SendCommand("!1NLSI0000" + String(MessageInt));
  else
    SendCommand("!1NLSI000" + String(MessageInt));
  WaitForResponse("FLD");
  
  //Set Enter
  SetText("Send", "Enter", "(Play)");
  SendCommand("!1OSDENTER");

  //Set Vol to -40
  delay(1000);
  SetText("Send", "Volume -40");
  SendCommand("!1MVL54"); //0x54 = 84dec = 40VOL
}
