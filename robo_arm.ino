#include <Servo.h>


#define SERVO_PIN0 4   // node1
#define SERVO_PIN1 5   // node2
#define SERVO_PIN2 6   // node3
#define SERVO_CLAW 7   // claw 
#define SERVO_BASE 3   // base

#define INPUT_PIN0 A3  // node1
#define INPUT_PIN1 A5  // node2
#define INPUT_PIN2 A4  // node3
#define INPUT_CLAW A0  // claw
#define INPUT_BASE A2  // base

#define BUTTON_PIN A1 // Button

Servo servo0;
Servo servo1;
Servo servo2;
Servo servo_base;
Servo servo_claw;

unsigned long long serial_timer = 0;
unsigned long long auto_timer = 0;
unsigned long long input_timer = 0;
unsigned long long doubleclick_timer = 0;

short rotate0, rotate1, rotate2, rotate_base, rotate_claw;  
short angle0, angle1, angle2, angle_base, angle_claw;  

#define positions_limit 30
short positions0[positions_limit] = {};
short positions1[positions_limit] = {};
short positions2[positions_limit] = {};
short positions_base[positions_limit] = {};
short positions_claw[positions_limit] = {};
short positions_counter = 0;
short positions_counter_auto = 0;
short auto_counter = 0;

bool button = 0;
bool button_flag = 0;
short clicks = 0;

bool manual_control = 1;


void manual_state() {

  button = !digitalRead(BUTTON_PIN);

  if (millis() - input_timer >= 10) {
    input_timer = millis();
    
    rotate0 = analogRead(INPUT_PIN0);
    rotate1 = analogRead(INPUT_PIN1);
    rotate2 = analogRead(INPUT_PIN2);
    rotate_base = analogRead(INPUT_BASE);
    rotate_claw = analogRead(INPUT_CLAW);


    angle0 = process_value(rotate0, 230);
    angle1 = process_value(rotate1, 230);
    angle2 = process_value(rotate2, 230);
    angle_base = process_value(rotate_base, 230);
    angle_claw = process_value(rotate_claw, 230);

    // avoid too hard clamp
    angle_claw = map(angle_claw, 0, 180, 15, (180-15));

    servo0.write(angle0);
    servo1.write(angle1);
    servo2.write(angle2);
    servo_base.write(angle_base);
    servo_claw.write(angle_claw);


    button_press();

  }

}

void auto_state() {
  button = !digitalRead(BUTTON_PIN);
  if (millis() - input_timer >= 10) {
    input_timer = millis();
    button_press();
  }

  if (millis() - auto_timer > 10) {
    auto_timer = millis();

    const short auto_counter_limit = 150;
    if (auto_counter > auto_counter_limit) { 
      auto_counter = 0; 
      ++positions_counter_auto;
      if (positions_counter_auto > (positions_counter - 1)) { positions_counter_auto = 0; }
    }

    
    short i = positions_counter_auto;
    short j = positions_counter_auto + 1;
    if (i == positions_counter - 1) { j = 0; }
    
    angle0 = positions0[i] + ((positions0[j] - positions0[i]) * auto_counter) / auto_counter_limit;
    angle1 = positions1[i] + ((positions1[j] - positions1[i]) * auto_counter) / auto_counter_limit;
    angle2 = positions2[i] + ((positions2[j] - positions2[i]) * auto_counter) / auto_counter_limit;
    angle_base = positions_base[i] + ((positions_base[j] - positions_base[i]) * auto_counter) / auto_counter_limit;
    angle_claw = positions_claw[i] + ((positions_claw[j] - positions_claw[i]) * auto_counter) / auto_counter_limit;
    
    ++auto_counter;

    servo0.write(angle0);
    servo1.write(angle1);
    servo2.write(angle2);
    servo_base.write(angle_base);
    servo_claw.write(angle_claw);

  }
  

  if (manual_control) {
    positions_counter = 0;
  }
}


void button_press() {

  if (millis() - doubleclick_timer > 750) {  
    doubleclick_timer = millis();
    clicks = 0;
  }

  if (button == 1 && button_flag == 0) {
      ++clicks;
      button_flag = 1;

      manual_control = !doubleclick();
      
      if (!manual_control) {
        return;
      } else if (positions_counter < positions_limit) {
        positions0[positions_counter] = angle0; 
        positions1[positions_counter] = angle1; 
        positions2[positions_counter] = angle2; 
        positions_base[positions_counter] = angle_base; 
        positions_claw[positions_counter] = angle_claw; 
        ++positions_counter;
      }
    }
    else if (button == 0 && button_flag == 1) {
      button_flag = 0;
    }
    return;
}

bool doubleclick() {  
  if (clicks > 1) {
    return true;
  }
  return false;
}


int process_value(int val, int lowest) {

  int highest = 1023 - lowest;
  val = map(val, 0, 1023, 1023, 0);
  if (val < lowest) {
    val =  lowest;
  } else if (val > highest) {
    val = highest;
  }

  return map(val, lowest, highest, 0, 180);

}


/*----------------------------------------------------------*/

void setup() {
  // put your setup code here, to run once:
  servo0.attach(SERVO_PIN0);
  servo1.attach(SERVO_PIN1);
  servo2.attach(SERVO_PIN2);
  servo_base.attach(SERVO_BASE);
  servo_claw.attach(SERVO_CLAW);

  pinMode(INPUT_PIN0, INPUT);
  pinMode(INPUT_PIN1, INPUT);
  pinMode(INPUT_PIN2, INPUT);
  pinMode(INPUT_BASE, INPUT);
  pinMode(INPUT_CLAW, INPUT);

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  Serial.begin(115200);
}

void loop() {
  // CHOSE STATEMENT
  // manual_control = choose_statement();
  if (manual_control) {
    manual_state();
  } else {
    auto_state();
  }


  // DEBUG
  if (millis() - serial_timer >= 100) {
    serial_timer = millis();

    Serial.print(positions_counter);
    Serial.print(" - positions - ");
    Serial.print(angle0);
    Serial.print(" - ");
    Serial.print(angle1);
    Serial.print(" - ");
    Serial.print(angle2);
    Serial.print(" - ");
    Serial.print(angle_base);
    Serial.print(" - ");
    Serial.print(angle_claw);
    
    Serial.print(" - manual_control - ");
    Serial.print(manual_control);
    
    Serial.print(" - clicks - ");
    Serial.print(clicks);
    

    Serial.println();
  }

}
