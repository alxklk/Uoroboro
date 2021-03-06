//
// Created by klk on 19.10.2019.
//

#ifndef UROBORO_SYSTEM_H
#define UROBORO_SYSTEM_H

#include <stdio.h>

#include "events.h"

typedef int(*EventCallback)(const SEvent&, void* userData);

class CommonSYS
{
	EventCallback eventCallback=nullptr;
	void* eventUserdata=nullptr;
public:
	void RegisterCallback(EventCallback newCB, void* userData)
	{
		eventCallback=newCB;
		eventUserdata=userData;
	}
	void CallEventCB(const SEvent& event)
	{
		if(eventCallback)
		{
			eventCallback(event, eventUserdata);
		}
	}
};


#ifdef __linux__

#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <X11/Xutil.h>
#include <ctime>
#include <cmath>

using WINDOW=Window;

inline void ParseKeyEvent(Display* dsp, const XEvent& ev, SKeyEvent& ke)
{
	int shift=0;
	if(ev.xkey.state&ShiftMask)
		shift=1;

	KeySym sym=XkbKeycodeToKeysym(dsp, ev.xkey.keycode, 0, shift);
	if(sym!=NoSymbol)
	{
		ke.sym=sym;
//			printf("Sym %i %c\n", sym, sym);
	}
	else
	{
		ke.sym=0;
//			printf("NoSym\n");
	}

	if(ev.xkey.keycode==111)ke.code=SKeyCode::UP;
	else if(ev.xkey.keycode==114)ke.code=SKeyCode::RIGHT;
	else if(ev.xkey.keycode==113)ke.code=SKeyCode::LEFT;
	else if(ev.xkey.keycode==116)ke.code=SKeyCode::DOWN;
	else if(ev.xkey.keycode== 65)ke.code=SKeyCode::SPACE;
	else if(ev.xkey.keycode==  9)ke.code=SKeyCode::ESC;

	else if(ev.xkey.keycode== 67)ke.code=SKeyCode::F1;
	else if(ev.xkey.keycode== 68)ke.code=SKeyCode::F2;
	else if(ev.xkey.keycode== 69)ke.code=SKeyCode::F3;
	else if(ev.xkey.keycode== 70)ke.code=SKeyCode::F4;
	else if(ev.xkey.keycode== 71)ke.code=SKeyCode::F5;
	else if(ev.xkey.keycode== 72)ke.code=SKeyCode::F6;
	else if(ev.xkey.keycode== 73)ke.code=SKeyCode::F7;
	else if(ev.xkey.keycode== 74)ke.code=SKeyCode::F8;
	else if(ev.xkey.keycode== 75)ke.code=SKeyCode::F9;
	else if(ev.xkey.keycode== 76)ke.code=SKeyCode::F10;
	else if(ev.xkey.keycode== 77)ke.code=SKeyCode::F11;
	else if(ev.xkey.keycode== 78)ke.code=SKeyCode::F12;

	else ke.code=SKeyCode::UNDEF;

	ke.sysCode=ev.xkey.keycode;

	if(ev.type==KeyPress)
		ke.type=SKeyEvent::DN;
	else if(ev.type==KeyRelease)
		ke.type=SKeyEvent::UP;
}

