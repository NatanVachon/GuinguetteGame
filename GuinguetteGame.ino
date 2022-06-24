#include <FastLED.h>
#include <eButton.h>
#include <TM1637Display.h>

/*
   Definition des couleurs
*/
const CRGB NOIR   = CRGB(0,   0,   0  );
const CRGB BLANC  = CRGB(255, 255, 255);
const CRGB ROUGE  = CRGB(255, 0,   0  );
const CRGB ORANGE = CRGB(255, 85,  0  );
const CRGB VERT   = CRGB(0,   255, 0  );
const CRGB BLEU   = CRGB(0,   0,   255);
const CRGB JAUNE  = CRGB(255, 255, 0  );
const CRGB ROSE   = CRGB(255, 0,   255);
const CRGB CYAN   = CRGB(0,   255, 255);


/*
   Definition des branchements
*/
const int LEDS_PIN       = 13;                      // Pin de donnees de la bande de led
const int PLAYER_BUTTON_PINS[4] = {32, 33, 25, 26}; // Pins des boutons {PLAYER1, PLAYER2, PLAYER3, PLAYER4}
const int MENU_SELECT_PIN = 14;                     // Pin du bouton de selection dans le menu
const int MENU_CHANGE_PIN = 12;                     // Pin du bouton de navigation dans le menu
const int DISPLAY_CLK_PIN = 22;                     // Pin CLK de l'afficheur
const int DISPLAY_DIO_PIN = 23;                     // Pin DIO de l'afficheur

/*
   Parametres generaux
*/
const int LEDS_NB = 1800;  // Nombre de LEDs sur la guirlande

/*
   Variables de fonctionnement du jeu
*/
// Variable utilitaire utilisee pour l'affichage
const uint8_t LED_STRIP_MASK[300] = { 255,   255,   255,   255,   255,   255,   255,   255,   255,   255,   255,   255,   251,   214,   185,   161,   141,   125,   112,   100,   90,   82,   75,   68,   63,   58,   53,   50,   46,   43,   40,   38,   35,   33,   31,   29,   28,   26,   25,   24,   22,   21,   20,   19,   19,   18,   17,   16,   16,   15,   14,   14,   13,   13,   12,   12,   11,   11,   11,   10,   10,   10,   9,   9,   9,   8,   8,   8,   8,   7,   7,   7,   7,   7,   6,   6,   6,   6,   6,   6,   6,   5,   5,   5,   5,   5,   5,   5,   5,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1 };

// Etats du jeu
enum State {
  Demo,
  GameChoice,
  LedRacerPlayerSelect,
  LedRacerGame,
  PongGame,
  TacGame
};

State state = Demo;

// Variables de fonctionnement des LEDs
CRGB leds[LEDS_NB];
int counter = 0;

// Variable de fonctionnement de l'afficheur
TM1637Display displayDigits(DISPLAY_CLK_PIN, DISPLAY_DIO_PIN);

// Variables de fonctionnement des boutons
eButton menuSelectButton;
eButton menuChangeButton;

eButton playerButtons[4];

// Variables de fonctionnement du menu
enum Games {
  LedRacer,
  Pong,
  Tac
};
Games selectedGame = LedRacer;

// Variables temporelles
unsigned long lastMillis = 0;
unsigned long beginTimer = 0;




// Fonction appelee au demarage de l'arduino
void setup() {
  Serial.print(LED_STRIP_MASK[0]);
  // Debugging
  Serial.begin(115200);

  // Initialisation des boutons
  pinMode(MENU_SELECT_PIN, INPUT_PULLUP);
  pinMode(MENU_CHANGE_PIN, INPUT_PULLUP);

  pinMode(PLAYER_BUTTON_PINS[0], INPUT_PULLUP);
  pinMode(PLAYER_BUTTON_PINS[1], INPUT_PULLUP);
  pinMode(PLAYER_BUTTON_PINS[2], INPUT_PULLUP);
  pinMode(PLAYER_BUTTON_PINS[3], INPUT_PULLUP);

  // Initialisation des LEDs
  pinMode(LEDS_PIN, OUTPUT);
  FastLED.addLeds<WS2812B, LEDS_PIN, GRB>(leds, LEDS_NB);

  // Initialisation de l'afficheur
  //displayDigits.init();
  //displayDigits.set(BRIGHT_TYPICAL);

}

// Fonction appelee en continu
void loop() {
  // Local variables definition
  bool pressed;
  int ledIdx;

  switch (state)
  {
    case Demo:
      // Create basic rainbow patern
      fill_rainbow(leds, LEDS_NB, counter++, 7);

      // Reduce brightness
      for (ledIdx = 0; ledIdx < LEDS_NB; ledIdx++)
      {
        leds[ledIdx] /= 32;
      }

      FastLED.show();

      // Check if a menu button is pressed
      pressed = !digitalRead(MENU_SELECT_PIN);
      if (menuSelectButton.press(pressed) == Simple)
      {
        // Go to game selection
        state = GameChoice;
      }
      break;

    case GameChoice:
      // Check if select button has been pressed
      pressed = !digitalRead(MENU_SELECT_PIN);
      if (menuSelectButton.press(pressed) == Simple)
      {
        // Update last millis variable before launching game
        lastMillis = millis();
        
        switch (selectedGame)
        {
          case LedRacer:
            // Go to led racer player selection
            state = LedRacerPlayerSelect;
            break;

          case Pong:
            state = PongGame;
            break;

          case Tac:
            state = TacGame;
            break;
        }
      }

      // Check if change menu button has been pressed
      pressed = !digitalRead(MENU_CHANGE_PIN);
      if (menuChangeButton.press(pressed) == Simple)
      {
        switch (selectedGame)
        {
          case LedRacer:
            selectedGame = Pong;
            break;

          case Pong:
            selectedGame = Tac;
            break;

          case Tac:
            selectedGame = LedRacer;
            break;
        }

        Serial.print("Selected game: "); Serial.println(selectedGame);
      }
      break;

    case LedRacerPlayerSelect:
      ledRacerSelectLoop();
      break;

    case LedRacerGame:
      ledRacerGameLoop();
      break;

    case PongGame:
      pongGameLoop();
      break;

    case TacGame:
      tacGameLoop();
      break;
  }
}
