/*
* This is the code that runs the -controller- for Adam
* it runs on an arduino nano
* we have 4 buttons, with LEDs built-in
* and a vibration motor we attached to the inside of the controller
* this code needs to handle all four buttons and send UART data to the cart
*/

#include <Arduino.h>

// Each button has one pin for us to check the button state
// and a nother lead for us to control the LED
#define BUTTON_1 2
#define BUTTON_2 3
#define BUTTON_3 4
#define BUTTON_4 5
#define BUTTON_LIGHT_1 6
#define BUTTON_LIGHT_2 7
#define BUTTON_LIGHT_3 8
#define BUTTON_LIGHT_4 9

// Motor driver uses pin A to drive direction (low - high -> reverse - forward)
// and pin B for the speed (0-5v -> 0-max speed)
#define MOTOR1_CHANNEL_A A2
#define MOTOR1_CHANNEL_B A3

void handle_buttons();
void vibrate_motor(int ms);
bool isPressed(int button_gpio_num);
void send_uart_message(char message[6]);

void setup(){
  //configure motor
  // the motor uses one pin for speed and the other pin for direction
  pinMode(MOTOR1_CHANNEL_A, OUTPUT);
  pinMode(MOTOR1_CHANNEL_B, OUTPUT);
  
  // configure buttons
  pinMode(BUTTON_1, INPUT_PULLUP); //button 1
  pinMode(BUTTON_2, INPUT_PULLUP); //button 2
  pinMode(BUTTON_3, INPUT_PULLUP); //button 3
  pinMode(BUTTON_4, INPUT_PULLUP); //button 4
  pinMode(BUTTON_LIGHT_1, OUTPUT);  // light 1
  pinMode(BUTTON_LIGHT_2, OUTPUT);  // light 2
  pinMode(BUTTON_LIGHT_3, OUTPUT);  // light 3
  pinMode(BUTTON_LIGHT_4, OUTPUT);  // light 4

  // may not be needed, but set all the button lights to low 
  digitalWrite(BUTTON_LIGHT_1,LOW);  
  digitalWrite(BUTTON_LIGHT_2,LOW);
  digitalWrite(BUTTON_LIGHT_3,LOW);
  digitalWrite(BUTTON_LIGHT_4,LOW);

  // UART coms
  // Arduino nano only has one serial channel
  // it is also used for programming, so make 
  // sure things are unplugged if you can't flash the board
  Serial.begin(9600);  /* 9600 baudrate ~ 960ish chars per sec */
}

// very simple main loop!  just check the buttons 
// and give the processor some time to chill
void loop(){
  handle_buttons();
  delay(5);
}

// This function is important for 
void handle_buttons(){
  // light up led's if buttons are pressed
  digitalWrite(BUTTON_LIGHT_1, !digitalRead(BUTTON_1));
  digitalWrite(BUTTON_LIGHT_2, !digitalRead(BUTTON_2));
  digitalWrite(BUTTON_LIGHT_3, !digitalRead(BUTTON_3));
  digitalWrite(BUTTON_LIGHT_4, !digitalRead(BUTTON_4));
  

  /*
  *  Button report string to send over UART to the cart
  *  The button report is 6 chars long:
  *     1 char per button (red, blue , yellow, green)
  *     1 char end-of-message character
  *     1 invisible char signifying end of string
  */
  char button_report[6]; 
  String s = ""; // we will add each button status to this

  /* this is not the most robust structure or method... 
  *  but we need a functional prototype 
  *  this way, multiple buttons overwrite each other (wouldn't be optimal)
  *  idealy we would use a more robust message packing/structure
  */
  if (isPressed(BUTTON_1)){
    s += 'g';  /* green button pressed */
    vibrate_motor(10);
  } else {s += 'n';}
  if (isPressed(BUTTON_2)){
    s += 'r';  /* red button pressed */
    vibrate_motor(10);
  } else {s += 'n';}
  if (isPressed(BUTTON_3)){
    s += 'y';  /* yellow button pressed */
    vibrate_motor(10);
  } else {s += 'n';}
  if (isPressed(BUTTON_4)){
    s += 'b';  /* blue button pressed */
    vibrate_motor(10);
  } else {s += 'n';}
  s += 'k';    /* end of uart message */
  s.toCharArray(button_report, 6);
  send_uart_message(button_report);
}

void send_uart_message(char message[6]){
  // send this to the cart microcontroller
  Serial.println(message);
}


// This function pulses the vibration motor
// you can use any motor driver you want, but this one 
void vibrate_motor(int ms){
  // turn motors on
  digitalWrite(MOTOR1_CHANNEL_A, HIGH);
  digitalWrite(MOTOR1_CHANNEL_B, HIGH);

  // making this longer leaves the motor running for longer
  delay(ms);

  // turn motors off
  digitalWrite(MOTOR1_CHANNEL_A, LOW);
  digitalWrite(MOTOR1_CHANNEL_B, LOW);
}

// checks if the buttons do in fact be pressed
bool isPressed(int button_gpio_num){
  if (digitalRead(button_gpio_num) == LOW){
    return true;
  }
  return false;
}

