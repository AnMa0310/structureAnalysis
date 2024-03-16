/*HOGANSKELETON001:CODED*/

#include <vcl.h>
#pragma hdrstop
#include <tchar.h>

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include <limits.h>
#include <time.h>

#include "canhead.h"                    /*DEFINITION OF COMMAND ID.*/

#define INPUTWIDTH 158 /*146*/      /*DIALOGUE FONT:"SYSTEM" 18pts.*/
#define BARWIDTH 10                          /*WIDTH OF SCROLL BAR.*/
#define DIALOGFACTOR 2.62 /*2.75*/
#define MAXWRITE 65535 /*65535*/ /*FOR CREATING BITMAP FILE*/
#define BOXMOUSE 5 /*BOX MOUSE SIZE.*/
#define RED   0
#define GREEN 1
#define BLUE  2

/*#ifndef PI*/
#define PI 3.1415926535897932384
/*#endif*/

/*STRUCTS*/
struct windowparams{
					int code;
					int nstring,nchilds;
					int sstatus;                    /*SCROLL STATUS*/
					char *classname;               /*NAME OF CLASS.*/
					HWND hwnd;
					HDC hdcD,hdcB,hdcC;     /*DISPLAY,BACK,COMPATI.*/
					int gstatus;                  /*GENERAL STATUS.*/
					int lstatus,rstatus;            /*MOUSE STATUS.*/
					struct windowparams *childs;   /*CHILD WINDOWS.*/
					int tx,ty;             /*MESSAGE TEXT POSITION.*/
					RECT vbar,hbar;                   /*SCROLL BAR.*/
				   };
struct winparamsreg{
					int code;
					int nwin;
					struct windowparams **wp;
				   };                   /*REGISTRY OF WINDOWPARAMS.*/

struct windowparams wmain={0,0,0,NEUTRAL,"CanMainWin",
						   NULL,NULL,NULL,NULL,
						   NEUTRAL,NEUTRAL,NEUTRAL,
						   NULL};
struct windowparams wdraw={2,0,0,NEUTRAL,"CanDrawWin",
						   NULL,NULL,NULL,NULL,
						   NEUTRAL,NEUTRAL,NEUTRAL,
						   NULL,0,0};

/*WINDOW SUBROUTINES.*/
LRESULT CALLBACK WindowProcedureMain(HWND,UINT,WPARAM,LPARAM);
LRESULT CALLBACK WindowProcedureSheet(HWND,UINT,WPARAM,LPARAM);
LRESULT CALLBACK WindowProcedureBack(HWND,UINT,WPARAM,LPARAM);
LRESULT CALLBACK WindowProcedureDraw(HWND,UINT,WPARAM,LPARAM);
static BOOL CALLBACK EnumChildProcSheet(HWND hwnd,LPARAM lParam);
int classnamealloc(struct windowparams *wp,char *cname);
int classdefinition(HINSTANCE hInstance,
					char *lpszclassname,
					WNDPROC lpfnwndproc,
					char *lpszmenuname,
					int br,int bg,int bb);
HDC createcompati(struct windowparams w);
HWND windowdefinition(struct windowparams *wp,
					  HINSTANCE hInstance,
					  char *lpszclassname,
					  WNDPROC lpfnwndproc,
					  char *lpszmenuname,
					  int br,int bg,int bb,
					  DWORD dwStyle,
					  int x,int y,int w,int h,
					  HWND hparent);
int registwindowparams(struct winparamsreg *wrapp,
					   struct windowparams *wpreg);
void clearwindow(struct windowparams wp);
void DrawSunken(HDC hdc,int left,int top,int right,int bottom);
RECT rectlocation(int l,int t,int r,int b);
struct windowparams *getwindowparams(HWND hwnd);
void vbarlocation(HWND hparent,HWND hchild,RECT *vbar);
void hbarlocation(HWND hparent,HWND hchild,RECT *hbar);
void drawvbar(HDC hdc,long int maxX,long int maxY,
			  struct windowparams *wp);
void drawhbar(HDC hdc,long int maxX,long int maxY,
			  struct windowparams *wp);
void setfontformat(HDC hdc,int h,int w,
				   char *fontname,int r,int g,int b);
void getclientsize(HWND hwnd,long int *w,long int *h);
void getwindowsize(HWND hwnd,long int *w,long int *h);
int overlayhdc(struct windowparams wp,DWORD dwrop);
HDC extendhdc(HDC hdc,long int w,long int h);

/*PROJECTION SUBROUTINES.*/
void drawgloballine(HDC hdc,int Hx,int Hy,int Tx,int Ty);

/*GLOBAL PARAMETERS*/
HINSTANCE hInstGlobal;                                  /*INSTANCE.*/
struct winparamsreg wrglobal={0,0,NULL};   /*REGISTED WINDOWPARAMS.*/
int gflag=1,px,py;
int globalstatus=NEUTRAL;
int prestatus=NEUTRAL;
POINT pbar;                                /*RELATIVE POINT ON BAR.*/
long int initx,inity;       /*INITIAL POSITION OF ROTATE,MOVE,TEXT.*/

