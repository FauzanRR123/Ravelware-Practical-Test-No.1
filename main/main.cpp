#include <iostream>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"

// Enum untuk status aktif LED
enum ActiveLevel { LOW_LEVEL = 0, HIGH_LEVEL = 1 };

class LED {
private:
    std::string name;
    gpio_num_t pin;
    ActiveLevel activeLevel;
    float frequency;
    bool isBlinking;

public:
    // Constructor
    LED(std::string n, gpio_num_t p, ActiveLevel al, float freq) 
        : name(n), pin(p), activeLevel(al), frequency(freq), isBlinking(false) {}

    void init() {
        gpio_reset_pin(pin);
        gpio_set_direction(pin, GPIO_MODE_OUTPUT);
        off(); // Pastikan mati saat start
    }

    void on() {
        gpio_set_level(pin, activeLevel);
        ESP_LOGI("LED_STATUS", "%s is ON", name.c_str());
    }

    void off() {
        gpio_set_level(pin, (activeLevel == HIGH_LEVEL) ? 0 : 1);
        ESP_LOGI("LED_STATUS", "%s is OFF", name.c_str());
    }

    void blinkTask() {
        isBlinking = true;
        TickType_t delay = (1000 / frequency) / 2 / portTICK_PERIOD_MS;
        
        while (isBlinking) {
            on();
            vTaskDelay(delay);
            off();
            vTaskDelay(delay);
        }
    }

    static void runBlinkWrapper(void* pvParameters) {
        LED* ledInstance = static_cast<LED*>(pvParameters);
        ledInstance->blinkTask();
    }
};

extern "C" void app_main(void) {
    // Definisi Pin (Sesuaikan dengan hardware jika ada, atau gunakan dummy pin untuk simulasi)
    LED redLED("LED Red", GPIO_NUM_2, LOW_LEVEL, 10.0);
    LED greenLED("LED Green", GPIO_NUM_4, HIGH_LEVEL, 5.0);
    LED blueLED("LED Blue", GPIO_NUM_5, HIGH_LEVEL, 2.0);

    // Inisialisasi
    redLED.init();
    greenLED.init();
    blueLED.init();

    // Jalankan secara independen menggunakan FreeRTOS Task
    xTaskCreate(LED::runBlinkWrapper, "red_blink", 2048, &redLED, 5, NULL);
    xTaskCreate(LED::runBlinkWrapper, "green_blink", 2048, &greenLED, 5, NULL);
    xTaskCreate(LED::runBlinkWrapper, "blue_blink", 2048, &blueLED, 5, NULL);
}