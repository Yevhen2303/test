/**
 * ESP32-C3 Flash Encryption Status Checker
 * Демонстрація перевірки стану захисту прошивки на пристрої
 */

#include <stdio.h>
#include "esp_flash_encrypt.h"
#include "esp_secure_boot.h"
#include "esp_efuse.h"
#include "esp_log.h"

static const char *TAG = "fw_protection";

void app_main(void)
{
    ESP_LOGI(TAG, "=== Перевірка стану захисту прошивки ===");

    /* Flash Encryption */
    bool flash_enc_enabled = esp_flash_encryption_enabled();
    ESP_LOGI(TAG, "Flash Encryption: %s", flash_enc_enabled ? "УВІМКНЕНО" : "вимкнено");

    if (flash_enc_enabled) {
        esp_flash_enc_mode_t mode = esp_get_flash_encryption_mode();
        switch (mode) {
            case ESP_FLASH_ENC_MODE_DEVELOPMENT:
                ESP_LOGW(TAG, "Режим: DEVELOPMENT (не для продакшн!)");
                break;
            case ESP_FLASH_ENC_MODE_RELEASE:
                ESP_LOGI(TAG, "Режим: RELEASE (повний захист)");
                break;
            default:
                ESP_LOGE(TAG, "Режим: невідомий");
        }
    }

    /* Secure Boot */
#ifdef CONFIG_SECURE_BOOT_V2_ENABLED
    bool secure_boot = esp_secure_boot_enabled();
    ESP_LOGI(TAG, "Secure Boot v2: %s", secure_boot ? "УВІМКНЕНО" : "вимкнено");
#else
    ESP_LOGW(TAG, "Secure Boot: не скомпільовано");
#endif

    /* JTAG статус */
    bool jtag_disabled = esp_efuse_read_field_bit(ESP_EFUSE_DIS_PAD_JTAG);
    ESP_LOGI(TAG, "JTAG: %s", jtag_disabled ? "ЗАБЛОКОВАНО" : "доступний");

    /* Download mode */
    bool dl_disabled = esp_efuse_read_field_bit(ESP_EFUSE_DIS_DOWNLOAD_MODE);
    ESP_LOGI(TAG, "UART Download Mode: %s", dl_disabled ? "ЗАБЛОКОВАНО" : "доступний");

    ESP_LOGI(TAG, "=========================================");

    if (flash_enc_enabled && jtag_disabled && dl_disabled) {
        ESP_LOGI(TAG, "Статус: ПОВНИЙ ЗАХИСТ ВІД КОПІЮВАННЯ");
    } else {
        ESP_LOGW(TAG, "Статус: захист НЕПОВНИЙ");
    }
}
