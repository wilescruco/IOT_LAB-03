#include <ESP32Servo.h>
#include <ESP32_MailClient.h>
#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"    // Libreria del sensor DHT11

SMTPData datosSMTP;
#define DHTPIN 13                // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11           // Definimos el tipo de sensor (DHT 11)
DHT Sensor11(DHTPIN, DHTTYPE);  // Se crea el objeto sensor11 (Sensor DHT11)
Servo motor;
#define servopin 27
int Led_puerta =12;
int Led_altaTemp =14;



//**************************************
//*********** MQTT CONFIG **************
//**************************************
const char *mqtt_server = "node02.myqtthub.com";
const int mqtt_port = 1883;
const char *mqtt_user = "esp32";
const char *mqtt_pass = "esp32";
const char *root_topic_subscribe1 = "Temperatura/esp32";
const char *root_topic_subscribe2 = "Puerta/esp32";
const char *root_topic_publish = "Topic_raiz/public_esp32";
const char *root_topic_publish1 = "Temperatura/public_esp32";
const char *root_topic_publish2 = "Humedad/public_esp32";
const char *root_topic_publish3 = "Puerta/public_esp32";


//**************************************
//*********** WIFICONFIG ***************
//**************************************
const char* ssid = "WILESCRUCO";
const char* password =  "Wilson-2021";



//**************************************
//*********** GLOBALES   ***************
//**************************************
WiFiClient espClient;
PubSubClient client(espClient);
char msg[75];
char msg1[25];
char msg2[25];
char msg3[25];
long count1=0;
long set=30;
String puerta ="Cerrar";
String estado_puerta ="cerrada";
String tema="";
float temp;


//************************
//** F U N C I O N E S ***
//************************
void callback(char* topic, byte* payload, unsigned int length);
void reconnect();
void setup_wifi();

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  Sensor11.begin();
  pinMode(Led_puerta, OUTPUT);
  pinMode(Led_altaTemp, OUTPUT);
  motor.attach(servopin);
  digitalWrite(Led_puerta, LOW);
  digitalWrite(Led_altaTemp, LOW);  
  motor.write(0);
}

void loop() {
  
  if (!client.connected()) {
    reconnect();
  }
  if (client.connected()){
    
    float hum = Sensor11.readHumidity();      //Definimos la variable "hum" de tipo flotante que almacena la lectura del dato humedad del sensor11
    float temp = Sensor11.readTemperature();  //Definimos la variable "temp" de tipo flotante que almacena la lectura del dato temperatura (en grados celsius) del sensor11
    
    if (isnan(hum) || isnan(temp)) {
    Serial.println(F("Sistema Control de temperatura apagado")); // Imprime ese comentario cuando no lee ningun dato del sensor11
    //return;
    }
    if (temp > set) {
      digitalWrite(Led_altaTemp, HIGH); // Cambiar estado del pin
      Serial.println(""); 
      Serial.println("ALTA TEMPERATURA!!!");            
    }else {
      digitalWrite(Led_altaTemp, LOW); // Cambiar estado del pin   
    }

    if (puerta=="Cerrar"){
      estado_puerta="cerrada";
      digitalWrite(Led_puerta, LOW); // Cambiar estado del pin
    }else if (puerta=="Abrir"){
      estado_puerta="abierta";
      digitalWrite(Led_puerta, HIGH); // Cambiar estado del pin          
    }
       
    String str  ="Temp (??C): " + String(temp) + " / Hum (%): " + String(hum) + " / Puerta: " + (estado_puerta);
    String str1 ="Temp (??C): " + String(temp);
    String str2 ="Hum (%): " + String(hum);
    String str3 ="Puerta: " + (estado_puerta);
    str.toCharArray(msg,75);
    str1.toCharArray(msg1,25);
    str2.toCharArray(msg2,25);
    str3.toCharArray(msg3,25);
    client.publish(root_topic_publish,msg);
    client.publish(root_topic_publish1,msg1);
    client.publish(root_topic_publish2,msg2);
    client.publish(root_topic_publish3,msg3);
    count1++;
    Serial.println("");
    Serial.println("(" + String (count1) + ") PUBLICANDO A: ");
    Serial.println(" Topic_raiz/public_esp32: " + String (msg));
    Serial.println("Temperatura/public_esp32: " + String (msg1));
    Serial.println("    Humedad/public_esp32: " + String(msg2));
    Serial.println("     Puerta/public_esp32: " + String(msg3));
    delay(12000);
  }
  client.loop();
}



//*****************************
//***    CONEXION WIFI      ***
//*****************************
void setup_wifi(){
  delay(5000);
  // Nos conectamos a nuestra red Wifi
  Serial.println();
  Serial.print("Conectando a ssid: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Conectado a red WiFi!");
  Serial.println("Direcci??n IP: ");
  Serial.println(WiFi.localIP());
}