int WINAPI WinMain(HINSTANCE hInstance,
				   HINSTANCE hPrevInstance,
				   LPSTR lpszCmdLine,
				   int nCmdShow)/*WINAPI? APIENTRY?*/
{
  HWND hwnd;
  HACCEL hAccel; /*ACCELERATOR*/
  MSG msg;

  if(!hPrevInstance)                      /*WINDOW CLASS DEFINITION*/
  {
	if(!classdefinition(hInstance,
						wmain.classname,
						WindowProcedureMain,
						"CANMENU",
						130,130,130)) return 0;
  }

  hInstGlobal = hInstance;

  hwnd = CreateWindow(wmain.classname,               /*WINDOW CLASS*/
					  "Hoganshi Skeleton",                /*CAPTION*/
					  WS_CLIPCHILDREN |
					  WS_CLIPSIBLINGS |
					  WS_OVERLAPPEDWINDOW,                  /*STYLE*/
					  50,10,                                  /*X,Y*/
					  400,600,                       /*WIDTH,HEIGHT*/
					  HWND_DESKTOP,                 /*PARENT WINDOW*/
					  NULL,                               /*MENU ID*/
					  hInstance,
					  NULL);
  wmain.hwnd=hwnd;

  registwindowparams(&wrglobal,&wmain);             /*REGISTRATION.*/
  hAccel=LoadAccelerators(hInstance,"CANMENU");          /*SHORTCUT*/

  ShowWindow(hwnd,nCmdShow);
  UpdateWindow(hwnd);             /*DISPATCH "WM_PAINT" IF UPDATED.*/

  while(GetMessage(&msg,NULL,0,0))
  {
	if(!TranslateAccelerator(hwnd,hAccel,&msg))
	{
	  TranslateMessage(&msg);                 /*KEY INTO CHARACTER.*/
	  DispatchMessage(&msg);                    /*DISPATCH MESSAGE.*/
	}
  }                                       /*REPEAT UNTIL "WM_QUIT".*/

  return msg.wParam;
}/*WinMain*/

LRESULT CALLBACK WindowProcedureMain(HWND hwnd,
									 UINT message,
									 WPARAM wParam,
									 LPARAM lParam)
/*WINDOW PROCEDURE FOR MAIN WINDOW.CREATION,DESTRUCTION OF WINDOWS.*/
{
  HDC hDC,hdc;
  HPEN hpen;
  HFONT hfont;
  HBITMAP hbit,pbit;
  HBRUSH hbrush,pbrush;
  WORD x,y;                                   /*WORD=unsigned short*/
  WPARAM wparam;
  BYTE *hp;                                        /* byte pointer */
  DWORD dwTmp;
  size_t n;
  char str[256];
  int i;
  long int maxX,maxY;

  switch(message)
  {
	case WM_PAINT:
	  DefWindowProc(hwnd,message,wParam,lParam);
	  if(wdraw.hwnd==NULL)
	  {
		wparam = MAKEWPARAM((WORD)IDM_DRAWINGS,(WORD)0);
		SendMessage(wmain.hwnd,WM_COMMAND,wparam,(WPARAM)0);
	  }
	  break;

	case WM_SIZE:
	  DefWindowProc(hwnd,message,wParam,lParam);
	  wparam = MAKEWPARAM((WORD)IDM_FITSHEET,(WORD)0);
	  if(wdraw.hwnd!=NULL)
	  {
		SendMessage((wdraw.childs+1)->hwnd,WM_COMMAND,wparam,0);
	  }
	  break;

	case WM_COMMAND:
	  switch(LOWORD(wParam))
	  {
		case IDM_END:                            /*END APPLICATION.*/
		  PostQuitMessage(0);                   /*CREATE "WM_QUIT".*/
		  break;

		case IDM_DRAWINGS:
		  if(wdraw.hwnd==NULL) /*CREATION.*/
		  {
			getclientsize(hwnd,&maxX,&maxY);

			windowdefinition(&wdraw,
							 hInstGlobal,
							 NULL,
							 WindowProcedureSheet,
							 NULL,
							 190,190,190,
							 WS_CLIPCHILDREN |
							 WS_CLIPSIBLINGS |
							 WS_CHILD |
							 WS_THICKFRAME |
							 WS_VISIBLE,
							 0,0,maxX,maxY,
							 wmain.hwnd);

			getclientsize(wdraw.hwnd,&maxX,&maxY);

			wdraw.nchilds=2;
			wdraw.childs=(struct windowparams *)
						 malloc(2*sizeof(struct windowparams));

			windowdefinition(wdraw.childs+0,
							 hInstGlobal,
							 "CanDrawBak",
							 WindowProcedureBack,
							 NULL,
							 190,190,190,
							 WS_CLIPCHILDREN |
							 WS_CLIPSIBLINGS |
							 WS_CHILD |
							 WS_VISIBLE,
							 1,1,maxX-BARWIDTH-5,maxY-BARWIDTH-5,
							 wdraw.hwnd);

			windowdefinition(wdraw.childs+1,
							 hInstGlobal,
							 "CanDrawDsp",
							 WindowProcedureDraw,
							 NULL,
							 0,0,0,
							 /*WS_CLIPCHILDREN |*/
							 WS_CLIPSIBLINGS |
							 WS_CHILD |
							 WS_VISIBLE,
							 0,0,1200,900,         /*0,0,900,1200,*/
							 (wdraw.childs+0)->hwnd);
			setfontformat((wdraw.childs+1)->hdcC,15,6,
						  "�l�r ����",0,0,255);

			getclientsize((wdraw.childs+1)->hwnd,&maxX,&maxY);
		  }
		  else if(wdraw.hwnd!=NULL) /*DESTRUCTION.*/
		  {
			for(i=1;i<=wdraw.nchilds;i++)
			{
			  ReleaseDC((wdraw.childs+i-1)->hwnd,
						(wdraw.childs+i-1)->hdcB);
			  ReleaseDC((wdraw.childs+i-1)->hwnd,
						(wdraw.childs+i-1)->hdcC);
			}
			ReleaseDC(wdraw.hwnd,wdraw.hdcB);
			ReleaseDC(wdraw.hwnd,wdraw.hdcC);
			DestroyWindow(wdraw.hwnd);

			free(wdraw.childs);
			wdraw.nchilds=0;
			wdraw.hwnd = NULL;
		  }
		  break;

		case IDM_HELP:                                 /*HELP TEXT.*/
		  MessageBox(wmain.hwnd,"No Help","Help",MB_OK);
		  break;

		default: /*OTHERS.*/
		  return DefWindowProc(hwnd,message,wParam,lParam);
	  }
	  break;

	case WM_LBUTTONDOWN:
	  hdc = GetDC(hwnd);
	  x = LOWORD(lParam);
	  y = HIWORD(lParam);
	  sprintf(str,"POSITION:%d %d",x,y);
	  SetBkMode(hdc,TRANSPARENT);                 /*TEXT BACKGROUND*/
	  TextOut(hdc,x,y,str,strlen(str));
	  ReleaseDC(hwnd,hdc);               /*CASE REDRAW UNAVAILABLE.*/
	  /*InvalidateRect(hwnd,NULL,TRUE);*/            /*CASE REDRAW.*/
	  break;

	case WM_DESTROY:
	  PostQuitMessage(0);                       /*CREATE "WM_QUIT".*/
	  break;

	default:
	  return DefWindowProc(hwnd,message,wParam,lParam);
  }
  return 0;
}/*WindowProcedureMain*/

