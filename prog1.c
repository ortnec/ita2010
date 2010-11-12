#include <msp430x20x3.h>

#define DELAY 150000
#define QUARTER_DELAY (DELAY / 4)
#define DOUBLE_DELAY (DELAY * 2)
#define QUAD_DELAY (DELAY * 4)
#define ON 1
#define OFF 0
#define LED1 BIT0
#define LED2 BIT6

struct Mapping {
  const char letter;
  const char pattern[5];
};

const struct Mapping map[] = {
  { 'A', ".-"    }, { 'B', "-..."  }, { 'C', "-.-."  },
  { 'D', "-.."   }, { 'E', "."     }, { 'F', "..-."  },
  { 'G', "--."   }, { 'H', "...."  }, { 'I', ".."    },
  { 'J', ".---"  }, { 'K', ".-.-"  }, { 'L', ".-.."  },
  { 'M', "--"    }, { 'N', "-."    }, { 'O', "---"   },
  { 'P', ".--."  }, { 'Q', "--.-"  }, { 'R', ".-."   },
  { 'S', "..."   }, { 'T', "-"     }, { 'U', "..-"   },
  { 'V', "...-"  }, { 'W', ".--"   }, { 'X', "-..-"  },
  { 'Y', "-.--"  }, { 'Z', "--.."  }, { '1', ".----" },
  { '2', "..---" }, { '3', "...--" }, { '4', "....-" },
  { '5', "....." }, { '6', "-...." }, { '7', "--..." },
  { '8', "---.." }, { '9', "----." }, { '0', "-----" }
};

const char message1[] = { "# VCC TO P14" };
const char message2[] = { "# TINYURL 37C5S8E" };

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
  delay(QUAD_DELAY);
  toggleLed(LED1, OFF);
  delay(DELAY);
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
    delay(code[i] == '.' ? QUARTER_DELAY : DOUBLE_DELAY);
    toggleLed(LED2, OFF);
    delay(DELAY);
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
        delay(DOUBLE_DELAY);
        break;
      case '\0':
        currentChar = (char *) currentMessage - 1; // -1 since we += 1 below
        break;
      default:
        printMorse(*currentChar);
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
    } else {
      delay(DELAY);
    }
  }

  return 0;
}
