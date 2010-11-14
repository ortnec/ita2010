#include <msp430x20x3.h>

#define UNIT_DELAY 100000
#define DOT_DELAY UNIT_DELAY
#define DASH_DELAY (UNIT_DELAY * 3)
#define LETTER_DELAY (UNIT_DELAY * 3)
#define SPACE_DELAY (UNIT_DELAY * 7)
#define ON 1
#define OFF 0
#define LED1 BIT0
#define LED2 BIT6

struct Mapping {
  const char letter;
  const char pattern[6];
};

const struct Mapping map[] = {
  { 'A', ".-"     }, { 'B', "-..."  }, { 'C', "-.-."  },
  { 'D', "-.."    }, { 'E', "."     }, { 'F', "..-."  },
  { 'G', "--."    }, { 'H', "...."  }, { 'I', ".."    },
  { 'J', ".---"   }, { 'K', ".-.-"  }, { 'L', ".-.."  },
  { 'M', "--"     }, { 'N', "-."    }, { 'O', "---"   },
  { 'P', ".--."   }, { 'Q', "--.-"  }, { 'R', ".-."   },
  { 'S', "..."    }, { 'T', "-"     }, { 'U', "..-"   },
  { 'V', "...-"   }, { 'W', ".--"   }, { 'X', "-..-"  },
  { 'Y', "-.--"   }, { 'Z', "--.."  }, { '1', ".----" },
  { '2', "..---"  }, { '3', "...--" }, { '4', "....-" },
  { '5', "....."  }, { '6', "-...." }, { '7', "--..." },
  { '8', "---.."  }, { '9', "----." }, { '0', "-----" },
  { '.', ".-.-.-" }, { '/', "-.--." }, { '-', "-...-" }
};

const char message1[] = { "#VCC TO PIN 1.4" };
const char message2[] = { "#TINYURL.COM/37C5S8E" };

void toggleLed(int led, int state) {
  if (state) {
    P1OUT |= led;
  } else {
    P1OUT &= ~led;
  }
}

void delay(unsigned long int cycles) {
  do (cycles--);
  while (cycles != 0);
}

void printHash() {
  toggleLed(LED1, ON);
  delay(LETTER_DELAY);
  toggleLed(LED1, OFF);
  delay(UNIT_DELAY);
}

void printMorse(char c) {
  int i;
  char *code = 0;

  for(i = 0; i < 36; i++) {
    if (c == map[i].letter) {
      code = (char *) map[i].pattern;
      break;
    }
  }

  if (code == 0) { return; }

  for(i = 0; i < 5; i++) {
    if (code[i] == '\0') { return; }

    toggleLed(LED2, ON);
    delay(code[i] == '.' ? DOT_DELAY : DASH_DELAY);
    toggleLed(LED2, OFF);
    delay(UNIT_DELAY);
  }
}

int main(void) {
  char *currentMessage = (char *) message1,
       *currentChar = (char *) message1,
       currentState = 0x00, state;

  WDTCTL = WDTPW + WDTHOLD;                 // Stop watchdog timer
  P1DIR |= LED1 | LED2;                     // Set P1.0 & P1.6 to output

  toggleLed(LED1, OFF);
  toggleLed(LED2, OFF);

  for (;;) {
    switch(*currentChar) {
      case '#':
        printHash();
        break;
      case ' ':
        delay(SPACE_DELAY);
        break;
      case '\0':
        currentChar = (char *) currentMessage - 1; // -1 since we += 1 below
        break;
      default:
        printMorse(*currentChar);
        delay(LETTER_DELAY);
        break;
    }
    currentChar += 1;

    state = P1IN & BIT4;
    if (state && (currentState != state)) { // P1.4 hi
      currentState = state;
      currentMessage = currentChar = (char *) message2;
    } else if (!state && (currentState != state)) { // P1.4 lo
      currentState = state;
      currentMessage = currentChar = (char *) message1;
    }
  }

  return 0;
}
