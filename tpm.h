#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include <tss2/tss2_fapi.h>
#include <tss2/tss2_common.h>

#define AUTH_RESULT int
#define AUTH_SUCCESS 1
#define AUTH_FAILED 0

#define ENROLL_RESULT int
#define ENROLL_SUCCESS 1
#define ENROLL_FAILED 0

#ifndef UNUSED
#define UNUSED(x) (void)(x)
#endif

ENROLL_RESULT enroll_user();

AUTH_RESULT auth_user(char *input);