#ifndef TYPES_H
#define TYPES_H


enum MODE {
  MODE_BROADCAST,
  MODE_ECHO,
  MODE_DELIVER,
};

enum HEADER {
  HDR_BROADCAST,
  HDR_ECHO,
  HDR_DELIVER,
  HDR_TRIGGER,
};

/*typedef struct __attribute__((packed)) sign {
  unsigned int sign;
  } SIGN;*/

typedef unsigned int SIGN;
typedef unsigned int PROCESS_ID;
typedef unsigned int SEQUENCE_NUM;
typedef unsigned int VALUE;

/*typedef struct __attribute__((packed)) broadcast {
  enum HEADER hdr;
  PROCESS_ID pid;
  SEQUENCE_NUM seq;
  VALUE val;
  SIGN sign[MAX_NUM_SIGNATURES];
} BROADCAST;

typedef struct __attribute__((packed)) echo {
  enum HEADER hdr;
  PROCESS_ID pid;
  SEQUENCE_NUM seq;
  VALUE val;
  SIGN sign[MAX_NUM_SIGNATURES];
} ECHO;

typedef struct __attribute__((packed)) deliver {
  enum HEADER hdr;
  PROCESS_ID pid;
  SEQUENCE_NUM seq;
  VALUE val;
  SIGN esign[MAX_NUM_SIGNATURES]; // signatures of the echo
  SIGN sign[MAX_NUM_SIGNATURES];  // signatures of the deliver
  } DELIVER;*/


#endif