inline bool ParseMouseEvent(const XEvent& ev, SMouseEvent& me)
{
	bool move=false;
	bool button=false;
	static int lastLDownX=0;
	static int lastLDownY=0;
	if(ev.type==MotionNotify)
	{
		me.l=me.r=me.m=me.c=me.s=me.a=false;
		me.w=0;
		if(ev.xmotion.state&Button1Mask)
			me.l=true;
		if(ev.xmotion.state&Button2Mask)
			me.m=true;
		if(ev.xmotion.state&Button3Mask)
			me.r=true;
		if(ev.xmotion.state&ShiftMask)  me.s=true;
		if(ev.xmotion.state&ControlMask)me.c=true;
		if(ev.xmotion.state&Mod1Mask)   me.a=true;
		me.x=ev.xmotion.x;
		me.y=ev.xmotion.y;
		me.type=SMouseEvent::MOVE;
		move=true;
	}
	else if(ev.type==ButtonPress)
	{
		me.l=me.r=me.m=me.c=me.s=me.a=false;
		me.w=0;
		button=true;
		if(ev.xbutton.button==Button1)
		{
			me.type=SMouseEvent::DN;
			lastLDownX=me.x;
			lastLDownY=me.y;
			me.l=true;
		}
		else if(ev.xbutton.button==Button3)
		{
			me.type=SMouseEvent::RDN;
			me.r=true;
		}
		else if(ev.xbutton.button==Button2)
		{
			me.type=SMouseEvent::MDN;
			me.m=true;
		}
		else if(ev.xbutton.button==Button4)
		{
			me.type=SMouseEvent::WHEEL;
			me.w=-1;
		}
		else if(ev.xbutton.button==Button5)
		{
			me.type=SMouseEvent::WHEEL;
			me.w=1;
		}
	}
	else if(ev.type==ButtonRelease)
	{
		me.l=me.r=me.m=me.c=me.s=me.a=false;
		me.w=0;
		button=true;
		if(ev.xbutton.button==Button1)
		{
			me.type=SMouseEvent::UP;
			me.l=false;
		}
		else if(ev.xbutton.button==Button3)
		{
			me.type=SMouseEvent::RUP;
			me.r=false;
		}
		else if(ev.xbutton.button==Button2)
		{
			me.type=SMouseEvent::MUP;
			me.m=false;
		}
	}

	if(button)
	{
		me.x=ev.xbutton.x;
		me.y=ev.xbutton.y;
	}

	if(move||button)
	{
		if(ev.xbutton.state&ShiftMask)  me.s=true;
		if(ev.xbutton.state&ControlMask)me.c=true;
		if(ev.xbutton.state&Mod1Mask)   me.a=true;
		me.fx=me.x;
		me.fy=me.y;
	}
	me.ldx=lastLDownX-me.x;
	me.ldy=lastLDownY-me.y;
	return move||button;
}


class SYS : public CommonSYS
{
	Display* dsp=nullptr;
	bool quitRequested=false;
	Atom WM_PROTOCOLS;
	Atom WM_DELETE_WINDOW;

public:

	Display* GetDSP()const{return dsp;}

	int Init()
	{
		dsp=XOpenDisplay (nullptr);
		if(dsp==nullptr)
		{
			fprintf(stderr, "No display available\n");
			return 1;
		}
		return 0;
	}

	WINDOW CreateWin(int width, int height, const char* title)
	{
		Window win;
		Window rootwin=XDefaultRootWindow(dsp);
		int defscreen=DefaultScreen(dsp);
		int defdepth=DefaultDepth(dsp, defscreen);
		int depth=defdepth;
		Visual* visual=DefaultVisualOfScreen(DefaultScreenOfDisplay(dsp));

		XVisualInfo vinfo;
		if(XMatchVisualInfo(dsp, XDefaultScreen(dsp), 32, TrueColor, &vinfo))
		{
			visual = vinfo.visual;
			depth = vinfo.depth;
		}
		else
		{
			fprintf(stderr, "No suitable visual available\n");
			return 1;
		}

		XSetWindowAttributes attrs;
		attrs.border_pixel=0;
		attrs.background_pixel=0;
		attrs.override_redirect=True;
		attrs.colormap=XCreateColormap(dsp, XDefaultRootWindow(dsp), visual, AllocNone);

		win=XCreateWindow(dsp,rootwin,0,0,width,height,0,depth,InputOutput,visual,
			CWBackPixel|CWBorderPixel|CWColormap,&attrs);

		XStoreName(dsp, win, title);
		XRaiseWindow(dsp,win);

		int EventMask=KeyPressMask|KeyReleaseMask|
			ButtonPressMask|ButtonReleaseMask|PointerMotionMask|
			FocusChangeMask|StructureNotifyMask;

		XMapWindow(dsp,win);
		XSelectInput(dsp, win, EventMask);

		WM_PROTOCOLS=XInternAtom(dsp,"WM_PROTOCOLS",false);
		WM_DELETE_WINDOW=XInternAtom(dsp,"WM_DELETE_WINDOW",false);
		XSetWMProtocols(dsp, win, &WM_DELETE_WINDOW, 1);
		return win;
	}

