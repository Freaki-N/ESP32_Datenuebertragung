//created by Freaki_N
//Last Update: 14.12.2021


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
    if((!dataSend[i].hasData()) && (!added)){
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
