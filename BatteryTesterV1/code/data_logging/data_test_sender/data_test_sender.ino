int cellSlot = 5; 
int cellVoltage = 4; 
int capcity = 3; 
int dischargeTime = 2; 


void setup() {
  Serial.begin(9600);
}


void loop() {
  Serial.write('45');
  Serial.write((int)cellVoltage);
//  Serial.write(capcity);
//  Serial.write(dischargeTime); 
  delay(1000);                       
}
