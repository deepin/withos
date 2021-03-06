#include "GUI.h"
#include "WM.h"
#include "BUTTON.h"
#include "DIALOG.h"
#include "LISTBOX.h"
#include "EDIT.h"
#include "SLIDER.h"
#include "FRAMEWIN.h"
#include "RADIO.h"
#include "SCROLLBAR.h"
#include "TEXT.h"
#include "MULTIEDIT.h"
#include "LISTVIEW.h"
#include "MULTIPAGE.h"
#include "GUI_HOOK.h"
#include "FRAMEWIN_Private.h"

#include <string.h>
#include <ctype.h>

#define NULL 0
#include "keyboard.h"


/*******************************************************************
*
* function declare
*/

void clearKeyBaord(WM_HWIN hWin);
void createKeys(KEYBAORDCFG* kbcfg);

/*******************************************************************
*
* 键盘的按键配置和键盘布局全局变量
*/

EDIT_Handle hEdtDebug,hEdtMsgC;

static WM_HWIN ghTarget;
static char* gCurrentKeyMap;
static U8 gShift;
static WM_HWIN gKeyBaord;
static GUI_HOOK gHook;
#define keyMapShift 3000
#define scaleFactorX 2
#define scaleFactorY 2
static char keys[]=
{
    '1','2','3','4','5','6','7','8','9','0',
    '+','.','/','-','*'
};


#define WindowWidth 300
#define WindowHeight 400
#define TitleHeight 10
#define TotalNum 21
#define KeyGapRatioX 200
#define KeyGapRatioY 160
#define KeyMarginRatioH 10
#define KeyMarginRatioV 10
#define ButtonXYRatio (1.0)
#define ClientHeight (WindowHeight - TitleHeight)
#define ClientWidth WindowWidth
#define XYRatio (ClientWidth/ClientHeight)
unsigned YNum;
unsigned XNum;// = (YNum*XYRatio/ButtonXYRatio);
unsigned ButtonWidth;// (ClientWidth / ((2/KeyMarginRatioH) + (XNum) + ((XNum-1)/KeyGapRatioX)))
unsigned ButtonHeight;// (ClientHeight / ((2/KeyMarginRatioV) + (YNum) + ((YNum-1)/KeyGapRatioY)))
unsigned MarginH;// (ButtonWidth / KeyMarginRatioH)
unsigned MarginV;// (ButtonHeight / KeyMarginRatioV)
unsigned GapX;// (ButtonWidth / KeyGapRatioX)
unsigned GapY;// (ButtonHeight / KeyGapRatioY)
unsigned FunctionKeyWidth;// (ButtonWidth*2+GapX)
#define ErrorX 0
#define ErrorY 0
void configButtonArray(void)
{
		double tmpYNum, a, b, c, delta;
		double theRatio;
		theRatio = XYRatio;
		a = 1.0*ClientWidth / ClientHeight + 1.0*ClientWidth / ClientHeight/KeyGapRatioY;
		b = 1.0*2*ClientWidth / ClientHeight/KeyMarginRatioV - 1.0*ClientWidth / ClientHeight/KeyGapRatioY - 1.0*2*ButtonXYRatio/KeyMarginRatioH + 1.0*ButtonXYRatio/KeyGapRatioX;
		c = -1.0*ButtonXYRatio*TotalNum*(1+1.0/KeyGapRatioX);
		delta = sqrt(b*b - 4*a*c);
		tmpYNum = (-b + delta) / 2 / a;
		YNum = (unsigned)tmpYNum;
		XNum = (unsigned)(TotalNum / YNum)+1;
		tmpYNum = ((double)ClientWidth / ((2/(double)KeyMarginRatioH) + ((double)XNum) + (((double)XNum-1)/(double)KeyGapRatioX)));
		ButtonWidth = (unsigned)tmpYNum;
		tmpYNum = ((double)ClientHeight / ((2/(double)KeyMarginRatioV) + ((double)YNum) + (((double)YNum-1)/(double)KeyGapRatioY)));
		ButtonHeight = (unsigned)tmpYNum;
		
		MarginH = (unsigned)ButtonWidth / KeyMarginRatioH;
		MarginV = (unsigned)(ButtonHeight / KeyMarginRatioV);
		GapX = (unsigned)(ButtonWidth / KeyGapRatioX);
		GapY = (unsigned)(ButtonHeight / KeyGapRatioY);
}


