/* screen.c
   avr32
   aleph
*/


// std 
/// FIXME: eliminate!!
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
// ASF
#include "gpio.h"
#include "usart.h"
#include "util.h"
#include "intc.h"
#include "print_funcs.h"
// aleph
#include "conf_aleph.h"
#include "fix.h"
#include "font.h"
#include "global.h"
#include "screen.h"

//-----------------------------
//---- variables
const U8 lines[CHAR_ROWS] = { 0, 8, 16, 24, 32, 40, 48, 56 };

// screen buffer
static U8 screen[GRAM_BYTES];

//-----------------------------
//---- static functions
static void write_data(U8 c);
static void write_data(U8 c) {
  usart_spi_selectChip(OLED_USART_SPI);
  // pull register select high to write data
  gpio_set_gpio_pin(OLED_REGISTER_PIN);
  usart_putchar(OLED_USART_SPI, c);
  usart_spi_unselectChip(OLED_USART_SPI);
}

static void write_command(U8 c);
static void write_command(U8 c) {
  usart_spi_selectChip(OLED_USART_SPI);
  // pull register select low to write a command
  gpio_clr_gpio_pin(OLED_REGISTER_PIN);
  usart_putchar(OLED_USART_SPI, c);
  usart_spi_unselectChip(OLED_USART_SPI);
}

// fill one column in a line of text with blank pixels (for spacing)
static void zero_col(U16 x, U16 y);
static void zero_col(U16 x, U16 y) {
  static U8 i;
  for(i=0; i<FONT_CHARH; i++) {
    screen_pixel(x, y+i, 0);
  }
}

//------------------
// external functions
void init_oled(void) {
  U32 i;
  volatile u64 delay;
  //  cpu_irq_disable();
  Disable_global_interrupt();
  // flip the reset pin
  gpio_set_gpio_pin(OLED_RESET_PIN);
  //  delay_ms(1);
  delay = FCPU_HZ >> 10 ; while(delay > 0) { delay--; }
  gpio_clr_gpio_pin(OLED_RESET_PIN);
  // delay_ms(1);
  delay=FCPU_HZ >> 10; while(delay > 0) { delay--; }
  gpio_set_gpio_pin(OLED_RESET_PIN);
  //delay_ms(10);
  delay = FCPU_HZ >> 8; while(delay > 0) { delay--; }
  //// initialize OLED
  write_command(0xAE);	// off
  write_command(0xB3);	// clock rate
  write_command(0x91);
  write_command(0xA8);	// multiplex
  write_command(0x3F);
  write_command(0x86);	// full current range
  write_command(0x81);	// contrast to full
  write_command(0x7F);
  write_command(0xB2);	// frame freq
  write_command(0x51);
  write_command(0xA8);	// multiplex
  write_command(0x3F);
  write_command(0xBC);	// precharge
  write_command(0x10);
  write_command(0xBE);	// voltage
  write_command(0x1C);
  write_command(0xAD);	// dcdc
  write_command(0x02);
  write_command(0xA0);	// remap
  write_command(0x50);
  write_command(0xA1);	// start
  write_command(0x0);
  write_command(0xA2);	// offset
  write_command(0x4C);
  write_command(0xB1);	// set phase
  write_command(0x55);
  write_command(0xB4);	// precharge
  write_command(0x02);
  write_command(0xB0);	// precharge
  write_command(0x28);
  write_command(0xBF);	// vsl
  write_command(0x0F);
  write_command(0xA4);	// normal display
	
  write_command(0xB8);		// greyscale table
  write_command(0x01);
  write_command(0x11);
  write_command(0x22);
  write_command(0x32);
  write_command(0x43);
  write_command(0x54);
  write_command(0x65);
  write_command(0x76);	
		
  // set update box (to full screen)
  write_command(0x15);
  write_command(0);
  write_command(63);
  write_command(0x75);
  write_command(0);
  write_command(63);
		
  // clear OLED RAM 
  for(i=0; i<GRAM_BYTES; i++) { write_data(0); }
  write_command(0xAF);	// on

  //  delay_ms(10) 
  delay = FCPU_HZ >> 8; while(delay > 0) { delay--; }
  //  cpu_irq_enable();
  Enable_global_interrupt();

}

// draw a single pixel
void screen_pixel(U16 x, U16 y, U8 a) {
  static U32 pos;
  // if (x >= NCOLS) return;
  // if (y >= NROWS) return;
  pos = (y * NCOLS__2) + (x>>1);
  if (x%2) {
    screen[pos] &= 0x0f;
    screen[pos] |= (a << 4);
  } else {
    screen[pos] &= 0xf0;
    screen[pos] |= (a & 0x0f);
  }
}

// get value of pixel
U8 screen_get_pixel(U8 x, U8 y) {
  static U32 pos;
  // if (x >= NCOLS) return;
  // if (y >= NROWS) return;
  pos = (y * NCOLS__2) + (x>>1);
  if (x%2) {
    return (screen[pos] & 0xf0) >> 4; 
   } else {
    return screen[pos] & 0x0f;
  }
}

