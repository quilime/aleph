/* ctl_op.h
 * aleph-avr32
 *
 * base classes implementing arbitrary control flow networks
 * 
 * derived classes need this,
 * other modules should hopefully only need ctl_interface.h
 */

#ifndef _CONTROL_OPERATOR_H_
#define _CONTROL_OPERATOR_H_

#include "compiler.h"

// WARNING please do not exceed this!
#define CTL_OP_MAX_SIZE 128

//---- input type
// a function pointer to represent an operator's input
// each function is passed a void* to its receiver
// and a pointer to const S32 for input value
typedef void(*ctl_in_t)(void* rec, const S32* input );

// ---- output type
// an index into the global output table
// a negative index is not evaluated
typedef S16 ctl_out_t; 

// ---- ctl_op_t
// base class for all processors in a control network
// exposes a set of IO points and takes action when (some) inputs are activated
typedef struct ctl_op_struct {
  U16 size;
  U8 numInputs;
  U8 numOutputs;
  // array of function pointers for input
  ctl_in_t* in;
  // array of pointer indices for output targets.
  ctl_out_t* out; 
  // name string
  const U8* opString;
  // input names concatenated into a single string
  const U8* inString;
  // output names concatenated into a single string
  const U8* outString;
  // offset in statically allocated op memory pool
  //  U32 memOffset;
} ctl_op_t;

/// get input name
const U8* ctl_op_getInString(ctl_op_t* op, const U8 idx);
// get output name
const U8* ctl_op_getOutString(ctl_op_t* op, const U8 idx);

/// ===== operator subclasses
// each of these structures holds the superclass and private state vairables
// inputs and outputs are described in the _init function defs

//--- op_sw_t : switch
typedef struct op_sw_struct {
  ctl_op_t super;
  S32 val; // anything to be used as output needs 4 bytes
  U8 tog;
  ctl_out_t outs[1];
} op_sw_t;
void op_sw_init(op_sw_t* sw);

//--- op_enc_t : encoder
typedef struct op_enc_struct {
  ctl_op_t super;
  U8 pos_now, pos_old;
} op_enc_t;
void op_enc_init(op_enc_t* sw);

//--- op_add_t : addition
typedef struct op_add_struct {
  ctl_op_t super;
  S32 a, b;
} op_add_t;
void op_add_init(op_add_t* add);

//--- op_mul_t : multiplication 
typedef struct op_mul_struct {
  ctl_op_t super;
  S32 a, b;
} op_mul_t;
void op_mul_init(op_mul_t* mul);

//--- op_gate_t : gate
typedef struct op_gate_struct {
  ctl_op_t super;
  S32 val;
  U32 gate;
} op_gate_t;
void op_gate_init(op_gate_t* gate);

//--- op_accum_t : accumulator
typedef struct op_accum_struct {
  ctl_op_t super;
  S32 val, min, max, step;
} op_accum_t;
void op_accum_init(op_accum_t* accum);

//--- op_sel_t : range selection 
typedef struct op_sel_struct {
  ctl_op_t super;
  S32 min, max;
} op_sel_t;
void op_sel_init(op_sel_t* sel);


//--- op_lin_t : linear map
typedef struct op_lin_struct {
  ctl_op_t super;
  S32 inMin, inMax, outMin, outMax, x;
} op_lin_t;
void op_lin_init(op_lin_t* lin);

/*
//--- op_exp_t : exponential map
typedef struct op_exp_struct {
  ctl_op_t super;
  S32 inMin, inMax, outMin, outMax, x;
} op_exp_t;
void op_exp_init(op_exp_t* exp);
*/

//--- op_param_t : receive parameter change
typedef struct op_param_struct {
  ctl_op_t super;
  U16 idx;
  S32 val;
} op_param_t;
void op_param_init(op_param_t* param);


//--- op_preset_t : preset store / recall
typedef struct op_preset_struct {
  ctl_op_t super;
  U16 idx;
} op_preset_t;
void op_preset_init(op_preset_t* preset);

#endif // header guard
