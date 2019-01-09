/*
 *  $Id: tab.c,v 1.62 2003/04/23 15:48:10 svenud Exp $
 *	This file is part of Powertweak Linux.
 *	(C) 2000 Dave Jones, Arjan van de Ven.
 *
 * 	Licensed under the terms of the GNU GPL License version 2.
 *
 * Collection of routines used to build the GTK GUI.
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <gtk/gtk.h>
#include <string.h>

#include <powertweak.h>
#include <config.h>

#include "gtkgui.h"

#include "yes.xpm"
#include "no.xpm"

extern GtkTooltips *tooltips;

void Change_SpinButton (GtkWidget pt_unused * widget, struct tweak *tweak)
{
	set_value_int(tweak->TempValue, gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (tweak->Widget)));
}


void Change_Slider (GtkWidget pt_unused * widget, struct tweak *tweak)
{
	GtkAdjustment *Adj;
	Adj = tweak->Widget;

	assert (Adj != NULL);
	set_value_int(tweak->TempValue, Adj->value);
}


void Change_CheckBox (GtkWidget pt_unused * widget, struct tweak *tweak)
{
	set_value_int(tweak->TempValue, gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (tweak->Widget)));
}

void Change_Button (GtkWidget pt_unused * widget, struct tweak pt_unused *tweak)
{
/*SVEN - this might need to send a command straight away.. (and then request a refresh for this item only?)*/
/*	NotImplementedYet();*/
/*	set_value_int(tweak->TempValue, gtk_toggle_button_get_active (GTK_BUTTON (tweak->Widget)));*/
		if (tweak->ChangeValue != NULL)
		{
			tweak->ChangeValue (tweak, tweak->TempValue, TRUE);			
		}
}


void Change_Combo (GtkWidget pt_unused * widget, struct tweak pt_unused * tweak)
{
}


static GtkWidget *create_widget_from_xpm(GtkWidget pt_unused *box,gchar **xpm_data)
{
	GdkPixmap *pixmap_data;
	GdkBitmap *mask=NULL;
	GtkWidget *pixmap_widget;
	
	gtk_widget_realize(MainWin);
	pixmap_data = gdk_pixmap_create_from_xpm_d(MainWin->window,&mask,NULL,xpm_data);
	pixmap_widget = gtk_pixmap_new(pixmap_data,mask);
	
	return pixmap_widget;
}


GtkWidget *Spinbox (struct tweak *tweak)
{
	GtkWidget *hbox;
	GtkWidget *label;
	GtkWidget *spinbutton;
	GtkAdjustment *adj;

	hbox = gtk_hbox_new (TRUE, 0);

	label = gtk_label_new (tweak->WidgetText);
	gtk_misc_set_alignment (GTK_MISC (label), 0, 0.5);
	gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, TRUE, 0);

	adj = (GtkAdjustment *) gtk_adjustment_new (tweak->GetValueInt (tweak),
						    tweak->MinValue, tweak->MaxValue, 1.0, 16.0, 0.0);
	spinbutton = gtk_spin_button_new (adj, 1, 0);
	tweak->Widget = spinbutton;
	gtk_widget_set_usize (spinbutton, 100, 0);
	gtk_spin_button_set_value (GTK_SPIN_BUTTON (spinbutton), tweak->GetValueInt (tweak)  );
	gtk_signal_connect (GTK_OBJECT (adj), "value_changed", GTK_SIGNAL_FUNC (Change_SpinButton), tweak);
	gtk_box_pack_start (GTK_BOX (hbox), spinbutton, FALSE, FALSE, 0);

	if (tweak->Description != NULL) {
		gtk_tooltips_set_tip (tooltips, spinbutton, tweak->Description, NULL);
	}

	gtk_widget_show (label);
	gtk_widget_show (spinbutton);
	gtk_widget_show (hbox);
	return hbox;
}


GtkWidget *Slider (struct tweak * tweak)
{
	GtkWidget *hbox;
	GtkWidget *label;
	GtkWidget *slider;
	GtkAdjustment *adj;

	hbox = gtk_hbox_new (TRUE, 0);

	label = gtk_label_new (tweak->WidgetText);
	gtk_misc_set_alignment (GTK_MISC (label), 0, 0.5);
	gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, TRUE, 0);

	adj = (GtkAdjustment *) gtk_adjustment_new (tweak->GetValueInt (tweak),
						    tweak->MinValue, tweak->MaxValue, 1.0, 16.0, 0.0);
	slider = gtk_hscale_new (adj);
	gtk_scale_set_digits (GTK_SCALE (slider), 0);
	tweak->Widget = adj;
	gtk_widget_set_usize (slider, 100, 0);
	gtk_adjustment_set_value (GTK_ADJUSTMENT (adj), tweak->GetValueInt (tweak));
	gtk_signal_connect (GTK_OBJECT (adj), "value_changed", GTK_SIGNAL_FUNC (Change_Slider), tweak);
	gtk_box_pack_start (GTK_BOX (hbox), slider, FALSE, FALSE, 0);

	if (tweak->Description != NULL) {
		gtk_tooltips_set_tip (tooltips, slider, tweak->Description, NULL);
	}

	gtk_widget_show (label);
	gtk_widget_show (slider);
	gtk_widget_show (hbox);
	return hbox;
}


