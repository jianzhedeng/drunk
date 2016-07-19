#include "KMP.h"

void get_nextval(char * t, int next[])
{
	int i = 1;
	int j = 0;
	next[0] = -1;
	while (i < (int)strlen(t))
	{
		if (j == -1 || t[i] == t[j])
		{
			i++;
			j++;
			if (t[i] != t[j])   //只有前后两个字符不相同时才会按前面的算法来计算next， 即next[i] = j
				next[i] = j;
			else
				next[i] = next[j];        //当前后两个字符相同时，进行向前找，能够减少比较的次数
		}
		else
		{
			j = next[j];
		}
	}
}

int index(char * s, char * t, int pos, int next[])
{
	int i = pos;
	int j = 0;
	int len1 = strlen(s);
	int len2 = strlen(t);
	while (i < len1&&j < len2)
	{
		if (j == -1 || s[i] == t[j])//第一个字符都不匹配或者字符相等的情况
		{
			i++;
			j++;
		}
		else   //i指针不回溯,j指针
		{
			//i=i-j+1;
			j = next[j];
		}
	}
	if (j >= len2)
		return i - len2;
	else
		return -1;
}

int KMPTinS(char s[], char t[], int pos)
{
	int iTLen, *next, iFlag;
	iTLen = (int)strlen(t);
	if (iTLen > 0 && pos >= 0)
	{
		next = (int *)calloc(iTLen + 1, sizeof(t));
		if (next)
		{
			get_nextval(t, next);
			iFlag = index(s, t, pos, next);
			free(next);
			return (iFlag);
		}
	}
	return (-1);
}
