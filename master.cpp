#include "system.h"
#include "master.h"
#include "gl.h"

CMaster::CMaster(const SYS& sys, WINDOW& win)
{
	gl.Init(&sys, win);
}

CMaster::~CMaster()
{
	gl.Done();
}

void CMaster::KeyEvent(const SKeyEvent& kevent)
{
	if((kevent.type==SKeyEvent::DN)||(kevent.type==SKeyEvent::REP))
	{
		if(kevent.code==SKeyCode::SPACE)
		{
			gl.NextMode();
		}
		else if(kevent.code==SKeyCode::F5)
		{
			gl.ReloadShaders();
		}
	}
}

void CMaster::MouseEvent(const SMouseEvent& mevent)
{
	gl.SetMousePos(mevent.x, mevent.y);
}

void CMaster::Render()
{
	gl.Render();
}
