#include <Wire.h>
#include <time.h>
#include <stdlib.h>
#include <Position.h>
#include <QMC5883L.h>
#include <HSM5H.h>
#include <Environment.h>

#include <SoftwareSerial.h>
#include <SerialESP8266wifi.h>
#define sw_serial_rx_pin A0 //  Connect this pin to TX on the esp8266
#define sw_serial_tx_pin A1 //  Connect this pin to RX on the esp8266
#define esp8266_reset_pin 8 // Connect this pin to CH_PD on the esp8266, not reset. (let reset be unconnected)


#define PinA_ROT_ENC 2
#define PinB_ROT_ENC 3
#define BUENOS_AIRES_DEC -0.13962634f
#define DELTA_CIRCUMFERENCE 2.0f

QMC5883L compass;
Position position;
SoftwareSerial swSerial(sw_serial_rx_pin, sw_serial_tx_pin);
SerialESP8266wifi wifi(Serial, Serial, esp8266_reset_pin, swSerial);//adding Serial enabled local echo and wifi debug
char buffer[50];

/*
 * Functrions for rotary encoder, 
 * to be triggered when interrupt
 * fired.
 */
void inc(){
  position.update(compass.getPreviousReading(), DELTA_CIRCUMFERENCE, 1);
}

void dec(){
  position.update(compass.getPreviousReading(), DELTA_CIRCUMFERENCE, -1);
}


void setup() {
  Serial.begin(9600);
  swSerial.begin(9600);

  //compass startup
  Wire.begin();
  compass.init();

  //rotary encoder setup
  initRotaryEncoder(PinA_ROT_ENC, PinB_ROT_ENC, inc, dec);

  //wifi startup
  wifi.setTransportToUDP();
  wifi.endSendWithNewline(true); // Will end all transmissions with a newline and carrage return ie println.. default is true
  wifi.begin();
  wifi.connectToAP("Retutatario", "");
  wifi.connectToServer("192.168.1.110", "9999");
  wifi.send(SERVER, "ESP8266 test app started");

  

}

/**
 * Some considerations:
 * 
 * I'm assuming that the wifi device will not be transmitting
 * at the same time the robot is moving in a straight line, i.e.,
 * it is not scanning for obstacles. 
 * 
 * This is because both SoftwareSerial and the rotary encoder
 * rely on interrupts to work correctly. 
 * 
 * Perhaps diminishing the baud rate on the ESP8266 may help. Nevertheless,
 * it doesn't seem feasible to transmit the position while the robot is moving
 * because of this issue.
 * 
 * Note that the compass (Wire) also depends on interrupts, but during tests
 * it seemed that the collision is minimal. However, during the scanning process
 * it would be reasonable to allow some time for the compass to update itself, for
 * instance, by diminishing the speed of the motors while rotating in the fixed position.
 */

 /**
  * EDIT:
  * 
  * With wifi using hwSerial now working, listening on swSerial no longer interrupts.
  * However, a "hack" was needed in order to make reading incoming messages work.
  * For some reason, "debugging" each char read (i.e., print it) seems to help. Otherwise,
  * there are some byte losses due to some unknown reason. 
  * 
  * Therefore, the swSerial instantiated above is just a hack. And as we are not expecting to
  * read anything from those pins, no interrupts are fired and the processor isn't slowed down 
  * with such interrupts, allowing to the qmc5883l and rotary encoder to work perfectly.
  */
void loop() {
  Environment::getInstance().updateHeading(compass.heading(BUENOS_AIRES_DEC));
  //Environment::getInstance().updateDistance(ultrasonic.read());
  Environment::getInstance().updatePosition(&position);
  sprintf(buffer,"%s;%s", String(position.getX()).c_str(), String(position.getY()).c_str());
  wifi.send(SERVER, buffer);
  delay(10);
}
