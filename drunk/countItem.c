#include <stdio.h>
#include <io.h>
#include <stdlib.h>
#pragma  warning(disable:4996)
#define MAXID (10000)
// char szFilepath[MAXID][128] = { '\0' };
//iFlag[0], maxHtmlNum From Html, iFlag[1], the num of real HtmlFile
int iFlag[2][MAXID] = { 0 };
//to store the detail id of HtmlFile
int iFlag_need[MAXID][100] = { 0 };
char szDir[] = { "www.meitulu.com/item/" }; 
char szFilter[] = { "*.html" };
char szTemp[1024] = { '\0' };
int iTemp = 0;
char chTemp = 0;
int iID = 0;

int KMPTinS(char s[], char t[], int pos);
int resolveIndexHtml(char *filePath)
{
	FILE *fp = NULL;
	fp = fopen(filePath, "rt");
	if (fp != NULL)
	{
		int fileSize = 0;
		int _iFlag = 0;
		int a1 = 0, a2 = 0, b1 = 0, b2 = 0, tb1 = 0;
		char *szTemp2 = NULL;
		fseek(fp, 0, SEEK_END);
		fileSize = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		szTemp2 = (char *)calloc(fileSize + 1, sizeof(char));
		if (NULL != szTemp2)
		{
			fread(szTemp2, sizeof(char), fileSize, fp);
			
			//读取完毕，开始KMP
			a1 = KMPTinS(szTemp2, "<center><div id=\"pages\">", 0);
			if (a1 >= 0)
			{
				a2 = KMPTinS(szTemp2, "</div></center>", a1);
				if (a2 >= 0)
				{
					for (tb1 = a1; 
						tb1 <= a2 && tb1 != -1; 
						tb1 = KMPTinS(szTemp2, "<a href=\"", tb1 + 1))
					{
						b1 = tb1;
					}
					if (b1 >= 0)
					{
						b2 = KMPTinS(szTemp2, "\">", b1);
						if (b2 >= 0)
						{
							//找到int了
							sscanf(szTemp2 + b2 + 2, "%d", &_iFlag);
							fclose(fp);
							return (_iFlag);
						}
					}
				}

				free(szTemp2);
				szTemp2 = NULL;
			}
		}

		fclose(fp);
		
	}
	return (-1);
}
int countHtml()
{
	struct _finddata_t files;
	int File_Handle;
	int i = 0;
	sprintf(szTemp, "%s%s", szDir, szFilter);
	File_Handle = _findfirst(szTemp, &files);
	if (File_Handle == -1)
	{
		printf("error\n");
		return 0;
	}
	do
	{
		//处理之
// 		printf("%s \n", files.name);
		sscanf(files.name, "%d%c%d", &iTemp, &chTemp, &iID);
		if (0 < iTemp && iTemp < MAXID)
		{
			++iFlag[1][iTemp];
			if ('.' == chTemp)
			{
				++iFlag_need[iTemp][1];
				sprintf(szTemp, "%s%s", szDir, files.name);
				iFlag[0][iTemp] = resolveIndexHtml(szTemp);
			}
			else if ('_' == chTemp)
			{
				++iFlag_need[iTemp][iID];
			}
		}
		i++;
	} while (0 == _findnext(File_Handle, &files));
	_findclose(File_Handle);
// 	printf("Find %d files\n", i);
	return 0;
}

void main(int argc, char *argv[])
{
	int i;
	int j;
	countHtml();
	
	for (i = 1; i < MAXID; ++i)
	{
		if (iFlag[0][i] != iFlag[1][i])
		{
// 			for (j = iFlag[1][i] + 1; j <= iFlag[0][i]; ++j)
// 			{
// 				printf("http://%s%d_%d.html\n", szDir, i, j);
// 			}
			if (iFlag[0][i] >= iFlag[1][i])
			{
				for (j = 2; j <= iFlag[0][i]; ++j)
				{
					if (0 == iFlag_need[i][j])
					{
						printf("http://%s%d_%d.html\n", szDir, i, j);
					}
				}
			}

		}
		if (3 <= i && i <= 7390 && iFlag[1][i] == 0)
		{
			printf("http://%s%d.html\n", szDir, i);
		}
	}

}

