#include <windows.h>
#include<time.h>
#include<math.h>
#include"man.h"
HINSTANCE hInst;

LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);

DWORD	tPre,tNow;
HDC hdc, hdcmem,mdc;
HWND	hwnd;
struct brick{//zhuan
	POINT p;
	POINT pt;
	double dr;
	int type;
	int flag;
};

struct ren{//ren
	POINT p;
	int flag;
};

int vbrick=10,stid;
int vman=40,vyman=20;
int heart,score;
static int win;
win = -2;

static RECT rect;
static HBITMAP bg[2],zh[6],rw,hp,sc,shu,js;
static BITMAP  bm,rm;
int i,sum,score1;
static struct brick zhuan[100];
static struct ren man;
static int num;

//全局函数声明
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
void				MyPaint(HDC hdc);

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	MSG msg;

	MyRegisterClass(hInstance);

	//初始化
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	 GetMessage(&msg,NULL,NULL,NULL);            //初始化msg  
	//消息循环
    while( msg.message!=WM_QUIT )
    {
        if( PeekMessage( &msg, NULL, 0,0 ,PM_REMOVE) )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
		else
		{
			tNow = GetTickCount();
			if(tNow-tPre >= 100)
				MyPaint(hdc);
		}
    }

	return msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= "canvas";
	wcex.hIconSm		= NULL;

	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HBITMAP bmp;
	hInst = hInstance;

	hwnd = CreateWindow("canvas", "浅墨的绘图窗口" , WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	if (!hwnd)
	{
		return FALSE;
	}

	MoveWindow(hwnd,10,10,900,720,TRUE);
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	hdc=GetDC(hwnd);
	GetClientRect(hwnd,&rect);
	hdcmem=CreateCompatibleDC(hdc);
	mdc=CreateCompatibleDC(hdc);
	bmp=CreateCompatibleBitmap(hdc,rect.right-rect.left,rect.bottom-rect.top);			
	SelectObject(mdc,bmp);
	bg[0]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_BEIJIN));
	bg[1]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_TOU));
	zh[0]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_DINGBAN));//钉子
	zh[1]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_SUIZHUAN));//碎砖
	zh[2]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_TANBAN));//弹
	zh[3]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_YOUZHUAN));//右转
	zh[4]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_ZHUAN));//普通砖
	zh[5]=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_ZUOZHUAN));//左转
	rw=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_RENWU));
	hp=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_HP));
	sc=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_SC));
	shu=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_SHU));
	js=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_JS));
	srand((unsigned)GetCurrentTime());
	for(i=0;i<100;++i)
	{
		zhuan[i].type=(rand()*(2000)/RAND_MAX)%5;
		zhuan[i].flag=0;
		zhuan[i].p.x=(rect.right-rect.left)/22+4+rand()*((rect.right-rect.left)*8/11-4)/RAND_MAX;
		zhuan[i].p.y=(rect.bottom-rect.top)/4+(rect.bottom-rect.top)*i/6;
		if(zhuan[i].type==0)
			zhuan[i].pt.x=zhuan[i].pt.y=10;
		else if(zhuan[i].type==4)
			zhuan[i].pt.x=2;
		else
			zhuan[i].pt.x=zhuan[i].pt.y=0;
		if(zhuan[i].type==0||zhuan[i].type==1||zhuan[i].type==3||zhuan[i].type==5)
			zhuan[i].dr=0.6;
		else if(zhuan[i].type==2)
			zhuan[i].dr=0.42;
		else
			zhuan[i].dr=0.25;
	}
	stid=0;
	man.flag=0;
	GetObject(zh[0],sizeof(BITMAP),&bm);
	GetObject(rw,sizeof(BITMAP),&rm);
	man.p.x=zhuan[2].p.x+bm.bmWidth*0.85*0.5-rm.bmWidth*5/44;
	man.p.y=zhuan[2].p.y-rm.bmHeight/4;
	heart=3;
	score=0;
	
	MyPaint(hdc);

	return TRUE;
}


