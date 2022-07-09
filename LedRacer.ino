/*
   Parametres Led Racer
*/
const int LR_LEDS_INTENSITY_DIV = 1;                          // Diviseur de l'intensite des leds, augmenter ce parametre pour reduire l'intensite des leds dans le led racer

const CRGB LR_PLAYER_COLORS[4]  = {VERT, ROUGE, BLEU, JAUNE}; // Couleurs des LEDs

const int   LR_MAX_TURNS        = 3;                          // Nombre de tours necessaires pour gagner
const float LR_ACCELERATION     = 0.03;                       // Correspond a la vitesse que l'on gagne en appuyant sur notre bouton
const float LR_DRAG             = 0.5;                        // Correspond aux LR_DRAG exerces sur chaque personne
const int   LR_PLAYER_SIZE      = 5;                          // Taille d'une voiture en nombre de led

/*
    Variables Led racer
*/
// Variables de fonctionnement led racer
struct LedRacerPlayer {
  bool isPresent;
  bool isPressed;

  float position;
  float speed;
  int turns;
};

LedRacerPlayer ledRacerPlayers[4] = { { 0 } };

/*
   Variables menu de selection du led racer
*/
//const float LR_SELECT_SPEED = 0.3f;

/*
   Fonctions Led racer
*/

/*void ledRacerSelectDisplay()
  {
  // Time variables computation
  unsigned long ms = millis();
  float dt = (ms - lastMillis) * 0.001f;
  lastMillis = ms;

  // Clear Led strip before printing
  FastLED.clear();

  int ledPosition ;
  for (int playerId = 0; playerId < 4; playerId++)
  {
    // Compute new positions
    ledRacerSelectPositions[playerId] += LR_SELECT_SPEED * dt;

    // Print position with mask
    ledPosition = LEDS_NB * ledRacerSelectPositions[playerId];

    for (int ledId = 0; ledId < LR_PLAYER_SIZE; ledId++)
    {
      if (ledPosition + ledId > 0.66f * LEDS_NB && ledPosition + ledId < LEDS_NB)
      {
        leds[ledPosition + ledId] = LR_PLAYER_COLORS[playerId] / (LEDS_INTENSITY_DIV * LR_LEDS_INTENSITY_DIV);
      }

    }
  }
  }*/

void ledRacerSelectLoop()
{
  // Local variables
  bool pressed;
  int ledIdx;
  int playerId;
  LedRacerPlayer* player;

  // Check if select button has been pressed
  pressed = !digitalRead(MENU_SELECT_PIN);
  if (menuSelectButton.press(pressed) == Simple)
  {
    // Start game
    ledRacerBegin();
    state = LedRacerGame;
  }

  // Check if players are pressing their controller
  for (playerId = 0; playerId < 4; playerId++)
  {
    // Select player
    player = &ledRacerPlayers[playerId];

    pressed = !digitalRead(PLAYER_BUTTON_PINS[playerId]);
    player->isPressed = pressed;

    if (playerButtons[playerId].press(pressed) == Simple)
    {
      player->isPresent = true;
    }
  }

  // Display player colors if they are present
  FastLED.clear();

  for (playerId = 0; playerId < 4; playerId++)
  {
    if (ledRacerPlayers[playerId].isPresent)
    {
      if (ledRacerPlayers[playerId].isPressed)
      {
        for (ledIdx = 0; ledIdx < LR_PLAYER_SIZE; ledIdx++)
        {
          leds[2 * playerId * LR_PLAYER_SIZE + ledIdx] = LR_PLAYER_COLORS[playerId] / (LEDS_INTENSITY_DIV * LR_LEDS_INTENSITY_DIV);
        }
      }
      else
      {
        for (ledIdx = 0; ledIdx < LR_PLAYER_SIZE; ledIdx++)
        {
          leds[2 * playerId * LR_PLAYER_SIZE + ledIdx] = LR_PLAYER_COLORS[playerId] / (4 * LEDS_INTENSITY_DIV * LR_LEDS_INTENSITY_DIV);
        }
      }
    }
  }

  FastLED.show();
}

