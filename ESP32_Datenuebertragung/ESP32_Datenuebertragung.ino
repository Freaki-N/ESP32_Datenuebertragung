//created by Freaki_N
//Last Update: 12.12.2021

const int PROTOCOL_INPUT_PIN = 25;
const int PROTOCOL_OUTPUT_PIN = 27;
const int device_id = 1;

const int TIME_PAUSE = 5;
const int TIME_START = 15;
const int TIME_END = 20;
const int TIME_NEXT_VALUE = 10;
const int TIME_VALUE = 5;

const int MAX_DATAGRAMMS_WAITING = 10;

const int LED_PIN_1 = 21;

const int BUTTON_PIN_1 = 22;
boolean Button_1_state = false;

boolean last_state = false;
int last_state_start = 0;
boolean dataAvailable = false;

class Datagramm{
    public:
      Datagramm();
      Datagramm(int _sender, int _addr, int _packet_length);
      int getSender();
      int getPacklength();
      int getAddr();
      boolean receive(boolean state, int duration);
      boolean hasData();
      void end();
      void startSending();
      void send();
      boolean isSending();
    private:
      boolean checkBool(int timeBefore, int timeNext, int timeCur, int statusTime);
      int sender;
      int addr;
      int packet_length;
      int state;
      int state_send;
      int time_last_send;
      int time_stop_sending;
      boolean last_state_sending;
      void set_value();
      void set_next_value();
      void set_end();
      void set_start();
      void set_pause();
      boolean onSending;
};

Datagramm::Datagramm(int _sender, int _addr, int _packet_length){
  sender = _sender;
  addr = _addr;
  packet_length = _packet_length;
  state = 3;
  state_send = -1;
}

Datagramm::Datagramm(){
  sender = 0;
  addr = 0;
  packet_length = 0;
  state = 0;
  state_send = -1;
}

boolean Datagramm::isSending(){
  return onSending;
}

boolean Datagramm::hasData(){
  return !(sender == 0 && addr == 0 && packet_length == 0 && state == 0);
}

void Datagramm::end(){
  state = 3;
}

boolean Datagramm::checkBool(int timeBefore, int timeNext, int timeCur, int statusTime){
  return timeCur-((timeCur-timeBefore)/2) < statusTime && timeCur+((timeNext-timeCur)/2) > statusTime;
}

boolean Datagramm::receive(boolean state, int duration){
  if(state){
    if(state < 3){
      if(checkBool(0, TIME_NEXT_VALUE, TIME_VALUE, duration)){
        if(state == 0){
          sender++;
        }else if(state == 0){
          addr++;
        }else if(state == 0){
          packet_length++;
        }
      }else if(checkBool(TIME_VALUE, TIME_START, TIME_NEXT_VALUE, duration)){
        state++;
      }else if(checkBool(TIME_NEXT_VALUE, TIME_END, TIME_START, duration)){
        sender = 0;
        addr = 0;
        packet_length = 0;
        state = 0;
      }else if(checkBool(TIME_START, TIME_END+10, TIME_END, duration)){
        state = 3;
      }
    }
  }else if(duration > TIME_PAUSE+5){
    state = 3;
  }
  return state < 3;
}

int Datagramm::getAddr(){
  return addr;
}

int Datagramm::getSender(){
  return sender;
}

int Datagramm::getPacklength(){
  return packet_length;
}

void Datagramm::startSending(){
  state_send = 0;
  state = 3;
  last_state_sending = false;
  onSending = true;
  set_start();

  /*
   * Sending states:
   *
   * 0:
   */
}