LRESULT CALLBACK WindowProcedureSheet(HWND hwnd,
									  UINT message,
									  WPARAM wParam,
									  LPARAM lParam)
/*SHEET WINDOW WITH VSCROLL,HSCROLL.*/
{
  HDC hdc;
  HBRUSH hbrush;
  POINT point,cp;
  int bl,bt,br,bb,bmax; /*LEFT,TOP,RIGHT,BOTTOM OF BAR.*/
  long int cw,ch,pw,ph,maxX,maxY,wintop,winleft; /*WINDOW SIZE.*/
  struct windowparams *wp;

  switch(message)
  {
	case WM_PAINT:
	case WM_SIZE:
	  DefWindowProc(hwnd,message,wParam,lParam);
	  /*EnumChildWindows(hwnd,(DLGPROC)EnumChildProcSheet,0);*/ /*UPDATE CHILDS.*/
	  EnumChildWindows(hwnd,EnumChildProcSheet,0); /*UPDATE CHILDS.*/

	  getclientsize(hwnd,&maxX,&maxY);

	  hdc=GetDC(hwnd);

	  hbrush = (HBRUSH)GetClassLong(hwnd,GCL_HBRBACKGROUND);
	  SelectObject(hdc,hbrush);
	  PatBlt(hdc,0,0,maxX,maxY,PATCOPY);

	  DrawSunken(hdc,0,0,(maxX-BARWIDTH-4),(maxY-BARWIDTH-4));
	  DrawSunken(hdc,(maxX-BARWIDTH-2),0,(maxX-1),(maxY-BARWIDTH-4));
      DrawSunken(hdc,0,(maxY-BARWIDTH-2),(maxX-BARWIDTH-4),(maxY-1));
      DrawSunken(hdc,(maxX-BARWIDTH-2),(maxY-BARWIDTH-2),
                     (maxX-1),(maxY-1));

      wp=getwindowparams(hwnd);
	  if(wp!=NULL && ((wp->childs)+1)!=NULL)
      {
        vbarlocation(wp->hwnd,(wp->childs+1)->hwnd,&(wp->vbar));
        hbarlocation(wp->hwnd,(wp->childs+1)->hwnd,&(wp->hbar));
        drawvbar(hdc,maxX,maxY,wp); /*REDRAW VBAR.*/
        drawhbar(hdc,maxX,maxY,wp); /*REDRAW HBAR.*/
      }
      ReleaseDC(hwnd,hdc);

      /*ExcludeClipRect(hdc,x1,y1,x2,y2);*/
      break;

    case WM_LBUTTONDOWN:
      point.x = LOWORD(lParam);
      point.y = HIWORD(lParam);

      wp=getwindowparams(hwnd);
      if(wp==NULL) break;

      getclientsize(hwnd,&maxX,&maxY); /*PARENT SHEET.*/
      getclientsize((wp->childs+0)->hwnd,&pw,&ph); /*BACKGROUND.*/
      getwindowsize((wp->childs+1)->hwnd,&cw,&ch); /*CHILD.*/

      if(/*ch>ph &&*/ PtInRect(&(wp->vbar),point))
	  {
        wp->sstatus=VSCROLLING; /*VERTICAL SCROLL BEGIN.*/
        pbar=point;
      }
      else if(/*cw>pw &&*/ PtInRect(&(wp->hbar),point))
      {
        wp->sstatus=HSCROLLING; /*HORIZONTAL SCROLL BEGIN.*/
        pbar=point;
      }
      else
      {
        wp->sstatus=NEUTRAL; /*SCROLL END.*/
      }
      break;

    case WM_MOUSEMOVE:
      wp=getwindowparams(hwnd);
      if(wp==NULL) break;

      if((wParam == MK_LBUTTON)&&
		 (wp->sstatus==VSCROLLING || wp->sstatus==HSCROLLING))
      {
        point.x = LOWORD(lParam);
        point.y = HIWORD(lParam);

        getclientsize(hwnd,&maxX,&maxY);
        getwindowsize((wp->childs+1)->hwnd,&cw,&ch); /*CHILD.*/

		cp.x=0;
        cp.y=0;
        ClientToScreen((wp->childs+1)->hwnd,&cp);
        ScreenToClient(hwnd,&cp); /*TOPLEFT OF CHILD.*/

        hdc=GetDC(hwnd);

        if(wp->sstatus==VSCROLLING)
		{
          bmax=maxY-BARWIDTH-5;

          bt=wp->vbar.top+point.y-pbar.y;
          bb=wp->vbar.bottom+point.y-pbar.y;

		  if(cp.y>1)
          {
            wp->vbar.top=1;
            if(bb>=bmax)     wp->vbar.bottom=bmax;
            else if(bb<bmax) wp->vbar.bottom=bb;
          }
          else if(ch+cp.y-1<=bmax)
          {
            wp->vbar.bottom=bmax;
            if(bt<=1 || cp.y==1) wp->vbar.top=1;
            else if(bt>1)        wp->vbar.top=bt;
          }
          else
          {
            if(bt<=1)
            {
			  wp->vbar.top   =1;
              wp->vbar.bottom=1+bb-bt;
            }
            else if(bb>=bmax)
            {
              wp->vbar.bottom=bmax;
			  wp->vbar.top   =bmax-bb+bt;
            }
            else
            {
              wp->vbar.top   =bt;
              wp->vbar.bottom=bb;
            }
          }
		  pbar=point;

          drawvbar(hdc,maxX,maxY,wp); /*REDRAW VBAR.*/
        }
        else if(wp->sstatus==HSCROLLING)
        {
		  bmax=maxX-BARWIDTH-5;

          bl=wp->hbar.left +point.x-pbar.x;
          br=wp->hbar.right+point.x-pbar.x;

          if(cp.x>1)
		  {
            wp->hbar.left=1;
            if(br>=bmax)     wp->hbar.right=bmax;
            else if(br<bmax) wp->hbar.right=br;
          }
          else if(cw+cp.x-1<=bmax)
          {
            wp->hbar.right=bmax;
            if(bl<=1 || cp.x==1) wp->hbar.left=1;
            else if(bl>1)        wp->hbar.left=bl;
          }
          else
          {
            if(bl<=1)
            {
              wp->hbar.left =1;
              wp->hbar.right=1+br-bl;
            }
			else if(br>=bmax)
            {
              wp->hbar.right=bmax;
              wp->hbar.left =bmax-br+bl;
            }
            else
			{
              wp->hbar.left =bl;
              wp->hbar.right=br;
            }
          }
          pbar=point;

          drawhbar(hdc,maxX,maxY,wp); /*REDRAW HBAR.*/
		}
		ReleaseDC(hwnd,hdc);

		winleft=-(cw)*(wp->hbar.left-1)/(maxX-BARWIDTH-6);
		wintop =-(ch)*(wp->vbar.top -1)/(maxY-BARWIDTH-6);

		MoveWindow((wp->childs+1)->hwnd,winleft,wintop,cw,ch,TRUE);
	  }
	  else
	  {
		wp->sstatus=NEUTRAL; /*SCROLL END*/
	  }
	  break;

	default:
	  return DefWindowProc(hwnd,message,wParam,lParam);
  }
  return 0;
}/*WindowProcedureSheet*/

