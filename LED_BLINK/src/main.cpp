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
} LedCommand_t;

void task_sender_1(void *pvParameters) {
    while (1) {
        LedCommand_t command = {255, 0, 0, 500};
        xQueueSend(ledQueue, &command, portMAX_DELAY);
        Serial.println("[Sender 1] Sent RED command (500ms)");
        vTaskDelay(pdMS_TO_TICKS(3000));
    }
}

void task_sender_2(void *pvParameters) {
    while (1) {
        LedCommand_t command = {0, 255, 0, 200};
        xQueueSend(ledQueue, &command, portMAX_DELAY);
        Serial.println("[Sender 2] Sent GREEN command (200ms)");
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

void task_receiver(void *pvParameters) {
    pinMode(LED_PIN, OUTPUT);
    pixels.begin();
    pixels.setBrightness(30);

    LedCommand_t receivedCommand;

    while (1) {
        if (xQueueReceive(ledQueue, &receivedCommand, portMAX_DELAY) == pdTRUE) {
            digitalWrite(LED_PIN, HIGH);
            pixels.setPixelColor(0, pixels.Color(receivedCommand.red, receivedCommand.green, receivedCommand.blue));
            pixels.show();

            Serial.printf("[Receiver] Executing -> R:%d G:%d B:%d | Delay:%d ms\n",
                          receivedCommand.red, receivedCommand.green, receivedCommand.blue, receivedCommand.delay_ms);

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
    Serial.println("\n=== LAB 2 - BÀI TẬP 4: MULTI-SENDER QUEUE ===");

    ledQueue = xQueueCreate(10, sizeof(LedCommand_t));

    if (ledQueue != NULL) {
        xTaskCreate(
            task_sender_1,
            "task_sender_1",
            2048,
            NULL,
            1,
            NULL
        );

        xTaskCreate(
            task_sender_2,
            "task_sender_2",
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