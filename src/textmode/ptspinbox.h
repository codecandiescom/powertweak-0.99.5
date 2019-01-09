#ifndef _INCLUDE_GUARD_SPINBOX_H_
#define _INCLUDE_GUARD_SPINBOX_H_

class TSpinbox : public TInputLine
{
	public:
		TSpinbox(const TRect& bounds, int maxlen,
				struct tweak *tweaks);
		
		virtual void handleEvent(TEvent& event);
	struct tweak *tweak;
};

void createSpinbox(int *Y,int width,struct tweak *twk, ScrollDialog *view);

#endif
