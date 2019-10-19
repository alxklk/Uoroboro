//
// Created by klk on 19.10.2019.
//

#ifndef UROBORO_SYSTEM_H
#define UROBORO_SYSTEM_H

#ifdef __linux__

#include <X11/Xlib.h>
#include <X11/Xutil.h>

struct SYS
{
	Display * dsp=nullptr;
	Window win;
	char *dspname=nullptr;
	dsp=XOpenDisplay (dspname);

	if (dsp==0)
	{
		fprintf(stderr, "No display available\n");
		return 1;
	}

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

	win=XCreateWindow(dsp,rootwin,0,0,1024,768,0,depth,InputOutput,visual,
		CWBackPixel|CWBorderPixel|CWColormap,&attrs);

	XRaiseWindow(dsp,win);

	int EventMask=KeyPressMask|KeyReleaseMask
								|ButtonPressMask|ButtonReleaseMask|PointerMotionMask
								|FocusChangeMask|StructureNotifyMask
								;

	XMapWindow (dsp,win);
	XSelectInput (dsp, win,  EventMask);

	Atom WM_PROTOCOLS=0;
	Atom WM_DELETE_WINDOW=0;
	WM_PROTOCOLS = XInternAtom (dsp,"WM_PROTOCOLS",false);
	WM_DELETE_WINDOW = XInternAtom (dsp,"WM_DELETE_WINDOW",false);
	XSetWMProtocols(dsp, win, &WM_DELETE_WINDOW, 1);

	XStoreName(dsp, win, "Test");
};

#endif

#ifdef WIN32

struct SYS
{

};

#endif

#endif //UROBORO_SYSTEM_H
