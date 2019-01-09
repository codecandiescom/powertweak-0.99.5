#define Uses_ScrollDialog
// =================================================
#define Uses_TDialog
// =================================================
#define Uses_ScrollGroup
// =================================================
#define Uses_TGroup
// =================================================

#define Uses_TScrollBar
#define Uses_TEvent
#define Uses_TKeys
// =================================================
#include <tv.h>
#include "dlg.h"
// =================================================

ScrollDialog::ScrollDialog(const TRect& bounds, const char* aTitle, ushort f) :
	TDialog(bounds, aTitle),
	TWindowInit(initFrame),
	scrollGroup(0)
{
	TScrollBar* hsb = 0;
	TScrollBar* vsb = 0;

	if(f & sbHorizontal)
		hsb = standardScrollBar(sbHorizontal|sbHandleKeyboard);
	if(f & sbVertical)
		vsb = standardScrollBar(sbVertical|sbHandleKeyboard);

	TRect r = getExtent();
	r.grow(-1,-1);
	scrollGroup = new ScrollGroup(r, hsb, vsb);
	scrollGroup->growMode = gfGrowHiX | gfGrowHiY;
	insert(scrollGroup);
}

void ScrollDialog::handleEvent(TEvent& event)
{
	if(event.what == evKeyDown &&
		(event.keyDown.keyCode == kbTab ||
			event.keyDown.keyCode == kbShiftTab))
		{
		scrollGroup->selectNext(Boolean(event.keyDown.keyCode == kbShiftTab));
		clearEvent(event);
		}
	if(event.what == evKeyDown &&
		(event.keyDown.keyCode == kbUp ||
			event.keyDown.keyCode == kbDown))
		{
		scrollGroup->selectNext(Boolean(event.keyDown.keyCode == kbUp));
		clearEvent(event);
		}
	TDialog::handleEvent(event);
}