LRESULT CALLBACK WindowProcedureBack(HWND hwnd,
									 UINT message,
									 WPARAM wParam,
									 LPARAM lParam)
/*DISPLAY AREA OF SHEET WINDOW.*/
{
  HWND hoya;
  long int maxX,maxY;

  switch(message)
  {
	case WM_COMMAND:
	  switch(LOWORD(wParam))
	  {
		case IDM_FITPARENT: /*FIT SIZE TO PARENT.*/
		  hoya=GetParent(hwnd);
		  getclientsize(hoya,&maxX,&maxY);

		  MoveWindow(hwnd,1,1,
						  (maxX-BARWIDTH-5),(maxY-BARWIDTH-5),TRUE);
		  break;

		default: /*OTHERS*/
		  return DefWindowProc(hwnd,message,wParam,lParam);
	  }
	  break;

	default:
	  return DefWindowProc(hwnd,message,wParam,lParam);
  }
  return 0;
}/*WindowProcedureBack*/

LRESULT CALLBACK WindowProcedureDraw(HWND hwnd,
									 UINT message,
									 WPARAM wParam,
									 LPARAM lParam)
/*WINDOW FOR DRAWING WITH POPUP MENU.*/
{
  HWND hoya;
  HDC hdc;
  HCURSOR hcursor;
  HPEN hpen;
  WPARAM wparam;
  /*MSG msg;*/
  /*HACCEL haccel;*/
  size_t n;
  char str[256];
  long int maxX,maxY,x,y;

  switch(message)
  {
	case WM_PAINT:
	  DefWindowProc(hwnd,message,wParam,lParam);
	  overlayhdc(*(wdraw.childs+1),SRCPAINT);
	  hdc = GetDC(hwnd);
	  setfontformat(hdc,15,6,"�l�r ����",255,255,255);
	  break;

	case WM_LBUTTONDOWN:
	  x = LOWORD(lParam);
	  y = HIWORD(lParam);

	  sprintf(str,"POSITION:%d %d",x,y);
	  SetTextColor((wdraw.childs+1)->hdcC,RGB(150,0,255)); /*COLOR.*/
	  TextOut((wdraw.childs+1)->hdcC,x,y,str,strlen(str));

	  if(gflag==1)
	  {
		px=x;
		py=y;
		gflag++;
	  }
	  else if(gflag==2)
	  {
		sprintf(str,"POSITION:%d %d",px,py);
		TextOut((wdraw.childs+1)->hdcC,px,py,str,strlen(str));

		hpen=CreatePen(PS_SOLID,1,RGB(100,0,200));
		SelectObject((wdraw.childs+1)->hdcC,hpen);
		drawgloballine((wdraw.childs+1)->hdcC,px,py,x,y);
		gflag--;
	  }

	  SendMessage(hwnd,WM_PAINT,0,0);
	  break;

	case WM_RBUTTONDOWN:
	  x = LOWORD(lParam);
	  y = HIWORD(lParam);
	  sprintf(str,"POSITION:%d %d",x,y);
	  SetTextColor((wdraw.childs+1)->hdcC,RGB(255,0,150)); /*COLOR.*/
	  TextOut((wdraw.childs+1)->hdcC,x,y,str,strlen(str));
	  SendMessage(hwnd,WM_PAINT,0,0);
	  break;

	case WM_MOUSEMOVE:
	  if(gflag==2)
	  {
		x = LOWORD(lParam);
		y = HIWORD(lParam);
		sprintf(str,"POSITION:%d %d",px,py);
		TextOut((wdraw.childs+1)->hdcC,px,py,str,strlen(str));
		sprintf(str,"POSITION:%d %d",x,y);
		TextOut((wdraw.childs+1)->hdcC,x,y,str,strlen(str));
		hpen=CreatePen(PS_SOLID,1,RGB(100,0,200));
		SelectObject((wdraw.childs+1)->hdcC,hpen);
		drawgloballine((wdraw.childs+1)->hdcC,px,py,x,y);
		SendMessage((wdraw.childs+1)->hwnd,WM_PAINT,0,0);
		clearwindow(*(wdraw.childs+1));
	  }
	  if((wParam == MK_RBUTTON))
	  {
		clearwindow(*(wdraw.childs+1));
		SendMessage((wdraw.childs+1)->hwnd,WM_PAINT,0,0);
	  }
	  break;

	case WM_COMMAND:
	  switch(LOWORD(wParam))
	  {
		case IDM_FITSHEET: /*NEVER USE FITPARENT.*/
		  hoya=GetParent(GetParent(hwnd)); /*PARENT SHEET.*/
		  getclientsize(wmain.hwnd,&maxX,&maxY);
		  MoveWindow(hoya,0,0,maxX,maxY,TRUE);
		  break;

		default: /*OTHERS.*/
		  return DefWindowProc(hwnd,message,wParam,lParam);
	  }
	  break;

	default:
	  return DefWindowProc(hwnd,message,wParam,lParam);
  }
  return 0;
}/*WindowProcedureDraw*/

