//created by Freaki_N
//Last Update: 13.12.2021

#include "protocol_classes.cpp"

const int PROTOCOL_INPUT_PIN = 25;
const int PROTOCOL_OUTPUT_PIN = 27;

const int device_id = 1;

const int LED_PIN_1 = 21;
const int BUTTON_PIN_1 = 22;

boolean Button_1_state = false;
boolean Button_1_toggle = false;

void readData(int _sender, int _packet){
  if(_packet == 2){
    digitalWrite(LED_PIN_1, HIGH);
  }else if(_packet == 3){
    digitalWrite(LED_PIN_1, LOW);
  }
}

void loop2(){
  if((digitalRead(BUTTON_PIN_1) == LOW) && (digitalRead(BUTTON_PIN_1) == LOW) != Button_1_state){
    Button_1_toggle = !Button_1_toggle;
    if (Button_1_toggle) {
      dataToSend.add(Datagramm(device_id, 0, 2));
    }else{
      dataToSend.add(Datagramm(device_id, 0, 3));
    }
  }

  if(!(digitalRead(BUTTON_PIN_1) == LOW) && (digitalRead(BUTTON_PIN_1) == LOW) != Button_1_state){

  }
  Button_1_state = (digitalRead(BUTTON_PIN_1) == LOW);
}
