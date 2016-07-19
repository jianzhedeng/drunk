#include <stdio.h>
#include <io.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>
#include <direct.h>
#pragma  warning(disable:4996)

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

int drunkstrlen(char *str)
{
	int i;
	for (i = 0; *(str + i) != '\r' && *(str + i) != '\n' && *(str + i) != '\0'; ++i)
	{
	}
	return (i);
}
void drunkUTF8ToGB2312(char *src, char *dst)
{
	//未在本工程中测试，待补充。
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

			wch = ((*(dst + j)) << 8) + (*(dst + j + 1));
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

			wch = ((*(dst + j)) << 8) + (*(dst + j + 1));
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
void drunkGB2312ToUTF8(char *src, char *dst)
{
	//输入的src应以'\0'结尾或换行结尾
	//若输入的src不止一行，则只输出对第一行的转换
	//输入的dst应确保空间足够大
	int sLen = 0;
	int dLen = 0;
	int i, j;
// 	wchar_t wcTemp[100] = { '\0' };
	wchar_t *wcTemp = NULL;
	sLen = drunkstrlen(src);
	if (sLen > 0)
	{
		wcTemp = (wchar_t *)calloc(sLen + 1, sizeof(wchar_t));
		if (NULL != wcTemp)
		{
			//需要locale.h头文件
			setlocale(LC_ALL, "");

			dLen = mbstowcs(wcTemp, src, sLen);
			//Unicode编码转uft8编码
			for (i = 0, j = 0; i < dLen; ++i)
			{
				if (0x7f == (* (wcTemp + i) | 0x7f))
				{
					//一个字节utf8编码
					*(dst + j) = *(wcTemp + i) & 0x7f;
					++j;
				}
				else if (0x07ff  == (* (wcTemp + i) | 0x07ff))
				{
					//2个字节utf8编码
					*(dst + j) = ((*(wcTemp + i) & 0x07c0) >> 5) | 0xc0;
					*(dst + j + 1) = (*(wcTemp + i) & 0x3f) | 0x80;
					j += 2;
				}
				else if (0xffff == (* (wcTemp + i) | 0xffff))
				{
					//3个字节utf8编码
					//避免wchar_t不是16位才做了上方判断
					*(dst + j) = ((*(wcTemp + i) & 0xf000) >> 12) | 0xe0;
					*(dst + j + 1) = ((*(wcTemp + i) & 0x0fc0) >> 6) | 0x80;
					*(dst + j + 2) = (*(wcTemp + i) & 0x3f) | 0x80;
					j += 3;
				}
			}
			//以'\0'结尾
			*(dst + j) = '\0';

			free(wcTemp);
			wcTemp = NULL;
		}
	}


}
void writeImageURL(char *FilePath, FILE *outfp)
{
	if (NULL != outfp)
	{
		FILE *fp = NULL;
		char szOutBuf[65536] = { '\0' }, szTemp[1024] = { '\0' };
		int fileSize = 0;
		char *szInBuf = NULL;
		int z1 = 0, z2 = 0, a1 = 0, a2 = 0;
		int iTemp = 0;
		fp = fopen(FilePath, "rt");
		if (NULL != fp)
		{

			fseek(fp, 0, SEEK_END);
			fileSize = ftell(fp);
			fseek(fp, 0, SEEK_SET);
			szInBuf = (char *)calloc(fileSize + 1, sizeof(char));
			if (NULL != szInBuf)
			{
				fread(szInBuf, sizeof(char), fileSize, fp);
				//读取完毕，开始分析信息
				z1 = KMPTinS(szInBuf, "<div class=\"content\">", 0);
				z1 += strlen("<div class=\"content\">");
				z2 = KMPTinS(szInBuf, "</div>", z1);
				for (a1 = z1 ; (1); )
				{
					a1 = KMPTinS(szInBuf, "<center><img src=", a1);
					if (a1 > z2 || a1 == (-1))
					{
						break;
					}
					a1 += strlen("<center><img src=");
					a2 = KMPTinS(szInBuf, "alt", a1);
					if (a1 >= 0 && a2 >= 0)
					{
						strncpy(szTemp, szInBuf + a1, a2 - a1);
						sprintf(szOutBuf, "%s%s\n", szOutBuf, szTemp);
					}
				}

				fputs(szOutBuf, outfp);

				free(szInBuf);
				szInBuf = NULL;
			}


			fclose(fp);
			fp = NULL;
		}
	}

}
void writeBasicInfo(char *FilePath, FILE *outfp)
{
	if (NULL != outfp)
	{
		FILE *fp = NULL;
		char szOutBuf[1024] = { '\0' }, szTemp[1024] = { '\0' };
		int fileSize = 0;
		char *szInBuf = NULL;
		int a1 = 0, a2 = 0;
		int iTemp = 0;
		fp = fopen(FilePath, "rt");
		if (NULL != fp)
		{
			
			fseek(fp, 0, SEEK_END);
			fileSize = ftell(fp);
			fseek(fp, 0, SEEK_SET);
			szInBuf = (char *)calloc(fileSize + 1, sizeof(char));
			if (NULL != szInBuf)
			{
				fread(szInBuf, sizeof(char), fileSize, fp);
				//读取完毕，开始分析信息
				a1 = KMPTinS(szInBuf, "<title>", 0);
				a1 += strlen("<title>");
				a2 = KMPTinS(szInBuf, "</title>", a1);
				//需要string.h头文件
				if (a1 >= 0 && a2 >= 0)
				{
					strncpy(szTemp, szInBuf + a1, a2 - a1);
					sprintf(szOutBuf, "%sTitle: %s\n", szOutBuf, szTemp);
				}

				drunkGB2312ToUTF8("图片数量：", szTemp);
				a1 = KMPTinS(szInBuf, szTemp, 0);
				a1 += strlen(szTemp);
				sscanf(szInBuf + a1, "%d", &iTemp);
				if (iTemp > 0)
				{
					sprintf(szOutBuf, "%sImageNumber: %d \n", szOutBuf, iTemp);
				}

				fputs(szOutBuf, outfp);

				free(szInBuf);
				szInBuf = NULL;
			}


			fclose(fp);
			fp = NULL;
		}
	}
}
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
int getImgList(char *szDir, char *szFilter)
{
	//传入的szDir应包含'/'作为结尾，并以'\0'终结
	struct _finddata_t files;
	int File_Handle;
	int i = 0;
	int j = 0;
	int iTemp = 0;
	int pageNum = 0;
	char chTemp = 0;
	char szTemp[1024] = { '\0' };
	char objItemPath[] = { "obj/item/" };
	FILE *objItemFP = NULL;
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
		sscanf(files.name, "%d%c", &iTemp, &chTemp);
		if ((0 < iTemp) && ('.' == chTemp))
		{

			sprintf(szTemp, "%s%s", szDir, files.name);
			pageNum = resolveIndexHtml(szTemp);
			//生成新文件的路径
			sprintf(szTemp, "%s%d-.txt", objItemPath, iTemp);
			drunkMkDirs(objItemPath);
			objItemFP = fopen(szTemp, "wt");
			if (NULL != objItemFP)
			{
				//分析基础信息
				sprintf(szTemp, "%s%s", szDir, files.name);
				writeBasicInfo(szTemp, objItemFP);
				//获取Img url
				writeImageURL(szTemp, objItemFP);
				for (j = 2; j <= pageNum; ++j)
				{
					sprintf(szTemp, "%s%d_%d.html", szDir, iTemp, j);
					writeImageURL(szTemp, objItemFP);
				}


				fclose(objItemFP);
				objItemFP = NULL;
			}
		}
		i++;
	} while (0 == _findnext(File_Handle, &files));
	_findclose(File_Handle);
	// 	printf("Find %d files\n", i);

	return (0);
}
void main(int argc, char *argv[])
{

	getImgList("www.meitulu.com/item/", "*.html");
}


