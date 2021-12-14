
void send(String text){
  
}

void setup(){
  
}

void loop(){
  if(!(digitalRead(BUTTON_PIN_1) == LOW) && (digitalRead(BUTTON_PIN_1) == LOW) != Button_1_state){
    Button_1_toggle = !Button_1_toggle;
  }
  Button_1_state = (digitalRead(BUTTON_PIN_1) == LOW);
}
