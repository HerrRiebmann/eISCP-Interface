//Display output in terminal/OLED/Whatever
void SetText(String line1) {
  if (line1.length() > 20) {
    SetText(line1.substring(0, 10), line1.substring(11, 20), line1.substring(21));
    return;
  }
  if (line1.length() > 10) {
    SetText(line1.substring(0, 10), line1.substring(11), "");
    return;
  }
  SetText(line1, "", "");
}

void SetText(String line1, String line2) {
  if (line2.length() > 10)
    SetText(line1, line2.substring(0, 10), line2.substring(11));
  else
    SetText(line1, line2, "");

}
void SetText(String line1, String line2, String line3) {
  //display.clear();
  //display.setColor(WHITE);  
  //display.setTextAlignment(TEXT_ALIGN_LEFT);
  //display.drawString(0, 0, line1);
  //display.drawString(0, 20, line2);
  //display.drawString(0, 40, line3);
  //display.display();
#if DEBUG
  Serial.print(line1);
  if (line2 != "") {
    Serial.print(": ");
    Serial.print(line2);
  }
  if (line3 != "") {
    Serial.print(" ");
    Serial.print(line3);
  }
  Serial.println();
#endif
}