	bool EventLoop()
	{
		XEvent ev;
		while(XPending (dsp))
		{
			XNextEvent (dsp,&ev);

			if(ev.type==DestroyNotify)
			{
				printf ("\nWindow destroyed.\n");
				quitRequested=true;
				break;
			}
			else if(ev.type==ClientMessage)
			{
				if((ev.xclient.message_type==WM_PROTOCOLS)&&(ev.xclient.data.l[0]==(signed)WM_DELETE_WINDOW))
				{
					printf ("\nWindow close button pressed.\n");
					quitRequested=true;
					break;
				}
			}

			SEvent mouseEvent;
			if (ParseMouseEvent (ev, mouseEvent.me))
			{
				mouseEvent.eventType=SEvent::Mouse;
				CallEventCB(mouseEvent);
			}
			if ((ev.type==KeyPress)||(ev.type==KeyRelease))
			{
				bool autoRepeat=false;

				if(ev.type==KeyRelease)
				{
					if(XEventsQueued(dsp,QueuedAfterReading))
					{
						XEvent nev;
						XPeekEvent(dsp, &nev);
						if(nev.type==KeyPress&&nev.xkey.time==ev.xkey.time&&nev.xkey.keycode==ev.xkey.keycode)
						{
							XNextEvent(dsp, &ev);
							autoRepeat=true;
						}
					}
				}
				SEvent keyEvent={SEvent::Key};
				ParseKeyEvent(dsp,ev,keyEvent.ke);
				if(autoRepeat)
					keyEvent.ke.type=SKeyEvent::REP;
				CallEventCB(keyEvent);
			}
		}
		return 0;
	}

	void Sleep(float dt)
	{
		timespec req;
		timespec rem;
		float sec=trunc(dt);
		req.tv_nsec=1000000000*(dt-sec);
		req.tv_sec=sec;
		nanosleep(&req, &rem);
	}

	double GetTime()
	{
		timespec ts;
		if(clock_gettime(CLOCK_MONOTONIC_RAW, &ts)!=-1)
			return ts.tv_nsec/1000000000.0+ts.tv_sec;
		else if(clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts)!=-1)
			return ts.tv_nsec/1000000000.0+ts.tv_sec;
		else return -1.0;
	}

	void CloseWin(WINDOW win)
	{
		XDestroyWindow(dsp,win);
	}

	void Done()
	{
		XCloseDisplay(dsp);
	}

	bool QuitRequested()
	{
		return quitRequested;
	}
};

#endif

#ifdef _WIN32

#include <windows.h>

using WINDOW=HWND;

struct SYS : public CommonSYS
{
	bool quitRequested=false;
	union XYLPARAM
	{
		unsigned int l;
		struct{
			short int x;
			short int y;
		};
		XYLPARAM(LPARAM I):l((unsigned int)I){}
	};

	static void ParseMouseEvent(SMouseEvent &e, WPARAM wpar, LPARAM lpar)
	{

		e.x0=XYLPARAM(lpar).x;
		e.y0=XYLPARAM(lpar).x;

		e.x=XYLPARAM(lpar).x;
		e.y=XYLPARAM(lpar).y;

		e.fx=XYLPARAM(lpar).x;
		e.fy=XYLPARAM(lpar).y;

		e.l=(0!=(wpar&MK_LBUTTON));
		e.r=(0!=(wpar&MK_RBUTTON));
		e.m=(0!=(wpar&MK_MBUTTON));
		e.c=(0!=(wpar&MK_CONTROL));
		e.s=(0!=(wpar&MK_SHIFT));
		e.a=(0!=(wpar&MK_ALT));

		if(e.type==SMouseEvent::DN)
			e.l=true;
	}

	static void stop(HWND hwnd,unsigned int imsg,WPARAM wpar,LPARAM lpar)
	{
		PostQuitMessage(0);
	}

