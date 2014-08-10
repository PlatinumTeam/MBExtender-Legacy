#ifndef _PLATFORMASSERT_H_
#define _PLATFORMASSERT_H_

#include "platform.h"

// No asserts 4 u
#define AssertFatal(x, y)   { (void)sizeof(x); (void)sizeof(y); }
#define AssertWarn(x, y)    { (void)sizeof(x); (void)sizeof(y); }

#endif