#pragma once

#include <stdio.h>
#include "system.h"

class CMaster
{
public:
	CMaster();
	~CMaster();
	bool Init(const SYS& sys, WINDOW& win);
	void KeyEvent(const SKeyEvent& kevent);
	void MouseEvent(const SMouseEvent& mevent);
	void Render();
	void Done();
	
};

