#include <avr/pgmspace.h>

#include "lcd.h"

int main(void) {
  LCD_Init();

  // LCD_puts_f(PSTR("AVR BUTTERFLY GCC"));
  LCD_puts("AVR GCC");

  for (;;) {
  }

  return 0;
}
