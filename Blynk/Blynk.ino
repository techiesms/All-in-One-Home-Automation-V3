
#define BLYNK_PRINT Serial

#define BLYNK_TEMPLATE_ID "TEMPLATE_ID"
#define BLYNK_TEMPLATE_NAME "TEMPELATE_NAME"
#define BLYNK_AUTH_TOKEN "AUTH_TOKEN"


#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <IRremote.h>     // https://github.com/Arduino-IRremote/Arduino-IRremote (3.6.1)
#include <AceButton.h>    // https://github.com/bxparks/AceButton (1.9.2)
#include <DHT.h>          // https://github.com/adafruit/DHT-sensor-library (1.4.3)

BlynkTimer timer; // Timer to check Blynk Connectivity

using namespace ace_button;

//#define BLYNK_DEBUG

#define APP_DEBUG

// IR Remote Code for Lights
#define IR_Relay1           0x1FE50AF
#define IR_Relay2           0x1FED827
#define IR_Relay3           0x1FEF807
#define IR_Relay4           0x1FE30CF
#define IR_Relay_All_Off    0x1FE48B7
#define IR_Relay_All_On     0x1FE7887

// IR Remote Code for Fan
#define IR_Speed_Up         0x1FE609F
#define IR_Speed_Dw         0x1FEA05F
#define IR_Fan_off          0x1FE10EF
#define IR_Fan_on           0x1FE906F

// Define Relay Pins
#define r1 25
#define r2 26
#define r3 4
#define r4 22

// Define Switch Pins
#define switch1 32
#define switch2 35
#define switch3 34
#define switch4 39
#define fan_switch 33

// Pins of Fan Regulator Knob
#define s1  27
#define s2  14
#define s3  23
#define s4  13

// Pins of Relay (Fan Speed Control)
#define Speed1 21
#define Speed2 19
#define Speed4 18

// Switch Flags
bool SWITCH5_FLAG = 1;
bool SWITCH6_FLAG = 1;
bool SWITCH7_FLAG = 1;
bool SWITCH8_FLAG = 1;

// Default Relay State
bool relay1 = LOW;
bool relay2 = LOW;
bool relay3 = LOW;
bool relay4 = LOW;

// Relay State
bool switch_state_ch1 = LOW;
bool switch_state_ch2 = LOW;
bool switch_state_ch3 = LOW;
bool switch_state_ch4 = LOW;

// Flags for Fan Speed
bool speed1_flag = 1;
bool speed2_flag = 1;
bool speed3_flag = 1;
bool speed4_flag = 1;
bool speed0_flag = 1;

int curr_speed          = 0;

#define DEBUG_SW 1

#define irPin 17 // IR sensor pin
#define DHTPIN 16

float temperature_value = 0;
float humidity_value    = 0;


DHT dht(DHTPIN, DHT11);
IRrecv irrecv(irPin);
decode_results results;

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "SmS_jiofi";
char pass[] = "sms123458956";


ButtonConfig config1;
AceButton button1(&config1);
ButtonConfig config2;
AceButton button2(&config2);
ButtonConfig config3;
AceButton button3(&config3);
ButtonConfig config4;
AceButton button4(&config4);
ButtonConfig config5;
AceButton button5(&config5);

void handleEvent1(AceButton*, uint8_t, uint8_t);
void handleEvent2(AceButton*, uint8_t, uint8_t);
void handleEvent3(AceButton*, uint8_t, uint8_t);
void handleEvent4(AceButton*, uint8_t, uint8_t);
void handleEvent5(AceButton*, uint8_t, uint8_t);

void  send_sensor();

BLYNK_WRITE(V0)
{
  int fan_speed = param.asInt(); // assigning incoming value from pin V1 to a variable
  if (fan_speed == 0)
  {
    speed_0();
  }
  if (fan_speed == 1)
  {
    speed_1();
  }
  if (fan_speed == 2)
  {
    speed_2();
  }
  if (fan_speed == 3)
  {
    speed_3();
  }
  if (fan_speed == 4)
  {
    speed_4();
  }
}

BLYNK_WRITE(V1)
{
  int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
  digitalWrite(r1, pinValue);
  // process received value
}

BLYNK_WRITE(V2)
{
  int pinValue = param.asInt(); // assigning incoming value from pin V2 to a variable
  digitalWrite(r2, pinValue);
  // process received value
}

BLYNK_WRITE(V3)
{
  int pinValue = param.asInt(); // assigning incoming value from pin V3 to a variable
  digitalWrite(r3, pinValue);
  // process received value
}

BLYNK_WRITE(V4)
{
  int pinValue = param.asInt(); // assigning incoming value from pin V4 to a variable
  digitalWrite(r4, pinValue);
  // process received value
}

