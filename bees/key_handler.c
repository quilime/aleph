/* key_handler.c
 * bees
 * aleph
 *
 * page-specific key handling routines
 */

//#include "compiler.h"
#include "op.h"
#include "op_math.h"
#include "menu_protected.h"
#include "param.h"
#include "preset.h"
#include "redraw.h"
#include "scene.h"
#include "key_handler.h"
//--------------------
//----- static variables

// param step constants
static const io_t kParamValStepSmall = 0.015625;
static const io_t kParamValStepLarge = 1.f;
// character selection tablei
static const char kLabelChars[] = "abcdefghijklmnopqrstuvwxyz_012345789";
#define NUM_LABEL_CHARS 47
// index in the selection table 
static s8 selectedLabelChar = 0;
// position in name
//static s8 selectedLabelPos = 0;

//========================================
//====== key handlers
// OPS
void key_handler_ops(uiKey_t key) {
  u16 n;
  switch(key) {
  case eKeyFnDownA: 
    // fnA go to selected operator's inputs on INS page
    pages[ePageIns].selected = net_op_in_idx(page->selected, 0);
    setPage(ePageIns);
    break;
  case eKeyFnDownB:
    // fnB : go to this operator's outputs on OUTS page
    pages[ePageOuts].selected = net_op_out_idx(page->selected, 0);
    setPage(ePageOuts);
    break;
  case eKeyFnDownC:
    // fnC : create new operator of specified type
    net_add_op(userOpTypes[newOpType]);
    redraw_ops();
    break;
  case eKeyFnDownD:
    // fnD : delete
    // FIXME: need to add arbitrary op deletion.
    // right now this will destroy the last created op
    if (net_op_status(net_num_ops() - 1) != eUserOp) {
      return;
    }
    net_pop_op();
    n = net_num_ops() - 1;
    if (page->selected > n) {
      page->selected = n;
    }
    redraw_ops();
    break;
    //// encoder A: scroll pages
  case eKeyEncUpA:
    scrollPage(1);
    break;
  case eKeyEncDownA:
    scrollPage(-1);
    break;
    //// encoder B: scroll selection
  case eKeyEncUpB:
    scrollSelect(1, net_num_ops() - 1);
    break;
  case eKeyEncDownB:
    scrollSelect(-1, net_num_ops() - 1);      
    break;
    //// encoder C: move up/down in order of execution
  case eKeyEncUpC:
    // TODO
    break;
  case eKeyEncDownC:
    // TODO
    break;
    //// encoder D: select new operator type for creation
  case eKeyEncUpD:
    newOpType++;
    if (newOpType >= NUM_USER_OP_TYPES) {
      newOpType = 0;
    }
    redraw_ops();
    break;
  case eKeyEncDownD:
    newOpType--;
    if (newOpType >= NUM_USER_OP_TYPES) {
      newOpType = NUM_USER_OP_TYPES_1;
    }
    redraw_ops();
    // nothing
    break;
  
  default:
    ;; // nothing
  }  
}

// INS
void key_handler_ins(uiKey_t key) {
  u32 i;
  switch(key) {
  case eKeyFnDownA:
    // fnA : gather
    numGathered = net_gather(page->selected, gathered);
    break;
  case eKeyFnDownB:
    // fnB : disconnect
    numGathered = net_gather(page->selected, gathered);
    for(i=0; i<numGathered; i++) {
      net_disconnect(*(gathered[i]));
    }
    break;
  case eKeyFnDownC:
    // fnC : overwrite preset value (and include)
    // TODO
    break;
  case eKeyFnDownD:
    // toggle preset inclusion
    net_toggle_in_preset(page->selected);
    break;
    //// encoder A: scroll pages
  case eKeyEncUpA:
    scrollPage(1);
    break;
  case eKeyEncDownA:
    scrollPage(-1);
    break;
    //// encoder B: scroll selection
  case eKeyEncUpB:
    scrollSelect(1, net_num_ins()-1);
    break;
  case eKeyEncDownB:
    scrollSelect(-1, net_num_ins()-1);      
    break;
  case eKeyEncUpC:
    // encoder C : value slow
    net_inc_in_value(page->selected, kParamValStepSmall);
    redraw_ins();
    break;
  case eKeyEncDownC:
    net_inc_in_value(page->selected, kParamValStepSmall * -1);
    redraw_ins();
    break;
  case eKeyEncUpD:
    // encoder D : value fast
    net_inc_in_value(page->selected, kParamValStepLarge);
    redraw_ins();
    break;
  case eKeyEncDownD:
    net_inc_in_value(page->selected, kParamValStepLarge * -1);
    redraw_ins();
    break;
  
  default:
    ;; // nothing
  }  
}

