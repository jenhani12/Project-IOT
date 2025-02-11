#include <stm32l475e_iot01.h> 
#include <stm32l475e_iot01_accelero.h> 
#include <stm32l475e_iot01_gyro.h> 
#include <stm32l475e_iot01_hsensor.h> 
#include <stm32l475e_iot01_magneto.h> 
#include <stm32l475e_iot01_psensor.h> 
#include <stm32l475e_iot01_qspi.h> 
#include <stm32l475e_iot01_tsensor.h> 
#include <Bridge.h>
#include <BridgeHttpClient.h>
#include <UrlEncode.h>
#include <SPI.h> 
#include <WiFiST.h> 
#include <WiFiUdpST.h> 
#include <MQTTClient.h> 
SPIClass SPI_3(PC12, PC11, PC10); 
WiFiClass WiFi (&SPI_3, PE0, PE1, PE8, PB13);
int status = WL_IDLE_STATUS;
WiFiClient net;
MQTTClient client;
String string_MQTT;
unsigned int localPort = 8002;           // local port to listen on
char packetBuffer[255];                  //buffer to hold incoming packet
char  ReplyBuffer[] = "acknowledged";    // a string to send back
WiFiUDP Udp;
unsigned long lastMillis = 0;
float sensor_value_T = 0;
float sensor_value_H = 0;
float sensor_value_P = 0;
float sensor_value_X = 0;
float sensor_value_Y = 0;
float sensor_value_Z = 0;
int16_t pDataXYZ[3] = {0};
float pGyroDataXYZ[3] = {0};
int16_t topic = 0;
//char ssid[] = "ORANGE_17C9";
char ssid[] = "zied";
//char pass[] = "BGAYZQ8C";  *
char pass[] = "12345678";
String phoneNumber = "+21693396103";
String apiKey = "1069436";
void sendMessage(String message){

  // Data to send with HTTP POST
  String url = "https://api.callmebot.com/whatsapp.php?phone=" + phoneNumber + "&apikey=" + apiKey + "&text=" + urlEncode(message);    
  BridgeHttpClient client;
  Bridge.begin();
  client.begin(url);

  // Specify content-type header
  client.addHeader("Content-Type: application/x-www-form-urlencoded");
  client.enableInsecure();
  
  // Send HTTP POST request
  
  client.post("https://api.callmebot.com?", "{\"&phone=\":\"+21693396103\",\"&apikey=\":\"1069436\",\"&text=\":\"urlEncode(message)\"}" );
  //"{\"&phone=\":\""phoneNumber"\",\"&text=\":\""urlEncode(message)"\"}"
  /*if (httpResponseCode == 200){
    Serial.print("Message sent successfully");
  }
  else{
    Serial.println("Error sending the message");
    Serial.print("HTTP response code: ");
    Serial.println(httpResponseCode);
  }*/

  // Free resources
  //client.end();
  Bridge.end()
  }
void setup() {
  BSP_TSENSOR_Init();
  BSP_HSENSOR_Init();
  BSP_PSENSOR_Init();
  BSP_MAGNETO_Init();
  BSP_GYRO_Init();
  BSP_ACCELERO_Init();
  Serial.begin(115200);
  Serial.println("OK");
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi module not present");
    // don't continue:
    while (true);
  }

  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(500);
  }
  Serial.println("Connected to wifi");
  printWifiStatus();
  Serial.println("\nStarting connection to server...");
  Serial.println("Received packet From ");
  client.begin("192.168.74.233", net);
  client.onMessage(messageReceived);
  Serial.print("\nconnecting...");   // Mot de passe: always; user: khaled
  while (!client.connect("STM32-B-L475-IOT01A2")) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("\n connected!");

  client.subscribe("/Reponse_Serveur");  // Ceci est un essai de souscription au topic /hello
  // client.unsubscribe("/hello");

}

