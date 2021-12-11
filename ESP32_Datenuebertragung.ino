const int PROTOCOL_INPUT_PIN = 25;
const int PROTOCOL_OUTPUT_PIN = 27;

const int TIME_PAUSE = 5;
const int TIME_START = 15;
const int TIME_END = 20;
const int TIME_NEXT_VALUE = 10;
const int TIME_VALUE = 5;

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
    private:
      boolean checkBool(int timeBefore, int timeNext, int timeCur, int statusTime);
      int sender;
      int addr;
      int packet_length;
      int state;     
};

Datagramm::Datagramm(int _sender, int _addr, int _packet_length){
  sender = _sender;
  addr = _addr;
  packet_length = _packet_length;
  state = 3;
}  

Datagramm::Datagramm(){
  sender = 0;
  addr = 0;
  packet_length = 0;
  state = 0;
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
    next =  new DatagrammListNode(newDatagramm);
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
    Datagramm dataSend[10];   
};

DatagrammList::DatagrammList(){ 
  for(int i=1; i < sizeof(dataSend); i++){
    dataSend[i] = new Datagramm();
  }
}

void DatagrammList::add(Datagramm newDatagramm){
  boolean added = false;
  for(int i=0; i < sizeof(dataSend); i++){
    if((dataSend[i].hasData()) && (!added)){
      dataSend[i] = newDatagramm;
      added = true;
    }else if(added){
      dataSend[i] = new Datagramm(); 
    }
  }
}

Datagramm DatagrammList::getNext(){
  if(isAvailable()){
    Datagramm data = dataSend[0];
    for(int i=1; i < sizeof(dataSend); i++){
      dataSend[i-1] = dataSend[i];
    }
    return data; 
  }else{
    return new Datagramm();
  }
}

boolean DatagrammList::isAvailable(){
  return dataSend[0] != new Datagramm();
}

Datagramm receivedData = new Datagramm();
DatagrammList dataToSend = new DatagrammList();

void delayAndReceive(int duration){
  int start = millis();

  while(millis()-start < duration){
    receive()  
  }
}

void receive(){
  boolean state_input = (digitalRead(PROTOCOL_INPUT_PIN) == LOW);

  if(state_input && !dataAvailable){
    dataAvailable = true;  
    last_state = true; 
    last_state_start = millis(); 
    receivedData = new Datagramm();
  }else if((last_state != state_input)){
    int duration = millis()-last_state_start;
    if (!receivedData.receive(last_state, duration)){
      dataAvailable = false;    
      dataToSend.add(receivedData);
    }
    last_state = state_input; 
    last_state_start = millis(); 
  }else if(!state_input && millis()-last_state_start >= 40 && dataAvailable){
    receivedData.end();
    dataAvailable = false;  
    dataToSend.add(receivedData);
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

void setup() {
  pinMode(PROTOCOL_INPUT_PIN, INPUT_PULLUP);
  pinMode(PROTOCOL_OUTPUT_PIN, OUTPUT);
  for(int i=0; i < 20; i++){
    dataToSend.add(new Datagramm(3, 3, 10));   
  } 
}

void loop() {
  receive();
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
}