void setup()
{
  // Debug console
  Serial.begin(115200);
  irrecv.enableIRIn(); // Enabling IR sensor

  pinMode(switch1, INPUT_PULLUP);
  pinMode(switch2, INPUT_PULLUP);
  pinMode(switch3, INPUT_PULLUP);
  pinMode(switch4, INPUT_PULLUP);
  pinMode(fan_switch, INPUT_PULLUP);


  pinMode(s1, INPUT_PULLUP);
  pinMode(s2, INPUT_PULLUP);
  pinMode(s3, INPUT_PULLUP);
  pinMode(s4, INPUT_PULLUP);


  pinMode(r1, OUTPUT);
  pinMode(r2, OUTPUT);
  pinMode(r3, OUTPUT);
  pinMode(r4, OUTPUT);
  pinMode(Speed1, OUTPUT);
  pinMode(Speed2, OUTPUT);
  pinMode(Speed4, OUTPUT);

  digitalWrite(r1, 0);
  digitalWrite(r2, 0);
  digitalWrite(r3, 0);
  digitalWrite(r4, 0);

  config1.setEventHandler(button1Handler);
  config2.setEventHandler(button2Handler);
  config3.setEventHandler(button3Handler);
  config4.setEventHandler(button4Handler);
  config5.setEventHandler(button5Handler);

  button1.init(switch1);
  button2.init(switch2);
  button3.init(switch3);
  button4.init(switch4);
  button5.init(fan_switch);

  WiFi.begin(ssid, pass);
  dht.begin();
  timer.setInterval(3000L, checkBlynk); // check if connected to Blynk server every 3 seconds

  Blynk.config(BLYNK_AUTH_TOKEN);//, ssid, pass);
}

void loop()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    if (DEBUG_SW) Serial.println("WiFi Not Connected");
  }
  else
  {
    if (DEBUG_SW) Serial.println("WiFi Connected");
    Blynk.run();
  }
  button1.check();
  button2.check();
  button3.check();
  button4.check();
  button5.check();
  fan();
  ir_remote();

  timer.run(); // Initiates SimpleTimer


  // put your main code here, to run repeatedly:
}



void ir_remote() {
  if (DEBUG_SW)Serial.println("Inside IR REMOTE");
  if (irrecv.decode(&results)) {
    if (DEBUG_SW)Serial.println(results.value, HEX); //print the HEX code
    switch (results.value) {
      case IR_Relay1:
        switch_state_ch1 = !switch_state_ch1;
        digitalWrite(r1, switch_state_ch1);
        if (DEBUG_SW)Serial.println("RELAY1 ON");
        Blynk.virtualWrite(V1, switch_state_ch1);
        delay(100);
        break;
      case IR_Relay2:
        switch_state_ch2 = !switch_state_ch2;
        digitalWrite(r2, switch_state_ch2);
        Blynk.virtualWrite(V2, switch_state_ch2);
        delay(100);
        break;
      case IR_Relay3:
        switch_state_ch3 = !switch_state_ch3;
        digitalWrite(r3, switch_state_ch3);
        Blynk.virtualWrite(V3, switch_state_ch3);
        delay(100);
        break;
      case IR_Relay4:
        switch_state_ch4 = !switch_state_ch4;
        digitalWrite(r4, switch_state_ch4);
        Blynk.virtualWrite(V4, switch_state_ch4);
        delay(100);
        break;
      case IR_Relay_All_Off:
        All_Lights_Off();
        break;
      case IR_Relay_All_On:
        All_Lights_On();
        break;
      case IR_Fan_on:
        if (curr_speed == 0)
        {
          speed_0                                                                                                                                                                                                                                          ();
        }
        else if (curr_speed == 1)
        {
          speed_1();
        }
        else if (curr_speed == 2)
        {
          speed_2();
        }
        else if (curr_speed == 3)
        {
          speed_3();
        }
        else if (curr_speed == 4)
        {
          speed_4();
        }
        else
        {}
        break;
      case IR_Fan_off:
        speed_0();
        break;
      case IR_Speed_Up:
        if (curr_speed == 1)
        {
          speed_2();
        }
        else if (curr_speed == 2)
        {
          speed_3();
        }
        else if (curr_speed == 3)
        {
          speed_4();
        }
        else if (curr_speed == 4)
        {
          //Do nothing
        }
        else {}

        break;
      case IR_Speed_Dw:
        if (curr_speed == 1)
        {
          //Do nothing
        }
        if (curr_speed == 2)
        {
          speed_1();
        }
        if (curr_speed == 3)
        {
          speed_2();
        }
        if (curr_speed == 4)
        {
          speed_3();
        }
        else
        {}

        break;
      default : break;
    }
    irrecv.resume();
  }
}

