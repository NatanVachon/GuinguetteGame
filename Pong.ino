/*
     Parametres Pong
*/
const int   PG_LEDS_INTENSITY_DIV = 1;      // Diviseur de l'intensite des leds, augmenter ce parametre pour reduire l'intensite des leds dans le pong

const int   PG_BALL_SIZE          = 10;     // Taille de la balle en nombre de leds
const float PG_BALL_SPEED         = 0.2;    // Vitesse de la balle
const float PG_ACCELERATION       = 8;      // Accelleration de la balle a chaque tir, si ACCELERATION = 10 on augmente la vitesse de 10 pourcent a chaque tir

const int   PG_HIT_ZONE           = 75;     // Nombre de LED pendant lesquelles on peut renvoyer la balle
const int   PG_MAX_SCORE          = 3;      // Score a obtenir pour gagner la partie
const int   PG_SCORE_SIZE         = 5;      // Taille d'un point de score en nombre de leds

const CRGB  PG_PLAYER1_COLOR      = ROUGE;  // Couleur player 1
const CRGB  PG_PLAYER2_COLOR      = BLEU;   // Couleur player 2
const CRGB  PG_BALL_COLOR         = BLANC;  // Couleur de la balle

/*
   Variables Pong
*/
const int PG_PLAYER1 = 0;
const int PG_PLAYER2 = 1;

int nextPlayer = PG_PLAYER1;
int lastWinner;

float ballSpeed = PG_BALL_SPEED;   // Vitesse de la balle
float ballPosition = 1.0f;         // Position de la balle sur la bande de led (Si ballPosition = 0, la balle est devant le player 1. Si ballPosition = 1, la balle est devant le player 2)
int player1Score = 0;              // Score du player 1
int player2Score = 0;              // Score du player 2
int exchangesCounter = 0;


/*
   Fonctions Pong
*/

// Fonction permettant de changer la couleur d'une LED relativement au player 1 ou 2
void setLedColor(int player, int pos, int len, CRGB color)
{
  if (player == PG_PLAYER1)
  {
    for (int i = 0; i < len; i++)
    {
      if (pos + i < LEDS_NB)
      {
        leds[pos + i] = color / (LEDS_INTENSITY_DIV * PG_LEDS_INTENSITY_DIV);
      }
    }
  }
  else // player == PLAYER2
  {
    for (int i = 0; i < len; i++)
    {
      if (LEDS_NB - 1 - (pos + i) >= 0)
      {
        leds[LEDS_NB - 1 - (pos + i)] = color / (LEDS_INTENSITY_DIV * PG_LEDS_INTENSITY_DIV);
      }
    }
  }
}
// Fonction permettant de changer la couleur d'une LED
void setLedColor(int pos, int len, CRGB color)
{
  for (int i = 0; i < len; i++)
  {
    if (pos + i < LEDS_NB)
    {
      leds[pos + i] = color / (LEDS_INTENSITY_DIV * PG_LEDS_INTENSITY_DIV);
    }
  }
}

