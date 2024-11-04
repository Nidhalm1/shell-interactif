#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "execute.h"

int builtin_pwd();
int builtin_cd(const char *path);
int builtin_ftype(const char *filename);
