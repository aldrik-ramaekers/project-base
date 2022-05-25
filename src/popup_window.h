/* 
*  BSD 2-Clause “Simplified” License
*  Copyright (c) 2022, Aldrik Ramaekers, aldrik.ramaekers@protonmail.com
*  All rights reserved.
*/

#ifndef POPUP_WINDOW
#define POPUP_WINDOW

typedef enum t_popup_window_option {
	POPUP_BTN_OK = 1,
	POPUP_BTN_CANCEL = 2,
	POPUP_BTN_CONFIRM = 4,
} popup_window_option;

void popup_window_show(char* title, char* text, popup_window_option options);

#endif