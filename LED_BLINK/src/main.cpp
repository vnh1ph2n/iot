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
    uint16_t delay_ms;
    uint8_t priority_level;
} PriorityCommand_t;

void task_low_priority_sender(void *pvParameters) {
    while (1) {
        PriorityCommand_t command = {0, 0, 255, 300, 1};
        xQueueSendToBack(ledQueue, &command, portMAX_DELAY);
        Serial.println("[Sender LOW] Sent BLUE command to BACK of queue");
        vTaskDelay(pdMS_TO_TICKS(2500));
    }
}

void task_high_priority_sender(void *pvParameters) {
    while (1) {
        PriorityCommand_t command = {255, 0, 0, 1000, 2};
        xQueueSendToFront(ledQueue, &command, portMAX_DELAY);
        Serial.println("[Sender HIGH] Sent RED OVERRIDE command to FRONT of queue");
        vTaskDelay(pdMS_TO_TICKS(4000));
    }
}

void task_receiver(void *pvParameters) {
    pinMode(LED_PIN, OUTPUT);
    pixels.begin();
    pixels.setBrightness(30);

    PriorityCommand_t receivedCommand;

    while (1) {
        if (xQueueReceive(ledQueue, &receivedCommand, portMAX_DELAY) == pdTRUE) {
            digitalWrite(LED_PIN, HIGH);
            pixels.setPixelColor(0, pixels.Color(receivedCommand.red, receivedCommand.green, receivedCommand.blue));
            pixels.show();

            Serial.printf("[Receiver] Processing Priority %d -> R:%d G:%d B:%d | Duration:%d ms\n",
                          receivedCommand.priority_level,
                          receivedCommand.red, receivedCommand.green, receivedCommand.blue,
                          receivedCommand.delay_ms);

            vTaskDelay(pdMS_TO_TICKS(receivedCommand.delay_ms));

            digitalWrite(LED_PIN, LOW);
            pixels.setPixelColor(0, pixels.Color(0, 0, 0));
            pixels.show();
        }
    }
}

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("\n=== LAB 2 - BÀI TẬP 5: QUEUE OVERWRITE & FRONT INSERTION ===");

    ledQueue = xQueueCreate(5, sizeof(PriorityCommand_t));

    if (ledQueue != NULL) {
        xTaskCreate(
            task_low_priority_sender,
            "task_low_sender",
            2048,
            NULL,
            1,
            NULL
        );

        xTaskCreate(
            task_high_priority_sender,
            "task_high_sender",
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
            2,
            NULL
        );
    }
}

void loop() {
}