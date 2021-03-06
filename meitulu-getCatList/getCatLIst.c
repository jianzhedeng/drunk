#include <stdio.h>
#include <io.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>
#include <direct.h>
#pragma  warning(disable:4996)

int KMPTinS(char s[], char t[], int pos);

void drunkUTF8ToGB2312(char *src, char *dst)
{
	//1.2版本
	//经测试，生成wch使用+符号会出现bug，改成|位运算符
	//位运算之前，使用&排除不参与运算的部分
	int i, j;
	char szTemp[12] = { '\0' };
	int tmep;
	wchar_t wch = '\0';

	setlocale(LC_ALL, "");

	for (i = 0, j = 0; *(src + i) != '\0' && *(src + i) != '\r' && *(src + i) != '\n';)
	{
		if ((((*(src + i)) & 0xf0) == 0xe0) &&
			(((*(src + i + 1)) & 0xc0) == 0x80) &&
			(((*(src + i + 2)) & 0xc0) == 0x80))
		{
			//三字节的汉字编码
			(*(dst + j + 1)) = ((*(src + i + 2)) & 0x3f) + (((*(src + i + 1)) & 0x03) << 6);
			(*(dst + j)) = (((*(src + i + 1)) & 0x3c) >> 2) + (((*(src + i)) & 0x0f) << 4);

			wch = ((*(dst + j) & 0xff) << 8) | (*(dst + j + 1) & 0xff);
			tmep = wctomb(szTemp, (wchar_t)wch);
			(*(dst + j)) = szTemp[0];
			(*(dst + j + 1)) = szTemp[1];

			i += 3;
			j += 2;
			continue;
		}
		else if ((((*(src + i)) & 0xe0) == 0xc0) &&
			(((*(src + i + 1)) & 0xc0) == 0x80))
		{
			//双字节的汉字编码
			(*(dst + j + 1)) = ((*(src + i + 1)) & 0x3f) + (((*(src + i)) & 0x03) << 6);
			(*(dst + j)) = ((*(src + i + 1)) & 0x1c) >> 2;

			wch = ((*(dst + j) & 0xff) << 8) | (*(dst + j + 1) & 0xff);
			wctomb(szTemp, wch);
			(*(dst + j)) = szTemp[0];
			(*(dst + j + 1)) = szTemp[1];

			i += 2;
			j += 2;
			continue;
		}
		else if (((*(src + i)) & 0x80) == 0)
		{
			//askii字符
			*(dst + j) = *(src + i);
			++i;
			++j;
			continue;
		}
		++i;

	}
	*(dst + j) = '\0';
	//i是src的字节总数
}

void solveSearchLink(char *szLink)
{
	char szLinkUTF8[128] = { '\0' }, 
		szLinkGB2312[128] = { '\0' }, 
		szOutDir[128] = { '\0' }, 
		szLinkInBat[128] = { '\0' };
	int i = 0, j = 0;
	for (i = 0, j = 0; *(szLink + i) != '\0'; )
	{
		if ('%' == *(szLink + i))
		{
			if ('a' <= *(szLink + i + 1))
			{
				*(szLinkUTF8 + j) = ((*(szLink + i + 1) - 'a' + 10) & 0x0f) << 4;
			}
			else if ('A' <= *(szLink + i + 1))
			{
				*(szLinkUTF8 + j) = ((*(szLink + i + 1) - 'A' + 10) & 0x0f) << 4;
			}
			else if ('0' <= *(szLink + i + 1))
			{
				*(szLinkUTF8 + j) = ((*(szLink + i + 1) - '0') & 0x0f) << 4;
			}

			if ('a' <= *(szLink + i + 2))
			{
				*(szLinkUTF8 + j) = *(szLinkUTF8 + j) | ((*(szLink + i + 2) - 'a' + 10) & 0x0f);
			}
			else if ('A' <= *(szLink + i + 2))
			{
				*(szLinkUTF8 + j) = *(szLinkUTF8 + j) | ((*(szLink + i + 2) - 'A' + 10) & 0x0f);
			}
			else if ('0' <= *(szLink + i + 2))
			{
				*(szLinkUTF8 + j) = *(szLinkUTF8 + j) | ((*(szLink + i + 2) - '0') & 0x0f);
			}

			++j;
			i += 3;
		}
		else
		{
			*(szLinkUTF8 + j++) = *(szLink + i++);
		}
	}
	//开始处理utf8字符串
	drunkUTF8ToGB2312(szLinkUTF8, szLinkGB2312);

	//生成路径;j在此处作为flag
	for (i = (int)strlen(szLinkGB2312) - 1; i >= 0; --i)
	{
		if ('/' == *(szLinkGB2312 + i))
		{
			*(szOutDir + i) = '\\';
			continue;
		}
		*(szOutDir + i) = *(szLinkGB2312 + i);
	}
	*(szOutDir + strlen(szLinkGB2312)) = '\\';
	*(szOutDir + strlen(szLinkGB2312) + 1) = '\0';

	//bat脚本中%作为转义字符，%用%%表示
	for (i = 0, j = 0; *(szLink + i) != '\0'; ++i, ++j)
	{
		*(szLinkInBat + j) = *(szLink + i);
		if ('%' == *(szLink + i))
		{
			*(szLinkInBat + ++j) = '%';
		}
	}
	*(szLinkInBat + j) = '\0';

	fprintf(stderr, ">nul 2>&1 md %%outdir%%%s\n" 
		">>%%outdir%%wget.log 2>>&1 wget -N -O %%outdir%%%sindex.html  %s\n", 
		szOutDir, 
		szOutDir, szLinkInBat);
	fprintf(stdout, "%s/\n", szLinkGB2312);

}
void getCatList(char *BookMarkFile)
{
	FILE *fp = NULL;
	int fileSize = 0;
	char *szTextContent = NULL;
	int a1 = 0, a2 = 0;
	char szLink[128] = { '\0' };

	fp = fopen(BookMarkFile, "rt");
	if (NULL != fp)
	{
		fseek(fp, 0, SEEK_END);
		fileSize = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		//多出的一字节会被初始化为'\0'
		szTextContent = (char *)calloc(fileSize + 1, sizeof(char));
		if (szTextContent != NULL)
		{
			fread(szTextContent, sizeof(char), fileSize, fp);

			//读取完毕，开始处理
			while (1)
			{
				a1 = KMPTinS(szTextContent, "<DT><A HREF=\"http://www.meitulu.com", a1);
				if (-1 == a1)
				{
					break;
				}
				a1 += strlen("<DT><A HREF=\"") + 7;
				a2 = KMPTinS(szTextContent, "\" ", a1);
				strncpy(szLink, szTextContent + a1, a2 - a1);
				szLink[a2 - a1] = '\0';
				if ('s' == szLink[16])
				{
					solveSearchLink(szLink);
				} 
				else if ('t' == szLink[16])
				{
					fprintf(stdout, "%s\n", szLink);
				}
			}


			free(szTextContent);
			szTextContent = NULL;
		}
		fclose(fp);
		fp = NULL;
	}


}
int main(int argc, char *argv[])
{

	if (argc > 1)
	{
		getCatList(argv[1]);
	}

	return (0);
}