void Datagramm::send(){
  if(millis() >= time_stop_sending){
    if(last_state_sending == true){
      set_pause();
    }

    if(last_state_sending == false){
        switch(state_send){
          case 0: if(sender > 0){
                state_send++;
                set_value();
                sender--;
             }else{
                state_send + 2;
                set_next_value();
             }
             break;

          case 1: if(sender > 0){
                set_value();
                sender--;
             }else{
                set_next_value();
                state_send++;
             }
            break;

          case 2: if(addr > 0){
                state_send++;
                set_value();
                addr--;
             }else{
                state_send + 2;
                set_next_value();
             }
             break;

          case 3: if(addr > 0){
                set_value();
                addr--;
             }else{
                set_next_value();
                state_send++;
             }
            break;

          case 4: if(packet_length > 0){
                state_send++;
                set_value();
                packet_length--;
             }else{
                state_send + 2;
                set_end();
             }
             break;

          case 5: if(packet_length > 0){
                set_value();
                packet_length--;
             }else{
                set_end();
                state_send++;
             }
            break;

          case 6: state_send++; onSending = false; break;

          default: break;
        }
    }
  }
}

void Datagramm::set_pause(){
  time_last_send = millis();
  time_stop_sending = time_last_send + TIME_PAUSE;
  last_state_sending = false;
  digitalWrite(PROTOCOL_OUTPUT_PIN, LOW);
}

void Datagramm::set_start(){
  time_last_send = millis();
  time_stop_sending = time_last_send + TIME_START;
  last_state_sending = true;
  digitalWrite(PROTOCOL_OUTPUT_PIN, HIGH);
}

void Datagramm::set_end(){
  time_last_send = millis();
  time_stop_sending = time_last_send + TIME_END;
  last_state_sending = true;
  digitalWrite(PROTOCOL_OUTPUT_PIN, HIGH);
}

void Datagramm::set_next_value(){
  time_last_send = millis();
  time_stop_sending = time_last_send + TIME_NEXT_VALUE;
  last_state_sending = true;
  digitalWrite(PROTOCOL_OUTPUT_PIN, HIGH);
}

void Datagramm::set_value(){
  time_last_send = millis();
  time_stop_sending = time_last_send + TIME_VALUE;
  last_state_sending = true;
  digitalWrite(PROTOCOL_OUTPUT_PIN, HIGH);
}


/*class DatagrammListNode{
  public:
    DatagrammListNode(Datagramm _datagramm);
    Datagramm getDatagramm();
    void add(Datagramm newDatagramm);
    DatagrammListNode deleteNext();
  private:
    DatagrammListNode next;
    Datagramm datagramm;
};

DatagrammListNode::DatagrammListNode(Datagramm _datagramm){
  datagramm = _datagramm;
}

Datagramm DatagrammListNode::getDatagramm(){
  return datagramm;
}

DatagrammListNode DatagrammListNode::deleteNext(){
  return next;
}

void DatagrammListNode::add(Datagramm newDatagramm){
  if(next == null){
    next = DatagrammListNode(newDatagramm);
  }else{
    return next.add();
  }
}*/

class DatagrammList{
  public:
    DatagrammList();
    void add(Datagramm newDatagramm);
    boolean isAvailable();
    Datagramm getNext();
  private:
    Datagramm dataSend[MAX_DATAGRAMMS_WAITING];
};

DatagrammList::DatagrammList(){
  for(int i=0; i < MAX_DATAGRAMMS_WAITING; i++){
    dataSend[i] = Datagramm();
  }
}

void DatagrammList::add(Datagramm newDatagramm){
  boolean added = false;
  for(int i=0; i < MAX_DATAGRAMMS_WAITING; i++){
    if((dataSend[i].hasData()) && (!added)){
      dataSend[i] = newDatagramm;
      added = true;
    }else if(added){
      dataSend[i] = Datagramm();
    }
  }
}

Datagramm DatagrammList::getNext(){
  if(isAvailable()){
    Datagramm data = dataSend[0];
    for(int i=1; i < MAX_DATAGRAMMS_WAITING; i++){
      dataSend[i-1] = dataSend[i];
    }
    return data;
  }else{
    return Datagramm();
  }
}

boolean DatagrammList::isAvailable(){
  return dataSend[0].hasData();
}

Datagramm receivedData = Datagramm();
Datagramm sendingData = Datagramm();
DatagrammList dataToSend = DatagrammList();

