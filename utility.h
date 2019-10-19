#pragma once

#include <time.h>

struct SKeyCode
{
	static const int UNDEF=-1;
	static const int ESC=2000;

	static const int UP   =4000;
	static const int DOWN =4001;
	static const int LEFT =4002;
	static const int RIGHT=4003;

	static const int F1 =2001;
	static const int F2 =2002;
	static const int F3 =2003;
	static const int F4 =2004;
	static const int F5 =2005;
	static const int F6 =2006;
	static const int F7 =2007;
	static const int F8 =2008;
	static const int F9 =2009;
	static const int F10=2010;
	static const int F11=2011;
	static const int F12=2012;

	static const int SPACE=' ';
};

struct SKeyEvent
{
	static const int NONE =0;
	static const int DN   =1;
	static const int UP   =2;
	static const int REP  =3;
	int type;
	int sysCode;
	int code;
	char sym;
};

struct SMouseEvent
{
	static const int NONE =0;
	static const int MOVE =1;
	static const int DN   =2;
	static const int UP   =3;
	static const int CLICK=4;// send to clicked control
	static const int RDN  =5;
	static const int RUP  =6;
	static const int MDN  =7;
	static const int MUP  =8;
	static const int WHEEL=9;
	int type;
	int win;
	int t;
	int x;
	int y;
	float w; // wheel
	bool l; // left button
	bool r; // right button
	bool m; // middle button
	bool c; // control
	bool a; // alt
	bool s; // shift
	int x0; // system global
	int y0; //
	float fx;
	float fy;
	int dx; //deltas from previous
	int dy;
	int ldx; //delta from most recent mouse down
	int ldy;
	void SetZ()
	{
		t=0;
		x=0;
		y=0;
		w=0;
		l=false;
		r=false;
		m=false;
		c=false;
		a=false;
		s=false;
		fx=0.0f;
		fy=0.0f;
		dx=0;
		dy=0;
		ldx=0;
		ldy=0;
	}
};


#include <X11/Xlib.h>
#include <X11/XKBlib.h>

class CUtil
{
public:
	static double GetTime()
	{
		timespec ts;
		if(clock_gettime(CLOCK_MONOTONIC_RAW, &ts)!=-1)
			return ts.tv_nsec/1000000000.0+ts.tv_sec;
		else if(clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts)!=-1)
			return ts.tv_nsec/1000000000.0+ts.tv_sec;
		else return -1.0;
	}

	static void ParseKeyEvent(Display* dsp, const XEvent& ev, SKeyEvent& ke)
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
	static inline bool ParseMouseEvent(const XEvent& ev, SMouseEvent& me)
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
			me.t=SMouseEvent::MOVE;
			move=true;
		}
		else if(ev.type==ButtonPress)
		{
			me.l=me.r=me.m=me.c=me.s=me.a=false;
			me.w=0;
			button=true;
			if(ev.xbutton.button==Button1)
			{
				me.t=SMouseEvent::DN;
				lastLDownX=me.x;
				lastLDownY=me.y;
				me.l=true;
			}
			else if(ev.xbutton.button==Button3)
			{
				me.t=SMouseEvent::RDN;
				me.r=true;
			}
			else if(ev.xbutton.button==Button2)
			{
				me.t=SMouseEvent::MDN;
				me.m=true;
			}
			else if(ev.xbutton.button==Button4)
			{
				me.t=SMouseEvent::WHEEL;
				me.w=-1;
			}
			else if(ev.xbutton.button==Button5)
			{
				me.t=SMouseEvent::WHEEL;
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
				me.t=SMouseEvent::UP;
				me.l=false;
			}
			else if(ev.xbutton.button==Button3)
			{
				me.t=SMouseEvent::RUP;
				me.r=false;
			}
			else if(ev.xbutton.button==Button2)
			{
				me.t=SMouseEvent::MUP;
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
	
};
