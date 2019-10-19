#include <stdio.h>

#include "master.h"
#include "system.h"

int main(int argc, char** argv)
{
	SYS sys;
	sys.Init();
	WINDOW win=sys.CreateWindow(1024, 768, "Uoroboro");
	CMaster* master=new CMaster;
	master->Init(sys,win);

	while(true)
	{
		if(sys.quitRequested)
		{
			break;
		}

		sys.EventLoop(
		[](SEvent& event, void*userdata)->int
		{
			CMaster* master=(CMaster*)userdata;
			if(event.eventType==SEvent::Key)
				master->KeyEvent(event.ke);
			else if(event.eventType==SEvent::Mouse)
				master->MouseEvent(event.me);
			return 0;
		},
		master
		);

		if(sys.quitRequested)
		{
			break;
		}

		master->Render();

		// prevent 100% cpu load
		sys.Sleep(0.005);
	}

	master->Done();

	printf ("\nOK\n");
	return 0;
}
