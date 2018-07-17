#include "config.h"

#include <ArduinoSTL.h>

using namespace std;

// X Y Z axis for robots
//X = Tibai
//Y = Coxa (Hip Horizontal)
//Z = Femur (Hip Vertical)

std::vector<String> legPositions;
std::vector<String> legList;
std::vector<std::pair<int, int> > legSequence;
std::vector<String> legCommands;

const std::vector<int> coxaPins = {
  cRRCoxaPin , cRMCoxaPin , cRFCoxaPin , cLRCoxaPin , cLMCoxaPin , cLFCoxaPin
};


const std::vector<int> rightRearJoints = {cRRCoxaPin , cRRFemurPin , cRRTibiaPin};
const std::vector<int> rightMidJoints = {cRRCoxaPin , cRRFemurPin , cRRTibiaPin};
const std::vector<int> rightFrontJoints = {cRRCoxaPin , cRRFemurPin , cRRTibiaPin};

const std::vector<int> leftRearJoints = {cRRCoxaPin , cRRFemurPin , cRRTibiaPin};
const std::vector<int> leftMidJoints = {cRRCoxaPin , cRRFemurPin , cRRTibiaPin};
const std::vector<int> leftFrontJoints = {cRRCoxaPin , cRRFemurPin , cRRTibiaPin};

/* const std::vector<int> xPins; */
/* const std::vector<int> zPins; */
const vector<int> xPins = {cRRTibiaPin, cRMTibiaPin, cRFTibiaPin, cLRTibiaPin, cLMTibiaPin, cLFTibiaPin};
const vector<int> zPins = {cRRFemurPin, cLFFemurPin, cLRFemurPin, cRFFemurPin, cLMFemurPin, cRMFemurPin};
const vector<int> yPins;

#define Y_MOD 0
#define Z_MOD 1
#define X_MOD 2

/* const int initPositions[18][2]  = { */
const vector<pair<int,int>> initPositions  = {
  { cRRCoxaPin, cRRInitPosY},
  { cRRFemurPin, cRRInitPosZ },
  { cRRTibiaPin, cRRInitPosX },

  { cRMCoxaPin, cRMInitPosY },
  { cRMFemurPin, cRMInitPosZ },
  { cRMTibiaPin, cRMInitPosX },

  { cRFCoxaPin, cRFInitPosY },
  { cRFFemurPin, cRFInitPosZ },
  { cRFTibiaPin, cRFInitPosX },

  { cLRCoxaPin, cLRInitPosY },
  { cLRFemurPin, cLRInitPosZ },
  { cLRTibiaPin, cLRInitPosX },

  { cLMCoxaPin, cLMInitPosY },
  { cLMFemurPin, cLMInitPosZ },
  { cLMTibiaPin, cLMInitPosX },

  { cLFCoxaPin, cLFInitPosY },
  { cLFFemurPin, cLFInitPosZ },
  { cLFTibiaPin, cLFInitPosX }
};


const std::vector<vector<int>> legJoints ={
  rightRearJoints ,rightMidJoints, rightFrontJoints,leftMidJoints,leftFrontJoints
};


const unsigned int degreeToPulse(const int degree){
  /* DebugSerial.println("degree: " + String(degree)); */
  const double valPerDegree = 1000.0 / 90.0;
  const double val = (degree * valPerDegree) + PulseOffset;
  /* DebugSerial.println("Pulse: " + String(val)); */

  // double -> int
  return int(val);
}

String moveCommandFor(int servo, int pulse, int time) {
  String str = String("#") + servo + "P" + pulse + " T" + time;
  return str;
}

void move(int servo, int pulse, int time, const boolean blocking ) {
  String str = moveCommandFor(servo, pulse, time);
  Serial.println(str);
  Serial1.println(str);
  if (blocking) {
    delay(time);
  }
}

// X Y Z axis for robots
//X = Tibai
//Y = Coxa (Hip Horizontal)
//Z = Femur (Hip Vertical)
void moveSingleServo(const int servo, const int degree,  const int duration = 1000, const boolean blocking = true) {
  move(servo, degreeToPulse(degree), duration, blocking);
}

const String getCommandSequence(const vector<int> pins, const vector<int> values, const int duration) {

  String cmd = "";

  for (std::size_t i = 0, e = pins.size(); i != e; ++i) {
    cmd += String("#") + pins[i] + String("P") + degreeToPulse(values[i]);
  }
  cmd += String("T") + duration;
  return cmd;
}

