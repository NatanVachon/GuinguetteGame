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
const int LEDS_PIN              = 13;               // Pin de donnees de la bande de led
const int PLAYER_BUTTON_PINS[4] = {32, 33, 25, 26}; // Pins des boutons {PLAYER1, PLAYER2, PLAYER3, PLAYER4}
const int MENU_SELECT_PIN       = 14;               // Pin du bouton de selection dans le menu
const int MENU_CHANGE_PIN       = 12;               // Pin du bouton de navigation dans le menu
const int DISPLAY_CLK_PIN       = 22;               // Pin CLK de l'afficheur
const int DISPLAY_DIO_PIN       = 23;               // Pin DIO de l'afficheur

/*
   Parametres generaux
*/
const int LEDS_NB                 = 300;  // Nombre de LEDs sur la guirlande
const int LEDS_INTENSITY_DIV      = 1;     // Diviseur de l'intensite des leds, augmenter ce parametre pour reduire l'intensite globale

/*
   Variables de fonctionnement du jeu
*/

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
  displayDigits.setBrightness(0x0f);
  displayDigits.showNumberDec(0, true);
}

// Continuously called function
void loop() {
  // Local variables definition
  bool pressed;

  switch (state)
  {
    case Demo:
      demoLoop();
      break;

    case GameChoice:
      // Check if select button has been pressed
      pressed = !digitalRead(MENU_SELECT_PIN);
      if (menuSelectButton.press(pressed) == Simple)
      {
        // Update last millis and beginTimer variable before launching game
        lastMillis = millis();
        beginTimer = lastMillis;

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
