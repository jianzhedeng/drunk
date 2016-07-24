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
	//1.2�汾
	//�����ԣ�����wchʹ��+���Ż����bug���ĳ�|λ�����
	//λ����֮ǰ��ʹ��&�ų�����������Ĳ���
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
			//���ֽڵĺ��ֱ���
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
			//˫�ֽڵĺ��ֱ���
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
			//askii�ַ�
			*(dst + j) = *(src + i);
			++i;
			++j;
			continue;
		}
		++i;

	}
	*(dst + j) = '\0';
	//i��src���ֽ�����
}

void solveSearchLink(char *szLink)
{
	char szLinkUTF8[128] = { '\0' },
		szLinkGB2312[128] = { '\0' },
		szOutDir[128] = { '\0' },
		szLinkInBat[128] = { '\0' };
	int i = 0, j = 0;
	for (i = 0, j = 0; *(szLink + i) != '\0';)
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
	//��ʼ����utf8�ַ���
	drunkUTF8ToGB2312(szLinkUTF8, szLinkGB2312);

	//����·��;j�ڴ˴���Ϊflag
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

	//bat�ű���%��Ϊת���ַ���%��%%��ʾ
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
void getXcopyBat(char *CatListFile, char *objDir)
{
	FILE *fp = NULL, *modelFP = NULL, *albumFP = NULL;
	fp = fopen(CatListFile, "rt");
	if (fp != NULL)
	{
		int fileSize = 0, modelFileSize = 0, albumFileSize = 0;
		char *szTemp2 = NULL, *szModleFileBuf = NULL, *szAlbumFileBuf = NULL;
		int iCurChar = 0, iModelFileCurChar = 0, iAlbumFileCurChar = 0;
		char szTemp[1024] = { '\0' },
			szModel[1024] = { '\0' },
			szAlbum[1024] = { '\0' },
			szJpg[1024] = { '\0' };
		int iTemp = 0, jTemp = 0;
		fseek(fp, 0, SEEK_END);
		fileSize = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		szTemp2 = (char *)calloc(fileSize + 1, sizeof(char));
		if (NULL != szTemp2)
		{
			fread(szTemp2, sizeof(char), fileSize, fp);
			//��t��ʽ�򿪵��ı���/r/n�Զ�ת��Ϊ/n��Ӧ��strlen���³���
			fileSize = strlen(szTemp2);
			//��ȡ���
			//1Start////////////////////////////////////////////////////////////////////////
			for (iCurChar = 0; iCurChar < fileSize; )
			{
				sscanf(szTemp2 + iCurChar, "%[^'\n']", szTemp);
				iCurChar += strlen(szTemp) + strlen("\n");

				// �õ�album��filePath
				szTemp[strlen(szTemp) - 1] = '\0';
				sprintf(szModel, "%s%s-.txt", objDir, szTemp);

				modelFP = fopen(szModel, "rt");
				if (NULL != modelFP)
				{
					fseek(modelFP, 0, SEEK_END);
					modelFileSize = ftell(modelFP);
					fseek(modelFP, 0, SEEK_SET);
					szModleFileBuf = (char *)calloc(modelFileSize + 1, sizeof(char));
					if (NULL != szModleFileBuf)
					{
						fread(szModleFileBuf, sizeof(char), modelFileSize, modelFP);
						//��t��ʽ�򿪵��ı���/r/n�Զ�ת��Ϊ/n��Ӧ��strlen���³���
						modelFileSize = strlen(szModleFileBuf);
						//2Start////////////////////////////////////////////////////////////////////////
						iModelFileCurChar = 0;
						//�ȶ�ȡ�ڶ��л��ModelName
						sscanf(szModleFileBuf + iModelFileCurChar, 
							"%*[^'\n']%*['\n']"
							"%*s %[^'\n']%*['\n']%n", 
							szModel, &iModelFileCurChar);
						for (; iModelFileCurChar < modelFileSize; )
						{
							sscanf(szModleFileBuf + iModelFileCurChar, "%[^'\n']", szTemp);
							iModelFileCurChar += strlen(szTemp) + strlen("\n");
							
							strncpy(szAlbum, 
								szTemp + strlen("http://"), 
								strlen(szTemp) - strlen("http://") - strlen(".html"));
							*(szAlbum + strlen(szTemp) - strlen("http://") - strlen(".html")) = '\0';
// 							strcat(szAlbum, "-.txt");
							sprintf(szTemp, "%s%s-.txt", objDir, szAlbum);
							strcpy(szAlbum, szTemp);
							albumFP = fopen(szAlbum, "rt");
							if (NULL != albumFP)
							{
								fseek(albumFP, 0, SEEK_END);
								albumFileSize = ftell(albumFP);
								fseek(albumFP, 0, SEEK_SET);
								szAlbumFileBuf = (char *)calloc(albumFileSize + 1, sizeof(char));
								if (NULL != szAlbumFileBuf)
								{
									fread(szAlbumFileBuf, sizeof(char), albumFileSize, albumFP);
									//��t��ʽ�򿪵��ı���/r/n�Զ�ת��Ϊ/n��Ӧ��strlen���³���
									albumFileSize = strlen(szAlbumFileBuf);

									//3Start////////////////////////////////////////////////////////////////////////
									iAlbumFileCurChar = 0;
									//�ȶ�ȡǰ���У���ȡ���ڶ��е�AlbumName
									sscanf(szAlbumFileBuf + iAlbumFileCurChar,
										"%*[^'\n']%*['\n']"
										"%*s %[^'\n']%*['\n']"
										"%*[^'\n']%*['\n']%n",
										szTemp, &iAlbumFileCurChar);
									//Ϊ��֤����������������У�Ӧ��ȥalbumName�е����пո�
									for (iTemp = 0, jTemp = 0; iTemp < (int)strlen(szTemp); ++iTemp)
									{
										if (*(szTemp + iTemp) == ' ')
										{
											continue;
										}
										*(szAlbum + jTemp++) = *(szTemp + iTemp);
									}
									for (; iAlbumFileCurChar < albumFileSize; )
									{
										sscanf(szAlbumFileBuf + iAlbumFileCurChar, "%[^'\n']", szTemp);
										iAlbumFileCurChar += strlen(szTemp) + strlen("\n");

										//Core Start////////////////////////////////////////////////////////////////////////
										//szMode, szAlbum���뱸
										//��szTemp�л��.jpg���ļ���
										for (szJpg[0] = '\0', iTemp = strlen(szTemp) - 1; iTemp >= 0; --iTemp)
										{
											if ('/' == *(szTemp + iTemp) )
											{
												strncpy(szJpg, szTemp + iTemp + 1, strlen(szTemp) - 1 - iTemp);
												szJpg[strlen(szTemp) - 1 - iTemp] = '\0';
												break;
											}
										}
										fprintf(stdout, 
											">>%%outdir%%xcopy.log.txt 2>>%%outdir%%xcopy.err.txt  "
											"xcopy /D /C /Y %%indir%%%s %%outdir%%%s\\%s\\ \n", 
											szJpg, szModel, szAlbum);
										//Core End////////////////////////////////////////////////////////////////////////
									}
									//3End////////////////////////////////////////////////////////////////////////

									free(szAlbumFileBuf);
									szAlbumFileBuf = NULL;
								}

								fclose(albumFP);
								albumFP = NULL;
							}
							else
							{
								//���ļ��򲻿����������������szAlbum�ڴ���������ļ�·��
								fprintf(stderr, "%s\n", szAlbum);

							}
						}
						//2End////////////////////////////////////////////////////////////////////////

						free(szModleFileBuf);
						szModleFileBuf = NULL;
					}
					fclose(modelFP);
					modelFP = NULL;
				}
				else
				{
					//���ļ��򲻿����������������szModel�ڴ���������ļ�·��
					fprintf(stderr, "%s\n", szModel);
				}
			}
			//1End////////////////////////////////////////////////////////////////////////
			free(szTemp2);
			szTemp2 = NULL;
		}
		fclose(fp);
		fp = NULL;
	}
}
int main(int argc, char *argv[])
{
	getXcopyBat("./normallist.txt", "./obj/");

	return (0);
}

