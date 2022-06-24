/*******************************
     PARAMETRES TIR A LA CORDE
 *******************************/
const float TAC_INCREMENT = 0.05;   // De combien avance la corde lorsqu'on appuie sur le bouton
const float TAC_ROPE_SMOOTH = 1.0;  // Parametre servant a lisser la position de la corde

const float TAC_WAVE_LENGTH = 0.2;  // Taille des ondes
const float TAC_WAVE_SPEED =  1.0;  // Vitesse des ondes

const CRGB TAC_PLAYER_COLORS[2] = {CYAN, ROSE};

/*******************************************************
      VARIABLES DE FONCTIONNEMENT DU TIR A LA CORDE
 *******************************************************/
float ropePosition = 0.5;           // Position non lissee de la corde
float displayedRopePosition = 0.5;  // Position lissee de la corde

// Representation des ondes lorsqu'on appuye sur un bouton
float player1Waves[10] = { 0 };
float player2Waves[10] = { 0 };

const int TAC_PLAYER1 = 0;
const int TAC_PLAYER2 = 1;


void tacGameLoop()
{
  // Calcul des temps
  unsigned long ms = millis();
  float dt = (ms - lastMillis) * 0.001f;
  lastMillis = ms;

  // Si moins d'une seconde s'est ecoule depuis le debut, on ne fait rien (pour eviter d'appuyer sans faire expres)
  if (ms - beginTimer < 1000)
    return;

  bool player1Push = !digitalRead(PLAYER_BUTTON_PINS[0]); // TODO peut etre faire deux equipes
  bool player2Push = !digitalRead(PLAYER_BUTTON_PINS[1]);

  if (playerButtons[0].press(player1Push))
  {
    // On incremente la position de la corde
    ropePosition += TAC_INCREMENT;

    // On lance une nouvelle onde
    for (int i = 0; i < 10; i++)
    {
      if (player1Waves[i] == 0.0f)
      {
        player1Waves[i] = 0.01f;
        break;
      }
    }
  }
  if (playerButtons[1].press(player2Push))
  {
    // On incremente la position de la corde
    ropePosition -= TAC_INCREMENT;

    // On lance une nouvelle onde
    for (int i = 0; i < 10; i++)
    {
      if (player2Waves[i] == 0.0f)
      {
        player2Waves[i] = 0.99f;
        break;
      }
    }
  }




  // On calcule la position lissee de la corde (displayedRopePosition correspond a la variable ropePosition avec un lissage supplementaire)
  displayedRopePosition += (ropePosition - displayedRopePosition) * dt / TAC_ROPE_SMOOTH;

  // On regarde si player 1 a gagne
  if (displayedRopePosition >= 1.0f)
  {
    lastWinner = TAC_PLAYER1;
    state = Demo;
    beginTimer = millis();

    ropePosition = 0.5f;
    displayedRopePosition = 0.5f;

    FastLED.clear();
    return;
  }
  // On regarde si player 2 a gagne
  else if (displayedRopePosition <= 0.0f)
  {
    lastWinner = TAC_PLAYER2;
    state = Demo;
    beginTimer = millis();

    ropePosition = 0.5f;
    displayedRopePosition = 0.5f;

    FastLED.clear();
    return;
  }

  // On propage les ondes
  float dx = TAC_WAVE_SPEED * dt;

  for (int wave = 0; wave < 10; wave++)
  {
    // Ondes player 1
    if (player1Waves[wave] != 0.0f)
    {
      player1Waves[wave] += dx;

      // Si toute l'onde depasse le point de la corde, on l'arrete
      if (player1Waves[wave] - TAC_WAVE_LENGTH > displayedRopePosition)
      {
        player1Waves[wave] = 0.0f;
      }
    }

    // Ondes player 2
    if (player2Waves[wave] != 0.0f)
    {
      player2Waves[wave] -= dx;

      // Si toute l'onde depasse le point de la corde, on l'arrete
      if (player2Waves[wave] + TAC_WAVE_LENGTH < displayedRopePosition)
      {
        player2Waves[wave] = 0.0f;
      }
    }
  }

  // On actualise la bande de leds
  float ledLuminosity;
  float ledPosition;

  FastLED.clear();

  // On commence par calculer le numero de la led correspondant a la position de la corde
  int ropeLedPosition = LEDS_NB * displayedRopePosition;

  // On dessine la zone du player 1
  // Pour cela on calcule la luminosite led par led
  for (int led = 0; led < ropeLedPosition; led++)
  {
    // La luminosite minimale d'une led est 10% de la luminosite max
    ledLuminosity = 0.1f;
    ledPosition = float(led) / float(LEDS_NB);

    // Pour chaque onde, on ajoute la luminosite a la led
    for (int wave = 0; wave < 10; wave++)
    {
      // Si l'onde n'est pas active ou si l'onde est avant la led, on n'ajoute aucune luminosite
      if (player1Waves[wave] == 0.0f || player1Waves[wave] < ledPosition)
        continue;

      // On ajoute de la luminosite de facon decroissante. plus l'onde est loin, moins on ajoute de luminosite.
      ledLuminosity += max(0.0f, 0.9f - (player1Waves[wave] - ledPosition) / TAC_WAVE_LENGTH);
    }
    // La valeur maximale de luminosite est 1.0
    if (ledLuminosity > 1.0f)
      ledLuminosity = 1.0f;

    // On actualise la valeur de luminosite de la led
    leds[led] = TAC_PLAYER_COLORS[0] / int(1.0f / ledLuminosity) / 5;
  }

  // On dessine la zone du player 2
  for (int led = ropeLedPosition + 1; led < LEDS_NB; led++)
  {
    // La luminosite minimale d'une led est 10% de la luminosite max
    ledLuminosity = 0.1f;
    ledPosition = float(led) / float(LEDS_NB);

    // Pour chaque onde, on ajoute la luminosite a la led
    for (int wave = 0; wave < 10; wave++)
    {
      // Si l'onde n'est pas active ou si l'onde est avant la led, on n'ajoute aucune luminosite
      if (player2Waves[wave] == 0.0f || ledPosition > player2Waves[wave])
        continue;

      // On ajoute de la luminosite de facon decroissante. plus l'onde est loin, moins on ajoute de luminosite.
      ledLuminosity += max(0.0f, 0.9f - (player2Waves[wave] - ledPosition) / TAC_WAVE_LENGTH);
    }
    // La valeur maximale de luminosite est 1.0
    if (ledLuminosity > 1.0f)
      ledLuminosity = 1.0f;

    // On actualise la valeur de luminosite de la led
    leds[led] = TAC_PLAYER_COLORS[1] / int(1.0f / ledLuminosity) / 5;
  }

  // On actualise la led correspondant a la position de la corde
  leds[ropeLedPosition] = BLANC;
  FastLED.show();
}
