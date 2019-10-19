//
// Created by klk on 19.10.2019.
//

#ifndef UROBORO_SYSTEM_H
#define UROBORO_SYSTEM_H

#include <stdio.h>

#include "events.h"

#ifdef __linux__

#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <X11/Xutil.h>
#include <ctime>

using WINDOW=Window;
typedef int(*EventCallback)(SEvent&, void* userData);

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


struct SYS
{
	Display* dsp=nullptr;
	bool quitRequested=false;
	Atom WM_PROTOCOLS;
	Atom WM_DELETE_WINDOW;

	int Init()
	{
		dsp=XOpenDisplay (nullptr);
		if(dsp== nullptr)
		{
			fprintf(stderr, "No display available\n");
			return 1;
		}
		return 0;
	}

	WINDOW CreateWindow(int width, int height, const char* title)
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

		XMapWindow (dsp,win);
		XSelectInput (dsp, win,  EventMask);

		WM_PROTOCOLS=XInternAtom(dsp,"WM_PROTOCOLS",false);
		WM_DELETE_WINDOW=XInternAtom(dsp,"WM_DELETE_WINDOW",false);
		XSetWMProtocols(dsp, win, &WM_DELETE_WINDOW, 1);
		return win;
	}

	bool EventLoop(EventCallback cb, void* userdata)
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
				cb(mouseEvent, userdata);
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
				SEvent keyEvent;
				ParseKeyEvent(dsp,ev,keyEvent.ke);
				if(autoRepeat)
					keyEvent.ke.type=SKeyEvent::REP;
				cb(keyEvent, userdata);
			}
		}
		return 0;
	}

	void Sleep(float dt)
	{
		timespec req;
		timespec rem;
		req.tv_nsec=1000000000*dt;
		req.tv_sec=0;
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

	void CloseWindow(WINDOW win)
	{
		CloseWindow(win);
	}

	void Done()
	{
		XCloseDisplay(dsp);
	}
};

#endif

#ifdef WIN32

struct SYS
{

};

#endif

#endif //UROBORO_SYSTEM_H
