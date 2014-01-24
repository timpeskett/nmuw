#ifndef COMMON_H
#define COMMON_H

/* Common definitions needed for all files */

#include <assert.h>
#include <stdint.h>

typedef enum { FALSE=0, TRUE=1 } boolean;

#define PORT_NUM 2000

#define absint(x) ((x)<0?-(x):(x))

#endif
