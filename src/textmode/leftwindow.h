#ifndef _INCLUDE_GUARD_LEFTWINDOW_H_
#define _INCLUDE_GUARD_LEFTWINDOW_H_

class TLeftWindow : public TWindow
{
	public:
	TLeftWindow(const TRect&r, const char *aTitle, short aNumber,
		struct tweak *tweaks);
};
#endif
