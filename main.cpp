#include <stdio.h>

#include "utility.h"
#include "master.h"

#ifdef __linux__

#include <X11/Xlib.h>
#include <X11/Xutil.h>

int main(int argc, char** argv)
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

	CMaster* master=new CMaster;
	master->Init(dsp,win);

	bool quitRequested=false;
	for(;;)
	{
		if(quitRequested)
		{
			break;
		}

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

			SMouseEvent me;
			if (CUtil::ParseMouseEvent (ev, me))
			{
				master->MouseEvent(me);
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
				SKeyEvent ke;
				CUtil::ParseKeyEvent(dsp,ev,ke);
				if(autoRepeat)
					ke.type=SKeyEvent::REP;

				if(ke.code==SKeyCode::ESC)
				{
					quitRequested=true;
					break;
				}
				master->KeyEvent(ke);

			}
		}

		if(quitRequested)
		{
			break;
		}

		master->Render();

		// prevent 100% cpu load
		timespec req;
		timespec rem;
		req.tv_nsec=1000000000*(1.0/120.0);
		req.tv_sec=0;
		nanosleep (&req, &rem);
	}

	master->Done();
	XCloseDisplay (dsp);

	printf ("\nOK\n");
	return 0;
}

#endif


#ifdef WIN32



#endif