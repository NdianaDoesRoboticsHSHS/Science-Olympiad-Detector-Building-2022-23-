int fsrPin = 0;
float fsrReading;
float fsrVoltage;
float fsrResistance;
float fsrConductance;
long LEDBrightness;
float fsrForce;
float fsrMass;
int counter;
char incomingCharacter;
float XArrayMean=0;
float YArrayMean=0;
float aLogRegVar[10];
float bLogRegVar[10];
float LogRegRMSD[10];
float LinRegRMSD[10];
float aLinRegVar[16];
float bLinRegVar[16];
int howOftenRegRecalculates =4;//change to needs of project
bool projectHasBeenCalibrated = false;
//EDIT DURING CALIBRATION:
float GreenLEDStartRange=0;
float GreenLEDEndRange=300;
float BlueLEDStartRange=300.01;
float BlueLEDEndRange=600.01;
float RedLEDStartRange=600.02;
float RedLEDEndRange=1000;
//POST-TEST NOTE: MAKE SURE THE VALUES, 'intialWeight' IS SET TO 50, AND 'weightIncrement' IS SET TO 40. ALSO MAKE SURE THAT THE ARRAY BOUNDS IN LINES 21 & 22 ARE THE SAME AS THE EQUATION RESULT IN LINE 20
     int initialWeight = 50;
     int weightIncrement = 50;
 float x[20];//(final y/weightIncrement)+2
float y[20];
 int N = sizeof(x)/sizeof(x[0]);
void setup(void){
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
}
void loop(void){
  handleSerial();
  delay(5000);// Wait
  Serial.println("---------------------------------------");
  Serial.println(N);//tester
  fsrReading = analogRead(fsrPin);
  fsrVoltage = map(fsrReading, 0, 1023, 0, 5000);
  Serial.println(fsrReading);
  Serial.println(fsrVoltage);
  Serial.println("");
  // The voltage = Vcc * R / (R + FSR) where R = 10K and Vcc = 5V
    // so FSR = ((Vcc - V) * R) / V  
    fsrResistance = 5000 - fsrVoltage;     // fsrVoltage is in millivolts so 5V = 5000mV
    fsrResistance *= 10000;                
    fsrResistance /= fsrVoltage;
    Serial.print("FSR resistance in ohms = ");
    Serial.println(fsrResistance);
 
    fsrConductance /= fsrResistance;
    Serial.print("Conductance in microMhos: ");
    Serial.println(fsrConductance);
  if(projectHasBeenCalibrated){
  //mass calculation:
  int SizeClassCounter = 1;
 
  while(SizeClassCounter<=(N/howOftenRegRecalculates)){
  if((fsrResistance<=x[howOftenRegRecalculates*(SizeClassCounter-1)]&&fsrResistance>=x[howOftenRegRecalculates*(SizeClassCounter)])||(SizeClassCounter==1 && (fsrResistance<=x[(howOftenRegRecalculates*(SizeClassCounter-1))+1]&&fsrResistance>=x[howOftenRegRecalculates*(SizeClassCounter)]))||(SizeClassCounter == N/howOftenRegRecalculates && fsrResistance>(initialWeight+weightIncrement*(N-2)))){
    if(LinRegRMSD[SizeClassCounter-1]< LogRegRMSD[SizeClassCounter-1]==true){
  fsrMass = aLinRegVar[SizeClassCounter-1]*fsrResistance + bLinRegVar[SizeClassCounter-1];
      break;
  }
   else if(LogRegRMSD[SizeClassCounter-1]<= LinRegRMSD[SizeClassCounter-1]==true){
  fsrMass = aLogRegVar[SizeClassCounter-1]*log(fsrResistance) + bLogRegVar[SizeClassCounter-1];
      break;
  }
 
  }
  else{
  SizeClassCounter++;
  }
  }
    if(fsrMass>=BlueLEDStartRange && fsrMass<=BlueLEDEndRange){
    digitalWrite(6, HIGH);
    digitalWrite(5, LOW);
    digitalWrite(3, LOW);
    }
     else if(fsrMass>=GreenLEDStartRange && fsrMass<=GreenLEDEndRange){
    digitalWrite(5, HIGH);
    digitalWrite(6, LOW);
    digitalWrite(3, LOW);
    }
     else  if(fsrMass>=RedLEDStartRange && fsrMass<=RedLEDEndRange){
    digitalWrite(3, HIGH);
    digitalWrite(5, LOW);
    digitalWrite(6, LOW);
    }
  Serial.print("The mass of the Object on the force Sensor in Grams(g):");
  Serial.println(fsrMass);
  Serial.println("---------------------------------------");
   }
 
}
void handleSerial(){
 while (Serial.available() > 0) {
   char incomingCharacter = Serial.read();
   switch (incomingCharacter) {
     case 'r':
Serial.println("Run Mode Initiated");
loop();
      break;
     case 'c':
     Serial.println("Calibration Mode Initiated");
     for(int counter=0;counter<N;counter++){
     Serial.print("Add a ");
     if(counter==0){Serial.print(0);}
     else if(counter==1){Serial.print(initialWeight);}
       else{Serial.print((counter-1)*weightIncrement + initialWeight);}
       Serial.println(" grams weight to the force sensor in 5 seconds");
     delay(2000);
    Serial.print("3,");
    delay(1000);
    Serial.print("2,");
    delay(1000);
    Serial.print("1,");
    delay(1000);
     Serial.println("*weight entered*");
     delay(2500);
     fsrReading = analogRead(fsrPin);
        Serial.println("*weight measured*");
     fsrVoltage = map(fsrReading, 0, 1023, 0, 5000);
     fsrResistance = 5000 - fsrVoltage;
     fsrResistance *= 10000;                
     fsrResistance /= fsrVoltage;
       if(isinf(fsrResistance)){fsrResistance = 0.01;}
       x[counter] = fsrResistance;
       if(counter==0){y[counter]=0;}
       else{y[counter] = (counter-1)*weightIncrement+initialWeight;}
       Serial.println(x[counter]);
       Serial.println(y[counter]);
     delay(500);
 }
 
     naturalLogarithmicRegCheck();
    }
 }
}

 void LinearRegCheck(){// y= b+a*ln(x)
   int j = 0;
  for(int i=0;i<(N/howOftenRegRecalculates);i++){
  aLinRegVar[i] = (y[i]-y[(i+howOftenRegRecalculates)])/(x[i]-x[i+howOftenRegRecalculates]);
      while((j/i)<N||isinf(j/i)){
      bLinRegVar[i] += y[j]-(aLinRegVar[i]*x[j]);
      j++;
      if(j%(howOftenRegRecalculates-1)==0){
      break;
      }
    }
    bLinRegVar[i]/=howOftenRegRecalculates;
    for(int Lin10RMSDCount=0;Lin10RMSDCount<howOftenRegRecalculates;Lin10RMSDCount++){
      LinRegRMSD[i] = 0;
    LinRegRMSD[i] += pow(y[Lin10RMSDCount]-(bLinRegVar[Lin10RMSDCount]) + aLinRegVar[Lin10RMSDCount]*log10(x[Lin10RMSDCount]),2);
      }
     LinRegRMSD[i] = sqrt(LinRegRMSD[i]/howOftenRegRecalculates);
    //Serial.println("Logarithmic Function of fsrResistance vs Mass is Mass=" + String(bLogRegVar) +
   // String(aLogRegVar) + "ln(fsrResistance)" );
    //Serial.println("RMSE =" + String(LogRegRMSD[i]));
  Serial.println(aLinRegVar[i]);
    Serial.println(bLinRegVar[i]);
    Serial.println(LinRegRMSD[i]);
    Serial.println(" ");
   
  }
   projectHasBeenCalibrated = true;
   loop();
  }