void All_Lights_Off()
{
  switch_state_ch1 = 0;
  digitalWrite(r1, LOW);
  Blynk.virtualWrite(V1, LOW);

  switch_state_ch2 = 0;
  digitalWrite(r2, LOW);
  Blynk.virtualWrite(V2, LOW);

  switch_state_ch3 = 0;
  digitalWrite(r3, LOW);
  Blynk.virtualWrite(V3, LOW);

  switch_state_ch4 = 0;
  digitalWrite(r4, LOW);
  Blynk.virtualWrite(V4, LOW);

}

void All_Lights_On()
{
  switch_state_ch1 = 1;
  digitalWrite(r1, HIGH);
  Blynk.virtualWrite(V1, HIGH);

  switch_state_ch2 = 1;
  digitalWrite(r2, HIGH);
  Blynk.virtualWrite(V2, HIGH);

  switch_state_ch3 = 1;
  digitalWrite(r3, HIGH);
  Blynk.virtualWrite(V3, HIGH);

  switch_state_ch4 = 1;
  digitalWrite(r4, HIGH);
  Blynk.virtualWrite(V4, HIGH);

}


void checkBlynk()
{
  bool isconnected = Blynk.connected();
  if (isconnected == false)
  {
    if (DEBUG_SW)Serial.println("Blynk Not Connected");
  }
  if (isconnected == true)
  {
    if (DEBUG_SW)Serial.println("Blynk Connected");
    send_sensor();
  }
}




//functions for defineing manual switch

void button1Handler(AceButton* button, uint8_t eventType, uint8_t buttonState) {
  if (DEBUG_SW)Serial.println("EVENT1");
  switch (eventType) {
    case AceButton::kEventPressed:
      if (DEBUG_SW)Serial.println("kEventPressed");
      Blynk.virtualWrite(V1, HIGH);
      digitalWrite(r1, HIGH);
      break;
    case AceButton::kEventReleased:
      if (DEBUG_SW)Serial.println("kEventReleased");
      Blynk.virtualWrite(V1, LOW);
      digitalWrite(r1, LOW);
      break;
  }
}
void button2Handler(AceButton* button, uint8_t eventType, uint8_t buttonState) {
  if (DEBUG_SW)Serial.println("EVENT2");
  //EspalexaDevice* d2 = espalexa.getDevice(1);
  switch (eventType) {
    case AceButton::kEventPressed:
      if (DEBUG_SW)Serial.println("kEventPressed");
      Blynk.virtualWrite(V2, HIGH);
      digitalWrite(r2, HIGH);
      break;
    case AceButton::kEventReleased:
      if (DEBUG_SW)Serial.println("kEventReleased");
      Blynk.virtualWrite(V2, LOW);
      digitalWrite(r2, LOW);
      break;
  }
}
void button3Handler(AceButton* button, uint8_t eventType, uint8_t buttonState) {
  if (DEBUG_SW)Serial.println("EVENT3");
  //EspalexaDevice* d3 = espalexa.getDevice(2);
  switch (eventType) {
    case AceButton::kEventPressed:
      if (DEBUG_SW)Serial.println("kEventPressed");
      Blynk.virtualWrite(V3, HIGH);
      digitalWrite(r3, HIGH);
      break;
    case AceButton::kEventReleased:
      if (DEBUG_SW)Serial.println("kEventReleased");
      Blynk.virtualWrite(V3, LOW);
      digitalWrite(r3, LOW);
      break;
  }
}
void button4Handler(AceButton* button, uint8_t eventType, uint8_t buttonState) {
  if (DEBUG_SW)Serial.println("EVENT4");
  //EspalexaDevice* d4 = espalexa.getDevice(3);
  switch (eventType) {
    case AceButton::kEventPressed:
      if (DEBUG_SW)Serial.println("kEventPressed");
      Blynk.virtualWrite(V4, HIGH);
      digitalWrite(r4, HIGH);
      break;
    case AceButton::kEventReleased:
      if (DEBUG_SW)Serial.println("kEventReleased");
      Blynk.virtualWrite(V4, LOW);
      digitalWrite(r4, LOW);
      break;
  }
}

void button5Handler(AceButton* button, uint8_t eventType, uint8_t buttonState) {
  if (DEBUG_SW)Serial.println("EVENT5");
  switch (eventType) {
    case AceButton::kEventPressed:
      if (DEBUG_SW)Serial.println("kEventPressed");
      if (curr_speed == 0)
      {
        speed_0();
      }
      if (curr_speed == 1)
      {
        speed_1();
      }
      if (curr_speed == 2)
      {
        speed_2();
      }
      if (curr_speed == 3)
      {
        speed_3();
      }
      if (curr_speed == 4)
      {
        speed_4();
      }
      break;
    case AceButton::kEventReleased:
      if (DEBUG_SW)Serial.println("kEventReleased");
      digitalWrite(Speed1, LOW);
      digitalWrite(Speed2, LOW);
      digitalWrite(Speed4, LOW);
      curr_speed = 0;
      Blynk.virtualWrite(V0, curr_speed);
      break;
  }
}