static BOOL CALLBACK EnumChildProcSheet(HWND hwnd,LPARAM lParam)
{
  WORD wlow;
  WPARAM wparam;

  wlow = IDM_FITPARENT; /*ONLY TO "WINDOWPROCEDUREBACK".*/
  wparam = MAKEWPARAM(wlow,(WORD)0);
  SendMessage(hwnd,WM_COMMAND,wparam,lParam);

  return TRUE;
}/*EnumChildProcSheet*/

int classnamealloc(struct windowparams *wp,char *cname)
{
  wp->classname=(char *)malloc(strlen(cname)+1);
  if(wp->classname==NULL) return 0;
  sprintf(wp->classname,cname);

  return 1;
}/*classnamealloc*/

int classdefinition(HINSTANCE hInstance,
					char *lpszclassname, /*CLASS NAME.*/
					WNDPROC lpfnwndproc, /*WINDOW PROCEDURE.*/
					char *lpszmenuname, /*MENU NAME.*/
					int br,int bg,int bb) /*BACKGROUND BRUSH.*/
{
  WNDCLASS wcl;

  wcl.hInstance = hInstance;
  wcl.lpszClassName = lpszclassname;
  wcl.lpfnWndProc = lpfnwndproc;
  wcl.style = 0;                                    /*STYLE DEFAULT*/
  wcl.hIcon = LoadIcon(hInstance,"CANICON");                 /*ICON*/
  wcl.hCursor = LoadCursor(hInstance,"CANCURSORW");        /*CURSOR*/
  wcl.lpszMenuName = lpszmenuname;                           /*MENU*/
  wcl.cbClsExtra = 0;
  wcl.cbWndExtra = 0;
  wcl.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(br,bg,bb));
  if(!RegisterClass(&wcl)) return 0;

  return 1;
}/*classdefinition*/

