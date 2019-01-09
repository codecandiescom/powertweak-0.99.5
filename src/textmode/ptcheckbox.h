#ifndef _INCLUDE_GUARD_CHECKBOX_H_
#define _INCLUDE_GUARD_CHECKBOX_H_

class TCheckbox : public TCheckBoxes
{
	public:
		TCheckbox(const TRect& bounds, TSItem *Strings,
				struct tweak *tweaks);
		
		virtual void press(int item);
		virtual void handleEvent(TEvent& event);
		
	struct tweak *tweak;
};

void createCheckbox(int *Y,int width,struct tweak *twk, ScrollDialog *view);

#endif
