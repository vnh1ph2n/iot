#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#define LED_PIN       48
#define NEOPIXEL_PIN  45
#define NUM_PIXELS    1

Adafruit_NeoPixel pixels(NUM_PIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

QueueHandle_t ledQueue;

typedef struct {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} LedCommand_t;

void task_sender(void *pvParameters) {
    pinMode(LED_PIN, OUTPUT);
    uint32_t state_count = 0;

    while (1) {
        digitalWrite(LED_PIN, HIGH);
        state_count++;

        LedCommand_t command;
        uint32_t mode = state_count % 3;

        if (mode == 1) {
            command = {255, 0, 0};
        } else if (mode == 2) {
            command = {0, 255, 0};
        } else {
            command = {0, 0, 255};
        }

        xQueueSend(ledQueue, &command, portMAX_DELAY);
        Serial.printf("[Task Sender] LED D13 ON | Sent Color Mode: %d\n", mode);
        vTaskDelay(pdMS_TO_TICKS(1000));

        digitalWrite(LED_PIN, LOW);
        Serial.println("[Task Sender] LED D13 OFF");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void task_receiver(void *pvParameters) {
    pixels.begin();
    pixels.setBrightness(30);

    LedCommand_t receivedCommand;

    while (1) {
        if (xQueueReceive(ledQueue, &receivedCommand, portMAX_DELAY) == pdTRUE) {
            pixels.setPixelColor(0, pixels.Color(receivedCommand.red, receivedCommand.green, receivedCommand.blue));
            pixels.show();
            Serial.printf("[Task Receiver] Received Color -> R:%d G:%d B:%d\n", 
                          receivedCommand.red, receivedCommand.green, receivedCommand.blue);
        }
    }
}

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("\n=== LAB 2 - BÀI TẬP 3: FREERTOS QUEUE COMMUNICATION ===");

    ledQueue = xQueueCreate(5, sizeof(LedCommand_t));

    if (ledQueue != NULL) {
        xTaskCreate(
            task_sender,
            "task_sender",
            2048,
            NULL,
            1,
            NULL
        );

        xTaskCreate(
            task_receiver,
            "task_receiver",
            2048,
            NULL,
            1,
            NULL
        );
    }
}

void loop() {
}