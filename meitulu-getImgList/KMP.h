#pragma once
#ifndef _PCAPBLI_KMP_H
#define _PCAPBLI_KMP_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void get_nextval(char* t, int next[]);
int index(char *s, char *t, int pos, int next[]);
int KMPTinS(char s[], char t[], int pos);

#endif // !_PCAPBLI_KMP_H