void createNumKeys(KEYBAORDCFG* kbcfg)
//创建按键
{
    int x0,y0,xSize,ySize,xSpan,ySpan;
    int i,starX,startY,num;
    WM_HWIN hWin;
    BUTTON_Handle hbtn;
    char tmp[20],*keyMap;
		configButtonArray();
    num=kbcfg->num;
    hWin=kbcfg->hWin;
    keyMap=kbcfg->keymap;
		for(i = 0; i < num; ++i){
				ii = (i/XNum)+1;
				jj = (i%(unsigned char)XNum) + 1;
				y0 = 	MarginV + (ii-1)*(ButtonHeight+GapY);
				x0 = 	MarginH + (jj-1)*(ButtonWidth+GapX);
				BUTTON_CreateAsChild(x0,y0,ButtonWidth,ButtonHeight,hWin,keyMap[i]+keyMapShift,BUTTON_CF_SHOW);
		}		

 /*   hbtn=BUTTON_CreateAsChild(x0=starX,y0=y0+ySpan+ySize,(xSize*3+2*xSpan),ySize,hWin,20+keyMapShift,BUTTON_CF_SHOW);
    BUTTON_SetText(hbtn,"Shift");
    hbtn=BUTTON_CreateAsChild(x0=x0+3*xSpan+3*xSize,y0,(xSize*6+5*xSpan),ySize,hWin,' '+keyMapShift,BUTTON_CF_SHOW);
    BUTTON_SetText(hbtn,"");

    hbtn=BUTTON_CreateAsChild(x0=starX+12*xSize+12*xSpan,y0=startY,xSize,ySize,hWin,8+keyMapShift,BUTTON_CF_SHOW);
    BUTTON_SetText(hbtn,"bs");
    hbtn=BUTTON_CreateAsChild(x0,y0+ySpan+ySize,xSize*2,3*ySize+2*ySpan,hWin,13+keyMapShift,BUTTON_CF_SHOW);
    BUTTON_SetText(hbtn,"En");*/

}

WM_HWIN CreateNumKeyBaord(unsigned x0, unsigned y0)
{
    WM_HWIN hFW,hWin;
    KEYBAORDCFG kbcfg;

    hFW=FRAMEWIN_Create("KeyBaord", &_cbKeyBaord, WM_CF_SHOW,x0, y0,WindowWidth,WindowHeight);
    FRAMEWIN_SetMoveable(hFW,FRAMEWIN_SF_MOVEABLE);
    //FRAMEWIN_AddCloseButton(hFW,FRAMEWIN_BUTTON_RIGHT,1);
    hWin=WM_GetClientWindow(hFW);
    gKeyBaord=hFW;
		FRAMEWIN_SetTitleHeight(hFW, TitleHeight);

    gCurrentKeyMap=keys;
    kbcfg.hWin=hWin;
    kbcfg.keymap=gCurrentKeyMap;
    kbcfg.x0=8;
		kbcfg.y0=10;
    kbcfg.xSize=15*scaleFactorX;
    kbcfg.ySize=18*scaleFactorY;
    kbcfg.xSpan=2*scaleFactorX;
    kbcfg.ySpan=3*scaleFactorY;
    kbcfg.num=sizeof(keys)/sizeof(char);
    createKeys(&kbcfg);

    //WM_SetCallback(hWin,_cbKeyBaord);
		return hFW;
}

/*******************************************************************
*
* callbacks
*/

 int _hookFW(WM_MESSAGE * pMsg)