HDC createcompati(struct windowparams w)
/*CREATE COMPATIBLE OF HWND.*/
{
  HDC hdc,hdccompati;
  HBRUSH hbrush,hmemorybrush;
  HBITMAP hbit,hmemorybit;
  long int maxX,maxY;

  if(w.hwnd!=NULL)
  {
	getclientsize(w.hwnd,&maxX,&maxY);

	hdc=GetDC(w.hwnd);
	hdccompati = CreateCompatibleDC(hdc);
	hbit = CreateCompatibleBitmap(hdc,maxX,maxY);
	hmemorybit = (HBITMAP)SelectObject(hdccompati,hbit);

	hbrush = (HBRUSH)GetClassLong(w.hwnd,GCL_HBRBACKGROUND);

	hmemorybrush = (HBRUSH)SelectObject(hdccompati,hbrush);
	PatBlt(hdccompati,0,0,maxX,maxY,PATCOPY);

	SetBkMode(hdccompati,TRANSPARENT); /*BACKGROUND OF TEXT.*/

	DeleteObject(hmemorybit);
	DeleteObject(hmemorybrush);
	ReleaseDC(w.hwnd,hdc);
  }
  return hdccompati;
}/*createcompati*/

HWND windowdefinition(struct windowparams *wp,
					  HINSTANCE hInstance,
					  char *lpszclassname,            /*CLASS NAME.*/
					  WNDPROC lpfnwndproc,      /*WINDOW PROCEDURE.*/
					  char *lpszmenuname,              /*MENU NAME.*/
					  int br,int bg,int bb,     /*BACKGROUND BRUSH.*/
					  DWORD dwStyle,                /*WINDOW STYLE.*/
					  int x,int y,int w,int h,     /*POSITION,SIZE.*/
					  HWND hparent)                  /*PARENT HWND.*/
/*DEFINITION OF NAME,HWND,COMPATIBLES.*/
{
  if(lpszclassname!=NULL) classnamealloc(wp,lpszclassname);

  classdefinition(hInstance,wp->classname,
							lpfnwndproc,
							lpszmenuname,
							br,bg,bb);          /*CLASS DEFINITION.*/

  wp->hwnd = CreateWindow(wp->classname,
						  wp->classname, /*CAPTION.*/
						  dwStyle,
						  x,y,w,h,
						  hparent,
						  NULL,
						  hInstance,
						  NULL
						 );
  wp->hdcB=createcompati(*wp); /*BACKGROUND.*/
  wp->hdcC=createcompati(*wp); /*COMPATIBLE.*/

  wp->lstatus=NEUTRAL;
  wp->rstatus=NEUTRAL;
  wp->tx=0;
  wp->ty=0;

  registwindowparams(&wrglobal,wp);

  return wp->hwnd;
}/*windowdefinition*/

int registwindowparams(struct winparamsreg *wrapp,
                       struct windowparams *wpreg)
/*REGIST WINDOWPARAM */
{
  if(wpreg==NULL) return wrapp->nwin;

  (wrapp->nwin)++;
  wrapp->wp=(struct windowparams **)
            realloc(wrapp->wp,
					(wrapp->nwin)*sizeof(struct windowparams *));
  *((wrapp->wp)+(wrapp->nwin-1))=wpreg;

  return wrapp->nwin;
}/*registwindowparams*/

void clearwindow(struct windowparams wp)
/*CLEAR HDC.*/
{
  long int maxX,maxY;

  if(wp.hdcC!=NULL)
  {
	getclientsize(wp.hwnd,&maxX,&maxY);
	PatBlt(wp.hdcC,0,0,maxX,maxY,PATCOPY);
  }
  return;
}/*clearwindow*/

void DrawSunken(HDC hdc,int left,int top,int right,int bottom)
/*DRAW SUNKEN RECTANGLE IN LIGHT GRAY GROUND.*/
{
  HPEN hpen1,hpen2,ppen;

  hpen1=CreatePen(PS_SOLID,1,RGB(150,150,150));            /*SHADOW*/
  ppen = (HPEN)SelectObject(hdc,hpen1);
  MoveToEx(hdc,right,top,NULL);
  LineTo(hdc,left,top);
  LineTo(hdc,left,bottom);
  hpen2=CreatePen(PS_SOLID,1,RGB(255,255,255));            /*LUSTER*/
  SelectObject(hdc,hpen2);
  LineTo(hdc,right,bottom);
  LineTo(hdc,right,top);

  SelectObject(hdc,ppen);
  DeleteObject(hpen1);
  DeleteObject(hpen2);

  return;
}/*DrawSunken*/