// OUTS
void key_handler_outs(uiKey_t key) {
  s16 i;
  static s32 target;
  switch(key) {
  case eKeyFnDownA: 
    // follow
    i = net_get_target(page->selected);
    if (i == -1) { return; } 
    pages[ePageIns].selected = i;
    setPage(ePageIns);
    break;
  case eKeyFnDownB:
    // disconnect
    break;
  case eKeyFnDownC:
    // re-send? store?
    break;
  case eKeyFnDownD:
    // toggle preset (target)
    i = net_get_target(page->selected);
    if(i>=0) { net_toggle_in_preset(i); }
    redraw_outs();
    break;
    //// encoder A: scroll pages
  case eKeyEncUpA:
    scrollPage(1);
    break;
  case eKeyEncDownA:
    scrollPage(-1);
    break;
    //// encoder B: scroll selection
  case eKeyEncUpB:
    scrollSelect(1, net_num_outs()-1);
    break;
  case eKeyEncDownB:
    scrollSelect(-1, net_num_outs()-1);      
    break;
    //// encoder C: scroll target
  case eKeyEncUpC:
    target++;
    if (target == net_num_ins()) {
      target = -1;
    }
    net_connect(page->selected, target);
    redraw_outs();
    break;
  case eKeyEncDownC:
    target--;
    if (target == -2) {
      target = net_num_ins() - 1;
    }
    net_connect(page->selected, target);
    redraw_outs();
    break;
  case eKeyEncUpD:
    // nothing
    break;
  case eKeyEncDownD:
    // nothing
    break;
    default:
    ;; // nothing
  }  
}

//// GATHERED
void key_handler_gathered(uiKey_t key) {
  key_handler_outs(key);
}

///// PLAy
void key_handler_play(uiKey_t key) {
  s32 val;
  s16 inIdx = -1;
  ///// FIXME; bad bad bad hacking
  /* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
     really should set up specialied apparatus in net.c
     to identify and use system output nodes.
     for now, we activate the 8 operators from
     net.c : add_sys_ops()
  */
  switch(key) {
    ///// keys
  case eKeyFnUpA:
    inIdx = net_op_in_idx(4, 0); // op 4: switch, in 0: val
    val = 0;
    break;
  case eKeyFnDownA:
    inIdx = net_op_in_idx(4, 0); // op 4: switch, in 0: val
    val = 1;
    break;
  case eKeyFnUpB:
    inIdx = net_op_in_idx(4, 0); // op 5: switch, in 0: val
    val = 0;
    break;
  case eKeyFnDownB:
    inIdx = net_op_in_idx(4, 0); // op 5: switch, in 0: val
    val = 1;
    break;
  case eKeyFnUpC:
    inIdx = net_op_in_idx(4, 0); // op 6: switch, in 0: val
    val = 0;
    break;
  case eKeyFnDownC:
    inIdx = net_op_in_idx(4, 0); // op 6: switch, in 0: val
    val = 1;
    break;
  case eKeyFnUpD:
    inIdx = net_op_in_idx(4, 0); // op 7: switch, in 0: val
    val = 0;
    break;
  case eKeyFnDownD:
    inIdx = net_op_in_idx(4, 0); // op 7: switch, in 0: val
    val = 1;
    break;
    ///// encoders
   case eKeyEncUpA:
    inIdx = net_op_in_idx(0, 0); // op 0: enc, in 0: move
    val = 1;
    break;
   case eKeyEncDownA:
    inIdx = net_op_in_idx(0, 0); // op 0: enc, in 0: move
    val = -1;
    break;
   case eKeyEncUpB:
    inIdx = net_op_in_idx(1, 0); // op 1: enc, in 0: move
    val = 1;
    break;
   case eKeyEncDownB:
    inIdx = net_op_in_idx(1, 0); // op 1: enc, in 0: move
    val = -1;
    break;
   case eKeyEncUpC:
    inIdx = net_op_in_idx(2, 0); // op 2: enc, in 0: move
    val = 1;
    break;
   case eKeyEncDownC:
    inIdx = net_op_in_idx(2, 0); // op 2: enc, in 0: move
    val = -1;
    break;
   case eKeyEncUpD:
    inIdx = net_op_in_idx(3, 0); // op 3: enc, in 0: move
    val = 1;
    break;
   case eKeyEncDownD:
    inIdx = net_op_in_idx(3, 0); // op 3: enc, in 0: move
    val = -1;
    break;
   default:
    ;;
    break;
  }
  if(inIdx >= 0) {
    net_activate(inIdx, val);
    redraw_play();
  }
}

