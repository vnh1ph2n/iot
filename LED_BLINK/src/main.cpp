#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#define LED_PIN       48
#define NEOPIXEL_PIN  45
#define NUM_PIXELS    1

Adafruit_NeoPixel pixels(NUM_PIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

volatile uint32_t toggle_count = 0;

void task_led_blinky(void *pvParameters) {
    pinMode(LED_PIN, OUTPUT);

    while (1) {
        digitalWrite(LED_PIN, HIGH);
        toggle_count++;
        Serial.printf("[Task LED] ON  | Toggle Count: %d\n", toggle_count);
        vTaskDelay(pdMS_TO_TICKS(1000));

        digitalWrite(LED_PIN, LOW);
        Serial.println("[Task LED] OFF");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void task_neopixel(void *pvParameters) {
    pixels.begin();
    pixels.setBrightness(30);

    while (1) {
        uint32_t mode = toggle_count % 3;

        if (mode == 1) {
            pixels.setPixelColor(0, pixels.Color(255, 0, 0));
        } else if (mode == 2) {
            pixels.setPixelColor(0, pixels.Color(0, 255, 0));
        } else {
            pixels.setPixelColor(0, pixels.Color(0, 0, 255));
        }

        pixels.show();
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("\n=== LAB 2 - BÀI TẬP 2: FREERTOS SHARED VARIABLES ===");

    xTaskCreate(
        task_led_blinky,
        "task_led_blinky",
        2048,
        NULL,
        1,
        NULL
    );

    xTaskCreate(
        task_neopixel,
        "task_neopixel",
        2048,
        NULL,
        1,
        NULL
    );
}

void loop() {
}