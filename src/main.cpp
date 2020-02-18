#include <Arduino.h>
#include <WiFi.h>

#define ALL_EVENT_BITS 0xFF
#define WIFI_CONNECTION_BIT (0x01UL)

const char *ssid = "CONNEXT-AXIATA";
const char *pass = "4xiatadigitallabs18";
int lastRequest = 0;

TaskHandle_t uartTaskHandle;
TaskHandle_t ledTaskHandle;
EventGroupHandle_t sampleGroup = NULL;

void setupWifi();
void uartTask(void *parameter);
void ledTask(void *parameter);

void setup() {
  // put your setup code here, to run once:
  pinMode(BUILTIN_LED, OUTPUT);
  Serial.begin(9600);
  vTaskDelay(1000);

  sampleGroup = xEventGroupCreate();
  configASSERT(sampleGroup);

  // Create RTOS task
  xTaskCreatePinnedToCore(uartTask, "UART Task", 2048, NULL, 1, &uartTaskHandle,
                          0);
  xTaskCreatePinnedToCore(ledTask, "LED Task", 2048, NULL, 2, &ledTaskHandle,
                          0);

  setupWifi();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (millis() - lastRequest > 10000) {
    if (WiFi.status() != WL_CONNECTED) {
      setupWifi();
    } else {
      Serial.println("WiFi is already connected...");
    }

    lastRequest = millis();
  }
}

void uartTask(void *parameter) {
  EventBits_t clientBits;

  for (;;) {
    clientBits = xEventGroupWaitBits(sampleGroup, ALL_EVENT_BITS, pdTRUE,
                                     pdFALSE, 1000 / portTICK_PERIOD_MS);
    clientBits &= WIFI_CONNECTION_BIT;

    if (clientBits != 0) {
      Serial.println("WiFi Connected...");
      Serial.println("");
      Serial.println("WiFi connected");
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());
    }
  }
}

void ledTask(void *parameter) {
  for (;;) {
    if (WiFi.status() == WL_CONNECTED) {
      digitalWrite(BUILTIN_LED, HIGH);
      vTaskDelay(1000);
      digitalWrite(BUILTIN_LED, LOW);
      vTaskDelay(1000);
    } else {
      digitalWrite(BUILTIN_LED, LOW);
    }
  }
}

void setupWifi() {
  vTaskDelay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    vTaskDelay(500);
    Serial.print(".");
  }

  randomSeed(micros());
  xEventGroupSetBits(sampleGroup, WIFI_CONNECTION_BIT);
}
