//Exemple 5
#include <Arduino.h>
    // Handle de la queue
    xQueueHandle queue;
void tacheEnvoi(void *parametres)
{
    int i = 100;
    while (1)
    {
        if (xQueueSend(queue, &i, portMAX_DELAY) == pdPASS)
        {
            Serial.printf("Envoi %d\n", i);
            i++;
        }
        else
        {
            Serial.printf("Envoi échec\n");
        }
        delay(2000);
    }
}
void tacheReception(void *parametres)
{
    int i;
    while (1)
    {
        if (xQueueReceive(queue, &i,0) != pdTRUE)
        {
            Serial.printf("Réception échec\n");
        }
        else
        {
            Serial.printf("Réception %d\n", i);
        }
        delay(1000);
    }
}
void setup()
{
    Serial.begin(115200);
    while (!Serial);
    Serial.printf("Départ\n");
    // Création de la file
    queue = xQueueCreate(10, sizeof(int));
    xTaskCreate(tacheEnvoi, "Envoi", 10000, NULL, 1, NULL);         /* Pointeur pour récupérer le « handle » de la tâche, optionnel */
    xTaskCreate(tacheReception, "Réception", 10000, NULL, 1, NULL); /* Pointeur pour récupérer le « handle » de la tâche, optionnel */
    vTaskDelete(NULL);
}

void loop()
{
    // Ne s'exécute pas
}