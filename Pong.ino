/***********************
     PARAMETRES PONG
 ***********************/
const float PG_BALL_SPEED = 0.5;   // Vitesse de la balle
const float PG_ACCELERATION = 8;   // Accelleration de la balle a chaque tir, si ACCELERATION = 10 on augmente la vitesse de 10 pourcent a chaque tir
const int   PG_HIT_ZONE = 75;      // Nombre de LED pendant lesquelles on peut renvoyer la balle
const int   PG_MAX_SCORE = 3;

const CRGB  PG_PLAYER1_COLOR = ROUGE;
const CRGB  PG_PLAYER2_COLOR = BLEU;
const CRGB  PG_BALL_COLOR = BLANC; // Couleur de la balle

/*
   Variables Pong
*/
const int PG_PLAYER1 = 0;
const int PG_PLAYER2 = 1;

int nextPlayer;
int lastWinner;

float ballSpeed = PG_BALL_SPEED;   // Vitesse de la balle
float ballPosition = 1.0f;         // Position de la balle sur la bande de led (Si ballPosition = 0, la balle est devant le player 1. Si ballPosition = 1, la balle est devant le player 2)
int player1Score = 0;           // Score du player 1
int player2Score = 0;           // Score du player 2


/*
   Fonctions Pong
*/

// Fonction permettant de changer la couleur d'une LED relativement au player 1 ou 2
void setLedColor(int player, int pos, CRGB color)
{
  if (player == PG_PLAYER1)
  {
    leds[pos] = color;
  }
  else // player == PLAYER2
  {
    leds[LEDS_NB - pos - 1] = color;
  }
}
// Fonction permettant de changer la couleur d'une LED
void setLedColor(int pos, CRGB color)
{
  leds[pos] = color;
}

// Fonction servant a afficher les scores
void showScore()
{
  // On commence par effacer toutes les couleurs de led
  FastLED.clear();

  // On allume le nombre de led correspondant au score du player 1
  for (int i = 0; i < player1Score; i++)
  {
    setLedColor(PG_PLAYER1, LEDS_NB / 2 - (i + 1), PG_PLAYER1_COLOR);
  }

  // On allume le nombre de led correspondant au score du player 2
  for (int i = 0; i < player2Score; i++)
  {
    setLedColor(PG_PLAYER2, LEDS_NB / 2 - (i + 1), PG_PLAYER2_COLOR);
  }

  // On envoie les nouvelles couleurs a la bande de led
  FastLED.show();

  // On fait clignotter trois fois
  if (lastWinner == PG_PLAYER1)
  {
    for (int i = 0; i < 3; i++)
    {
      // On eteint la derniere LED pendant 0.5s
      delay(500);
      setLedColor(PG_PLAYER1, LEDS_NB / 2 - player1Score, NOIR);
      FastLED.show();

      // On allume la derniere LED pendant 0.5s
      delay(500);
      setLedColor(PG_PLAYER1, LEDS_NB / 2 - player1Score, PG_PLAYER1_COLOR);
      FastLED.show();
    }
  }
  else // lastWinner == PLAYER2
  {
    for (int i = 0; i < 3; i++)
    {
      // On eteint la derniere LED pendant 0.5s
      delay(500);
      setLedColor(PG_PLAYER2, LEDS_NB / 2 - player2Score, NOIR);
      FastLED.show();

      // On allume la derniere LED pendant 0.5s
      delay(500);
      setLedColor(PG_PLAYER2, LEDS_NB / 2 - player2Score, PG_PLAYER2_COLOR);
      FastLED.show();
    }
  }

  // Si la partie est terminee on va a l'affichage de fin
  if (player1Score == PG_MAX_SCORE || player2Score == PG_MAX_SCORE)
  {
    state = Demo;

    // On reinitialise les scores
    player1Score = 0;
    player2Score = 0;

    // On reinitialise la vitesse
    ballSpeed = PG_BALL_SPEED;

    // On reinitialise les leds
    FastLED.clear();
  }
  // Sinon on reprend le jeu
  else
  {
    ballSpeed = PG_BALL_SPEED;
  }
}


