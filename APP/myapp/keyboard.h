#ifndef _keyboard
#define _keyboard
#include "wm.h"
#include "edit.h"

typedef struct
{/*�������̵Ĳ����ṹ*/
    WM_HWIN hWin;
    char* keymap;
    int x0,y0,xSize,ySize,xSpan,ySpan;
    int num;
} KEYBAORDCFG;


int _hookFW(WM_MESSAGE * pMsg);
void _setHook(WM_HWIN hWin);
void _cbKeyBaord(WM_MESSAGE * pMsg);
void clearKeyBaord(WM_HWIN hWin);
WM_HWIN CreateKeyBaord(void);

#endif