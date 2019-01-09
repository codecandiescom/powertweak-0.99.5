/*
 *  $Id: widgetvalues.c,v 1.5 2001/09/02 16:30:27 davej Exp $
 *	This file is part of Powertweak Linux.
 *	(C) 2000 Dave Jones, Arjan van de Ven.
 *
 * 	Licensed under the terms of the GNU GPL License version 2.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <gtk/gtk.h>

#include <powertweak.h>


static void update_widget(struct tweak *tweak)
{
	value_t val;
	GtkWidget *widget;
	GSList *slist;
	
	if (tweak->GetValue==NULL)
		return;
	val = tweak->GetValue(tweak);
	copy_values(&tweak->TempValue,val);
	release_value(&val);
	
	widget = tweak->Widget;
	switch (tweak->Type) {
		case TYPE_NODE_ONLY:
		case TYPE_TREE:
		case TYPE_TAB:
		case TYPE_FRAME:
		case TYPE_HFRAME:
		case TYPE_TEXT:
		case TYPE_COMBO:
		case TYPE_COMBOELEM:
		case TYPE_LABEL:
		case TYPE_INFO_BOOL:
		case TYPE_RADIO_GROUP:
			break;
		case TYPE_CHECKBOX:
			gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (widget), get_value_int(tweak->TempValue));			
			break;
		case TYPE_SPINBOX:
			gtk_spin_button_set_value (GTK_SPIN_BUTTON (widget), get_value_int(tweak->TempValue) );
			break;
		case TYPE_SLIDER:
			gtk_adjustment_set_value (GTK_ADJUSTMENT (widget), get_value_int(tweak->TempValue));
			break;
		case TYPE_INFO_INT:
			if (tweak->TempValue.strVal==NULL)
				tweak->TempValue.strVal=calloc(1,9);
			assert(tweak->TempValue.strVal!=NULL);
			snprintf (tweak->TempValue.strVal,7, "%d", get_value_int(tweak->TempValue));

			gtk_editable_set_editable (GTK_EDITABLE (widget), TRUE);
			gtk_entry_set_text (GTK_ENTRY (widget), tweak->TempValue.strVal);
			gtk_editable_set_editable (GTK_EDITABLE (widget), FALSE);
			break;
		case TYPE_INFO_STRING:
			gtk_entry_set_text (GTK_ENTRY (widget), tweak->TempValue.strVal);
			break;
		case TYPE_RADIO_OPTION:
			slist = gtk_radio_button_group(tweak->Widget);
			for (; slist ; slist=slist->next) {
				if ((slist == tweak->Widget) &&
					(get_value_int(tweak->TempValue)==1))
					gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(slist->data), TRUE);
				else
					gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(slist->data), FALSE);
			}
			break;
		default:
			printf("Woops unsupported update: %s\n",tweak->WidgetText);
			break;					
	}
}

void update_GTK_tree_values(struct tweak *tweak)
{
	if (tweak->Sub!=NULL)
		update_GTK_tree_values(tweak->Sub);
	if (tweak->Next!=NULL)
		update_GTK_tree_values(tweak->Next);
		
	update_widget(tweak);		
}
