#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#if ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#include "Motor.h"
#include "SerialESP8266wifi.h"
#include "Singleton.h"
#include "Position.h"


class Environment: public Singleton<Environment>{

  public:
    void setWiFi(SerialESP8266wifi * wifi);
    SerialESP8266wifi * wifi();

    void setLeftMotor(Motor * m);
    Motor * LMotor();

    void setRightMotor(Motor * m);
    Motor * RMotor();

    void updateHeading(float heading);
    float heading();

    void updateDistance(float distance);
    float distance();

    void updatePosition(Position * p);
    Position * position();

    void addCoveredDistance(float covered);
    float coveredDistance();
    void resetCoveredDistance();

    void addSweptDegrees(float delta);
    float sweptDegrees();
    void resetSweptDegrees();

  private:
    float h,d,c=0,sd=0;
    SerialESP8266wifi * w;
    Motor *l, *r;
    Position * p;
};

#endif
