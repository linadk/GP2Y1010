/* TechDonuts' Sharp GP2Y1010 Sensor Sketch */

/* User Adjustible Variables */
float maxVoltage = 0.0;   // Voltage we get when the sensor is completely blocked
float minVoltage = 0.0;   // Voltage we get when the sensor is sealed with tape and left for 30m
const byte measurePin   = 0;     // Analog pin we measure the output from. 
const byte ledClock     = 2;     // Digital pin we provide the clock signal from. 

/* Globals / Constants you shouldn't touch unless you know what you're doing! */
int samplingTime = 280;   // How long we wait after turning on the LED to take a reading. ( 28ms is according to spec )
int samplingPad  = 40;    // Our LED Pulse should last 32ms , this means samplingTime + samplingPad
int sleepTime    = 9480;  // The time between pulses should be 10ms , or sleeptime + samplingPad + samplingTime + readTime
int readTime     = 200;   // Amount of time alloted to the ADC read/conversion 
int adcReading   = 0;     // Value that comes out of the ADC ( 0-1023 )
#define PRINT(x) Serial.print(x)

/* SETUP */
void setup(){
  Serial.begin(9600);
  pinMode(ledClock , OUTPUT);

  // Set up our ADC so that we can manually invoke it without using analogRead()
  // Note: analogRead() totally hecks up our led driving wave's timing!
  // Note: Don't feel bad if you don't get this. It's just wiggling bits to tell the ADC how to behave!
  ADCSRA = bit(ADEN);                             // Enable ADC
  ADCSRA |= bit(ADPS0) | bit(ADPS1) | bit(ADPS2); // ADC Prescaler (128)
  ADMUX = bit(REFS0) | (measurePin & 0x07);       // Voltage Ref

}

/* LOOP */
void loop(){

  int numLoopsTillRest = 10;
  int numLoops = 0;

  // Take a number of readings, then rest
  while(numLoops != numLoopsTillRest){
    
    digitalWrite(ledClock,LOW); // Start our LED Pulse
    delayMicroseconds(samplingTime); // Wait until we are in spec to take a reading
  
    // Tell ADC to start taking reading without waiting for it to finish/blocking
    bitSet(ADCSRA , ADSC); 
  
    // Pad our LED pulse out to (hopefully) 32ms
    delayMicroseconds(samplingPad); 
    digitalWrite(ledClock,HIGH);
  
    // Allow time for ADC to finish reading
    delayMicroseconds(readTime);
  
    // If our ADC is done, read the value.
    if(bit_is_clear(ADCSRA , ADSC)){
      adcReading = ADC; // Read
    }
  
    // Sleep until next pulse start
    delayMicroseconds(sleepTime);

    numLoops++;
    
  }

  delay(500);

  // Standard ADC voltage transform
  float voltage = adcReading * (5.0/1023.0);

  // Grab our min/max voltages - we sanity check these because vibration and other factors can cause bad readings
  if(voltage > maxVoltage && voltage < 5.0){ maxVoltage = voltage; }
  if(voltage < minVoltage && voltage > 0.3){ minVoltage = voltage; }
  if(minVoltage == 0.0){ minVoltage = voltage; } // Catch our zero!

  // Debug stuffs
  PRINT("Voltage: "); PRINT(voltage); PRINT(" Max Voltage: ");  PRINT(maxVoltage); PRINT(" Min Voltage: "); PRINT(minVoltage); PRINT("\n");
  PRINT("DeltaV: "); PRINT( voltage - minVoltage ); PRINT("\n");


}
