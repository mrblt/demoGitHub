/*!
 *  @file Adafruit_MPU6050.h
 *
 *  I2C Driver for MPU6050 6-DoF Accelerometer and Gyro
 *
 *  This is a library for the Adafruit MPU6050 breakout:
 *  https://www.adafruit.com/products/3886
 *
 *  Adafruit invests time and resources providing this open source code,
 *  please support Adafruit and open-source hardware by purchasing products from
 *  Adafruit!
 *
 *
 *  BSD license (see license.txt)
 */

#include "Arduino.h"
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <String.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

Adafruit_MPU6050 mpu;

char FlagCalcul = 0;
float Ve, Vs = 0;
float Te = 10;   // période d'échantillonage en ms
float Tau = 200; // constante de temps du filtre en ms
float theta_gyro;
float theta_filtre;
float theta_W;
float theta_final;
int Moteur_Davant = 18;
int Moteur_Darriere = 17;
int Moteur_Gavant = 16;
int Moteur_Garriere = 4;
int freq=20000;
int resolution = 10;
int canal0=0;
int canal1=1;
int canal2=2;
int canal3=3;

sensors_event_t a, g, temp;

// coefficient du filtre
float A, B;

void controle(void *parameters)
{
  TickType_t xLastWakeTime;
  xLastWakeTime = xTaskGetTickCount();
  while (1)
  {

    mpu.getEvent(&a, &g, &temp);
    theta_gyro = atan2(a.acceleration.y, a.acceleration.x) * 180 / PI;
    theta_filtre = A * theta_gyro + B * theta_filtre;
    theta_W = A * Tau / 1000 * g.gyro.z + B * theta_W;
    theta_final = theta_W + theta_filtre;

    FlagCalcul = 1;
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(Te));

    //moteur Droit
    ledcWrite(canal0, 0);
    ledcWrite(canal1, 0);

     //Moteur Gauche
    ledcWrite(canal2, 0);
    ledcWrite(canal3,0);
  }
}

void Vin(void *parameters)
{
  Ve = 1;
  while (1)
  {
    if (Ve == 1)
      Ve = 0;
    else
      Ve = 1;
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.printf("Bonjour \n\r");
  Wire.begin();
  if (!mpu.begin())
  {
    Serial.println("Sensor init failed");
    while (1)
    {
    }
  }
  Serial.println("Found a MPU-6050 sensor");
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_2000_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  ledcSetup(canal0, freq, resolution);
  ledcAttachPin(Moteur_Davant,canal0 );
  
  ledcSetup(canal1, freq, resolution);
  ledcAttachPin(Moteur_Darriere,canal1 );


  ledcSetup(canal2, freq, resolution);
  ledcAttachPin(Moteur_Gavant,canal2 );

  ledcSetup(canal3, freq, resolution);
  ledcAttachPin(Moteur_Garriere,canal3 );

 

  xTaskCreate(
      controle,   // nom de la fonction
      "controle", // nom de la tache que nous venons de vréer
      10000,      // taille de la pile en octet
      NULL,       // parametre
      10,         // tres haut niveau de priorite
      NULL        // descripteur
  );
  xTaskCreate(
      Vin,   // nom de la fonction
      "Vin", // nom de la tache que nous venons de vréer
      10000, // taille de la pile en octet
      NULL,  // parametre
      1,     // bas niveau de priorite
      NULL   // descripteur
  );

  // calcul coeff filtre
  A = 1 / (1 + Tau / Te);
  B = Tau / Te * A;
}

void reception(char ch)
{

  static int i = 0;
  static String chaine = "";
  String commande;
  String valeur;
  int index, length;

  if ((ch == 13) or (ch == 10))
  {
    index = chaine.indexOf(' ');
    length = chaine.length();
    if (index == -1)
    {
      commande = chaine;
      valeur = "";
    }
    else
    {
      commande = chaine.substring(0, index);
      valeur = chaine.substring(index + 1, length);
    }

    if (commande == "Tau")
    {
      Tau = valeur.toFloat();
      // calcul coeff filtre
      A = 1 / (1 + Tau / Te);
      B = Tau / Te * A;
    }
    if (commande == "Te")
    {
      Te = valeur.toInt();
      A = 1 / (1 + Tau / Te);
      B = Tau / Te * A;
    }

    chaine = "";
  }
  else
  {
    chaine += ch;
  }
}

void loop()
{

  if (FlagCalcul == 1)
  {

    Serial.printf("%lf %lf %lf %lf", theta_gyro, theta_filtre, -g.gyro.z, theta_final);

    /*
      Serial.print("Acceleration X: ");
      Serial.print(a.acceleration.x);
      Serial.print(", Y: ");
      Serial.print(a.acceleration.y);
      Serial.print(", Z: ");
      Serial.print(a.acceleration.z);
      Serial.println(" m/s^2");

      Serial.print("Rotation X: ");
      Serial.print(g.gyro.x);
      Serial.print(", Y: ");
      Serial.print(g.gyro.y);
      Serial.print(", Z: ");
      Serial.print(g.gyro.z);
      Serial.println(" rad/s");
    */

    /* Serial.print("Temperature: ");
     Serial.print(temp.temperature);
     Serial.println(" degC");*/
    
   

    

  

    delay(2000);
    Serial.println("");

    FlagCalcul = 0;
  }
}

void serialEvent()
{
  while (Serial.available() > 0) // tant qu'il y a des caractères à lire
  {
    reception(Serial.read());
  }
}
