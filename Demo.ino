/*
   Parametres mode Demo
*/
const int DEMO_LEDS_INTENSITY_DIV = 2;     // Diviseur de l'intensite des leds, augmenter ce parametre pour reduire l'intensite du mode demo

/*
   Variables mode Demo
 */
unsigned long demoLastWake = 0;
int demoFunctionIndex = 0;

/*
   Fonctions mode Demo
*/

void demoLoop()
{
  // Check for change button hit
  bool pressed = !digitalRead(MENU_CHANGE_PIN);
  if (menuChangeButton.press(pressed) == Simple)
  {
    demoFunctionIndex += 1;
    if (demoFunctionIndex > 1)
      demoFunctionIndex = 0;

    // Reset leds
    FastLED.clear();
  }

  // Demo animation
  switch (demoFunctionIndex)
  {
    case 0:
      demoRainbowLoop();
      break;
      
    case 1:
      demoCylonLoop();
      break;
  }

  // Check if a menu button is pressed
  pressed = !digitalRead(MENU_SELECT_PIN);
  if (menuSelectButton.press(pressed) == Simple)
  {
    // Go to game selection
    state = GameChoice;
  }
}

void demoRainbowLoop()
{
  // Create basic rainbow patern
  fill_rainbow(leds, LEDS_NB, counter++, 7);

  // Reduce brightness
  for (int ledIdx = 0; ledIdx < LEDS_NB; ledIdx++)
  {
    leds[ledIdx] /= 16 * (LEDS_INTENSITY_DIV * DEMO_LEDS_INTENSITY_DIV);
  }

  // Display leds
  FastLED.show();
}

void demoCylonLoop()
{
  static int dir = 1;
  static int i = 0;

  unsigned long ms = millis();

  if (ms - demoLastWake > 10)
  {
    // First slide the led in one direction
    if (dir == 1)
    {
      // Set the i'th led to red
      leds[i] = CHSV(counter++, 255, 255);
      // Show the leds
      FastLED.show();
      // now that we've shown the leds, reset the i'th led to black
      for (int i = 0; i < LEDS_NB; i++) {
        leds[i].nscale8(250);
      }

      // Increment i
      i += 1;
      if (i == LEDS_NB - 1)
      {
        // change dir
        dir = 0;
      }
    }

    // Now go in the other direction.
    else
    {
      // Set the i'th led to red
      leds[i] = CHSV(counter++, 255, 255);
      // Show the leds
      FastLED.show();
      // now that we've shown the leds, reset the i'th led to black
      for (int i = 0; i < LEDS_NB; i++) {
        leds[i].nscale8(250);
      }

      // Decrement i
      i -= 1;
      if (i == 0)
      {
        // change dir
        dir = 1;
      }
    }

    demoLastWake = ms;
  }
}
