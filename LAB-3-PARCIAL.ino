#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"    // Libreria del sensor DHT11


#define DHTPIN 13                // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11           // Definimos el tipo de sensor (DHT 11)
DHT Sensor11(DHTPIN, DHTTYPE);  // Se crea el objeto sensor11 (Sensor DHT11)



//**************************************
//*********** MQTT CONFIG **************
//**************************************
const char *mqtt_server = "node02.myqtthub.com";
const int mqtt_port = 1883;
const char *mqtt_user = "esp32";
const char *mqtt_pass = "esp32";
const char *root_topic_subscribe = "Temperatura/esp32";
const char *root_topic_publish = "Topic_raiz/public_esp32";
const char *root_topic_publish1 = "Temperatura/public_esp32";
const char *root_topic_publish2 = "Humedad/public_esp32";


//**************************************
//*********** WIFICONFIG ***************
//**************************************
const char* ssid = "CLARO_WIFI79";
const char* password =  "CLAROI18069";



//**************************************
//*********** GLOBALES   ***************
//**************************************
WiFiClient espClient;
PubSubClient client(espClient);
char msg[50];
char msg1[25];
char msg2[25];
long count=0;
long count1=0;
long count2=0;


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
}

void loop() {
  
  if (!client.connected()) {
    reconnect();
  }

  if (client.connected()){
    
    float hum = Sensor11.readHumidity();      //Definimos la variable "hum" de tipo flotante que almacena la lectura del dato humedad del sensor11
    float temp = Sensor11.readTemperature();  //Definimos la variable "temp" de tipo flotante que almacena la lectura del dato temperatura (en grados celsius) del sensor11
    float f = Sensor11.readTemperature(true); //Definimos la variable "f" de tipo flotante que almacena la lectura del dato temperatura (en grados fahrenheit) del sensor11

    if (isnan(hum) || isnan(temp) || isnan(f)) {
    Serial.println(F("Sistema Control de temperatura apagado")); // Imprime ese comentario cuando no lee ningun dato del sensor11
    //return;
    }
    Serial.print("Huemdad: ");   
    Serial.print(hum);    //Imprime el dato de humedad
    Serial.print(", ");    //Imprime una coma (,) para ordenar por columnas en excel
    Serial.print("Temp (°C): ");
    Serial.print(temp);   //Imprime el dato de temperatura en gados celsius
    Serial.print(", ");    //Imprime una coma (,) para ordenar por columnas en excel
    Serial.print("Temp (°F): ");
    Serial.println(f);    //Imprime el dato de temperatura en gados fahrenheit

    String str = String(count) +" => Temp (°C): " + String(temp) + " / Hum (%): " + String(hum);
    String str1 = String(count1)+" => Temp (°C): " + String(temp);
    String str2 = String(count2)+" => Hum (%): " + String(hum);
    str.toCharArray(msg,50);
    str1.toCharArray(msg1,25);
    str2.toCharArray(msg2,25);
    client.publish(root_topic_publish,msg);
    client.publish(root_topic_publish1,msg1);
    client.publish(root_topic_publish2,msg2);
    count++;
    count1++;
    count2++;
    Serial.println(msg);
    Serial.println(msg1);
    Serial.println(msg2);
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
  Serial.println("Dirección IP: ");
  Serial.println(WiFi.localIP());
}



//*****************************
//***    CONEXION MQTT      ***
//*****************************

void reconnect() {

  while (!client.connected()) {
    Serial.print("Intentando conexión Broker...");
    // Creamos un cliente ID
    String clientId = "Micro_Esp32";
    
    // Intentamos conectar
    if (client.connect(clientId.c_str(),mqtt_user,mqtt_pass)) {
      Serial.println("Conectado al broker!");
      // Nos suscribimos
      if(client.subscribe(root_topic_subscribe)){
        Serial.println("Suscripcion a topic "+ String(root_topic_subscribe));
      }else{
        Serial.println("fallo Suscripciión a topic "+ String(root_topic_subscribe));
      }
    } else {
      Serial.print("falló conexión broker:( con error -> ");
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
  Serial.print("Mensaje recibido desde -> ");
  Serial.print(topic);
  Serial.println("");
  for (int i = 0; i < length; i++) {
    incoming += (char)payload[i];
  }
  incoming.trim();
  Serial.println("Mensaje -> " + incoming);

}
