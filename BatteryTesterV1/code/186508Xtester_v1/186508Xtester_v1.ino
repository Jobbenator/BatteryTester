//               _____________________________________+Battery      
//              |                           |
//           ___|___                       | |
//         _|_     _|_                  R3 | |
//        |   |   |   |                     |____A0
//        |CR1|   |CR2|                     |
//        |   |   |   |                    | |
//        |___|   |___|                  R4| |
//          |_______|                      _|_
//              |                          GND
//              |_________
//              |         |  
//            __|__      | |
//          []     |   R1| |
//    PWM___[]     ^      |___A1
//          []_____|      |
//              |        | |
//             _|_     R2| |
//             GND       _|_
//                       GND 
//
//
//#########Ceramic Resistor#########
//       RC1*RC2    10*10 
// CRs = ------- => ----- = 5Ω (10W)
//       RC1+RC2    10+10
//###################################

//######## Voltage Devider A1 ##########                         ######### Voltage Devider A0 ##########
//               _______Vb                                                       Vb    
//              |                           Vb*R2                                 |
//           ___|___                      --------- = A1                         | |             Vb*R4
//         _|_     _|_                    CRs+R1+R2                           R3 | |             ----- = A0
//        |   |   |   |                                                           |___A0         R3+R4
//        |CR1|   |CR2|                                                           |    
//        |   |   |   |                                                          | |
//        |___|   |___|                                                       R4 | |      
//          |_______|                                                            _|_
//              |                                                                GND
//              |         
//             | |
//          R1 | |
//              |__________ A1
//              |     |    |             Vb*(R2||RL)           RL -> ∞                I*R2
//             | |    |   | |         ---------------- = Vout              Vout = Vb --------   
//           R2| |  Vout  | | RL      (R1+CRs)+(R2||RL)                              I(R1+R2)
//             _|_    |____|
//             GND        _|_
//
//###################################################################################################################

#define MOSFET_Pin 9
#define Bat_Pin A0
#define Res_Pin A1
//

// To avoid going thru batteries that are not plugged in.  
#define batteryStatus_1 0
#define batteryStatus_2 0
#define batteryStatus_3 0
#define batteryStatus_4 0
#define batteryStatus_5 0
#define batteryStatus_6 0
#define batteryStatus_7 0
#define batteryStatus_8 0


//int pwm_value = 182;
//doubleDischargeRate = 500; //mAh
//doubleCapacity = 0.0; 
//doubleRes_Value = 5.5;  
//doubleVcc = 5; // Voltage of Arduino 5V pin ( Mesured by Multimeter )  
//doubleCurrent = 0.0; 
//doublemA=0;         
//doubleBat_Volt = 0.0;  
//doubleRes_Volt = 0.0;  
//unsigned long previousMillis = 0; 
//unsigned long millisPassed = 0;  
//doublesample1 =0.000;
//doublesample2= 0.000;
//int x = 0;
//int row = 0;

  unsigned long previousMillis = 0; //Keeps track of the millies past sins the start of the program to reset individual counters. 
  unsigned long millisPassed = 0; 
  
  int  R1 = 10000; 
  int  R2 = 10000;
  int  R3 = 10000; 
  int  R4 = 10000;
  double  RL_A = 100000000;
  double CR1 = 10; 
  double CR2 = 10; 
  double Rds = 0.035;  //Static Drain-to-Source On-Resistance for ~4V 

//  void buz()
//  {
//  digitalWrite(2, HIGH);
//  delay(100);          
//  digitalWrite(2, LOW);  
//  delay(10);            
//
//}   



long readVcc() {
  long result;
  // Read 1.1V reference against AVcc
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Convert
  while (bit_is_set(ADCSRA,ADSC));
  result = ADCL;
  result |= ADCH<<8;
  result = 1125300L / result; // Back-calculate AVcc in mV
  return result;
}

 //####### Battery Voltage Function #######  //Checks the voltage of the cell. 
double batteryVoltageFunk(double analogVoltageVal)
{    
  analogVoltageVal = analogVoltageVal * (4.9 / 1024.0);
  double supply = readVcc() / 1000.0;
  analogVoltageVal = supply / 5 * analogVoltageVal;

//  unsigned int ADCValue;
//  double Vcc = readVcc()/1000.0;
//  ADCValue = analogRead(0);
//  double Voltage = (ADCValue / 1024.0) * Vcc;
  

  Serial.print(analogVoltageVal);
//  Serial.print("        ");
//  Serial.print(Voltage);
                                                                                                         
  double batteryVoltage = (analogVoltageVal*(R3+(R4*RL_A)/(R4+RL_A))/((R4*RL_A)/(R4+RL_A)));      
  
  //doublebatteryVoltage = (analogVoltageVal*(R3+R4))/R4;
 
  return batteryVoltage;
}
//####################################### 