RECT rectlocation(int l,int t,int r,int b)
/*LOCATE RECT BY LEFTTOP,RIGHTBOTTOM.*/
{
  RECT rect;

  rect.left  =l;
  rect.top   =t;
  rect.right =r;
  rect.bottom=b;

  return rect;
}/*rectlocation*/

struct windowparams *getwindowparams(HWND hwnd)
{
  int i;

  for(i=0;i<wrglobal.nwin;i++)
  {
	if(hwnd==(*(wrglobal.wp+i))->hwnd) return *(wrglobal.wp+i);
  }
  return NULL;
}/*getwindowparams*/

void vbarlocation(HWND hparent,HWND hchild,RECT *vbar)
{
  RECT rect;
  POINT point;
  long int pw,ph,ch,bmax;

  getclientsize(hparent,&pw,&ph);
  bmax=ph-BARWIDTH-5; /*MAX SIZE OF BAR.*/

  GetWindowRect(hchild,&rect);
  /*cw = rect.right-rect.left;*/
  ch = rect.bottom-rect.top;
  point.x=rect.left;
  point.y=rect.top;

  ScreenToClient(hparent,&point);

  vbar->left  =pw-BARWIDTH-1;
  vbar->right =pw-2;

  if(point.y>=1)
  {
	vbar->top=1;
  }
  else if(ch+point.y-1<=bmax)
  {
	vbar->top=1+bmax*(-point.y+1)/(-point.y+1+bmax);
  }
  else
  {
	vbar->top=1+bmax*(-point.y+1)/ch;
  }

  if(ch+point.y-1<=bmax)
  {
	vbar->bottom=bmax+1;
  }
  else if(point.y>=1)
  {
	vbar->bottom=(bmax+1)-bmax*(ch+point.y-1-bmax)/(ch+point.y-1);
  }
  else
  {
    vbar->bottom=(bmax+1)-bmax*(ch+point.y-1-bmax)/ch;
  }

  return;
}/*vbarlocation*/

void hbarlocation(HWND hparent,HWND hchild,RECT *hbar)
{
  RECT rect;
  POINT point;
  long int pw,ph,cw,bmax;

  getclientsize(hparent,&pw,&ph);
  bmax=pw-BARWIDTH-5; /*MAX SIZE OF BAR.*/

  GetWindowRect(hchild,&rect);
  cw = rect.right-rect.left;
  /*ch = rect.bottom-rect.top;*/
  point.x=rect.left;
  point.y=rect.top;

  ScreenToClient(hparent,&point);

  hbar->top   =ph-BARWIDTH-1;
  hbar->bottom=ph-2;

  if(point.x>=1)
  {
    hbar->left=1;
  }
  else if(cw+point.x-1<=bmax)
  {
    hbar->left=1+bmax*(-point.x+1)/(-point.x+1+bmax);
  }
  else
  {
    hbar->left=1+bmax*(-point.x+1)/cw;
  }

  if(cw+point.x-1<=bmax)
  {
	hbar->right=bmax+1;
  }
  else if(point.x>=1)
  {
    hbar->right=(bmax+1)-bmax*(cw+point.x-1-bmax)/(cw+point.x-1);
  }
  else
  {
	hbar->right=(bmax+1)-bmax*(cw+point.x-1-bmax)/cw;
  }

  return;
}/*hbarlocation*/

void drawvbar(HDC hdc,long int maxX,long int maxY,
			  struct windowparams *wp)
{
  HBRUSH hbrush,pbrush;
  HBITMAP hbit,pbit;

  hbit = CreateCompatibleBitmap(hdc,maxX,maxY);
  pbit = (HBITMAP)SelectObject(wp->hdcC,hbit);

  PatBlt(wp->hdcC,maxX-BARWIDTH-1,1,
                  BARWIDTH,maxY-BARWIDTH-5,PATCOPY); /*DELETE BAR.*/

  hbrush = CreateSolidBrush(RGB(255,255,0)); /*REDRAW BAR*/
  pbrush = (HBRUSH)SelectObject(wp->hdcC,hbrush);

  PatBlt(wp->hdcC,(wp->vbar.left),(wp->vbar.top),
                  (wp->vbar.right)-(wp->vbar.left)+1,
                  (wp->vbar.bottom)-(wp->vbar.top)+1,PATINVERT);
  BitBlt(hdc,maxX-BARWIDTH-1,1,BARWIDTH,maxY-BARWIDTH-5,
         wp->hdcC,maxX-BARWIDTH-1,1,SRCCOPY);

  SelectObject(wp->hdcC,pbit);
  SelectObject(wp->hdcC,pbrush);
  DeleteObject(hbit);
  DeleteObject(hbrush);

  return;
}/*drawvbar*/

void drawhbar(HDC hdc,long int maxX,long int maxY,
			  struct windowparams *wp)
{
  HBRUSH hbrush,pbrush;
  HBITMAP hbit,pbit;

  hbit = CreateCompatibleBitmap(hdc,maxX,maxY);
  pbit = (HBITMAP)SelectObject(wp->hdcC,hbit);

  PatBlt(wp->hdcC,1,maxY-BARWIDTH-1,
				  maxX-BARWIDTH-5,BARWIDTH,PATCOPY); /*DELETE BAR.*/

