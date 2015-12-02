#include <DHT.h>
#define DHTPIN 2     // connecting DHT sensor pin to pin 2 on our Arduino
#define DHTTYPE DHT22   // we are using DHT sensor 22
DHT dht(DHTPIN, DHTTYPE);
#include <SPI.h>
#include <Ethernet.h>
#include <LiquidCrystal.h> 

LiquidCrystal lcd(3, 4, 5, 6, 7, 8); //the pins used for the lcd display
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; //mac address of ethernet shield
IPAddress ip(10,0,1,250); // ip address of ethernet shield
EthernetServer server(80); //sets the port for the ethernet shield- eg: port 80 http

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2); // change this back to 16, 2
  dht.begin();
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("Server address:"); //prints server address to serial connection
  Serial.println(Ethernet.localIP());
}


void loop() { 
  float h = dht.readHumidity();
  float t = dht.readTemperature(); 
    if (isnan(t) || isnan(h)) {
    Serial.println("Count not detect sensor");
  } else { //prints sensor output to lcd and serial monitor
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    lcd.setCursor(10, 0);
    lcd.print(t*1.8+32);
    lcd.setCursor(0, 1);
    lcd.print("Humidity: ");
    lcd.setCursor(10, 1);
    lcd.print(h);
    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.print(" %\t");
    Serial.print("Temperature: ");
    Serial.print(t*1.8+32);
    Serial.println(" *F");
  }

  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    boolean blankLine = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        if (c == '\n' && blankLine) { //initializing HTTP
   client.println("HTTP/1.1 200 OK");
   client.println("Content-Type: text/html");
   client.println("Connection: close");  // the connection will be closed after completion of the response
   client.println("Refresh: 5");  // refresh the page automatically every 5 sec
   client.println();
   client.println("<!DOCTYPE HTML>");
   client.println("<html>");
     client.println("<center>");
     client.println("<H1>");
     client.println("Current Readings:");
     client.println("</H1>");
       client.println("<H2>");
         client.println("Temperature: ");
       client.println("</H2>");
       client.println("<H1>");
         client.print("[");
         client.print(t*1.8+32);
         client.println("] &#176;");
         client.println("F");
       client.println("</H1>");
       client.println("<br />");
       client.println("<H2>");
         client.println("Humidity: ");
       client.println("</H2>");
       client.println("<p />");
       client.println("<H1>");
         client.print("[");
         client.print(h);
         client.print("] %\t");
       client.println("</H1>");
       client.println("<p />"); 
     client.println("</center>");
   client.println("</html>");
          break;
        }
        if (c == '\n') {
          blankLine = true;
        }
        else if (c != '\r') {
          blankLine = false;
        }
      }
    }
    delay(1);
    client.stop();
    Serial.println("Disonnected");
  }
}
