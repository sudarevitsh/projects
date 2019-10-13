#include <ESP8266WiFi.h> 

//parametri pristupne tačke
  const char* ssid = "Zavrsni_Rad";              //naziv mreže
  const char* password = "12345678";             //lozinka

//parametri webservera
  const byte port = 80;                          //broj porta
  IPAddress host (8,8,8,8);                      //ip adresa webservera
  String host_str = "8.8.8.8";                   //ista ip samo u obliku znakovnog niza
  String route = "/client2/";                    //ruta po kojoj klijent ostvaruje komunikaciju sa serverom

//definisanje wifi klijenta.
  WiFiClient client;                             //inicijalizacija klijenta
  byte id = 2;                               //identifikacioni broj koji server zahtjva od klijenta za komunikaciju
  
//promjenljive u koje se unose rezultati senzorskih mjerenja
  float temp;                                    //temperatura
  float humi;                                    //vlažnost vazduha
  float moist;                                   //vlažnost zemljišta

//promjenljive kojima se reguliše vremenski interval slanja zahtjeva na server
  unsigned long req_timer = (10*1000);           //koliko često će se slati zahtjev i podaci.
  int interval_counter = 1;                      //brojač proteklih intervala, odgovara i broju poslanih zahtjeva plus 1
  unsigned long time_counter;                    //brojač vremena, ispisuje rezultat funkcije millis()

  const int reg_temperature = 14;                //GPIO14 = D5, pin regulatora temperature
  const int reg_humidity = 12;                   //GPIO12 = D6, pin regulatora vlažnosti vazduha
  const int reg_moisture = 13;                   //GPIO13 = D7, pin regulatora vlažnosti zemljišta

//-----------------------------------------------------------------------------------------------------------------------

void setup(){
//definisanje izlaza
    pinMode(LED_BUILTIN, OUTPUT);         
    pinMode(reg_temperature, OUTPUT);
    pinMode(reg_humidity, OUTPUT);
    pinMode(reg_moisture, OUTPUT);
 
//podešavanje klijenta (stanice) i povezivanje na pristupnu tačku
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
  
//rutina blinkanja ugrađene lampice dok se klijent ne poveže sa serverom, prestaje nakon povezivanja  
    while(WiFi.status() != WL_CONNECTED){
      digitalWrite(LED_BUILTIN, LOW);
      delay(200);
      digitalWrite(LED_BUILTIN, HIGH);
      delay(200);
    }  
}

//-----------------------------------------------------------------------------------------------------------------------

void loop(){

//praćenje vremena rada klijenta, na osnovu njega se određuje vremenski interval slanja zahtjeva na server
    time_counter = millis();

//dodjela vrijednosti mjerenim veličinama
    temp = random(10,50);
    humi = random(10,80);
    moist = random(5,90);

//regulisanje vremenskog intervala zahtjeva, objasniti dodatno...
    if(time_counter > (req_timer * interval_counter)){
      interval_counter += 1;  
    
  //povezivanje klijenta sa serverom, a zatim ormiranje i slanje zahtjeva u kom se nalaze podaci očitavanja senzora 
      client.connect(host_str, port); 
    
  //način slanja podataka: ruta + id + temperatura + vlažnost vazduha + vlažnost zemljišta
      String request = String(route + "?client_id=" + String(id) + "&temperature=" + String(temp) + "&humidity=" + String(humi) + "&moist=" + String(moist));
 
  //standardni način slanja HTTP zahtjeva na server  
      client.print(String("GET " + request + " HTTP/1.1\r\n" + "Host: " + host_str + "\r\n" + "Connection: close\r\n\r\n"));
      client.stop();                             //prekid veze
   }
  
//pokretanje regulatora temperature na osnovu neke željene vrijednosti temperature  
    if (temp < 5){
      digitalWrite(reg_temperature, HIGH);       //slanje upravljačkog signala na definisani pin
    }
    else if (temp >= 5){
      digitalWrite(reg_temperature, LOW);
    }

//pokretanje regulatora vlažnosti vazduha na osnovu zadane vrijednosti vlažnosti vazduha
    if (humi < 50){
      digitalWrite(reg_humidity, HIGH);          //slanje upravljačkog signala na definisani pin
    }
    else if (humi >= 50){
      digitalWrite(reg_temperature, LOW);
    }
  
//pokretanje regulatora vlažnosti zemljišta na osnovu zadane vrijednosti vlažnosti zemljišta
    if (moist < 40){
      digitalWrite(reg_moisture, HIGH);          //slanje upravljačkog signala na definisani pin  
    }
    else if (temp >= 40){
      digitalWrite(reg_temperature, LOW);
    }
}