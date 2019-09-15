//including necessary libraries
#include <SoftwareSerial.h>
#include <Servo.h>
#include <Stepper.h>

//Creating necessary objects and variables for the bluetooth module
SoftwareSerial Bluetooth(2, 3);
String dataIn = "";

//Initialising servos as instances of the Servo class and defining variables for their angles
Servo base;
Servo shoulder;
Servo elbow;
Servo wrist;
float baseAngle;
float shoulderAngle;
float elbowAngle;
float wristAngle;

//Creating/defining variables for the stepper motor and creating an instance of the Stepper class for it
const float STEPS_PER_REV = 32;
const float GEAR_RED = 64;
const float STEPS_PER_OUT_REV = STEPS_PER_REV * GEAR_RED;
const int headSpeed = 1000;
int stepsRequired;
bool scratch = false;
Stepper head(STEPS_PER_REV, 8, 10, 9, 11);

//Creating the hasScratched for use in logic paths later in the code
bool hasScratched;



//The set up function
void setup() {
  //Setting up the bluetooth connection and timeout period
  Bluetooth.begin(9600);
  Bluetooth.setTimeout(1);

  //A loop to keep the motors limp until data is available
  while (Bluetooth.available() == 0) {
    delay(10);
  }

  //Connecting the servos to their corresponding pins
  base.attach(4);
  shoulder.attach(5);
  elbow.attach(6);
  wrist.attach(7);

  //Calling the reset functino to direct the servos to their starting positions
  reset();
}



//The loop function to be repeated throughout the coded
void loop() {
    hasScratched = false;
  
  //Checking if there is incoming bluetooth data
  if (Bluetooth.available() > 0) {
    //Assigning the bluetooth data to a variable
    dataIn = Bluetooth.readString();

    //Checking if the data is for the head's scratching to be turned on and making subsequent adjustments
    if (dataIn == "HG"){
      scratch = true;
    }
    //Checking if the data is for the head's scratching to be turned off and making subsequent adjustments
    else if (dataIn == "HS"){
      scratch = false;
    }
  }

  //Checking if the most recent data is for the "Up Go" instruction
  if (dataIn == "UG") {
    //Checking if the elbow is still within the custom limit
    if (elbowAngle > 26 && shoulderAngle > 90) {
      //Using the rotate function to adjust the multiple necessary motors
      rotate(elbow, elbowAngle, -2);
      rotate(shoulder, shoulderAngle, -1);
      rotate(wrist, wristAngle, -1);
    }
  }

  //Checking if the most recent data is for the "Down Go" instruction
  else if (dataIn == "DG") {
    //Testing if the extension of the arm is within the custom limit
    if (elbowAngle < 140) {
      //Using the rotate function to adjust the multiple necessary motors
      rotate(elbow, elbowAngle, 2);
      rotate(shoulder, shoulderAngle, 1);
      rotate(wrist, wristAngle, 1);
    }
  }

  //Checking if the most recent data is for the "Left Go" instruction
  else if (dataIn == "LG") {
    //Using the rotate function to rotate the base servo left by 1.5 degrees
    rotate(base, baseAngle, -1.5);
  }

  //Checking if the most recent data is for the "Right Go" instruction
  else if (dataIn == "RG") {
    //Using the rotate function to rotate the base servo right by 1.5 degrees
    rotate(base, baseAngle, 1.5);
  }

  //Checking if the most recent data is for the "In Go" instruction
  else if (dataIn == "IG") {
    //Using the rotate function to rotate the shoulder servo down by 1 degree
    rotate(shoulder, shoulderAngle, 1);
  }

  //Checking if the most recent data is for the "Out Go" instruction
  else if (dataIn == "OG") {
    //Checking if the joint is within the custom limit of 90 degrees
    if (shoulderAngle > 90) {
      //Using the rotate function to rotate the shoulder servo up by 1 degree
      rotate(shoulder, shoulderAngle, -1);
    }
  }

  //Checking if the most recent data is for the "Head Up Go" instruction
  else if (dataIn == "HUG") {
    //Using the rotate function to rotate the wrist servo up by 2 degrees
    rotate(wrist, wristAngle, 2);
  }

  //Checking if the most recent data is for the "Head Down Go" instruction
  else if (dataIn == "HDG") {
    //Using the rotate function to rotate the wrist servo down by 2 degrees
    rotate(wrist, wristAngle, -2);
  }

  //Checking if the most recent data is for the "Reset Position" instruction
  else if (dataIn == "R") {
    //Calling the reset function to reset the servos and variables, and reseting the incoming data as to not run this path twice
    reset();
    dataIn = "";
  }

  //Checking if the scratch value is true, in the case that no motors are moving
  else if (scratch){
    head.setSpeed(headSpeed);
    stepsRequired = STEPS_PER_OUT_REV / 20;
    head.step(stepsRequired);
    hasScratched = true;
  }

  //Delaying the next loop as to space apart the actions
  if (not hasScratched) {
    delay(50);
  }
}



//Defining the reset function
void reset() {
  //Setting the angle variables for each motor back to the initial position
  baseAngle = 90;
  shoulderAngle = 130;
  elbowAngle = 90;
  wristAngle = 110;

  //Moving the motors to the new angles
  base.write(baseAngle);
  shoulder.write(shoulderAngle);
  elbow.write(elbowAngle);
  wrist.write(wristAngle);
}



//Defining the rotate function, taking the servo, its angle variable and the amount to increment as parameters
void rotate(Servo &thisServo, float &angle, float increment) {
  //Checking if the increment is negative
  if (increment < 0) {
    //Checking if the angle is at its minimum
    if (angle > 0) {
      //decreasing the angle variable by the increment and moving the servo to this new angle
      angle += increment;
      thisServo.write(angle);
    }
  }
  //Conditional statement of the increment being positive
  else {
    //Checking if the angle is at its maximum
    if (angle < 180) {
      //increasing the angle variable by the increment and moving the servo to this new angle
      angle += increment;
      thisServo.write(angle);
    }
  }
}
