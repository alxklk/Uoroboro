#include <stdio.h>

#include "utility.h"
#include "master.h"
#include "system.h"

int main(int argc, char** argv)
{
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