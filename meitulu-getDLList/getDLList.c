#include <stdio.h>
#include <io.h>
// #include <stdlib.h>
// #include <locale.h>
// #include <string.h>
#include <direct.h>
#pragma  warning(disable:4996)
void drunkMkDirs(char *dirPath)
{
	char curDirPath[1024] = { '\0' };
	int i = 0, len = strlen(dirPath);
	for (i = 0; i < len; ++i)
	{
		*(curDirPath + i) = *(dirPath + i);
		if (*(curDirPath + i) == '/' || *(curDirPath + i) == '\\')
		{
			//需要direct.h 头文件
			_mkdir(curDirPath);
		}
	}
	_mkdir(curDirPath);
}
int drunkstrlen(char *str)
{
	int i;
	for (i = 0; *(str + i) != '\r' && *(str + i) != '\n' && *(str + i) != '\0'; ++i)
	{
	}
	return (i);
}

int getDLList(char *szDir, char *szFilter)
{
	//传入的szDir应包含'/'作为结尾，并以'\0'终结
	struct _finddata_t files;
	int File_Handle;
	int i = 0;
	int j = 0;
	int iTemp = 0;
	int pageNum = 0;
	char chTemp = 0;
	char szTemp[1024] = { '\0' }, szOutBuf[1024] = { '\0' };
	char finListPath[] = { "fin/DLList/" };
	FILE *rFp = NULL, *wFp1 = NULL, *wFp2 = NULL;
	int urlNum = 0;
	//创建输出目录
	drunkMkDirs(finListPath);
	sprintf(szTemp, "%s%s", szDir, szFilter);
	File_Handle = _findfirst(szTemp, &files);
	if (File_Handle == -1)
	{
// 		printf("error\n");
		return (-1);
	}
	do
	{
		//开始处理
		sprintf(szTemp, "%s%s", szDir, files.name);
		rFp = fopen(szTemp, "rt");
		if (NULL == wFp1)
		{
			sprintf(szTemp, "%s%s", finListPath, "DLList_all.txt");
			wFp1 = fopen(szTemp, "wt");
		}
		if (0 == urlNum)
		{
			sprintf(szTemp, "%slist%dk.txt", finListPath, urlNum / 1000 + 1);
			if (NULL != wFp2)
			{
				fclose(wFp2);
				wFp2 = NULL;
			}
			wFp2 = fopen(szTemp, "wt");
		}

		if (rFp && wFp1 && wFp2)
		{
			
			while ((1))
			{
				fgets(szOutBuf, 1024, rFp);
				if (('h' == szOutBuf[0]) && ('t' == szOutBuf[1]) && ('t' == szOutBuf[2]) && ('p' == szOutBuf[3]))
				{
					++urlNum;
					if (1 == urlNum % 1000 && urlNum > 1000)
					{
						sprintf(szTemp, "%slist%dk.txt", finListPath, urlNum / 1000 + 1);
						if (NULL != wFp2)
						{
							fclose(wFp2);
							wFp2 = NULL;
						}
						wFp2 = fopen(szTemp, "wt");
					}

// 					sprintf(szOutBuf, "%s\n", szOutBuf);
					szOutBuf[drunkstrlen(szOutBuf)] = '\0';
					szOutBuf[drunkstrlen(szOutBuf)] = '\n';
					fputs(szOutBuf, wFp1);
					fputs(szOutBuf, wFp2);

				}

				if (feof(rFp))
				{
					//读到文件尾
					if (NULL != rFp)
					{
						fclose(rFp);
						rFp = NULL;
					}
					break;
				}
			}


		}

		i++;
	} while (0 == _findnext(File_Handle, &files));
	_findclose(File_Handle);
	// 	printf("Find %d files\n", i);
	if (NULL != rFp)
	{
		fclose(rFp);
		rFp = NULL;
	}
	if (NULL != wFp1)
	{
		fclose(wFp1);
		wFp1 = NULL;
	}
	if (NULL != wFp2)
	{
		fclose(wFp2);
		wFp2 = NULL;
	}



	return (0);

}
void main()
{
	getDLList("obj/item/", "*.txt");
}