// draw a single character glyph with fixed spacing
U8 screen_char_fixed(U16 col, U16 row, char gl, U8 a) {
  static U8 x, y;
  for(y=0; y<FONT_CHARH; y++) {
    for(x=0; x<FONT_CHARW; x++) {
      if( (font_data[gl - FONT_ASCII_OFFSET].data[x] & (1 << y))) {
	screen_pixel(x+col, y+row, a);
      } else {
	screen_pixel(x+col, y+row, 0);
      }
    }
  }
  return x+1;
}

// draw a single character glyph with proportional spacing
U8 screen_char_squeeze(U16 col, U16 row, char gl, U8 a) {
  static U8 y, x;
  static U8 xnum;
  static const glyph_t * g;
  g = &(font_data[gl - FONT_ASCII_OFFSET]);
  xnum = FONT_CHARW - g->first - g->last;
  
  for(y=0; y<FONT_CHARH; y++) {
    for(x=0; x<xnum; x++) {
      if( (g->data[x + g->first] & (1 << y))) {
	screen_pixel(x + col, y + row, a);
      } else {
	screen_pixel(x + col, y + row, 0);
      }
    }
  }
  return xnum;
}


// draw a string with fixed spacing
U8 screen_string_fixed(U16 x, U16 y, char *str, U8 a) {
  while(*str != 0) {
    x += screen_char_fixed(x, y, *str, a) + 1;
    str++;
  }
  return x;
}

// draw a string with proportional spacinvg
U8 screen_string_squeeze(U16 x, U16 y, char *str, U8 a) {
  while(*str != 0) {
    x += screen_char_squeeze(x, y, *str, a);
    zero_col(x, y);
    // extra pixel... TODO: maybe variable spacing here
    x++;
    str++;
  }
  refresh = 1;
  return x;
}

// draw a string (default) m
inline U8 screen_string(U16 x, U16 y, char *str, U8 a) {
  return screen_string_squeeze(x, y, str, a);
}

// print a formatted integer
U8 screen_int(U16 x, U16 l, S16 i, U8 a) {
  static u8 y;
  y = lines[l];
  //  static char buf[32];
  //  snprintf(buf, 32, "%d", (int)i);
  static char buf[FIX_DIG_TOTAL];
  //snprintf(buf, 32, "%.1f", (float)f);
  //  print_fix16(buf, (u32)i << 16 );
  itoa_whole(i, buf, 5);
  //buf = ultoa(int);
  return screen_string_squeeze(x, y, buf, a);
}

// print a formatted float
/*
U8 screen_float(U16 x, U16 y, F32 f, U8 a) {
  static char buf[32];
  snprintf(buf, 32, "%.1f", (float)f);
  return screen_string_squeeze(x, y, buf, a);
}
*/
// print a formatted fix_t
U8 screen_fix(U16 x, U16 y, fix16_t v, U8 a) {
  static char buf[FIX_DIG_TOTAL];
  //snprintf(buf, 32, "%.1f", (float)f);
  print_fix16(buf, v);
  return screen_string_squeeze(x, y, buf, a);
}

// send screen buffer contents to OLED
void screen_refresh(void) {
  U16 i;
  //  cpu_irq_disable();
  //  Disable_global_interrupt();
  for(i=0; i<GRAM_BYTES; i++) { 
    write_data(screen[i]);  
    //write_data(i % 0xf);
  }
  //  cpu_irq_enable();
  //  Enable_global_interrupt();
}

// fill a line with blank space to end
void screen_blank_line(U16 x, U16 y) {
  U8 i, j;
  for(i=x; i<NCOLS; i++) {
    for(j=y; j<(FONT_CHARH + y); j++) {
      screen_pixel(i, j, 0);
    }
  }
 }

// highlight a line
void screen_hl_line(U16 x, U16 y, U8 a) {
  U8 i, j;
  for(i=x; i<NCOLS; i++) {
    for(j=y; j<(y+FONT_CHARH); j++) {
      if (screen_get_pixel(i, j) == 0) {
	screen_pixel(i, j, a);
      }
    }
  }
}

// draw a line and blank to end
//U8 screen_line(U16 x, U16 y, char *str, U8 hl) {
// argument is line number
U8 screen_line(U16 x, U16 l, char *str, U8 hl) {
  static u8 y;
  y = lines[l];
  // FIXME
  //  hl = ( (hl << 1) & 0xf);
  //  if (hl ) { hl =0xf;a }

  x = screen_string(x, y, str, hl);
  screen_blank_line(x, y);

  //  print_dbg("\r\n");
  //  if(hl > 2) { print_dbg("__"); }
  //  print_dbg(str);

  refresh = 1;

  return NCOLS;
}

// fill graphics ram with a test pattern
void screen_test_fill(void) {
  u32 i;
  u32 x=0;
  u32 y=0;
  for(i=0; i<font_nglyphs; i++) {

    x = x + screen_char_squeeze(x, y, i + FONT_ASCII_OFFSET, 0xf);
    x++;
    if (x > NCOLS) {
      x -= NCOLS;
      y += FONT_CHARH;
    }
  }
  refresh = 1;
}