//*****************************
//***    CONEXION MQTT      ***
//*****************************

void reconnect() {

  while (!client.connected()) {
    Serial.println("");
    Serial.print("Intentando conexi??n Broker...");
    // Creamos un cliente ID
    String clientId = "Micro_Esp32";
    
    // Intentamos conectar
    if (client.connect(clientId.c_str(),mqtt_user,mqtt_pass)) {
      Serial.println("Conectado al broker!");
      // Nos suscribimos
      if(client.subscribe(root_topic_subscribe1)){
        Serial.println("Suscripcion a topic "+ String(root_topic_subscribe1));
      }else{
        Serial.println("fallo Suscripcii??n a topic "+ String(root_topic_subscribe1));
      }
      if(client.subscribe(root_topic_subscribe2)){
        Serial.println("Suscripcion a topic "+ String(root_topic_subscribe2));
        Serial.println("");
      }else{
        Serial.println("fallo Suscripcii??n a topic "+ String(root_topic_subscribe2));
      }
    } else {
      Serial.print("fall?? conexi??n broker:( con error -> ");
      Serial.print(client.state());
      Serial.println(" Intentamos de nuevo en 5 segundos");
      delay(5000);
    }
  }
}



//*****************************
//***       CALLBACK        ***
//*****************************

void callback(char* topic, byte* payload, unsigned int length){
  String incoming = "";
  Serial.println("");
  for (int i = 0; i < length; i++) {
    incoming += (char)payload[i];
  }
  incoming.trim();
  tema=topic;
    
  if (tema=="Puerta/esp32"){
    if (incoming=="Cerrar"){
    puerta=incoming;
    Serial.println("");
    Serial.println("=====");
    Serial.println("Mensaje recibido de: Puerta/esp32: " + incoming);
    Serial.println("=====");
    CerrarPuerta();
    }else if (incoming=="Abrir"){
      puerta=incoming;
      Serial.println("");
      Serial.println("=====");
      Serial.println("Mensaje recibido de: Puerta/esp32: " + incoming);
      Serial.println("=====");
      AbrirPuerta();
      datosSMTP.setLogin("smtp.gmail.com",465,"electivaiotunisangil@gmail.com","ElectivaIOT20");
      datosSMTP.setSender("Micro_Esp32","electivaiotunisangil@gmail.com");
      datosSMTP.setPriority("High");
      datosSMTP.setSubject("Informaci??n del estado de la puerta");
      datosSMTP.setMessage("LA PUERTA SE ABRIO!!!",false);
      datosSMTP.addRecipient("electivaiotunisangil@gmail.com");
      if (!MailClient.sendMail(datosSMTP)){
        Serial.println("Error enciando correo, " +  MailClient.smtpErrorReason());
        }
      datosSMTP.empty();
    }
  } 
  
  if (tema=="Temperatura/esp32"){
    temp= Sensor11.readTemperature();
    set=incoming.toInt();
    Serial.println("");
    Serial.println("=======");
    Serial.println("Mensaje recibido de:");
    Serial.println("Temperatura/esp32 / " + incoming);
    Serial.println("Setting de temperatura: " + String (set) + " y  temp actual: " + String (temp) );
    Serial.println("======"); 
     
    if (temp>set){
      datosSMTP.setLogin("smtp.gmail.com",465,"electivaiotunisangil@gmail.com","ElectivaIOT20");
      datosSMTP.setSender("Micro_Esp32","electivaiotunisangil@gmail.com");
      datosSMTP.setPriority("High");
      datosSMTP.setSubject("Informaci??n del control de temperatura");
      datosSMTP.setMessage("ALTA TEMPERATURA!!!",false);
      datosSMTP.addRecipient("electivaiotunisangil@gmail.com");
      Serial.println("S E   E N V I ??  C O R R E O");
      Serial.println("======");
      if (!MailClient.sendMail(datosSMTP)){
        Serial.println("Error enciando correo, " +  MailClient.smtpErrorReason());
      }
    datosSMTP.empty();
    }
  }
}



//*****************************
//***    ABRIR PUERTA      ***
//*****************************

void AbrirPuerta(){
  motor.write(20);
  delay(500);
   motor.write(40);
  delay(500);
  motor.write(60);
  delay(500);
  motor.write(80);
  delay(500);
  motor.write(100);
  delay(500);

}



//*****************************
//***    CERRAR PUERTA      ***
//*****************************

void CerrarPuerta(){
  motor.write(100);
  delay(500);
  motor.write(80);
  delay(500);
  motor.write(60);
  delay(500);
  motor.write(40);
  delay(500);
  motor.write(20);
  delay(500);
  motor.write(0);
  delay(500);
}
