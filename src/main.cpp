// Exemple 6
#include <Arduino.h>

SemaphoreHandle_t mutex;

void tache1(void *parametres)
{
    int i = 0;
    while (1)
    {
        xSemaphoreTake(mutex, portMAX_DELAY);
        Serial.printf("Dans la tâche 1 : ");
        delay(1);
        Serial.printf("%d\n", i);
        xSemaphoreGive(mutex);
        i++;
        delay(1000);
    }
}
void tache2(void *parametres)
{
    int i = 100;
    while (1)
    {
         xSemaphoreTake(mutex, portMAX_DELAY);
        Serial.printf("Dans la tâche 2 : ");
        delay(1);
        Serial.printf("%d\n", i);
        xSemaphoreGive(mutex);
        i++;
        delay(1000);
    }
}
void setup()
{
    mutex = xSemaphoreCreateMutex();

    Serial.begin(115200);
    while (!Serial)
        ;
    Serial.printf("Départ\n");
    xTaskCreate(tache1, "Tâche 1", 10000, NULL, 1, NULL);
    xTaskCreate(tache2, "Tâche 2", 10000, NULL, 1, NULL);
    vTaskDelete(NULL);
}
void loop()
{
    // Ne s'exécute pas
}