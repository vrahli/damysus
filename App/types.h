#ifndef TYPES_H
#define TYPES_H


#include <stdint.h>


enum unit { tt };

typedef uint8_t HEADER;

#define HDR_NEWVIEW     0x0
#define HDR_PREPARE     0x1
#define HDR_PREPARE_LDR 0x2
#define HDR_PRECOMMIT   0x3
#define HDR_COMMIT      0x4

#define HDR_NEWVIEW_ACCUM     0x5
#define HDR_PREPARE_LDR_ACCUM 0x6
#define HDR_PREPARE_ACCUM     0x7
#define HDR_PRECOMMIT_ACCUM   0x8

#define HDR_NEWVIEW_COMB     0x9
#define HDR_PREPARE_LDR_COMB 0x10
#define HDR_PREPARE_COMB     0x11
#define HDR_PRECOMMIT_COMB   0x12

#define HDR_TRANSACTION 0x13
#define HDR_REPLY       0x14
#define HDR_START       0x15
//#define HDR_STOP        0x16

#define HDR_NEWVIEW_CH     0x16
#define HDR_PREPARE_LDR_CH 0x17
#define HDR_PREPARE_CH     0x18

#define HDR_NEWVIEW_CH_COMB     0x19
#define HDR_PREPARE_LDR_CH_COMB 0x20
#define HDR_PREPARE_CH_COMB     0x21

typedef uint8_t Phase1;

#define PH1_NEWVIEW   0x0
#define PH1_PREPARE   0x1
#define PH1_PRECOMMIT 0x2
#define PH1_COMMIT    0x3

/*enum Phase2 {
  PH2_NEWVIEW,
  PH2_PREPARE,
  PH2_PRECOMMIT,
  PH2_COMMIT,
};*/

typedef unsigned int PID; // process ids
typedef unsigned int CID; // client ids
typedef unsigned int TID; // transaction ids
typedef unsigned int PORT;
typedef unsigned int View;
typedef unsigned int Value;


// Used in CBlock.h to differentiate elements for the union types of Cert and Accum
typedef uint8_t CAtag;

#define CERT  0x0
#define ACCUM 0x1


#endif
