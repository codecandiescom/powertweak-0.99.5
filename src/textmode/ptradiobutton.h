#ifndef _INCLUDE_GUARD_RADIOBUTTON_H_
#define _INCLUDE_GUARD_RADIOBUTTON_H_

class TRadiobutton : public TRadioButtons
{
	public:
		TRadiobutton(const TRect& bounds, TSItem *Strings,
				struct tweak *tweaks);
		
		virtual void press(int item);
		virtual void handleEvent(TEvent& event);
		
	struct tweak *tweak;
};

void createRadiobutton(int *Y,int width,struct tweak *twk, ScrollDialog *view);

#endif
