#ifndef CONFIG_H
#define CONFIG_H


#include "params.h"


// Uncomment to use basic HotStuff-like
//#define BASIC_BASELINE

// Uncomment to use basic Cheap
//#define BASIC_CHEAP

// Uncomment to use basic Quick
//#define BASIC_QUICK

// Uncomment to use basic Quick - debug version (no SGX)
//#define BASIC_QUICK_DEBUG

// Uncomment to use basic Cheap&Quick
//#define BASIC_CHEAP_AND_QUICK

// Uncomment to use chained HotStuff-like
//#define CHAINED_BASELINE

// Uncomment to use chained Cheap&Quick
//#define CHAINED_CHEAP_AND_QUICK

// Uncomment to use chained Cheap&Quick - debug version (no SGX)
//#define CHAINED_CHEAP_AND_QUICK_DEBUG

// debug switches
#define DEBUG   false
#define DEBUG0  true
#define DEBUG1  false
#define DEBUG2  false
// to, in particular, print 'EXECUTE' messages
#define DEBUGE  false //true
// to, in particular, print client messages
#define DEBUGC  false


#define NO_SOCKET          -1
#define CONF_FILE_SIZE     250
#define MAXLINE            256
#define RSA_NUM_BITS4k     4096
#define RSA_NUM_BITS2k     2048

//#define SOCK_KIND SOCK_STREAM // TCP
//#define SOCK_KIND SOCK_DGRAM  // UDP


// ----------------------------------------
// Colors
// ------

#define KNRM  "\x1B[0m"

// default background & different foreground colors
#define KRED  "\x1B[49m\x1B[31m"
#define KGRN  "\x1B[49m\x1B[32m"
#define KYEL  "\x1B[49m\x1B[33m"
#define KBLU  "\x1B[49m\x1B[34m"
#define KMAG  "\x1B[49m\x1B[35m"
#define KCYN  "\x1B[49m\x1B[36m"
#define KWHT  "\x1B[49m\x1B[37m"

// default background & different (light) foreground colors
#define KLRED  "\x1B[49m\x1B[91m"
#define KLGRN  "\x1B[49m\x1B[92m"
#define KLYEL  "\x1B[49m\x1B[93m"
#define KLBLU  "\x1B[49m\x1B[94m"
#define KLMAG  "\x1B[49m\x1B[95m"
#define KLCYN  "\x1B[49m\x1B[96m"
#define KLWHT  "\x1B[49m\x1B[97m"

// diferent background colors & white foreground
#define KBRED  "\x1B[41m\x1B[37m"
#define KBGRN  "\x1B[42m\x1B[37m"
#define KBYEL  "\x1B[43m\x1B[37m"
#define KBBLU  "\x1B[44m\x1B[37m"
#define KBMAG  "\x1B[45m\x1B[37m"
#define KBCYN  "\x1B[46m\x1B[37m"
#define KBWHT  "\x1B[47m\x1B[30m"

#endif
