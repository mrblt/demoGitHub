#include <Arduino.h>
int led=2;
void tachePeriodique(void *pvParameters)
{

 TickType_t xLastWakeTime;
 double x = 0, y = 0;
 // Lecture du nombre de ticks quand la tâche débute
 xLastWakeTime = xTaskGetTickCount();
 while (1)
 {
    digitalWrite(led,HIGH);
 TickType_t debCalcul = xTaskGetTickCount();
 // Des calculs pour que la tâche occupe le processeur
 int nbTour = 3000 + rand()%3000;
 for (int i = 0; i < nbTour; i++) {
 double xn = sin(x) + cos(y);
 double yn = cos(x) + sin(y);
 double d = sqrt(xn * xn + yn * yn);

 if (d == 0) {
 x = 0;
 y = 0;
 } else {
 x = xn / d;
 y = yn / d;
 }
 }
 
 TickType_t finCalcul = xTaskGetTickCount();
 Serial.printf("Temps de calcul = %u\n", finCalcul - debCalcul);
 // Endort la tâche pendant le temps restant par rapport au réveil,
 // ici 200ms, donc la tâche s'effectue toutes les 200ms
 digitalWrite(led,LOW);
 vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(200)); 
 }
}
void setup()
{
 Serial.begin(115200);
 Serial.printf("Initialisation\n");
 // Création de la tâche périodique
 xTaskCreate(tachePeriodique, "Tâche périodique", 10000, NULL, 2, NULL);
 pinMode(led,OUTPUT);
}
void loop()
{
 static int i = 0;
 Serial.printf("Boucle principale : %d\n", i++);
 delay(1000);
}