	static int MouseEvent(HWND hwnd,unsigned int imsg,WPARAM wpar,LPARAM lpar, SEvent& ev, bool& defproc)
	{
		bool nonclient=false;
		SMouseEvent me;
		me.SetZ();

		bool l=(0!=(wpar&MK_LBUTTON));
		bool r=(0!=(wpar&MK_RBUTTON));
		bool m=(0!=(wpar&MK_MBUTTON));
		bool c=(0!=(wpar&MK_CONTROL));
		bool s=(0!=(wpar&MK_SHIFT));
		bool a=(GetAsyncKeyState(VK_MENU)&0x8000)!=0;

		int x=XYLPARAM(lpar).x;
		int y=XYLPARAM(lpar).y;

		static int old_ldnx=x;
		static int old_ldny=y;

		switch(imsg)
		{
			case WM_LBUTTONDOWN:me.type=SMouseEvent::DN;SetCapture(hwnd);break;
			case WM_LBUTTONUP  :me.type=SMouseEvent::UP;ReleaseCapture();break;
			case WM_RBUTTONDOWN:me.type=SMouseEvent::RDN;SetCapture(hwnd);break;
			case WM_RBUTTONUP  :me.type=SMouseEvent::RUP;ReleaseCapture();break;
			case WM_MBUTTONDOWN:me.type=SMouseEvent::MDN;SetCapture(hwnd);break;
			case WM_MBUTTONUP  :me.type=SMouseEvent::MUP;ReleaseCapture();break;

			case WM_MOUSEWHEEL :
				me.type=SMouseEvent::WHEEL;
				POINT pnt;
				pnt.x=x;
				pnt.y=y;
				ScreenToClient(hwnd,&pnt);
				x=pnt.x;
				y=pnt.y;

				me.w=-GET_WHEEL_DELTA_WPARAM(wpar)/WHEEL_DELTA;
				break;

			case WM_MOUSEMOVE  :
				me.type=SMouseEvent::MOVE;
				break;
			default: return 0;
		}
		me.a=a;
		me.c=c;
		me.s=s;
		me.l=l;
		me.r=r;
		me.m=m;

		me.x=x;
		me.y=y;
		me.fx=x;
		me.fy=y;

		if(me.type==SMouseEvent::DN)
		{
			me.ldx=x-old_ldnx;
			me.ldy=y-old_ldny;
		}
		ev.me=me;
		ev.eventType=SEvent::Mouse;
		defproc=false;
		return 1;
	}


	static bool keyEvent(HWND hwnd,unsigned int imsg,WPARAM wpar,LPARAM lpar, SEvent& ke)
	{
		bool isKeyEvent=false;
		bool down=false;
		if((imsg==WM_KEYDOWN)||(imsg==WM_SYSKEYDOWN))
		{
			down=true;
			isKeyEvent=true;
		}
		else if((imsg==WM_KEYUP)||(imsg==WM_SYSKEYUP))
		{
			isKeyEvent=true;
		}
		if(!isKeyEvent)
			return false;

		unsigned char key=(unsigned char)((lpar&0x0ff0000)>>16);
		ke.eventType=SEvent::Key;
		ke.ke.sysCode=key;

		bool alt=(GetKeyState(VK_MENU)&0x8000)!=0;
		bool ctl=(GetKeyState(VK_CONTROL)&0x8000)!=0;
		bool shift=(GetKeyState(VK_SHIFT)&0x8000)!=0;


		if(down)
		{
			if(lpar&(1<<30))
				ke.ke.type=SKeyEvent::REP;
			else
				ke.ke.type=SKeyEvent::DN;
		}
		else
		{
			ke.ke.type=SKeyEvent::UP;
		}

		//TODO: add other keys
		/***/if(wpar==VK_UP)         ke.ke.code=SKeyCode::UP;
		else if(wpar==VK_DOWN)       ke.ke.code=SKeyCode::DOWN;
		else if(wpar==VK_LEFT)       ke.ke.code=SKeyCode::LEFT;
		else if(wpar==VK_RIGHT)      ke.ke.code=SKeyCode::RIGHT;
		else if(wpar==VK_ESCAPE)     ke.ke.code=SKeyCode::ESC;
		else if(wpar==VK_SPACE)      ke.ke.code=SKeyCode::SPACE;
		else if(wpar==VK_F1)         ke.ke.code=SKeyCode::F1;
		else if(wpar==VK_F2)         ke.ke.code=SKeyCode::F2;
		else if(wpar==VK_F3)         ke.ke.code=SKeyCode::F3;
		else if(wpar==VK_F4)         ke.ke.code=SKeyCode::F4;
		else if(wpar==VK_F5)         ke.ke.code=SKeyCode::F5;
		else if(wpar==VK_F6)         ke.ke.code=SKeyCode::F6;
		else if(wpar==VK_F7)         ke.ke.code=SKeyCode::F7;
		else if(wpar==VK_F8)         ke.ke.code=SKeyCode::F8;
		else if(wpar==VK_F9)         ke.ke.code=SKeyCode::F9;
		else if(wpar==VK_F10)        ke.ke.code=SKeyCode::F10;
		else if(wpar==VK_F11)        ke.ke.code=SKeyCode::F11;
		else if(wpar==VK_F12)        ke.ke.code=SKeyCode::F12;
		return true;
	}


