/*
  page_ops.c
 */

#include "menu_protected.h"
#include "pages.h"
#include "render.h"


//-------------------------
//---- static

// handler declarations
static void handle_enc_0(s32 val);
static void handle_enc_1(s32 val);
static void handle_enc_2(s32 val);
static void handle_enc_3(s32 val);
static void handle_key_0(s32 val);
static void handle_key_1(s32 val);
static void handle_key_2(s32 val);
static void handle_key_3(s32 val);

// function keys
void handle_key_0(s32 val) {
}

void handle_key_1(s32 val) {
}

void handle_key_2(s32 val) {
}

void handle_key_3(s32 val) {
}

void handle_enc_0(s32 val) {
}

void handle_enc_1(s32 val) {
}

void handle_enc_2(s32 val) {
}

void handle_enc_3(s32 val) {
}


//----------------------
// ---- extern 
// refresh 
void refresh_ops(void) { 
}

const page_handler_t handler_ops[eNumPageHandlers] = {
  &handle_enc_0,
  &handle_enc_1,
  &handle_enc_2,
  &handle_enc_3,
  &handle_key_0,
  &handle_key_1,
  &handle_key_2,
  &handle_key_3,
};
