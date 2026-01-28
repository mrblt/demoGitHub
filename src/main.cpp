
#include <Arduino.h>
TaskHandle_t hMaTache1=NULL;
void maTache1(void *parametres)
{
 int i = 0;
 while (1) // boucle infinie
 {
 Serial.printf("maTache1 %4d\n", i++);
 delay(500);
 }
}
TaskHandle_t hMaTache2=NULL;
void maTache2(void *parametres)
{
 int i = 0;
 while (1) // boucle infinie
 {
 Serial.printf("maTache2 %4d\n", i++);
 delay(2000);
 }
}
void setup()
{
 Serial.begin(115200);
 while (!Serial);
 Serial.printf("Départ\n");
 xTaskCreate(maTache1,"Ma tâche 1",10000, NULL, 1,&hMaTache1); 
 xTaskCreate(maTache2,"Ma tâche 2",10000,NULL,1,&hMaTache2);

}
void loop()
{
 static int i = 0;
 Serial.printf("Loop %4d\n", i++);
  if(i==6)
 {
  vTaskSuspend(hMaTache1);
 }
 if (i>>11)
 {
  vTaskDelete(hMaTache2);
 }
 if (i==16)
 {
  vTaskResume(hMaTache1);
 }
 delay(1000);
}