void MyPaint(HDC hdc)
	{
		GetClientRect(hwnd,&rect);
		SelectObject(hdcmem,bg[0]);
		GetObject(bg[0],sizeof(BITMAP),&bm);//背景
		StretchBlt(mdc,0,(rect.bottom-rect.top)*0.1,rect.right-rect.left,(rect.bottom-rect.top)*0.9,hdcmem,0,0,bm.bmWidth,bm.bmHeight,SRCCOPY);
		SelectObject(hdcmem,bg[1]);
		GetObject(bg[1],sizeof(BITMAP),&bm);//头
		StretchBlt(mdc,0,0,rect.right-rect.left,(rect.bottom-rect.top)*0.1,hdcmem,0,0,bm.bmWidth,bm.bmHeight,SRCCOPY);
		if((man.p.y-rm.bmHeight/4)>rect.bottom||man.p.y<=(rect.bottom-rect.top)/6)
		{
			win=-1;
			heart=0;
		}
		SelectObject(hdcmem,hp);
		GetObject(hp,sizeof(BITMAP),&bm);//HP
		TransparentBlt(mdc,30,0,bm.bmWidth*3/10+bm.bmWidth*heart/5,bm.bmHeight,hdcmem,10,0,bm.bmWidth*3/10+bm.bmWidth*heart/5,bm.bmHeight,RGB(255,255,255));
		SelectObject(hdcmem,sc);
		GetObject(sc,sizeof(BITMAP),&bm);//SC
		TransparentBlt(mdc,(rect.right-rect.left)*1/2,bm.bmHeight/2,bm.bmWidth/2,bm.bmHeight/2,hdcmem,0,0,bm.bmWidth,bm.bmHeight,RGB(255,255,255));
		SelectObject(hdcmem,shu);
		GetObject(shu,sizeof(BITMAP),&bm);//SHU
		score1=score;
		for(i=0;i<9;++i)
		{
			num=score1%10;
			TransparentBlt(mdc,rect.right-bm.bmWidth*(i+1)/9,0,bm.bmWidth/10,bm.bmHeight,hdcmem,bm.bmWidth*num/10,0,bm.bmWidth/10,bm.bmHeight,RGB(255,255,255));
			score1/=10;
		}
		GetObject(zh[0],sizeof(BITMAP),&bm);
		sum=0;
		i=stid;
		while(sum<6){
			i%=100;
			if(zhuan[i].type==1&&zhuan[i].flag==5)
			{
				++i;
				++sum;
				continue;
			}
			SelectObject(hdcmem,zh[zhuan[i].type]);
			StretchBlt(mdc,zhuan[i].p.x,zhuan[i].p.y,bm.bmWidth*0.85,bm.bmHeight*zhuan[i].dr,hdcmem,zhuan[i].pt.x,zhuan[i].pt.y,bm.bmWidth*0.87,bm.bmHeight*zhuan[i].dr,SRCCOPY);
			++i;
			++sum;
		}
		SelectObject(hdcmem,rw);
		GetObject(rw,sizeof(BITMAP),&rm);
		if(man.flag<0)//左
			TransparentBlt(mdc,man.p.x,man.p.y,rm.bmWidth*5/22,rm.bmHeight/4,hdcmem,5+rm.bmWidth/4*(abs(man.flag)-1),10+rm.bmHeight/4,rm.bmWidth/5,rm.bmHeight/6,RGB(255,255,255));
		else if(man.flag>0)//右
			TransparentBlt(mdc,man.p.x,man.p.y,rm.bmWidth*5/22,rm.bmHeight/4,hdcmem,rm.bmWidth/4*(man.flag-1),10+rm.bmHeight/4*2,rm.bmWidth/5,rm.bmHeight/6,RGB(255,255,255));
		else if(man.flag==0)
			TransparentBlt(mdc,man.p.x,man.p.y,rm.bmWidth*5/22,rm.bmHeight/4,hdcmem,10,10,rm.bmWidth/5,rm.bmHeight/6,RGB(255,255,255));
		if(win==-1)
		{
			SelectObject(hdcmem,js);
			GetObject(js,sizeof(BITMAP),&rm);
			TransparentBlt(mdc,(rect.right-rect.left)/2-rm.bmWidth/2,(rect.bottom-rect.top)/2-rm.bmHeight/2,rm.bmWidth,rm.bmHeight,hdcmem,0,0,rm.bmWidth,rm.bmHeight,RGB(255,255,255));
		}
		BitBlt(hdc,0,0,rect.right,rect.bottom,mdc,0,0,SRCCOPY);
		if(win==-2)
		{
			MessageBoxA(hwnd,"用←→控制移动，按确定开始游戏","提示",NULL);
			win = 0;
		}
		if(win==0)
		{
			for(i=0;i<100;++i){
				zhuan[i].p.y-=vbrick;
				if(zhuan[i].type==3)//向右加速的变化
				{
					if(zhuan[i].flag==0)
					{
						zhuan[i].pt.x=bm.bmWidth-7;
						zhuan[i].flag=1;
					}
					else
					{
						zhuan[i].pt.x=0;
						zhuan[i].flag=0;
					}
				}
				if(zhuan[i].type==5)//向左加速的变化
				{
					if(zhuan[i].flag==0)
					{
						zhuan[i].pt.x=bm.bmWidth;
						zhuan[i].flag=1;
					}
					else
					{
						zhuan[i].pt.x=0;
						zhuan[i].flag=0;
					}
				}
				if(zhuan[i].type==1)//踩了会碎的变化
				{
					if(zhuan[i].flag==1)
					{
						zhuan[i].pt.x=bm.bmWidth;
						zhuan[i].dr=1.5;
						zhuan[i].flag=2;
					}
					else if(zhuan[i].flag==2)
					{
						zhuan[i].pt.x=bm.bmWidth*2.14;
						zhuan[i].dr=1.5;
						zhuan[i].flag=3;
					}
					else if(zhuan[i].flag==3)
					{
						zhuan[i].pt.x=3.25*bm.bmWidth;
						zhuan[i].dr=0.8;
						zhuan[i].p.y+=2*vbrick;
						zhuan[i].flag=4;
					}
					else if(zhuan[i].flag==4)
					{
						zhuan[i].flag=5;
						zhuan[i].p.y-=2*vbrick;
						zhuan[i].pt.x=bm.bmWidth*0.85;
						zhuan[i].dr=0.6;
					}
				}
				if(zhuan[i].type==2)//踩了会弹的变化
				{
					if(zhuan[i].flag==1)
					{
						zhuan[i].dr=0.55;
						zhuan[i].flag=2;
					}
					else if(zhuan[i].flag==2)
					{
						zhuan[i].dr=0.42;
						zhuan[i].flag=0;
					}
				}
			}
			if((zhuan[stid].p.y+zhuan[i].dr*bm.bmHeight)<=((rect.bottom-rect.top)/6))
			{
				if(stid==0)
					zhuan[stid].p.y=zhuan[99].p.y+(rect.bottom-rect.top)/6;
				else
					zhuan[stid].p.y=zhuan[stid-1].p.y+(rect.bottom-rect.top)/6;
				zhuan[stid].flag=0;
				stid+=1;
				stid%=100;
			}
			sum=0;
			i=stid;
			while(sum<6){
				i%=100;
				if(zhuan[i].type==1&&zhuan[i].flag>0)
				{
					++i;
					++sum;
					continue;
				}
				if((man.p.x+rm.bmWidth*5/44)>zhuan[i].p.x&&man.p.x<(zhuan[i].p.x+bm.bmWidth*0.85-rm.bmWidth*5/44)&&(man.p.y+rm.bmHeight/4+vyman)>=zhuan[i].p.y&&(man.p.y+vyman)<=zhuan[i].p.y)
				{
					man.p.y=zhuan[i].p.y-rm.bmHeight/4;
					if(zhuan[i].type==0&&zhuan[i].flag==0)//钉子，扣血
					{
						--heart;
						zhuan[i].flag=-1;
						if(heart==0)
							win=-1;
					}
					else if(zhuan[i].type==1)//碎砖
					{
						zhuan[i].flag=1;
						score+=30;
					}
					else if(zhuan[i].type==2)//弹
					{
						zhuan[i].flag=1;
						man.p.y-=50;
						score+=20;
					}
					else if(zhuan[i].type==3)//右转
					{
						man.p.x+=15;
					}
					else if(zhuan[i].type==5)//左转
					{
						man.p.x-=15;
					}
					else if(zhuan[i].type==4)//普通砖
					{
						score+=10;
					}
					break;
				}
				++i;
				++sum;
			}
			if(sum==6)
				man.p.y+=vyman;
		}
		tPre = GetTickCount();
	}

