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
      player->speed += ACCELERATION;
    }


    // PHYSICS COMPUTATION
    // Apply drag
    player->speed -= FROTTEMENTS * player->speed * dt;

    // Compute position
    player->position += player->speed * dt;

    // Clip player position between 0.0 and 1.0
    if (player->position >= 1.0f)
    {
      player->position -= 1.0f;
      player->turns += 1;

      // If enough turns are done, player wins
      if (player->turns == MAX_TURNS)
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
      for (int i = 0; i < PLAYER_SIZE * (player->turns + 1); i++)
      {
        if (ledPosition + i < LEDS_NB)
        {
          leds[ledPosition + i] += playerColor[playerId] / 2;
        }
        else // ledPosition + i >= LEDS_NB
        {
          leds[ledPosition + i - LEDS_NB] += playerColor[playerId] / 2;
        }
      }
    }
  }

  FastLED.show();
}

// Fonction appelee lorsqu'une personne gagne
void ledRacerVictory(int playerId)
{
  // On fait clignotter les leds de la couleur gagnante
  for (int i = 0; i < 3; i++)
  {
    FastLED.clear();
    fill_solid(leds, LEDS_NB, playerColor[playerId] / 50);
    FastLED.show();
    delay(500);

    FastLED.clear();
    FastLED.show();
    delay(500);
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
}


void ledRacerBegin()
{
  // Compte a rebours
  FastLED.clear();
  fill_solid(leds, LEDS_NB, ROUGE / 50);
  FastLED.show();
  //tone(SPEAKER_PIN, 220, 500);
  delay(500);

  FastLED.clear();
  FastLED.show();
  delay(500);

  fill_solid(leds, LEDS_NB, ROUGE / 50);
  FastLED.show();
  //tone(SPEAKER_PIN, 220, 500);
  delay(500);

  FastLED.clear();
  FastLED.show();
  delay(500);

  fill_solid(leds, LEDS_NB, ORANGE / 50);
  FastLED.show();
  //tone(SPEAKER_PIN, 220, 500);
  delay(500);

  FastLED.clear();
  FastLED.show();
  delay(500);

  fill_solid(leds, LEDS_NB, VERT / 50);
  FastLED.show();
  //tone(SPEAKER_PIN, 440, 500);
  delay(200);

  lastMillis = millis();
}