void naturalLogarithmicRegCheck(){// y= b+a*ln(x)
  int j=0;
  for(int i=0;i<(N/howOftenRegRecalculates);i++){
  aLogRegVar[i] = (y[i]-y[(i+howOftenRegRecalculates)])/(log(x[i])-log(x[i+howOftenRegRecalculates]));
    while((j/i)<=N||isinf(j/i)){
      bLogRegVar[i] += y[j]-(aLogRegVar[i]*log(x[j]));
      j++;
      if(j%(howOftenRegRecalculates)==0){
      break;
      }
    }
    bLogRegVar[i]/=howOftenRegRecalculates;

    for(int LogRMSDCount=0;LogRMSDCount<howOftenRegRecalculates;LogRMSDCount++){
    LogRegRMSD[i] += pow(pow(y[i]-(bLogRegVar[i] + aLogRegVar[i]*log(x[i])),2),0.5);
      }
    LogRegRMSD[i] = sqrt(LogRegRMSD[i]/howOftenRegRecalculates);
   
    //Serial.println("Logarithmic Function of fsrResistance vs Mass is Mass=" + String(bLogRegVar) +
   // String(aLogRegVar) + "ln(fsrResistance)" );
    //Serial.println("RMSE =" + String(LogRegRMSD[i]));
    Serial.println("Natural Logarithmic Function");
  Serial.println(aLogRegVar[i]);
    Serial.println(bLogRegVar[i]);
    Serial.println(LogRegRMSD[i]);
    Serial.println(" ");
    }
LinearRegCheck();
  }