void fan()
{
  if ( digitalRead(s1) == LOW && speed1_flag == 1)
  {
    speed_1();
    speed1_flag = 0;
    speed2_flag = 1;
    speed3_flag = 1;
    speed4_flag = 1;
    speed0_flag = 1;


  }
  if (digitalRead(s2) == LOW && digitalRead(s3) == HIGH && speed2_flag == 1)
  {
    speed_2();
    speed1_flag = 1;
    speed2_flag = 0;
    speed3_flag = 1;
    speed4_flag = 1;
    speed0_flag = 1;

  }
  if (digitalRead(s2) == LOW && digitalRead(s3) == LOW && speed3_flag == 1)
  {
    speed_3();
    speed1_flag = 1;
    speed2_flag = 1;
    speed3_flag = 0;
    speed4_flag = 1;
    speed0_flag = 1;
  }
  if (digitalRead(s4) == LOW  && speed4_flag == 1)
  {
    speed_4();
    speed1_flag = 1;
    speed2_flag = 1;
    speed3_flag = 1;
    speed4_flag = 0;
    speed0_flag = 1;
  }
  if (digitalRead(s1) == HIGH && digitalRead(s2) == HIGH && digitalRead(s3) == HIGH && digitalRead(s4) == HIGH  && speed0_flag == 1)
  {
    speed_0();
    speed1_flag = 1;
    speed2_flag = 1;
    speed3_flag = 1;
    speed4_flag = 1;
    speed0_flag = 0;
  }

}


//functions for defineing of speeds

void speed_0()
{
  //All Relays Off - Fan at speed 0
  if (DEBUG_SW)Serial.println("SPEED 0");
  curr_speed = 0;
  Blynk.virtualWrite(V0, curr_speed);
  digitalWrite(Speed1, LOW);
  digitalWrite(Speed2, LOW);
  digitalWrite(Speed4, LOW);

}

void speed_1()
{
  //Speed1 Relay On - Fan at speed 1
  if (DEBUG_SW)Serial.println("SPEED 1");
  curr_speed = 1;
  Blynk.virtualWrite(V0, curr_speed);
  digitalWrite(Speed1, LOW);
  digitalWrite(Speed2, LOW);
  digitalWrite(Speed4, LOW);
  delay(1000);
  digitalWrite(Speed1, HIGH);
}

void speed_2()
{
  //Speed2 Relay On - Fan at speed 2
  if (DEBUG_SW)Serial.println("SPEED 2");
  curr_speed = 2;
  Blynk.virtualWrite(V0, curr_speed);
  digitalWrite(Speed1, LOW);
  digitalWrite(Speed2, LOW);
  digitalWrite(Speed4, LOW);
  delay(1000);
  digitalWrite(Speed2, HIGH);
}

void speed_3()
{
  //Speed1 & Speed2 Relays On - Fan at speed 3
  if (DEBUG_SW)Serial.println("SPEED 3");
  curr_speed = 3;
  Blynk.virtualWrite(V0, curr_speed);
  digitalWrite(Speed1, LOW);
  digitalWrite(Speed2, LOW);
  digitalWrite(Speed4, LOW);
  delay(1000);
  digitalWrite(Speed1, HIGH);
  digitalWrite(Speed2, HIGH);

}

void speed_4()
{
  //Speed4 Relay On - Fan at speed 4
  if (DEBUG_SW)Serial.println("SPEED 4");
  curr_speed = 4;
  Blynk.virtualWrite(V0, curr_speed);
  digitalWrite(Speed1, LOW);
  digitalWrite(Speed2, LOW);
  digitalWrite(Speed4, LOW);
  delay(1000);
  digitalWrite(Speed4, HIGH);
}

void send_sensor() {

  float h = dht.readHumidity();
  float t = dht.readTemperature(); // or dht.readTemperature(true) for Fahrenheit

  if (isnan(h) || isnan(t)) {
    if (DEBUG_SW)Serial.println("Failed to read from DHT sensor!");
    return;
  }
  else
  {
    humidity_value = h;
    temperature_value = t;
    if (DEBUG_SW)Serial.print("Temperature - "); if (DEBUG_SW)Serial.println(t);
    if (DEBUG_SW)Serial.print("Humidity - "); if (DEBUG_SW)Serial.println(h);
    Blynk.virtualWrite(V5, temperature_value);
    Blynk.virtualWrite(V6, humidity_value);
  }
}
