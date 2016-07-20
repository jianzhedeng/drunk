#include <stdio.h>
#include <io.h>
#include <stdlib.h>
// #include <locale.h>
#include <string.h>
#include <direct.h>
#pragma  warning(disable:4996)
// void drunkMkDirs(char *dirPath)
// {
// 	char curDirPath[1024] = { '\0' };
// 	int i = 0, len = strlen(dirPath);
// 	for (i = 0; i < len; ++i)
// 	{
// 		*(curDirPath + i) = *(dirPath + i);
// 		if (*(curDirPath + i) == '/' || *(curDirPath + i) == '\\')
// 		{
// 			//需要direct.h 头文件
// 			_mkdir(curDirPath);
// 		}
// 	}
// 	_mkdir(curDirPath);
// }
// int drunkstrlen(char *str)
// {
// 	int i;
// 	for (i = 0; *(str + i) != '\r' && *(str + i) != '\n' && *(str + i) != '\0'; ++i)
// 	{
// 	}
// 	return (i);
// }
// 
// int getDLList(char *szDir, char *szFilter)
// {
// 	//传入的szDir应包含'/'作为结尾，并以'\0'终结
// 	struct _finddata_t files;
// 	int File_Handle;
// 	int i = 0;
// 	int j = 0;
// 	int iTemp = 0;
// 	int pageNum = 0;
// 	char chTemp = 0;
// 	char szTemp[1024] = { '\0' }, szOutBuf[1024] = { '\0' };
// 	char finListPath[] = { "fin/DLList/" };
// 	FILE *rFp = NULL, *wFp1 = NULL, *wFp2 = NULL;
// 	int urlNum = 0;
// 	//创建输出目录
// 	drunkMkDirs(finListPath);
// 	sprintf(szTemp, "%s%s", szDir, szFilter);
// 	File_Handle = _findfirst(szTemp, &files);
// 	if (File_Handle == -1)
// 	{
// 		// 		printf("error\n");
// 		return (-1);
// 	}
// 	do
// 	{
// 		//开始处理
// 		sprintf(szTemp, "%s%s", szDir, files.name);
// 		rFp = fopen(szTemp, "rt");
// 		if (NULL == wFp1)
// 		{
// 			sprintf(szTemp, "%s%s", finListPath, "DLList_all.txt");
// 			wFp1 = fopen(szTemp, "wt");
// 		}
// 		if (0 == urlNum)
// 		{
// 			sprintf(szTemp, "%slist%dk.txt", finListPath, urlNum / 1000 + 1);
// 			if (NULL != wFp2)
// 			{
// 				fclose(wFp2);
// 				wFp2 = NULL;
// 			}
// 			wFp2 = fopen(szTemp, "wt");
// 		}
// 
// 		if (rFp && wFp1 && wFp2)
// 		{
// 
// 			while ((1))
// 			{
// 				fgets(szOutBuf, 1024, rFp);
// 				if (('h' == szOutBuf[0]) && ('t' == szOutBuf[1]) && ('t' == szOutBuf[2]) && ('p' == szOutBuf[3]))
// 				{
// 					++urlNum;
// 					if (1 == urlNum % 1000 && urlNum > 1000)
// 					{
// 						sprintf(szTemp, "%slist%dk.txt", finListPath, urlNum / 1000 + 1);
// 						if (NULL != wFp2)
// 						{
// 							fclose(wFp2);
// 							wFp2 = NULL;
// 						}
// 						wFp2 = fopen(szTemp, "wt");
// 					}
// 
// 					// 					sprintf(szOutBuf, "%s\n", szOutBuf);
// 					szOutBuf[drunkstrlen(szOutBuf)] = '\0';
// 					szOutBuf[drunkstrlen(szOutBuf)] = '\n';
// 					fputs(szOutBuf, wFp1);
// 					fputs(szOutBuf, wFp2);
// 
// 				}
// 
// 				if (feof(rFp))
// 				{
// 					//读到文件尾
// 					if (NULL != rFp)
// 					{
// 						fclose(rFp);
// 						rFp = NULL;
// 					}
// 					break;
// 				}
// 			}
// 
// 
// 		}
// 
// 		i++;
// 	} while (0 == _findnext(File_Handle, &files));
// 	_findclose(File_Handle);
// 	// 	printf("Find %d files\n", i);
// 	if (NULL != rFp)
// 	{
// 		fclose(rFp);
// 		rFp = NULL;
// 	}
// 	if (NULL != wFp1)
// 	{
// 		fclose(wFp1);
// 		wFp1 = NULL;
// 	}
// 	if (NULL != wFp2)
// 	{
// 		fclose(wFp2);
// 		wFp2 = NULL;
// 	}
// 
// 
// 
// 	return (0);
// 
// }

int KMPTinS(char s[], char t[], int pos);


void solveSearchLink(char *szLink)
{

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
				else
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
// 	struct _finddata_t files;
// 	int File_Handle;
// 	File_Handle = _findfirst("D:/wget/www.meitulu.com/t/*", &files);
// 	if (File_Handle == -1)
// 	{
// 		// 		printf("error\n");
// 		return (-1);
// 	}
// 	do
// 	{
// 
// 
// 	} while (0 == _findnext(File_Handle, &files));
// 	_findclose(File_Handle);

	if (argc > 1)
	{
		getCatList(argv[1]);
	}
	else
	{ 
		getCatList("C:/Users/deng/Desktop/bookmarks_16_7_15.html");
	}

	return (0);
}

