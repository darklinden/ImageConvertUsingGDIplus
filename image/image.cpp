// image.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <Stdio.h> 
#include <Objbase.h> 
//#include <Windows.h> 
#include <Gdiplus.h> 
//#include <GdiPlusEnums.h> 

using namespace Gdiplus; 

#pragma comment(lib,"gdiplus") 

int GetCodecClsid(const WCHAR*, CLSID*);

HRESULT __fastcall AnsiToUnicode(LPCSTR pszA, LPOLESTR *ppszW) 
{ 
	ULONG cCharacters; 
	DWORD dwError; 
	if (NULL == pszA) 
	{ 
		*ppszW = NULL; 
		return NOERROR; 
	} 
	cCharacters = strlen(pszA)+1; 
	*ppszW = (LPOLESTR)CoTaskMemAlloc(cCharacters*2); 
	if (NULL == *ppszW) 
		return E_OUTOFMEMORY; 
	if (0 == MultiByteToWideChar(CP_ACP, 0, pszA, cCharacters,*ppszW, cCharacters)) 
	{ 
		dwError = GetLastError(); 
		CoTaskMemFree(*ppszW); 
		*ppszW = NULL; 
		return HRESULT_FROM_WIN32(dwError); 
	} 
	return NOERROR; 
} 

int LetsChange(WCHAR *src, WCHAR *des)
{
	int n_ret=0; 

	GdiplusStartupInput gdiplusStartupInput; 
	gdiplusStartupInput.SuppressBackgroundThread = FALSE;
	ULONG gdiplusToken = NULL;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	{ 
		CLSID codecClsid; 
		Status stat; 

		Gdiplus::Image img(src, 0);

		GetCodecClsid(L"image/gif", &codecClsid);

		stat = img.Save(des, &codecClsid);  

		if (stat == Ok) 
		{ 
			printf("ok");
			n_ret = 0; 			  
		} 
		else 
		{ 
			printf("fail");
			n_ret = -2;  			  
		}
	} 
	GdiplusShutdown(gdiplusToken); 
	return n_ret;
}

int main()
{
	WCHAR path[MAX_PATH];
	WCHAR src[MAX_PATH];
	WCHAR des[MAX_PATH];
	const WCHAR src_title[11] = L"\\test.bmp";
	const WCHAR des_title[9] = L"\\ok.gif";

	GetModuleFileName(NULL, path, MAX_PATH);

	*wcsrchr(path,'\\') = L'\0'; 
	//截取路径
	
	size_t len = wcslen(path);
	ZeroMemory(src, MAX_PATH * sizeof(WCHAR));
	_memccpy(src, path, sizeof(WCHAR), len * sizeof(WCHAR));
	_memccpy(&src[len], src_title, sizeof(WCHAR), wcslen(src_title) * sizeof(WCHAR));
	
	len = wcslen(path);
	ZeroMemory(des, MAX_PATH * sizeof(WCHAR));
	_memccpy(des, path, sizeof(WCHAR), len * sizeof(WCHAR));
	_memccpy(&des[len], des_title, sizeof(WCHAR), wcslen(des_title) * sizeof(WCHAR));

	LetsChange(src, des);
	getchar();
	return 0; 
}
int GetCodecClsid(const WCHAR* format, CLSID* pClsid) 
{ 
	UINT num = 0; 
	UINT size = 0; 
	ImageCodecInfo* pImageCodecInfo = NULL; 
	GetImageEncodersSize(&num, &size); 
	if (size == 0) 
		return -1;  
	pImageCodecInfo = (ImageCodecInfo*)(malloc(size)); 
	if (pImageCodecInfo == NULL) 
		return -1;  
	GetImageEncoders(num, size, pImageCodecInfo);  
	for (UINT j = 0; j < num; ++j) 
	{ 
		if ( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 ) 
		{ 
			*pClsid = pImageCodecInfo[j].Clsid; 
			return j; 
		} 
	} 
	return -1; 
}
