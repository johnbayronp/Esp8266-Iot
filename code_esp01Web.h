#include <SoftwareSerial.h>
SoftwareSerial johanWifi(3,2); 
void setup(){
  
 Serial.begin(9600);  // monitor serial del arduino
 johanWifi.begin(9600); // baud rate del ESP8255
 
 pinMode(13,OUTPUT);
 digitalWrite(13,LOW);
 
 sendData("AT+RST\r\n",2000);      // resetear módulo
 sendData("AT+CWMODE=3\r\n",1000); // configurar como cliente
 sendData("AT+CWJAP=\"FLIA CENTENO\",\"S1elultimoreino\"\r\n",8000); //SSID y contraseña para unirse a red 
 sendData("AT+CIFSR\r\n",1000);    // obtener dirección IP
 sendData("AT+CIPMUX=1\r\n",1000); // configurar para multiples conexiones
 sendData("AT+CIPSERVER=1,80\r\n",1000);         // servidor en el puerto 80
}

void loop(){
 if(johanWifi.available()){   // revisar si hay mensaje del ESP8266
 
  if(johanWifi.find("+IPD,")){ // revisar si el servidor recibio datos
    delay(1500); // esperar que lleguen los datos hacia el buffer
    int conexionID = johanWifi.read()-48; // obtener el ID de la conexión para poder responder
    johanWifi.find("led="); // bucar el texto "led="
 
    int state = (johanWifi.read()-48); // Obtener el estado del pin a mostrar
    digitalWrite(13, state); // Cambiar estado del pin

  while(johanWifi.available()){
    char c = johanWifi.read();
    Serial.print(c);
 }
 
 //responder y cerrar la conexión para que el navegador no se quede cargando 
 // página web a enviar
 String webpage = "";

 if (state==1) webpage += "<h1>LED = encendido!</h1><img style=\"height:40px; width:40px\" src=\"https://pngimage.net/wp-content/uploads/2018/06/red-led-png-.png\"><br><form action=\"led=0\" method=\"GET\"><input style=\"background:red;color:white\" type=\"submit\" value=\"Apagar\"> </form>";
 else { webpage += "<h1>LED = Apagado!</h1><img style=\"height:40px; width:40px\" src=\"https://www.clker.com/cliparts/q/l/T/H/l/f/red-led-2-md.png\"><br><form action=\"led=1\" method=\"GET\"><input type=\"submit\" style=\"background:green;color:white\" value=\"Encender\"> </form>";}
 
 // comando para enviar página web
 String comandoWebpage = "AT+CIPSEND=";
 comandoWebpage+=conexionID;
 comandoWebpage+=",";
 comandoWebpage+=webpage.length();
 comandoWebpage+="\r\n";
 
 sendData(comandoWebpage,1000);
 sendData(webpage,1000);
 
 // comando para terminar conexión
 String comandoCerrar = "AT+CIPCLOSE=";
 comandoCerrar+=conexionID;
 comandoCerrar+="\r\n";
 sendData(comandoCerrar,3000);
 }
 }
}

/*
Enviar comando al esp8266 y verificar la respuesta del módulo, todo esto dentro del tiempo timeout
*/
void sendData(String comando, const int timeout)
{
 long int time = millis(); // medir el tiempo actual para verificar timeout
 
 johanWifi.print(comando); // enviar el comando al ESP8266
 
 while( (time+timeout) > millis())
 {
  while(johanWifi.available()) //mientras haya datos por leer
 { 
  
 // Leer los datos disponibles
  char c = johanWifi.read(); // leer el siguiente caracter
  Serial.print(c);
 }
 } 
 return;
}
