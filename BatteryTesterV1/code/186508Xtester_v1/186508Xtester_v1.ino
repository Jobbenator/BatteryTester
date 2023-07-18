String command;

#define BatPin A14
#define ResPin A15

double maxVolt = 4.2; // maximum accepteble cell voltage.
double minVolt = 2.9; //miniimum accepteble cell voltage
double minRec = 2.0; //miniimum recognizeble voltage

double resValue = 5; 

// set the pins connected to the mosfet's
int mosPin[] = {0, 1, 2, 3, 4, 5, 6, 7};
// pins used for measure voltage close to the battery
int analogBatPin[] = {0, 2, 4, 6, 8, 10, 12, 14};
// pins used for measure voltage close to the resistors
int analogResPin[] = {1, 3, 5, 7, 9, 11, 13, 15};


long startTime[] = {0, 0, 0, 0, 0, 0, 0, 0}; 
long millisPassed[] = {0, 0, 0, 0, 0, 0, 0, 0};
double seksPassed[] = {0, 0, 0, 0, 0, 0, 0, 0};

int doneBool[] = {0, 0, 0, 0, 0, 0, 0, 0};
int slotBool[] = {0, 0, 0, 0, 0, 0, 0, 0}; // Keeps track of plugged in cells. 

double idCell[] = {0, 0, 0, 0, 0, 0, 0, 0};
double voltageBat[] = {0, 0, 0, 0, 0, 0, 0, 0}; 
double current[] = {0, 0, 0, 0, 0, 0, 0, 0};
double capacity[] = {0, 0, 0, 0, 0, 0, 0, 0};

 


void setup() {
    Serial.begin(9600); 
    Serial.println("Program is up and running.");
    
    // MOSPIN config
    for(int i = 0; 7>i; i++){
      pinMode(mosPin[i], OUTPUT);
      digitalWrite(mosPin[i], LOW);
    }
}
 
void loop() {  
  
    if(Serial.available()){
        command = Serial.readStringUntil('\n');


        
        if(command.equals("voltage")){
           // Serial.println(getVoltage(i));
        }

        else if(command.equals("b")){
            Serial.println("Motors set backwards:"); 

        }     
          
        
        else if(command.equals("commands")){
            Serial.println("Stop: stop");

        }
        else{
            Serial.println("");
            Serial.println("Invalid command.");
            Serial.println("Type commands to see commands.");
        }
    }

// ================ Test Function calls. ====================
  for(int i = 0; 7>=i; i++){

        int slotNum = i+1; // to see slot num begin on 1
        setCellData(i);
        
        // too high voltage
        if(getVoltage(i)>maxVolt){
          stopTest(i); 
        }
//        // too low voltage
        else if((getVoltage(i)<minVolt)&&(getVoltage(i)>minRec)){
          stopTest(i);           
          if(doneBool[i]==1){
            Serial.print("Cell done on Slot");
            Serial.println(slotNum);
          }
          else{
          Serial.println("");
          Serial.print("Warning!: Voltage too low on Slot: ");
          Serial.println(slotNum);
          }
        }

        // cell setup
        else if((getVoltage(i)<maxVolt)&&(getVoltage(i)>minVolt)){
          if(slotBool[i]==0){ 
            Serial.println("Initiate setup.");
            cellSetup(i); 
          }
          // cell go
          else if((slotBool[i]=1)){
            Serial.println();
            Serial.print("Testing: ");
            Serial.println(slotNum);
            startDis(i);
            seksPassed[i] = getTime(i)/1000; 
            getCellData(i);
          }
        }
//        
//        // not connected
        else if((getVoltage(i)<minRec)){
          stopTest(i); 
          Serial.println("");
          Serial.print("Nothing connected to Slot: ");
          Serial.println(slotNum);
          resetCell(i);
        }
       // getCellData(i);
    delay(1000); 
  }
}




void cellSetup(int i){
  slotBool[i]=1;
  // giveId();
  setStartTime(i);
  startDis(i);
}

void resetCell(int i){
  //resetCellId();
  resetTime(i);
  slotBool[i]=0;
  doneBool[i] = 0;
}

void stopTest(int i){
  stopDis(i);
  resetCell(i);
}

// ================ MOSFET conttrol ================
void startDis(int i){
  //digitalWrite(7, HIGH);
  analogWrite(mosPin[i], 190); // mosPin[i]
}
void stopDis(int i){
  digitalWrite( mosPin[i], LOW);
  //Serial.println("Test Stopped");
}
// ===============================================

//============= time counting functions ============
// stores start time 
int setStartTime(int i){
  startTime[i] = millis(); 
  return startTime[i];
}

// returns counted time
double getTime(int i){
  millisPassed[i] = millis()-startTime[i];
  return millisPassed[i];
}

// resets time counter
void resetTime(int i){ 
  startTime[i] = 0;
}
//==================================================



int getId(){
  return 0; 
}

// ========== Voltage measurments ===============
double getVoltage(int i){
  
  voltageBat[i] = 2*((analogRead(analogBatPin[i]) * 5.0)/1024);
  return voltageBat[i]; 
}

double getCurrent(int i){
    double voltageRes = 2*((analogRead(analogResPin[i]) * 5.0)/1024);
    current[i] = (voltageBat[i] - voltageRes) / resValue;
    return current[i];
}

void getCapacity(int i){
  
}
// =============================================


void setCellData(int i){
  getVoltage(i); 
  getCurrent(i);
  getCapacity(i);
}

void getCellData(int i){
  double cellNum = i+1;
  setCellData(i);
  double dataArr[] = {cellNum, seksPassed[i], voltageBat[i], current[i], capacity[i]}; // {i, idCell[i], voltageBat[i], current[i], capacity[i]}
  for(int j = 0; 4>=j; j++){
    Serial.print(dataArr[j]);
    Serial.print("  ");
  }
  Serial.println();
}