void delayAndReceive(int duration){
  int start = millis();

  while(millis()-start < duration){
    receive();
    sendData();
  }
}

void sendData(){
  if(sendingData.isSending()){
    sendingData.send();
  }else if(dataToSend.isAvailable()){
    sendingData = dataToSend.getNext();
    sendingData.startSending();
  }
}

void receive(){
  boolean state_input = (digitalRead(PROTOCOL_INPUT_PIN) == LOW);

  if(state_input && !dataAvailable){
    dataAvailable = true;
    last_state = true;
    last_state_start = millis();
    receivedData = Datagramm();
  }else if((last_state != state_input) && dataAvailable){
    int duration = millis()-last_state_start;
    if (!receivedData.receive(last_state, duration)){
      dataAvailable = false;
      processData(receivedData);
    }
    last_state = state_input;
    last_state_start = millis();
  }else if(!state_input && millis()-last_state_start >= 40 && dataAvailable){
    receivedData.end();
    dataAvailable = false;
    dataToSend.add(receivedData);
  }

  if(Serial.available() > 0){
    String input_serial = Serial.readStringUntil('\n');
    String input_text = "";

    for(int i=0; i < sizeof(input_serial); i++){
      char curChar = input_serial[i];

      if(curChar == ' '){

      }else{
        input_text = input_text + curChar;
      }
    }
  }
}

void processData(Datagramm data_input){
  if(receivedData.getAddr() == device_id){
    readData(receivedData.getSender(), receivedData.getPacklength());
  }else{
    dataToSend.add(receivedData);
  }
}

void readData(int _sender, int _packet){
  if(_packet == 1){
    digitalWrite(LED_PIN_1, HIGH);
  }else if(_packet == 2){
    digitalWrite(LED_PIN_1, LOW);
  }
}

void send_pause(){
  digitalWrite(PROTOCOL_OUTPUT_PIN, LOW);
  delayAndReceive(TIME_PAUSE);
}

void send_start(){
  digitalWrite(PROTOCOL_OUTPUT_PIN, HIGH);
  delayAndReceive(TIME_START);
}

void send_end(){
  digitalWrite(PROTOCOL_OUTPUT_PIN, HIGH);
  delayAndReceive(TIME_END);
}

void send_next_value(){
  digitalWrite(PROTOCOL_OUTPUT_PIN, HIGH);
  delayAndReceive(TIME_NEXT_VALUE);
}

void send_value(){
  digitalWrite(PROTOCOL_OUTPUT_PIN, HIGH);
  delayAndReceive(TIME_VALUE);
}

/*void sendDatagramm(){
  if(dataToSend.isAvailable()){
    Datagramm sendingData = dataToSend.getNext();
    int sender = sendingData.getSender();
    int addr = sendingData.getAddr();
    int packet_length = sendingData.getPacklength();

    send_start();
    send_pause();

    for(int i=0; i < sender; i++){
        send_value();
        send_pause();
    }

    send_pause();
    send_next_value();

    for(int i=0; i < addr; i++){
        send_value();
        send_pause();
    }

    send_pause();
    send_next_value();

    for(int i=0; i < packet_length; i++){
        send_value();
        send_pause();
    }

    send_end();
  }
}*/

void setup() {
  pinMode(PROTOCOL_INPUT_PIN, INPUT_PULLUP);
  pinMode(PROTOCOL_OUTPUT_PIN, OUTPUT);
  pinMode(BUTTON_PIN_1, INPUT_PULLUP);
  pinMode(LED_PIN_1, OUTPUT);
  for(int i=0; i < 20; i++){
    dataToSend.add(Datagramm(3, 3, 10));
  }

  Serial.begin(9600);
}

void loop() {
  receive();
  sendData();

  if((digitalRead(BUTTON_PIN_1) == LOW) && (digitalRead(BUTTON_PIN_1) == LOW) != Button_1_state){

  }
  Button_1_state = (digitalRead(BUTTON_PIN_1) == LOW);
}