void loop() {
 
 /* sensor_value_T = BSP_TSENSOR_ReadTemp(); 
 Serial.print("\n TEMPERATURE = "); 
 Serial.print(sensor_value_T); 
 string_MQTT = String(sensor_value_T, 1); 
 client.publish("/temperature", string_MQTT);*/ 
  
  

  if (!client.connected()) {
    client.disconnect();


    // wait 10 seconds for connection:

    client.begin("192.168.109.234", net);
    client.connect("STM32-B-L475-IOT01A2");
    delay(1000);
    string_MQTT =  String(sensor_value_H, 1);
    client.publish("/test", "Hello");
    topic = -1;

  }

  // publish a message roughly every second.
 
 if (millis() - lastMillis > 1000) {
    topic++;
    Serial.print("\n *************TEMPERATURE*************** ");
    sensor_value_T = BSP_TSENSOR_ReadTemp();
    Serial.print("\n TEMPERATURE = ");
    Serial.print(sensor_value_T);
    Serial.print("\n *************************************** ");
    Serial.print("\n *************HUMIDITY****************** ");
    sensor_value_H = BSP_HSENSOR_ReadHumidity();
    Serial.print("\n Humidite = ");
    Serial.print(sensor_value_H);
    Serial.print("\n *************************************** ");
    Serial.print("\n *************PRESSURE****************** ");
    sensor_value_P = BSP_PSENSOR_ReadPressure();
    Serial.print("\n Pression = ");
    Serial.print(sensor_value_P);
    Serial.print("\n *************************************** ");
    Serial.print("\n *************MAGNET******************** ");
    BSP_MAGNETO_GetXYZ(pDataXYZ);
    Serial.print("\n MAGNETO_X........... = ");
    Serial.print(pDataXYZ[0]);
    Serial.print("\n MAGNETO_Y........... = ");
    Serial.print(pDataXYZ[1]);
    Serial.print("\n MAGNETO_Z........... = ");
    Serial.print(pDataXYZ[2]);
    Serial.print("\n *************************************** ");
    Serial.print("\n *************GYRO********************** ");
    BSP_GYRO_GetXYZ(pGyroDataXYZ);
    Serial.print("\n GYRO_X ............. = ");
    Serial.print(pGyroDataXYZ[0]);
    Serial.print("\n GYRO_Y ............. = ");
    Serial.print(pGyroDataXYZ[1]);
    Serial.print("\n GYRO_Z ............. = ");
    Serial.print(pGyroDataXYZ[2]);
    Serial.print("\n ************************************** ");
    Serial.print("\n *************ACCELERATION************** ");
    BSP_ACCELERO_AccGetXYZ(pDataXYZ);
    sensor_value_X = pDataXYZ[0];
    sensor_value_Y = pDataXYZ[1];
    sensor_value_Z = pDataXYZ[2];
    Serial.print("\n ACCELERO_X ......... = ");
    Serial.print(pDataXYZ[0]);
    Serial.print("\n ACCELERO_Y ......... = ");
    Serial.print(pDataXYZ[1]);
    Serial.print("\n ACCELERO_Z ......... = ");
    Serial.print(pDataXYZ[2]);
    lastMillis = millis();
    if (topic == 1) {
      Serial.println("Ok ****************");
      string_MQTT =  String(sensor_value_T, 1);
      client.publish("/temperature", string_MQTT);
    }
    if (topic == 2) {
      string_MQTT =  String(sensor_value_H, 1);
      client.publish("/Humidite", string_MQTT);
    }
    if (topic == 3) {
      string_MQTT =  String(sensor_value_P, 1);
      client.publish("/Pression", string_MQTT);
    }
    if (topic == 4) {
      string_MQTT =  String(pGyroDataXYZ[0], 1);
      client.publish("/GyroX", string_MQTT);
    }
    if (topic == 5) {
      string_MQTT =  String(pGyroDataXYZ[1], 1);
      client.publish("/GyroY", string_MQTT);
    }
    if (topic == 6) {
      string_MQTT =  String(pGyroDataXYZ[2], 1);
      client.publish("/GyroZ", string_MQTT);
      
    }
    if (topic == 7) {
      string_MQTT =  String(sensor_value_X, 1);
      client.publish("/ACCELEROX", string_MQTT);
    }
    if (topic == 8) {
      string_MQTT =  String(sensor_value_Y, 1);
      client.publish("/ACCELEROY", string_MQTT);
    }
    if (topic == 9) {
      string_MQTT =  String(sensor_value_Z, 1);
      client.publish("/ACCELEROZ", string_MQTT);
      topic = 0;
    }
  }
    while ( sensor_value_T > 27){
    
    sendMessage("Attention Temperature =");
    sendMessage(String(sensor_value_T));
    break;  
  }
  while ( sensor_value_H > 60){
    sendMessage("Attention Humidité =");
    sendMessage(String(sensor_value_H));
    break;
  }
  }
void messageReceived(String &topic, String &payload) {
  Serial.print("\n -------------------------------------- ");
  Serial.println("\n\n incoming: " + topic + " - " + payload);
  Serial.print("\n\n -------------------------------------- ");
}
void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi device's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
