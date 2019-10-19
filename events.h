//
// Created by klk on 10/20/19.
//

#ifndef UROBORO_EVENTS_H
#define UROBORO_EVENTS_H

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

struct SWindowEvent
{
	static const int None=0;
	static const int Close=1;
	int type;
};

struct SEvent
{
	static const int Mouse=1;
	static const int Key=2;
	static const int Window=3;
	int eventType;
	union
	{
		SMouseEvent me;
		SKeyEvent ke;
		SWindowEvent we;
	};
};


#endif //UROBORO_EVENTS_H