//####### Discharge Rate Function ####### // Checks the current drain of the battetery.  
double discharegeRate(double batteryVoltage, double analogDrainVal)
{
  double CRs = (CR1*CR2)/(CR1+CR2);
  double R4_RLA = (R4*RL_A)/(R4+RL_A);     // Rtot = (R3+(R4||RLA))||(CRs+(Rds||(R1+(R2||RLA))))
  double R2_RLA = (R2*RL_A)/(R2+RL_A);     //        \_____A_____/   \____________B____________/ 
  double A = R3+(R4_RLA);
  double B = CRs+( (Rds * (R1+R2_RLA)/(Rds + (R1+R2_RLA)) ));
  double Rtot = (A*B)/(A+B);
   
  Serial.print("     ");
  Serial.print(Rtot);
  
  double currentDrainTot = batteryVoltage/Rtot; 

  analogDrainVal = analogRead(Res_Pin) * (4.9 / 1024.0);
  double supply = readVcc() / 1000.0;
  analogDrainVal = supply / 5 * analogDrainVal;
 Serial.print("     ->");
  Serial.print(analogDrainVal);
  
  double valtageDrop = (analogDrainVal*(R1+R2_RLA))/R2;
  double currentDrain = (batteryVoltage-valtageDrop)/CRs;
  
  return currentDrain;   
}
//#######################################

// Cell stores the information of each cell before sending it as a string to a python script. 
class Cell { // Create a Cell class with some attributes
  private: 
  String idName; // Gives a uniqe id-name to keep track of the cell data after testing. 
  String date; // Stores the date the cell was tested on. 
  int cellNum; // Gives the order number of the cell.
  float capacity; // Stores the capacity value of the cell in mAh. 
  float startVolt; // Stores the voltage value at the start of the test. 
  float endVolt;  // Stores the voltage value at the end of the test.
};

  void setup() {
   Serial.begin(9600);
   pinMode(MOSFET_Pin, OUTPUT);
 //pinMode(2, OUTPUT);
   digitalWrite(MOSFET_Pin, LOW); 
  }

  void loop() {
    
   //double analogDrainVal = 2.099; // The value of A1 above.  
      
  // double currentDrain = discharegeRate(batteryVoltage, analogDrainVal);
   
  
//   Serial.print(analogVoltageVal);
//   Serial.print(" Ohm     ");   
//   Serial.print(batteryVoltage);
//   Serial.print(" V     ");
//   Serial.print(currentDrain*1000); 
//   Serial.print(" mA     ");
//   Serial.println();
//   Serial.println();

  //#################Battery Function Call Section#####################

  //Check battery voltage
  double analogVoltageVal = analogRead(Bat_Pin); // The value of A0 above.
  
  if(batteryStatus_1 == true){
  millisPassed = millis() - previousMillis;
  double analogVoltageVal = analogRead(Bat_Pin); // The value of A0 above.
  double batVolt = batteryVoltageFunk(analogVoltageVal);
  }
   
   delay(1000);
  }




  








  
//  for(int i=0;i< 100;i++)
//  {
//   sample1=sample1+analogRead(Bat_Pin);
//   delay (2);
//  }
//  sample1=sample1/100; 
//  Bat_Volt = 2* sample1 *Vcc/ 1024.0; 
//
//   for(int i=0;i< 100;i++)
//  {
//   sample2=sample2+analogRead(Res_Pin); 
//   delay (2);
//  }
//  sample2=sample2/100;
//  Res_Volt = 2* sample2 * Vcc/ 1024.0;
//  
//  if ( Bat_Volt > 4.3){
//    digitalWrite(MOSFET_Pin, LOW); 
////    buz();
//    Serial.println( "Warning Voltage Too High! ");
//    Serial.print("Volrage:");
//    Serial.print(Bat_Volt);
//    Serial.println("V");
//    delay(1000);
//   }
//
//   else if(Bat_Volt < 1){
//      digitalWrite(MOSFET_Pin, LOW);
//      Serial.println( "No battery inserted.");
//      Serial.print("Volrage:");
//      Serial.print(Bat_Volt);
//      Serial.println("V");
//      delay(1000);
//  }
//   
//   else if(Bat_Volt < 2.9){
//      digitalWrite(MOSFET_Pin, LOW);
////      buz();
//      Serial.println( "Warning Voltage Too Low! ");
//      Serial.print("Volrage:");
//      Serial.print(Bat_Volt);
//      Serial.println("V");
//      delay(1000);
//  }
//  
//  else if(Bat_Volt > 2.9 && Bat_Volt < 4.3  ) { 
//
////      
////      //Fallowes the desired discharge rate. 
//      if(mA > DischargeRate)
//      {
//        pwm_value--; 
//      }
//
//      else if(mA < DischargeRate)
//      {
//        pwm_value++; 
//      }
//
//      analogWrite(MOSFET_Pin, pwm_value);
//    
//      millisPassed = millis() - previousMillis;
//      Current = (Bat_Volt - Res_Volt) / Res_Value;
//      mA = Current * 1000.0 ;
//      Capacity = Capacity + mA * (millisPassed / 3600000.0);
//      previousMillis = millis();
//      row++;
//      x++;
//
////{"Battery 1337" : {"Voltage" : 3.55, "time" : 365, "ampere" : 2.44, "current capacity" : 150}}
//      Serial.print("{'Battery_0' : ");
//
//      Serial.print("{'voltage' : ");
//      Serial.print(Bat_Volt);
//      Serial.print(", ");
//
//      Serial.print("'time' : ");
//      Serial.print(mA);
//      Serial.print(", ");
//      
//      Serial.print("'ampere' : ");
//      Serial.print(mA);
//      Serial.print(", ");
//      
//      Serial.print("'capacity' : ");
//      Serial.print(Capacity);
//      Serial.print("}}");
//      Serial.println();
//  
//      delay(100); 
 
//     }
// }   