GtkWidget *BigText (struct tweak * tweak)
{
	GtkWidget *text;

	text = gtk_text_new (NULL, NULL);

	gtk_text_insert (GTK_TEXT (text), NULL, NULL, NULL, tweak->WidgetText, strlen (tweak->WidgetText));
	gtk_text_set_editable (GTK_TEXT (text), FALSE);

	gtk_widget_show (text);
	return text;
}


GtkWidget *ComboElem (struct tweak * tweak)
{
	GtkWidget *menu;
	assert (tweak->Type == TYPE_COMBOELEM);

	/*printf ("ELEMENT : %s \n", tweak->WidgetText);*/

	menu = gtk_menu_item_new_with_label (tweak->WidgetText);

	gtk_widget_show (menu);
	return menu;

}


GtkWidget *ComboBox (struct tweak * tweak)
{
	GtkWidget *hbox;
	GtkWidget *label;
	GtkWidget *combo, *menu;
	struct tweak *sub;

	hbox = gtk_hbox_new (TRUE, 0);
	label = gtk_label_new (tweak->WidgetText);
	gtk_misc_set_alignment (GTK_MISC (label), 0, 0.5);
	gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, TRUE, 0);

	combo = gtk_option_menu_new ();
	menu = gtk_menu_new ();

	sub = tweak->Sub;
	/*if (sub == NULL)
		printf ("sub is NULL!!\n");*/

	while (sub != NULL) {
		gtk_menu_append (GTK_MENU (menu), ComboElem (sub));
		sub = sub->Next;
	}

	gtk_widget_show (menu);
	gtk_option_menu_set_menu (GTK_OPTION_MENU (combo), menu);
	tweak->Widget = combo;


	gtk_signal_connect (GTK_OBJECT (combo), "value_changed", GTK_SIGNAL_FUNC (Change_Combo), tweak);

	gtk_widget_set_usize (combo, 100, 0);
	gtk_box_pack_start (GTK_BOX (hbox), combo, FALSE, FALSE, 0);

	gtk_widget_show (label);
	gtk_widget_show (combo);
	gtk_widget_show (hbox);
	return hbox;
}