void moveCommandGroup(const vector<int> pins, const vector<int> values, const int duration) {
  String cmd = getCommandSequence(pins, values, duration);
  Serial.println("Executing command group:");
  Serial.println(cmd);
  SSCSerial.println(cmd);
  delay(duration);
}

void defaultPosition() {
  /* for(auto const &pair : initPositions){ */
  /*     moveSingleServo(pair.first, pair.second, 1000, false); */
  /* } */

  vector<int> pins;
  vector<int> values;
  std::transform( initPositions.begin(), initPositions.end(), std::back_inserter( pins ), [](pair<int,int> pair_) { return pair_.first; });

  std::transform( initPositions.begin(), initPositions.end(), std::back_inserter( values ), [](pair<int,int> pair_) { return pair_.second; });

  moveCommandGroup(pins, values, 1000);
  delay(2000);
}

void sitDown() {
  DebugSerial.println("Starting sit down series");
  defaultPosition();
  DebugSerial.println("Done with sit down series");
}


void initYAxis() {
  // we could use std::transform and a lambda with modulus to extract the x,y,z vectors
  // that way, we could create a command group instead of non blocking single commands
  for (std::size_t i = 0, e = initPositions.size(); i != e; ++i) {
    if(i % 3 == Y_MOD){
      moveSingleServo(initPositions[i].first, initPositions[i].second, 1000, false);
    }
  }
  delay(2000);
}

// template programming seems to be not working / limited...
/* template<typename T> */
/* const std::vector<T> combine(const std::vector<T> a, const std::vector<T> b){ */
 const std::vector<int> combine(const std::vector<int> a, const std::vector<int> b){
   std::vector<int> c;
   c.reserve( a.size() + b.size() ); // preallocate memory
   c.insert( c.end(), a.begin(), a.end() );
   c.insert( c.end(), b.begin(), b.end() );
   return c;
}

void standUp(const int xVal = 45, const int zVal = 10) {
  DebugSerial.println("Starting stand up  series");
  initYAxis();

  const int lowPinSign =  +1;
  const int highPinSign = -1;
  const int lowPinMax = 15;

  std::vector<int> xValues;
  std::vector<int> zValues;


  // Unfortunately std::function is not yet available in ArduinoSTL ...
  /* std::function<int(int)> f =[lowPinMax, lowPinSign, highPinSign](int pin) { */
  /*   if (pin <= lowPinMax) { */
  /*     return xVal * lowPinSign; */
  /*   } else { */
  /*     return xVal * highPinSign; */
  /*   } */
  /* }; */
  /* std::transform( xPins.begin(), xPins.end(), std::back_inserter( xValues ), f); */
  /* std::transform( zPins.begin(), zPins.end(), std::back_inserter( zValues ), f); */



  for (std::size_t i = 0, e = initPositions.size(); i != e; ++i) {
    if (xPins[i] <= lowPinMax) {
      xValues.push_back(xVal * lowPinSign);
    } else {
      xValues.push_back(xVal * highPinSign);
    }
    if (zPins[i] <= lowPinMax) {
      zValues.push_back(zVal * lowPinSign);
    } else {
      zValues.push_back(zVal * highPinSign);
    }
  }

  moveCommandGroup(combine(xPins, zPins), combine(xValues, zValues), 3000);

  /* moveCommandGroup(zPins, zValues, 1000); */
  delay(5000);
  DebugSerial.println("Done with stand up series");
}

void freeServos() {
  String cmd;

  //for(std::vector<T>::iterator it = v.begin(); it != v.end(); ++it) {
  //    /* std::cout << *it; ... */
  //}

  for (auto const& leg : legJoints) {
    cmd = String("");
    for (auto const& joint : leg) {
      cmd += String("#") + joint + "P0";
    }
    cmd += "T200";
    Serial.println("Resettings servos");
    Serial.println(cmd);
    SSCSerial.println(cmd);
  }

}

const initializeVectors() {
  /* legJoints.push_back(rightRearJoints); */
  /* legJoints.push_back(rightMidJoints); */
  /* legJoints.push_back(rightFrontJoints); */
  /* legJoints.push_back(leftRearJoints); */
  /* legJoints.push_back(leftMidJoints); */
  /* legJoints.push_back(leftFrontJoints); */


}


void setup() {
  initializeVectors();
  freeServos();
  DebugSerial.begin(115200);
  SSCSerial.begin(115200);
  delay(3000);
  sitDown();
}

void loop() {
  // x, z
  standUp(70, 40);
  standUp(45, 10);
  standUp(20, -20);
  standUp(0, -50);
  standUp(20, -20);
  standUp(45, 10);
  sitDown();
  freeServos();

  while(true){}
}