LRESULT CALLBACK WndProc(HWND hwnd,UINT message,WPARAM  wParam,LPARAM lParam)
{
	switch(message)
	{
	case WM_KEYDOWN:
		{
			if(win!=0)
				break;
			switch(wParam)
			{
			case VK_LEFT:
				{
					if(man.flag==-4)
						man.flag=-1;
					else if(man.flag<0)
						--man.flag;
					else
						man.flag=-1;
					if(((man.p.x-vman)>((rect.right-rect.left)/22+4)))
						man.p.x-=vman;
					break;
				}
			case VK_RIGHT:
				{
					if(man.flag==4)
						man.flag=1;
					else if(man.flag>0)
						++man.flag;
					else
						man.flag=1;
					GetObject(zh[0],sizeof(BITMAP),&bm);
					if((man.p.x+rm.bmWidth*5/22+vman)<((rect.right-rect.left)*17/22+bm.bmWidth*0.85))
						man.p.x+=vman;
					break;
				}	
			}
			break;
		}
/*	case WM_KEYUP:
		{
			InvalidateRect(hwnd,NULL,0);
			break;
		}*/
	case WM_LBUTTONDOWN:
		{
			if(win==-2)
				win=0;
			break;
		}
/*	case WM_LBUTTONUP:
		{
			InvalidateRect(hwnd,NULL,0);
		}*/

	case WM_DESTROY:
		DeleteDC(mdc);
		DeleteDC(hdcmem);
		ReleaseDC(hwnd,hdc);
		PostQuitMessage(0);
	default:
		return DefWindowProc(hwnd,message,wParam,lParam);
	}
	return 0;
}