static void fill_frame (GtkWidget * vbox, struct tweak *tweak)
{
	GtkWidget *widget, *box, *ebox;
	GtkWidget pt_unused *label;
	GtkWidget *RadioGroup = NULL;
	GtkStyle *gtk_style;

	while (tweak != NULL) {
		if (tweak->GetValue != NULL) {
			value_t value;
			value = tweak->GetValue(tweak);
			copy_and_release_value(&tweak->TempValue,&value);
		}
		switch (tweak->Type) {
		case TYPE_CHECKBOX:

			widget = gtk_check_button_new_with_label (tweak->WidgetText);
			gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (widget), get_value_int(tweak->TempValue));
			gtk_signal_connect (GTK_OBJECT (widget), "clicked", GTK_SIGNAL_FUNC (Change_CheckBox), tweak);
			tweak->Widget = widget;

			gtk_container_border_width (GTK_CONTAINER (widget), 1);
			gtk_widget_show (widget);

			if (tweak->Description != NULL) {
				gtk_tooltips_set_tip (tooltips, widget, tweak->Description, NULL);
			}
			gtk_box_pack_start (GTK_BOX (vbox), widget, FALSE, FALSE, 0);
			break;

		case TYPE_BUTTON:

			widget = gtk_button_new_with_label (tweak->WidgetText);
/*			gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (widget), get_value_int(tweak->TempValue));*/
			gtk_signal_connect (GTK_OBJECT (widget), "clicked", GTK_SIGNAL_FUNC (Change_Button), tweak);
			tweak->Widget = widget;

			gtk_container_border_width (GTK_CONTAINER (widget), 5);
			gtk_widget_show (widget);

			if (tweak->Description != NULL) {
				gtk_tooltips_set_tip (tooltips, widget, tweak->Description, NULL);
			}
			gtk_box_pack_start (GTK_BOX (vbox), widget, FALSE, FALSE, 0);
			break;

		case TYPE_RADIO_OPTION:
			if (RadioGroup!=NULL)
				widget = gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON(RadioGroup),tweak->WidgetText);
			else
				widget = gtk_radio_button_new_with_label (NULL,tweak->WidgetText);
			RadioGroup = widget;
			gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (widget), get_value_int(tweak->TempValue));
			gtk_signal_connect (GTK_OBJECT (widget), "clicked", GTK_SIGNAL_FUNC (Change_CheckBox), tweak);
			tweak->Widget = widget;

			gtk_box_pack_start (GTK_BOX (vbox), widget, FALSE, FALSE, 0);
			gtk_container_border_width (GTK_CONTAINER (widget), 1);
			gtk_widget_show (widget);
			if (tweak->Description != NULL) {
				gtk_tooltips_set_tip (tooltips, widget, tweak->Description, NULL);
			}
			break;

		case TYPE_SPINBOX:
			widget = Spinbox (tweak);
			gtk_box_pack_start (GTK_BOX (vbox), widget, FALSE, FALSE, 0);
			gtk_container_border_width (GTK_CONTAINER (widget), 3);
			gtk_widget_show (widget);
			break;

		case TYPE_SLIDER:
			widget = Slider (tweak);
			gtk_box_pack_start (GTK_BOX (vbox), widget, FALSE, FALSE, 0);
			gtk_container_border_width (GTK_CONTAINER (widget), 3);
			gtk_widget_show (widget);
			break;

		case TYPE_TEXT:
			widget = BigText (tweak);
			gtk_box_pack_start (GTK_BOX (vbox), widget, FALSE, FALSE, 0);
			gtk_widget_show (widget);
			break;

		case TYPE_LABEL:
			widget = gtk_label_new (tweak->WidgetText);
			gtk_misc_set_alignment (GTK_MISC (widget), 0, 0.5);
			gtk_box_pack_start (GTK_BOX (vbox), widget, FALSE, FALSE, 0);
			gtk_widget_show (widget);
			break;

		case TYPE_INFO_INT:
			ebox = gtk_event_box_new ();
			box = gtk_hbox_new (FALSE, 0);

			widget = gtk_label_new (tweak->WidgetText);
			gtk_misc_set_padding (GTK_MISC (widget), 5, 10);
			gtk_box_pack_start (GTK_BOX (box), widget, FALSE, FALSE, 0);
			gtk_widget_show (widget);

			widget = gtk_entry_new ();
			tweak->TempValue.strVal = malloc (8);
			tweak->Widget = widget;
			snprintf (tweak->TempValue.strVal,7, "%d", get_value_int(tweak->TempValue));

			gtk_style = gtk_widget_get_style(widget);
			gtk_widget_set_usize (widget, 11+gdk_string_width(gtk_style->font, tweak->TempValue.strVal), 0);
		
			gtk_entry_set_text (GTK_ENTRY (widget), tweak->TempValue.strVal);

			gtk_editable_set_editable (GTK_EDITABLE (widget), FALSE);
			gtk_box_pack_start (GTK_BOX (box), widget, FALSE, FALSE, 0);
			if (tweak->Description != NULL) {
				gtk_tooltips_set_tip (tooltips, widget, tweak->Description, NULL);
			}

			gtk_widget_show (widget);

			gtk_widget_show (box);
			gtk_container_add (GTK_CONTAINER (ebox), GTK_WIDGET (box));
			gtk_widget_show (ebox);
			gtk_box_pack_start (GTK_BOX (vbox), ebox, FALSE, FALSE, 0);
			break;


		case TYPE_INFO_BOOL:
			box = gtk_hbox_new (FALSE, 0);

			if (get_value_int(tweak->TempValue)!=0) {
				widget = create_widget_from_xpm(vbox,yes_xpm);
			} else {
				widget = create_widget_from_xpm(vbox,no_xpm);
			}	
			gtk_box_pack_start (GTK_BOX (box), widget, FALSE, FALSE, 0);
			gtk_widget_show (widget);
			gdk_pixmap_unref((GdkPixmap*)widget); /* ehhh we seem to need this  */

			gtk_box_pack_start (GTK_BOX (vbox), box, FALSE, FALSE, 0);

			widget = gtk_label_new (tweak->WidgetText);
			gtk_misc_set_padding (GTK_MISC (widget), 5, 10);
			gtk_box_pack_start (GTK_BOX (box), widget, FALSE, FALSE, 0);
			gtk_widget_show (widget);
			gtk_widget_show (box);
			break;

		case TYPE_INFO_STRING:
		{
			if (tweak->TempValue.strVal==NULL) {
/*				printf("%s %i: NULL string value for %s \n",
					__FILE__,__LINE__,tweak->WidgetText);*/
				break;
			}
			ebox = gtk_event_box_new ();
			box = gtk_hbox_new (FALSE, 0);

			widget = gtk_label_new (tweak->WidgetText);
			gtk_misc_set_padding (GTK_MISC (widget), 5, 10);
			gtk_box_pack_start (GTK_BOX (box), widget, FALSE, FALSE, 0);
			gtk_widget_show (widget);

			widget = gtk_entry_new ();
			tweak->Widget = widget;

			gtk_style = gtk_widget_get_style(widget);
			gtk_widget_set_usize (widget, 11+gdk_string_width(gtk_style->font, tweak->TempValue.strVal), 0); /*FIXME - why do i need to add 11?*/
			gtk_entry_set_text (GTK_ENTRY (widget), tweak->TempValue.strVal);
			gtk_editable_set_editable (GTK_EDITABLE (widget), FALSE);
			gtk_box_pack_start (GTK_BOX (box), widget, FALSE, FALSE, 0);
			if (tweak->Description != NULL) {
				tweak->Tooltip = gtk_tooltips_new();
				gtk_tooltips_set_tip (tweak->Tooltip, widget, tweak->Description, NULL);
			}
			gtk_widget_show (widget);

			gtk_widget_show (box);
			gtk_container_add (GTK_CONTAINER (ebox), GTK_WIDGET (box));
			gtk_widget_show (ebox);

			gtk_box_pack_start (GTK_BOX (vbox), ebox, FALSE, FALSE, 0);
			break;
		}
		case TYPE_FRAME:
			widget = gtk_frame_new (tweak->WidgetText);
			box = gtk_vbox_new (FALSE, 0);
			gtk_container_add (GTK_CONTAINER (widget), box);

			fill_frame (box, tweak->Sub);

			gtk_widget_show (box);
			gtk_box_pack_start (GTK_BOX (vbox), widget, FALSE, FALSE, 0);
			gtk_container_border_width (GTK_CONTAINER (widget), 5);
			gtk_widget_show (widget);
			break;

		case TYPE_HFRAME:
			widget = gtk_frame_new (tweak->WidgetText);
			box = gtk_hbox_new (FALSE, 0);
			gtk_container_add (GTK_CONTAINER (widget), box);

			fill_frame (box, tweak->Sub);

			gtk_widget_show (box);
			gtk_box_pack_start (GTK_BOX (vbox), widget, FALSE, FALSE, 0);
			gtk_container_border_width (GTK_CONTAINER (widget), 5);
			gtk_widget_show (widget);
			break;

		case TYPE_COMBO:
			widget = ComboBox (tweak);
			gtk_box_pack_start (GTK_BOX (vbox), widget, FALSE, FALSE, 0);
			gtk_widget_show (widget);
			break;

		case TYPE_RADIO_GROUP:
			fill_frame (vbox, tweak->Sub);
			break;
		case TYPE_COMBOELEM:
			/* nothing to do here */
			break;
		case TYPE_TAB:
			break;


		default:
			printf ("Unknown widget type : %i!!!\n", tweak->Type);
			assert (0);
		}

		tweak = tweak->Next;
	}
}


GtkWidget *TabWidget (struct tweak *tweak)
{
	GtkWidget *notebook;

	notebook = gtk_notebook_new ();
	gtk_notebook_set_tab_pos (GTK_NOTEBOOK (notebook), GTK_POS_TOP);
	gtk_widget_set_usize (notebook, (WindowWidth / 3) * 2, RightWindowHeight);
	gtk_container_border_width (GTK_CONTAINER (notebook), 10);

	while (tweak != NULL) {
		GtkWidget *label;
		GtkWidget *frame;
		GtkWidget *scrolled_win;

		scrolled_win = gtk_scrolled_window_new (NULL, NULL);
		gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_win),
						GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
		gtk_widget_show (scrolled_win);


		label = gtk_label_new (tweak->WidgetText);
		frame = gtk_vbox_new (FALSE, 0);
		gtk_container_border_width (GTK_CONTAINER (frame), 5);

		fill_frame (frame, tweak->Sub);
		gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (scrolled_win), frame);
		gtk_widget_show (frame);
		gtk_widget_show (label);
		gtk_notebook_append_page (GTK_NOTEBOOK (notebook), scrolled_win, label);

		tweak = tweak->Next;
	}

	return notebook;
}
