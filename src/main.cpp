#include <Arduino.h>
#include <String.h>

char FlagCalcul = 0;
float Ve, Vs = 0;
float Te = 10;    // période d'échantillonage en ms
float Tau = 1000; // constante de temps du filtre en ms

// coefficient du filtre
float A, B;

void controle(void *parameters)
{
  TickType_t xLastWakeTime;
  xLastWakeTime = xTaskGetTickCount();
  while (1)
  {

    Vs = A * Ve + B * Vs;

    FlagCalcul = 1;
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(Te));
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

  // calcul coeff filtre (voir demonstration sur la diapo ihm)
  A = 1 / (1 + Tau / Te);
  B = Tau / Te * A;
}

void reception(char ch)
{

  static int i = 0;
  static String chaine = ""; //espace et string pour garder la valeur de la variable à la fin 
  String commande;           //espace et string pour garder la commande à la fin
  String valeur;             //espace et string pour garder la valeur à la fin
  int index, length;         //index est la position de l'espace dans la chaine de caractère, length pour la longueur de la chaine

  if ((ch == 13) or (ch == 10)) // si on reçoit un retour chariot ou un saut de ligne
  {
    index = chaine.indexOf(' '); // on cherche la position de l'espace dans la chaine
    length = chaine.length();     // on cherche la longueur de la chaine
    if (index == -1) // si pas d'espace dans la chaine exemple BO ou BF dans le terminal 
    {
      commande = chaine; // on met toute la chaine dans commande 
      valeur = ""; 
    }
    else
    {
      commande = chaine.substring(0, index); // on met dans commande la partie avant l'espace
      valeur = chaine.substring(index + 1, length); // on met dans valeur la partie après l'espace
    }

    if (commande == "Tau") // si la commande est Tau
    {
      Tau = valeur.toFloat(); // on convertit la valeur en float et on la stocke dans Tau
      
      // calcul coeff filtre
      A = 1 / (1 + Tau / Te);
      B = Tau / Te * A;
    }
    if (commande == "Te") // si la commande est Te
    {
      Te = valeur.toInt(); // on convertit la valeur en int et on la stocke dans Te
      
      // recalcul des coefficients du filtre
      A = 1 / (1 + Tau / Te); 
      B = Tau / Te * A; 
    }

    chaine = ""; // on vide la chaine pour la prochaine commande
  }
  else
  {
    chaine += ch; // on ajoute le caractère reçu à la chaine
  }
}

void loop()
{
  if (FlagCalcul == 1) // si le drapeau de calcul est levé 
  {
    Serial.printf("%lf %lf \n", Ve, Vs); // on affiche les valeurs de Ve et Vs

    FlagCalcul = 0;   // on baisse le drapeau de calcul (donc on le met à 0)
  }
}

void serialEvent()
{
  while (Serial.available() > 0) // tant qu'il y a des caractères à lire
  {
    reception(Serial.read()); // on lit le caractère et on l'envoie à la fonction de réception
  }
}