// presets
extern void key_handler_presets(uiKey_t key) {
  //  s16 i;
  switch(key) {
  case eKeyFnDownA: // clear
    switch(page->mode) {
    case eModeNone:
      page->mode = eModeClear;
      break;
    case eModeClear:
      //preset_clear(page->selected);
      page->mode = eModeNone;
      break;
    default:
      page->mode = eModeNone;
    }
    break;
  case eKeyFnDownB: // copy
    switch(page->mode) {
    case eModeNone:
      page->mode = eModeCopy;
      break;
    case eModeCopy:
      //preset_copy(page->selected);
      page->mode = eModeNone;
      break;
    default:
      page->mode = eModeNone;
    }
    break;
  case eKeyFnDownC: // store
    switch(page->mode) {
    case eModeNone:
      page->mode = eModeStore;
      break;
    case eModeStore:
      preset_store(page->selected);
      page->mode = eModeNone;
      break;
    default:
      page->mode = eModeNone;
    }
    break;
  case eKeyFnDownD: // recall
    switch(page->mode) {
    case eModeNone:
      page->mode = eModeRecall;
      break;
    case eModeRecall:
      preset_recall(page->selected);
      page->mode = eModeNone;
      break;
    default:
      page->mode = eModeNone;
    }
    break;
    //// encoder A: scroll pages
  case eKeyEncUpA:
    scrollPage(1);
    break;
  case eKeyEncDownA:
    scrollPage(-1);
    break;
    //// encoder B: scroll selection
  case eKeyEncUpB:
    scrollSelect(1, NET_PRESETS_MAX - 1);
    break;
  case eKeyEncDownB:
    scrollSelect(-1, NET_PRESETS_MAX - 1);      
    break;
    //// encoder C: scroll name pos
  case eKeyEncUpC:
    break;
  case eKeyEncDownC:
    break;
  case eKeyEncUpD:
    // scroll name char
    break;
  case eKeyEncDownD:
    // scroll name char
    break;
    default:
    ;; // nothing
  }  
  (*(page->redraw))();
}

/// SCENES
extern void key_handler_scenes(uiKey_t key) {
  switch(key) {
  case eKeyFnDownA: // clear
    switch(page->mode) {
    case eModeNone:
      page->mode = eModeClear;
      break;
    case eModeClear:
      //scene_clear(page->selected);
      page->mode = eModeNone;
      break;
    default:
      page->mode = eModeNone;
    }
    break;
  case eKeyFnDownB: // copy
    switch(page->mode) {
    case eModeNone:
      page->mode = eModeCopy;
      break;
    case eModeCopy:
      //scene_copy(page->selected);
      page->mode = eModeNone;
      break;
    default:
      page->mode = eModeNone;
    }
    break;
  case eKeyFnDownC: // store
    switch(page->mode) {
    case eModeNone:
      page->mode = eModeStore;
      break;
    case eModeStore:
      scene_store(page->selected);
      page->mode = eModeNone;
      break;
    default:
      page->mode = eModeNone;
    }
    break;
  case eKeyFnDownD: // recall
    switch(page->mode) {
    case eModeNone:
      page->mode = eModeRecall;
      break;
    case eModeRecall:
      scene_recall(page->selected);
      page->mode = eModeNone;
      break;
    default:
      page->mode = eModeNone;
    }
    break;
    //// encoder A: scroll pages
  case eKeyEncUpA:
    scrollPage(1);
    break;
  case eKeyEncDownA:
    scrollPage(-1);
    break;
    //// encoder B: scroll selection
  case eKeyEncUpB:
    scrollSelect(1, SCENE_COUNT -1 );
    break;
  case eKeyEncDownB:
    scrollSelect(-1, SCENE_COUNT -1 );
    break;
  case eKeyEncUpC: // cursor: position in name
    page->cursor++;
    if (page->cursor > NUM_LABEL_CHARS) {
      page->cursor = 0;
    } 
    break;
  case eKeyEncDownC:  // cursor: position in name
    page->cursor--;
    if (page->cursor < 0) {
      page->cursor = NUM_LABEL_CHARS - 1;
    } 
    break;
  case eKeyEncUpD:     // scroll name char at pos
    selectedLabelChar++;
    if (selectedLabelChar > NUM_LABEL_CHARS) {
      selectedLabelChar = 0;
    } 

    break;
  case eKeyEncDownD:     // scroll name char at pos
    page->cursor--;
    if (page->cursor < 0) {
      page->cursor = NUM_LABEL_CHARS - 1;
    } 
    
    break;
  default:
    ;; // nothing
  }  
  (*(page->redraw))();
}
