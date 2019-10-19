#pragma once

#include <stdio.h>
#include "utility.h"

class CMaster
{
public:
	CMaster();
	~CMaster();

	bool Init(Display* dsp, Window& win);
	void KeyEvent(const SKeyEvent& kevent);
	void MouseEvent(const SMouseEvent& mevent);
	void Render();
	void Done();
	
};

