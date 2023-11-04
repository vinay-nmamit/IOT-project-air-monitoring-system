#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <WifiClient.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>

char auth[] = "";
char ssid[] = "";     // Replace with your Wi-Fi SSID
char pass[] = ""; // Replace with your Wi-Fi password

BlynkTimer timer;

#define DHT_PIN_DATA 0 // Replace with the actual digital pin number to which your DHT11 sensor is connected
#define DHT_TYPE DHT11
#define MQ135_PIN_AOUT 12 // Replace with the actual pin connected to the analog output of the MQ-135

DHT dht(DHT_PIN_DATA, DHT_TYPE);

int gasThreshold = 300;
int temperatureThreshold = 25;

void sendSensorData() 
{
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  
  int analogSensor = analogRead(MQ135_PIN_AOUT);

  Blynk.virtualWrite(V0, t);
  Blynk.virtualWrite(V1, h);
  Blynk.virtualWrite(V2, analogSensor);

  if (t > temperatureThreshold) 
      Blynk.notify("High Temperature Alert! Temperature is " + String(t) + "°C.");
  
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print("°C, Humidity: ");
  Serial.print(h);
  Serial.print("%, Gas Value (Analog): ");
  Serial.print(analogSensor);
  
  // Print the gas value converted to PPM (parts per million) if you have a calibration curve
  // Replace these coefficients with your own calibration data
  float ppm = map(analogSensor, 0, 1023, 0, 1000);  // Example calibration
  Serial.print(", Gas Value (PPM): ");
  Serial.print(ppm);
  
  Serial.println();

  if (analogSensor > gasThreshold) {
    Blynk.logEvent("pollution_alert", "Bad Air");
  }

  if (t > temperatureThreshold) {
      Blynk.email("vk5022702@gmail.com", "High Temperature Alert", "Temperature is " + String(t) + "°C");
    }
}


void setup() {
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass,IPAddress(117,236,190,213),8080);
  dht.begin();
  
  timer.setInterval(30000L, sendSensorData);
}

void loop() {
  Blynk.run();
  timer.run();
}