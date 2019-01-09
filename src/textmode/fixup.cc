#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

extern "C" {
	#include <powertweak.h>
	#include <client.h>
}

int i=0;

void FixUpTree(struct tweak *tweak)
{
	value_t value;

	if (tweak==NULL) 
		return;

	FixUpTree(tweak->Sub);
	FixUpTree(tweak->Next);

	if (tweak->GetValue!=NULL)  {
		value = tweak->GetValue(tweak);

		copy_and_release_value(&tweak->TempValue,&value);

	} else {
		switch (tweak->Type) {
			/* Ignore the nodes that don't have GetValue() */
			case TYPE_NODE_ONLY:
			case TYPE_TREE:
			case TYPE_TAB:
			case TYPE_FRAME:
			case TYPE_HFRAME:
			case TYPE_RADIO_GROUP:
				break;

			/* If we get here, we have a node that -should- have a GetValue() */
			default:
				/* FIXME: Close down turbovision properly, this way mangles
				          the keyboard & tty. */
				printf("Uh oh. Expected a GetValue() for tweaktype:%d description: %s\n",
					tweak->Type, tweak->WidgetText);
				exit(-1);
		}
	}

	/* This is pure voodoo. Don't touch! */
	if ((tweak->Sub!=NULL) && (tweak->Sub->Type==TYPE_TREE) &&
		(tweak->Sub->Sub!=NULL)&&(tweak->Sub->Sub->Type!=TYPE_TREE)
&& (tweak->Sub->Next==NULL))
	{	
		if (strcmp(tweak->WidgetText,tweak->Sub->WidgetText)==0)
		{
			struct tweak *tmp,*tmp2;
			tmp = tweak->Sub->Sub;
			tmp2 = tweak->Sub;
			tweak->Sub->Next=NULL;
			tweak->Sub->Sub= NULL;
			tweak->Sub = tmp;
			tmp2->Destroy(tmp2);	
			free(tmp2);
		}
	}
	
	/* Currently, TABs are not supported.. mapped to tree */
	if (tweak->Type == TYPE_TAB) 
		tweak->Type = TYPE_TREE;
}