// Fonction servant a afficher les scores
void showScore()
{
  // On regarde si la partie est terminee
  if (player1Score == PG_MAX_SCORE || player2Score == PG_MAX_SCORE)
  {
    // Selection couleur gagnante
    CRGB winnerColor = player1Score == PG_MAX_SCORE ? PG_PLAYER1_COLOR : PG_PLAYER2_COLOR;

    // Clignottement couleurs
    for (int i = 0; i < 3; i++)
    {
      FastLED.clear();
      fill_solid(leds, LEDS_NB, winnerColor / (32 * LEDS_INTENSITY_DIV * PG_LEDS_INTENSITY_DIV));
      FastLED.show();
      delay(1000);

      FastLED.clear();
      FastLED.show();
      delay(1000);
    }

    state = Demo;

    // On reinitialise les scores
    player1Score = 0;
    player2Score = 0;

    // On reinitialise le prochain player
    nextPlayer = PG_PLAYER1;

    // On reinitialise la position
    ballPosition = 1.0f;

    // On reinitialise la vitesse
    ballSpeed = PG_BALL_SPEED;

    // On reinitialise le compteur
    exchangesCounter = 0;
    displayDigits.showNumberDec(exchangesCounter, false);

    return;
  }

  // On commence par effacer toutes les couleurs de led
  FastLED.clear();

  // On allume le nombre de led correspondant au score du player 1
  setLedColor(PG_PLAYER1, LEDS_NB / 2 - player1Score * PG_SCORE_SIZE, player1Score * PG_SCORE_SIZE, PG_PLAYER1_COLOR);

  // On allume le nombre de led correspondant au score du player 2
  setLedColor(PG_PLAYER2, LEDS_NB / 2 - player2Score * PG_SCORE_SIZE, player2Score * PG_SCORE_SIZE, PG_PLAYER2_COLOR);

  // On envoie les nouvelles couleurs a la bande de led
  FastLED.show();

  // On fait clignotter trois fois
  if (lastWinner == PG_PLAYER1)
  {
    for (int i = 0; i < 3; i++)
    {
      // On eteint la derniere LED pendant 0.5s
      delay(1000);
      setLedColor(PG_PLAYER1, LEDS_NB / 2 - player1Score * PG_SCORE_SIZE, PG_SCORE_SIZE, NOIR);
      FastLED.show();

      // On allume la derniere LED pendant 0.5s
      delay(1000);
      setLedColor(PG_PLAYER1, LEDS_NB / 2 - player1Score * PG_SCORE_SIZE, PG_SCORE_SIZE, PG_PLAYER1_COLOR);
      FastLED.show();
    }
  }
  else // lastWinner == PLAYER2
  {
    for (int i = 0; i < 3; i++)
    {
      // On eteint la derniere LED pendant 0.5s
      delay(1000);
      setLedColor(PG_PLAYER2, LEDS_NB / 2 - player2Score * PG_SCORE_SIZE, PG_SCORE_SIZE, NOIR);
      FastLED.show();

      // On allume la derniere LED pendant 0.5s
      delay(1000);
      setLedColor(PG_PLAYER2, LEDS_NB / 2 - player2Score * PG_SCORE_SIZE, PG_SCORE_SIZE, PG_PLAYER2_COLOR);
      FastLED.show();
    }
  }

  // On reprend le jeu
  ballSpeed = PG_BALL_SPEED;
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
    if (digitalRead(PLAYER_BUTTON_PINS[0]) == LOW && currentMillis - beginTimer > 1000)
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

        // On incremente le compteur d'echanges
        exchangesCounter += 1;

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
    if (digitalRead(PLAYER_BUTTON_PINS[1]) == LOW && currentMillis - beginTimer > 1000)
    {
      // Si la balle est hors de la zone de tir, l'autre player marque un point
      if (ballLed + PG_BALL_SIZE < LEDS_NB - PG_HIT_ZONE)
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

        // On incremente le compteur d'echanges
        exchangesCounter += 1;
        displayDigits.showNumberDec(exchangesCounter, false);

        // On joue la note de musique
        //tone(HAUT_PARLEUR_PIN, PONG_SOUND, 100);
      }

      return;
    }

    // On fait avancer la balle dans l'autre sens
    ballPosition += ballSpeed * (currentMillis - lastMillis) * 0.001f;

    // On regarde si la balle est sortie de la zone
    if (ballPosition + float(PG_BALL_SIZE) / LEDS_NB >= 1.0f)
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
  // On allume de chaque cote
  setLedColor(PG_PLAYER1, 0, PG_HIT_ZONE, PG_PLAYER1_COLOR / 10);  // On divise la couleur par 10 pour la rendre 10 fois moins puissante
  setLedColor(PG_PLAYER2, 0, PG_HIT_ZONE, PG_PLAYER2_COLOR / 10);


  // Ensuite on allume faiblement les LEDs correspondant aux scores
  // Pour le player 1
  setLedColor(PG_PLAYER1, LEDS_NB / 2 - player1Score * PG_SCORE_SIZE, player1Score * PG_SCORE_SIZE, PG_PLAYER1_COLOR / 15);

  // Pour le player 2
  setLedColor(PG_PLAYER2, LEDS_NB / 2 - player2Score * PG_SCORE_SIZE, player2Score * PG_SCORE_SIZE, PG_PLAYER2_COLOR / 15);

  // Ensuite on actualise la position de la balle
  // On donne la couleur de la led en fonction de si la balle est dans la zone d'un player ou non

  // Si la balle est dans le camp d'un des player, elle est rouge.
  if (ballLed < PG_HIT_ZONE || ballLed >= LEDS_NB - PG_HIT_ZONE)
  {
    setLedColor(ballLed, PG_BALL_SIZE, ROUGE);
  }
  // Si elle en est proche, elle est jaune
  else if (ballLed < 2 * PG_HIT_ZONE || ballLed >= LEDS_NB - 2 * PG_HIT_ZONE)
  {
    setLedColor(ballLed, PG_BALL_SIZE, JAUNE);
  }
  // Sinon la balle a sa couleur par defaut
  else
  {
    setLedColor(ballLed, PG_BALL_SIZE, PG_BALL_COLOR);
  }

  // On envoie la couleur des leds a la bande de leds
  FastLED.show();

  // On actualise la variable lastMillis pour la boucle suivante
  lastMillis = currentMillis;
}