void pongGameLoop()
{
  // Calcul du temps ecoule entre deux boucles
  unsigned long currentMillis = millis();

  // On calcule le numero de la LED allumee
  int ballLed = ballPosition * LEDS_NB;

  // On s'assure que la position de la balle ne depasse pas la taille de la bande de LED
  ballLed = min(ballLed, LEDS_NB - 1);

  // On regarde qui est en train de jouer
  if (nextPlayer == PG_PLAYER1)
  {
    // On regarde si le player a appuye sur son bouton et si le delai de debut de jeu est passe
    if (digitalRead(PLAYER_BUTTON_PINS[0]) == LOW && currentMillis - beginTimer > 500)
    {
      // Si la balle est hors de la zone de tir, l'autre player marque un point
      if (ballLed >= PG_HIT_ZONE)
      {
        player2Score += 1;
        lastWinner = PG_PLAYER2;
        ballPosition = 0;

        // On joue le son de defaite
        //tone(HAUT_PARLEUR_PIN, LOOSE_SOUND, 800);

        // On passe en mode affichage des scores
        showScore();

        // C'est a l'autre player de jouer
        nextPlayer = PG_PLAYER2;

        // Actualisation de la variable lastMillis
        lastMillis = millis();
      }
      else
      {
        // On accelere la balle
        ballSpeed *= 1.0 + PG_ACCELERATION / 100;

        // C'est a l'autre player de jouer
        nextPlayer = PG_PLAYER2;

        // On joue la note de musique
        //tone(HAUT_PARLEUR_PIN, PONG_SOUND, 100);
      }

      return;
    }

    // On fait avancer la balle
    ballPosition -= ballSpeed * (currentMillis - lastMillis) * 0.001f;

    // On regarde si la balle est sortie de la zone
    if (ballPosition < 0.0f)
    {
      // Si oui le player 2 marque un point
      player2Score += 1;
      lastWinner = PG_PLAYER2;
      ballPosition = 0;

      // On joue le son de defaite
      //tone(HAUT_PARLEUR_PIN, LOOSE_SOUND, 800);

      // On passe en mode affichage des scores
      showScore();

      // C'est a l'autre player de jouer
      nextPlayer = PG_PLAYER2;

      // Actualisation de la variable lastMillis
      lastMillis = millis();
      return;
    }
  }
  else // player == PLAYER2
  {
    // On regarde si le player a appuye sur son bouton et si le delai de debut de jeu est passe
    if (digitalRead(PLAYER_BUTTON_PINS[1]) == LOW && currentMillis - beginTimer > 500)
    {
      // Si la balle est hors de la zone de tir, l'autre player marque un point
      if (ballLed < LEDS_NB - PG_HIT_ZONE)
      {
        player1Score += 1;
        lastWinner = PG_PLAYER1;
        ballPosition = 1;

        // On joue le son de defaite
        //tone(HAUT_PARLEUR_PIN, LOOSE_SOUND, 800);

        // On passe en mode affichage des scores
        showScore();

        // C'est a l'autre player de jouer
        nextPlayer = PG_PLAYER1;

        // Actualisation de la variable lastMillis
        lastMillis = millis();
      }
      else
      {
        // On accelere la balle
        ballSpeed *= 1.0 + PG_ACCELERATION / 100;

        // C'est a l'autre player de jouer
        nextPlayer = PG_PLAYER1;

        // On joue la note de musique
        //tone(HAUT_PARLEUR_PIN, PONG_SOUND, 100);
      }

      return;
    }

    // On fait avancer la balle dans l'autre sens
    ballPosition += ballSpeed * (currentMillis - lastMillis) * 0.001f;

    // On regarde si la balle est sortie de la zone
    if (ballPosition >= 1.0f)
    {
      // Si oui le player 1 marque un point
      player1Score += 1;
      lastWinner = PG_PLAYER1;
      ballPosition = 1.0f;

      // On joue le son de defaite
      //tone(HAUT_PARLEUR_PIN, LOOSE_SOUND, 800);

      // On passe en mode affichage des scores
      showScore();
      // C'est a l'autre player de jouer
      nextPlayer = PG_PLAYER1;

      // Actualisation de la variable lastMillis
      lastMillis = millis();
      return;
    }
  }

  ///// AFFICHAGE BANDE DE LEDs /////
  // Premierement on efface toutes les couleurs precedentes
  FastLED.clear();

  // Ensuite on allume faiblement les LEDs correspondant a la zone de chaque cote
  for (int i = 0; i < PG_HIT_ZONE; i++)
  {
    // On allume de chaque cote
    setLedColor(PG_PLAYER1, i, PG_PLAYER1_COLOR / 10);  // On divise la couleur par 10 pour la rendre 10 fois moins puissante
    setLedColor(PG_PLAYER2, i, PG_PLAYER2_COLOR / 10);
  }

  // Ensuite on allume faiblement les LEDs correspondant aux scores
  // Pour le player 1
  for (int i = 0; i < player1Score; i++)
  {
    setLedColor(PG_PLAYER1, LEDS_NB / 2 - (i + 1), PG_PLAYER1_COLOR / 15);
  }
  // Pour le player 2
  for (int i = 0; i < player2Score; i++)
  {
    setLedColor(PG_PLAYER2, LEDS_NB / 2 - (i + 1), PG_PLAYER2_COLOR / 15);
  }

  // Ensuite on actualise la position de la balle
  // On donne la couleur de la led en fonction de si la balle est dans la zone d'un player ou non

  // Si la balle est dans le camp d'un des player, elle est rouge.
  if (ballLed < PG_HIT_ZONE || ballLed >= LEDS_NB - PG_HIT_ZONE)
  {
    setLedColor(ballLed, ROUGE);
  }
  // Si elle en est proche, elle est jaune
  else if (ballLed < 2 * PG_HIT_ZONE || ballLed >= LEDS_NB - 2 * PG_HIT_ZONE)
  {
    setLedColor(ballLed, JAUNE);
  }
  // Sinon la balle a sa couleur par defaut
  else
  {
    setLedColor(ballLed, PG_BALL_COLOR);
  }

  // On envoie la couleur des leds a la bande de leds
  FastLED.show();

  // On actualise la variable lastMillis pour la boucle suivante
  lastMillis = currentMillis;
}
