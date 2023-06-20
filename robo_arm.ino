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

unsigned long long serial_counter = 0;
unsigned long long input_counter = 0;

int rotate0, rotate1, rotate2, rotate_base, rotate_claw;  
int angle0, angle1, angle2, angle_base, angle_claw;  

int positions0[25] = {};
int positions1[25] = {};
int positions2[25] = {};
int positions_base[25] = {};
int positions_claw[25] = {};
int positions_counter = 0;

bool button = 0;
bool button_flag = 0;

bool manual_control = 1;


void manual_state() {

  button = !digitalRead(BUTTON_PIN);

  if (millis() - input_counter >= 50) {
    input_counter = millis();
    
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
  if (millis() - input_counter >= 50) {
    input_counter = millis();
    button_press();
  }
}

void button_press() {
  if (button == 1 && button_flag == 0) {
      button_flag = 1;

      manual_control = !doubleclick();
      
      if (!manual_control) {
        return;
      } else if (positions_counter < 20) {
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
  // if (input_counter - doubleclick_counter < 100) {  
  //   return true;
  // }
  return false;
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

int process_value(int val, int lowest) {

  // int lowest = 230;
  int highest = 1023 - lowest;
  val = map(val, 0, 1023, 1023, 0);
  if (val < lowest) {
    val =  lowest;
  } else if (val > highest) {
    val = highest;
  }

  return map(val, lowest, highest, 0, 180);

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
  if (millis() - serial_counter >= 100) {
    serial_counter = millis();
    // Serial.print(rotate0);
    // Serial.print(" - ");
    // Serial.print(rotate1);
    // Serial.print(" - ");
    // Serial.print(rotate2);
    // Serial.print(" - ");
    // Serial.print(rotate_base);
    // Serial.print(" - ");
    // Serial.print(rotate_claw);
    // Serial.print(" - ");
    // Serial.print(button_flag);
    // Serial.print(" - ");
    Serial.print(positions_counter);
    Serial.print(" - positions - ");
    Serial.print(positions0[0]);
    Serial.print(" - ");
    Serial.print(positions0[1]);
    Serial.print(" - ");
    Serial.print(positions0[2]);
    Serial.print(" - ");
    Serial.print(positions0[3]);
    Serial.print(" - ");
    Serial.print(positions0[4]);
    
    Serial.print(" - manual_control - ");
    Serial.print(manual_control);
    

    Serial.println();
  }

}
