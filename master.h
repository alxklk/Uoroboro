#pragma once

#include <stdio.h>
#include "system.h"
#include "gl.h"

class CMaster
{
	CGL gl;
public:
	CMaster(const SYS& sys, WINDOW& win);
	~CMaster();
	void KeyEvent(const SKeyEvent& kevent);
	void MouseEvent(const SMouseEvent& mevent);
	void Render();
};

