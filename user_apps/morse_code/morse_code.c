#include <gpiod.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

// GPIO setup
#define GPIO_CHIP_NAME "gpiochip1"
#define GPIO_LINE_LED 23

// Morse code timing (in microseconds)
// 1 time unit = 0.2 s
#define TIME_UNIT 200000
#define DOT_DURATION TIME_UNIT        // Duration of dot (1 time unit)
#define DASH_DURATION (3 * TIME_UNIT) // Duration of dash (3 time units)
#define SYMBOL_SPACE TIME_UNIT        // Space between symbols (1 time unit)
#define LETTER_SPACE (3 * TIME_UNIT)  // Space between letters (3 time units)
#define WORD_SPACE (7 * TIME_UNIT)    // Space between words (7 time units)

// Morse code dictionary for A-Z and 0-9
const char *morse_code[] = {
    ".-",    "-...",  "-.-.",  "-..",   ".",
    "..-.",  "--.",   "....",  "..", // A-I
    ".---",  "-.-",   ".-..",  "--",    "-.",
    "---",   ".--.",  "--.-",  ".-.", // J-R
    "...",   "-",     "..-",   "...-",  ".--",
    "-..-",  "-.--",  "--..", // S-Z
    "-----", ".----", "..---", "...--", "....-",
    ".....", "-....", "--...", "---..", "----." // 0-9
};

// Function to get Morse code for a character
const char *get_morse_code(char c) {
  if (c >= 'A' && c <= 'Z')
    return morse_code[c - 'A'];
  if (c >= 'a' && c <= 'z')
    return morse_code[c - 'a'];
  if (c >= '0' && c <= '9')
    return morse_code[c - '0' + 26];
  return ""; // Unsupported characters are ignored
}

// Function to blink LED for a given Morse symbol
void blink_symbol(struct gpiod_line *led_line, char symbol) {
  int duration = (symbol == '.') ? DOT_DURATION : DASH_DURATION;

  // Turn LED on
  gpiod_line_set_value(led_line, 1);
  usleep(duration);

  // Turn LED off and add space between symbols
  gpiod_line_set_value(led_line, 0);
  usleep(SYMBOL_SPACE);
}

int main() {
  struct gpiod_chip *chip = gpiod_chip_open_by_name(GPIO_CHIP_NAME);
  if (!chip) {
    perror("Open GPIO chip failed");
    return 1;
  }

  struct gpiod_line *led_line = gpiod_chip_get_line(chip, GPIO_LINE_LED);
  if (!led_line) {
    perror("Get GPIO line failed");
    gpiod_chip_close(chip);
    return 1;
  }

  if (gpiod_line_request_output(led_line, "morse_code", 0) < 0) {
    perror("Request GPIO line as output failed");
    gpiod_chip_close(chip);
    return 1;
  }

  while (1) {
    char text[256];
    printf("Enter text to convert to Morse code (empty line to exit): ");
    fgets(text, sizeof(text), stdin);

    // Check for empty line to exit
    if (text[0] == '\n')
      break;

    // Process each character in the sentence
    for (size_t i = 0; i < strlen(text); ++i) {
      char ch = text[i];

      // Handle spaces (word separator)
      if (ch == ' ') {
        usleep(WORD_SPACE);
        continue;
      }

      const char *code = get_morse_code(ch);
      if (*code == '\0')
        continue; // Skip unsupported characters

      // Blink LED for each Morse code symbol (dot/dash) in the letter
      for (const char *symbol = code; *symbol != '\0'; ++symbol) {
        blink_symbol(led_line, *symbol);
      }

      // Space between letters
      usleep(LETTER_SPACE);
    }
  }

  gpiod_line_release(led_line);
  gpiod_chip_close(chip);
  return 0;
}