void ledRacerGameLoop()
{
  // Time variables computation
  unsigned long ms = millis();
  float dt = (ms - lastMillis) * 0.001f;
  lastMillis = ms;

  // Boucle de controle pour tout le monde
  LedRacerPlayer* player;
  for (int playerId = 0; playerId < 4; playerId++)
  {
    player = &ledRacerPlayers[playerId];

    // Check if a player pressed its controller
    bool pressed = !digitalRead(PLAYER_BUTTON_PINS[playerId]);
    if (playerButtons[playerId].press(pressed) == Simple)
    {
      // If so, accelerate
      player->speed += LR_ACCELERATION;
    }


    // PHYSICS COMPUTATION
    // Apply drag
    player->speed -= LR_DRAG * player->speed * dt;

    // Compute position
    player->position += player->speed * dt;

    // Clip player position between 0.0 and 1.0
    if (player->position >= 1.0f)
    {
      player->position -= 1.0f;
      player->turns += 1;

      // If enough turns are done, player wins
      if (player->turns == LR_MAX_TURNS)
      {
        ledRacerVictory(playerId);
        return;
      }
    }
  }

  // Led display update
  FastLED.clear();

  int ledPosition;
  for (int playerId = 0; playerId < 4; playerId++)
  {
    player = &ledRacerPlayers[playerId];

    // Check that player is in the game
    if (player->isPresent)
    {
      ledPosition = LEDS_NB * player->position;

      // Draw the entier car depending on car size and turn nb
      for (int i = 0; i < LR_PLAYER_SIZE * (player->turns + 1); i++)
      {
        if (ledPosition + i < LEDS_NB)
        {
          leds[ledPosition + i] += LR_PLAYER_COLORS[playerId] / (2 * LEDS_INTENSITY_DIV * LR_LEDS_INTENSITY_DIV);
        }
        else // ledPosition + i >= LEDS_NB
        {
          leds[ledPosition + i - LEDS_NB] += LR_PLAYER_COLORS[playerId] / (2 * LEDS_INTENSITY_DIV * LR_LEDS_INTENSITY_DIV);
        }
      }
    }
  }

  FastLED.show();

  // Digit display update
  // Display time in seconds
  displayDigits.showNumberDecEx((ms - beginTimer) / 10);
  displayDigits.showNumberDecEx(0, 0x20, true); // Dot display
}

// Fonction appelee lorsqu'une personne gagne
void ledRacerVictory(int playerId)
{
  // On fait clignotter les leds de la couleur gagnante
  for (int i = 0; i < 3; i++)
  {
    FastLED.clear();
    fill_solid(leds, LEDS_NB, LR_PLAYER_COLORS[playerId] / (32 * LEDS_INTENSITY_DIV * LR_LEDS_INTENSITY_DIV));
    FastLED.show();
    delay(1000);

    FastLED.clear();
    FastLED.show();
    delay(1000);
  }

  // On reinitialise les variables
  state = Demo;

  for (int playerId = 0; playerId < 4; playerId++)
  {
    ledRacerPlayers[playerId].position = 0.0f;
    ledRacerPlayers[playerId].speed = 0.0f;
    ledRacerPlayers[playerId].turns = 0;
    ledRacerPlayers[playerId].isPresent = false;
    ledRacerPlayers[playerId].isPressed = false;
  }

  displayDigits.showNumberDec(0, true);
}


void ledRacerBegin()
{
  // Compte a rebours
  FastLED.clear();
  fill_solid(leds, LEDS_NB, ROUGE / (32 * LEDS_INTENSITY_DIV * LR_LEDS_INTENSITY_DIV));
  FastLED.show();
  //tone(SPEAKER_PIN, 220, 500);
  delay(500);

  FastLED.clear();
  FastLED.show();
  delay(500);

  fill_solid(leds, LEDS_NB, ROUGE / (32 * LEDS_INTENSITY_DIV * LR_LEDS_INTENSITY_DIV));
  FastLED.show();
  //tone(SPEAKER_PIN, 220, 500);
  delay(500);

  FastLED.clear();
  FastLED.show();
  delay(500);

  fill_solid(leds, LEDS_NB, ORANGE / (32 * LEDS_INTENSITY_DIV * LR_LEDS_INTENSITY_DIV));
  FastLED.show();
  //tone(SPEAKER_PIN, 220, 500);
  delay(500);

  FastLED.clear();
  FastLED.show();
  delay(500);

  fill_solid(leds, LEDS_NB, VERT / (32 * LEDS_INTENSITY_DIV * LR_LEDS_INTENSITY_DIV));
  FastLED.show();
  //tone(SPEAKER_PIN, 440, 500);
  delay(200);

  lastMillis = millis();
}
