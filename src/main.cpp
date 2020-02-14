#include <Arduino.h>
#include <WiFi.h>

const char *ssid = "CONNEXT-AXIATA";
const char *pass = "4xiatadigitalllabs18";
int lastRequest = 0;
TaskHandle_t uartTaskHandle;
TaskHandle_t ledTaskHandle;

void reconnect();
void uartTask(void *parameter);
void ledTask(void *parameter);

void setup() {
  // put your setup code here, to run once:
  pinMode(BUILTIN_LED, OUTPUT);
  Serial.begin(9600);
  delay(1000);

  // Create RTOS task
  xTaskCreatePinnedToCore(uartTask, "Uart Task", 2048, NULL, 1, &uartTaskHandle,
                          0);
  xTaskCreatePinnedToCore(ledTask, "LED Task", 2048, NULL, 2, &ledTaskHandle,
                          0);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (millis() - lastRequest > 60000) {
    if (WiFi.status() != WL_CONNECTED) {
      reconnect();
    }

    lastRequest = millis();
  }
}

void uartTask(void *parameter) {
  for (;;) {
    Serial.println("Tes uart first...");
    delay(3000);
    Serial.println("Test uart second...");
    delay(3000);
  }
}

void ledTask(void *parameter) {
  for (;;) {
    digitalWrite(BUILTIN_LED, HIGH);
    delay(3000);
    digitalWrite(BUILTIN_LED, LOW);
    delay(3000);
  }
}

void reconnect() {
  WiFi.begin(ssid, pass);
  Serial.println("WiFi connected...");
}