	static LRESULT CALLBACK wp(HWND hwnd,unsigned int imsg,WPARAM wpar,LPARAM lpar)
	{
	#undef GWL_USERDATA
		static const int GWL_USERDATA=-21;

		SEvent event;
		bool callDefProc=false;
		if(MouseEvent(hwnd,imsg,wpar,lpar,event,callDefProc))
		{
			SYS* sys=(SYS*)GetWindowLongPtrW(hwnd, GWL_USERDATA);
			sys->CallEventCB(event);
		}
		else if(keyEvent(hwnd,imsg,wpar,lpar,event))
		{
			SYS* sys=(SYS*)GetWindowLongPtrW(hwnd, GWL_USERDATA);
			sys->CallEventCB(event);
		}
		else if(imsg==WM_DESTROY)
			PostQuitMessage(0);
		return DefWindowProcW(hwnd, imsg, wpar, lpar);
	}

	int Init()
	{
		HCURSOR arrowCursor=LoadCursor(0, IDC_ARROW);

		WNDCLASSEXW wc={sizeof(WNDCLASSEXA),0,wp,0,0,0,0,0,0,0,L"APP01"};
		wc.hCursor=arrowCursor;
		RegisterClassExW(&wc);

		return 0;
	}

	WINDOW CreateWin(int width, int height, const char* title)
	{
		RECT wrect={0,0,width,height};
		AdjustWindowRectEx(&wrect,WS_OVERLAPPEDWINDOW,FALSE,WS_EX_APPWINDOW);
		HWND wnd=CreateWindowEx(WS_EX_APPWINDOW,"APP01",title,WS_OVERLAPPEDWINDOW,CW_USEDEFAULT,CW_USEDEFAULT,
			wrect.right-wrect.left,wrect.bottom-wrect.top,0,0,0,0);

		static const int GWL_USERDATA = -21;
		SetWindowLongPtrW(wnd, GWL_USERDATA, (LONG_PTR)(this));

		ShowWindow(wnd,SW_SHOWNORMAL);
		UpdateWindow(wnd);
		return wnd;
	}

	bool EventLoop()
	{
		MSG msg={0,0,0,0,0,{0,0}};
		{
			while(PeekMessageW(&msg,0,0,0,PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessageW(&msg);
				if(msg.message==WM_QUIT)
				{
					quitRequested=true;
					break;
				}
			}
			if(msg.message==WM_QUIT)
			{
				quitRequested=true;
				return 1;
			}
		}
		return 0;
	}

	void Sleep(float dt)
	{
		::Sleep(dt*1000);
	}

	double GetTime()
	{
		return ::GetTickCount()/1000.0;
	}

	void CloseWindow(WINDOW win)
	{
		::CloseWindow(win);
	}

	void Done()
	{

	}

	bool QuitRequested()
	{
		return quitRequested;
	}
};

#endif

#endif //UROBORO_SYSTEM_H