/*目标窗口的挂钩函数*/
{
    if (pMsg->MsgId==WM_TOUCH_CHILD)//pMsg->MsgId==WM_NOTIFY_PARENT|| 
    {
        GUI_PID_STATE * pState;
      pState = ( GUI_PID_STATE * )( ( WM_MESSAGE * )pMsg->Data.p )->Data.p;
        if (( pState!=0 )&&(pState->Pressed))
        {
            I32 NCode=pMsg->Data.v;
            if ((pMsg->hWinSrc)&&(NCode!=WM_NOTIFICATION_LOST_FOCUS )&&(NCode!=WM_NOTIFICATION_CHILD_DELETED))
            {
                ghTarget=pMsg->hWinSrc;
            }
        }
    }
		//WM_SendMessage(pMsg ->hWin, pMsg);
    return 0;
}
 void _setHook(WM_HWIN hWin)
/*将处理按键的挂钩函数关联到指定窗口*/
{
	

    FRAMEWIN_Obj* pObj;
	GUI_Lock();//asion 20091016 add
	pObj= FRAMEWIN_H2P(hWin);//asion 20091016 edit
    GUI_HOOK_Add(&pObj->pFirstHook,&gHook,&_hookFW);
	GUI_Unlock();//asion 20091016 add
}

//#include "filerw.h"
 void _cbKeyBaord(WM_MESSAGE * pMsg)
/*键盘的回调函数*/
{
		I32 Id;
		WM_KEY_INFO key ={0};
		WM_MESSAGE msg={0};
	showModeCounter = 0;
  switch (pMsg->MsgId){
		
    case WM_PAINT:
        GUI_SetBkColor(0xc3c3c3);
        GUI_Clear();
        GUI_SetBkColor(0xFFFFFF);
        break;
		
    case WM_NOTIFY_PARENT:
			{  			
				if (pMsg->Data.v!=WM_NOTIFICATION_RELEASED){
					break;
				}        //Id=GUI_GetKey();
				Id=WM_GetId(pMsg->hWinSrc);//asion 20091016 edit add

        if (Id!=NULL)//&&(pMsg->hWin!=pMsg->hWinSrc))
        {
           switch (Id)
           {
            case 20+keyMapShift:
            {
                KEYBAORDCFG kbcfg={0};
                clearKeyBaord(WM_GetClientWindow( pMsg->hWin));
                gCurrentKeyMap=(gCurrentKeyMap==keys)?keys2:keys;
                kbcfg.hWin= pMsg->hWin;
                kbcfg.keymap=gCurrentKeyMap;
                kbcfg.x0=5;
								kbcfg.y0=15;
                kbcfg.xSize=15*scaleFactorX;
                kbcfg.ySize=18*scaleFactorY;
                kbcfg.xSpan=2*scaleFactorX;
                kbcfg.ySpan=3*scaleFactorY;
                kbcfg.num=sizeof(keys)/sizeof(char);
                createKeys(&kbcfg);
            }
            break;
						
            case 25+keyMapShift:
                gShift=~gShift;
                break;
            case 13+keyMapShift:
                WM_BringToBottom(pMsg ->hWin);//gShift=~gShift;
                break;						
            default:
                if (WM_IsWindow(ghTarget)&&(iscntrl(Id-keyMapShift)||isprint(Id-keyMapShift))&&(Id-keyMapShift<0x7f))
                {                    
                    key.Key=Id-keyMapShift;
                    key.PressedCnt=1;
                    msg.MsgId=WM_KEY;
                    msg.Data.p=&key;
                    WM_SendMessage(ghTarget,&msg);
                    //GUI_SendKeyMsg (Id, 1);
                    //WM_BroadcastMessage(&gMsg);
                    WM_SetFocus(ghTarget);
										WM_BringToTop(pMsg->hWin);
                }
            }

        }
    }
    break;
		
		
    default:
        WM_DefaultProc(pMsg);

    }
}


void clearKeyBaord(WM_HWIN hWin)
{

    WM_HWIN hCWin;

    for (hCWin=WM_GetFirstChild(hWin); hCWin!=0; hCWin=WM_GetFirstChild(hWin))
    {
        //hCWin=WM_GetNextSibling(hCWin)
        WM_DeleteWindow(hCWin);
    }
}
