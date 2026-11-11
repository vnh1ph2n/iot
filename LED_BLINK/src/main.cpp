#include <Arduino.h>

#define LED_PIN 48

void task_led_blinky(void *pvParameters) {
    pinMode(LED_PIN, OUTPUT);

    while (1) {
        digitalWrite(LED_PIN, HIGH);
        Serial.println("[Task LED] State: SÁNG (1s)");
        vTaskDelay(pdMS_TO_TICKS(1000));

        digitalWrite(LED_PIN, LOW);
        Serial.println("[Task LED] State: TẮT (2s)");
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("\n=== LAB 2 - BÀI TẬP 1: FREERTOS BLINKY STARTED ===");

    xTaskCreate(
        task_led_blinky,
        "task_led_blinky",
        2048,
        NULL,
        1,
        NULL
    );
}

void loop() {
}