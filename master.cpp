#include "master.h"
#include "gl.h"

CGL gl;

CMaster::CMaster()
{
}

CMaster::~CMaster()
{
}

bool CMaster::Init(Display* dsp, Window& win)
{
	gl.Init(dsp, win);
	return true;
}

void CMaster::KeyEvent(const SKeyEvent& kevent)
{
	if((kevent.type==SKeyEvent::DN)||(kevent.type==SKeyEvent::REP))
	{
		if(kevent.code==SKeyCode::DOWN)
		{
			gl.DecreaseBlur();
			fflush(stdout);
		}
		else if(kevent.code==SKeyCode::UP)
		{
			gl.IncreaseBlur();
			fflush(stdout);
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

void CMaster::Done()
{
	
}