  hbrush = CreateSolidBrush(RGB(255,255,0)); /*REDRAW BAR*/
  pbrush = (HBRUSH)SelectObject(wp->hdcC,hbrush);

  PatBlt(wp->hdcC,(wp->hbar.left),(wp->hbar.top),
                  (wp->hbar.right)-(wp->hbar.left)+1,
                  (wp->hbar.bottom)-(wp->hbar.top)+1,PATINVERT);
  BitBlt(hdc,1,maxY-BARWIDTH-1,maxX-BARWIDTH-5,BARWIDTH,
		 wp->hdcC,1,maxY-BARWIDTH-1,SRCCOPY);

  SelectObject(wp->hdcC,pbit);
  SelectObject(wp->hdcC,pbrush);
  DeleteObject(hbit);
  DeleteObject(hbrush);

  return;
}/*drawhbar*/

void setfontformat(HDC hdc,int h,int w,
				   char *fontname,int r,int g,int b)
/*SET FORMAT OF FONT.*/
{
  HFONT hfont,pfont;

  if(hdc!=NULL)
  {
	hfont=CreateFont(h,w,                            /*HEIGHT,WIDTH*/
					 0,0,      /*TANGENT OF LINE,LETTER[0.1 DEGREE]*/
					 100,                                  /*WEIGHT*/
					 0,0,0,            /*ITALIC,UNDERLINE,STRIKEOUT*/
					 /*ANSI_CHARSET,*/
					 DEFAULT_CHARSET,
					 /*SHIFTJIS_CHARSET,*/    /*CHAR SET*/
					 OUT_DEFAULT_PRECIS,
					 CLIP_DEFAULT_PRECIS,
					 DEFAULT_QUALITY,
					 DEFAULT_PITCH | FF_DONTCARE,
					 fontname);                         /*FONT NAME*/
	pfont=(HFONT)SelectObject(hdc,hfont);
	DeleteObject(pfont);

	SetBkMode(hdc,TRANSPARENT);               /*BACKGROUND OF TEXT.*/
	SetTextColor(hdc,RGB(r,g,b));                     /*TEXT COLOR.*/
  }
  return;
}/*setfontformat*/

void getclientsize(HWND hwnd,long int *w,long int *h)
/*GET WIDTH,HEIGHT OF CLIENT OF WINDOW.*/
{
  RECT rect;

  GetClientRect(hwnd,&rect);
  *w = rect.right - rect.left;
  *h = rect.bottom - rect.top;

  return;
}/*getclientsize*/

void getwindowsize(HWND hwnd,long int *w,long int *h)
/*GET WIDTH,HEIGHT OF WINDOW.*/
{
  RECT rect;

  GetWindowRect(hwnd,&rect);
  *w = rect.right - rect.left;
  *h = rect.bottom - rect.top;

  return;
}/*getwindowsize*/

int overlayhdc(struct windowparams wp,DWORD dwrop)
/*OVERLAY HDC COMPATI,BACK.*/
{
  HWND hoya;
  HDC hdc;
  POINT pc,pp;
  long int x,y,maxX,maxY;

  if(wp.hdcB != NULL && wp.hdcC != NULL)
  {
    pc.x=0;
    pc.y=0;
    ClientToScreen(wp.hwnd,&pc);

	hoya=GetParent(wp.hwnd);
    pp.x=0;
	pp.y=0;
    ClientToScreen(hoya,&pp);

    x=pp.x-pc.x;
	y=pp.y-pc.y;

	getwindowsize(hoya,&maxX,&maxY);

	PatBlt(wp.hdcB,x,y,maxX,maxY,PATCOPY);
	BitBlt(wp.hdcB,x,y,maxX,maxY,wp.hdcC,x,y,
		   dwrop); /*GROUND BLACK:SRCPAINT WHITE:SRCAND.*/

	hdc = GetDC(wp.hwnd);
	BitBlt(hdc,x,y,maxX,maxY,wp.hdcB,x,y,SRCCOPY);
	ReleaseDC(wp.hwnd,hdc);

	return 1;
  }
  return 0;
}/*overlayhdc*/

HDC extendhdc(HDC hdc,long int w,long int h)
/*EXTEND HDC.*/
{
  HDC hdcC;
  HBITMAP hbit,pbit;
  BITMAP bmp;

  if(hdc==NULL) return NULL;

  hdcC=CreateCompatibleDC(hdc);
  hbit=CreateCompatibleBitmap(hdc,w,h);
  pbit=(HBITMAP)SelectObject(hdc,hbit);
  SelectObject(hdcC,pbit);

  GetObject(pbit,sizeof(BITMAP),(LPSTR)&bmp);

  PatBlt(hdc,0,0,w,h,PATCOPY);
  BitBlt(hdc,0,0,bmp.bmWidth,bmp.bmHeight,hdcC,0,0,SRCCOPY);

  DeleteObject(pbit);
  DeleteDC(hdcC);

  return hdc;
}/*extendhdc*/

void drawgloballine(HDC hdc,int Hx,int Hy,int Tx,int Ty)
/*DRAW LINE ON SCREEN.*/
{
  MoveToEx(hdc,Hx,Hy,NULL);
  LineTo(hdc,Tx,Ty);

  return;
}/*drawgloballine*/

