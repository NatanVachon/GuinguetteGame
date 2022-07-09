/*
   Parametres mode Demo
*/
const int DEMO_LEDS_INTENSITY_DIV = 2;     // Diviseur de l'intensite des leds, augmenter ce parametre pour reduire l'intensite du mode demo

/*
 * Fonctions mode Demo
 */

void demoLoop()
{
  // Demo animation
  // REMPLACER ICI SI TU VEUX CHANGER D'ANIMATION
  demoRainbowLoop();

  // Check if a menu button is pressed
  bool pressed = !digitalRead(MENU_SELECT_PIN);
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
