#ifndef ECHO_ASSERT_H
#define ECHO_ASSERT_H

#include <cassert>

#define ECHO_ASSERT(condition, message) assert(condition && message);
#define ECHO_ASSERT_NOT_NULL(value) assert(value && "Cannot be null");
#define ECHO_ASSERT_NOT_ZERO(value) assert(value && "Cannot be zero");

#endif
