/* 
*  BSD 2-Clause “Simplified” License
*  Copyright (c) 2019, Aldrik Ramaekers, aldrik.ramaekers@protonmail.com
*  All rights reserved.
*/

inline bool mouse_interacts_peak(s32 x, s32 y, s32 w, s32 h)
{
	// Mouse is in rectangle and mouse has not been clicked yet.
	vec4 area = renderer->render_get_scissor();
	bool result = (!_global_mouse.is_hovering_item) && 
				  (_global_mouse.x >= x && _global_mouse.x <= x+w && _global_mouse.y >= y && _global_mouse.y <= y+h) &&
				  (_global_mouse.x >= area.x && _global_mouse.x <= area.x+area.w && _global_mouse.y >= area.y && _global_mouse.y <= area.y+area.h);
	return result;
}

inline bool mouse_interacts(s32 x, s32 y, s32 w, s32 h)
{
	// Mouse is in rectangle and mouse has not been clicked yet.
	vec4 area = renderer->render_get_scissor();
	bool result = (!_global_mouse.is_hovering_item) && 
				  (_global_mouse.x >= x && _global_mouse.x <= x+w && _global_mouse.y >= y && _global_mouse.y <= y+h) &&
				  (_global_mouse.x >= area.x && _global_mouse.x <= area.x+area.w && _global_mouse.y >= area.y && _global_mouse.y <= area.y+area.h);
	if (result) _global_mouse.is_hovering_item = true;
	return result;
}

inline bool is_left_down_peak()
{
	return _global_mouse.left_state & MOUSE_DOWN;
}

inline bool is_left_down()
{
	bool result = _global_mouse.left_state & MOUSE_DOWN;
	if (result) reset_left_down();
	return result;
}

inline bool is_left_released()
{
	return _global_mouse.left_state & MOUSE_RELEASE;
}

inline bool is_left_clicked_peak()
{
	return _global_mouse.left_state & MOUSE_CLICK && _global_mouse.inside_of_window;
}

inline bool is_left_clicked()
{
	bool result = _global_mouse.left_state & MOUSE_CLICK && _global_mouse.inside_of_window;
	if (result) reset_left_click();
	return result;
}

inline bool is_left_double_clicked()
{
	return _global_mouse.left_state & MOUSE_DOUBLE_CLICK;
}

inline bool is_right_down()
{
	return _global_mouse.right_state & MOUSE_DOWN;
}

inline bool is_right_released()
{
	return _global_mouse.right_state & MOUSE_RELEASE;
}

inline bool is_right_clicked()
{
	return _global_mouse.right_state & MOUSE_CLICK;
}

inline bool keyboard_is_key_down(s16 key)
{
	return _global_keyboard.keys[key];
}

inline bool keyboard_is_key_pressed(s16 key)
{
	return _global_keyboard.input_keys[key];
}

inline void keyboard_set_input_text(char *text)
{
	string_copyn(_global_keyboard.input_text, text, MAX_INPUT_LENGTH);
	u32 len = utf8len(_global_keyboard.input_text);
	_global_keyboard.cursor = len;
	_global_keyboard.input_text_len = len;
}

mouse_input mouse_input_create()
{
	mouse_input mouse;
	mouse.x = -1;
	mouse.y = -1;
	mouse.move_x = 0;
	mouse.move_y = 0;
	mouse.left_state = 0;
	mouse.right_state = 0;
	mouse.is_hovering_item = false;
	mouse.inside_of_window = false;
	
	return mouse;
}

keyboard_input keyboard_input_create()
{
	keyboard_input keyboard;
	keyboard.modifier_state = 0;
	keyboard.input_text = mem_alloc(MAX_INPUT_LENGTH);
	keyboard.input_text[0] = '\0';
	keyboard.take_input = false;
	keyboard.cursor = 0;
	keyboard.input_text_len = 0;
	keyboard.input_mode = INPUT_FULL;
	keyboard.has_selection = false;
	keyboard.selection_begin_offset = 0;
	keyboard.selection_length = 0;
	memset(keyboard.keys, 0, MAX_KEYCODE);
	
	return keyboard;
}

void keyboard_set_input_mode(keyboard_input_mode mode)
{
	_global_keyboard.input_mode = mode;
}

inline void keyboard_input_destroy()
{
	mem_free(_global_keyboard.input_text);
}

static void keyboard_handle_input_copy_and_paste(platform_window *window)
{
	bool is_lctrl_down = _global_keyboard.keys[KEY_LEFT_CONTROL];
	
	if (is_lctrl_down && keyboard_is_key_pressed(KEY_V))
	{
		char buf[MAX_INPUT_LENGTH];
		bool result = platform_get_clipboard(window, buf);
		
		if (_global_keyboard.input_mode == INPUT_NUMERIC && !string_is_numeric(buf))
		{
			return;
		}
		
		if (_global_keyboard.has_selection)
		{
			_global_keyboard.cursor = _global_keyboard.selection_begin_offset;
			utf8_str_remove_range(_global_keyboard.input_text, _global_keyboard.selection_begin_offset,
								  _global_keyboard.selection_begin_offset + _global_keyboard.selection_length);
			_global_keyboard.has_selection = false;
			_global_keyboard.text_changed = true;
		}
		
		if (result)
		{
			s32 len = utf8len(buf);
			utf8_str_insert_utf8str(_global_keyboard.input_text, _global_keyboard.cursor, buf);
			
			_global_keyboard.cursor += len;
			_global_keyboard.input_text_len += len;
			_global_keyboard.text_changed = true;
		}
	}
	else if (is_lctrl_down && keyboard_is_key_pressed(KEY_C))
	{
		char buffer[MAX_INPUT_LENGTH];
		utf8_str_copy_range(_global_keyboard.input_text, 
							_global_keyboard.selection_begin_offset,
							_global_keyboard.selection_begin_offset+_global_keyboard.selection_length,
							buffer);
		
		if (!string_equals(buffer, ""))
			platform_set_clipboard(window, buffer);
	}
}

void keyboard_handle_input_string(platform_window *window, char *ch)
{
	keyboard_handle_input_copy_and_paste(window);
	
	bool is_lctrl_down = _global_keyboard.keys[KEY_LEFT_CONTROL];
	
	if (ch)
	{
		if (string_equals(ch, "\n")) is_lctrl_down = false;
		
		if (_global_keyboard.input_text_len < MAX_INPUT_LENGTH && !is_lctrl_down)
		{
			if (_global_keyboard.has_selection)
			{
				_global_keyboard.cursor = _global_keyboard.selection_begin_offset;
				utf8_str_remove_range(_global_keyboard.input_text, _global_keyboard.selection_begin_offset,
									  _global_keyboard.selection_begin_offset + _global_keyboard.selection_length);
				_global_keyboard.has_selection = false;
				_global_keyboard.text_changed = true;
			}
			
			if (_global_keyboard.input_text_len)
			{
				utf8_str_insert_at(_global_keyboard.input_text, _global_keyboard.cursor, *ch);
				_global_keyboard.text_changed = true;
			}
			else
			{
				string_appendn(_global_keyboard.input_text, ch, MAX_INPUT_LENGTH);
				_global_keyboard.text_changed = true;
			}
			
			_global_keyboard.cursor++;
			_global_keyboard.input_text_len = utf8len(_global_keyboard.input_text);
		}
	}
	else
	{
		bool is_lctrl_down = _global_keyboard.keys[KEY_LEFT_CONTROL];
		
		// cursor movement
		if (keyboard_is_key_down(KEY_LEFT) && _global_keyboard.cursor > 0)
		{
			if (is_lctrl_down)
				_global_keyboard.cursor = 0;
			else
				_global_keyboard.cursor--;
		}
		if (keyboard_is_key_down(KEY_RIGHT) && _global_keyboard.cursor < _global_keyboard.input_text_len)
		{
			if (is_lctrl_down)
				_global_keyboard.cursor = utf8len(_global_keyboard.input_text);
			else
				_global_keyboard.cursor++;
		}
	}
	
	if (keyboard_is_key_pressed(KEY_X))
	{
		bool is_lctrl_down = _global_keyboard.keys[KEY_LEFT_CONTROL];
		
		if (is_lctrl_down)
		{
			// copy selection
			if (_global_keyboard.has_selection)
			{
				char buffer[MAX_INPUT_LENGTH];
				utf8_str_copy_range(_global_keyboard.input_text, 
									_global_keyboard.selection_begin_offset,
									_global_keyboard.selection_begin_offset+_global_keyboard.selection_length,
									buffer);
				
				if (!string_equals(buffer, ""))
					platform_set_clipboard(window, buffer);
			}
			
			// delete selection
			if (_global_keyboard.has_selection)
			{
				utf8_str_remove_range(_global_keyboard.input_text, _global_keyboard.selection_begin_offset,
									  _global_keyboard.selection_begin_offset + _global_keyboard.selection_length);
				_global_keyboard.has_selection = false;
				_global_keyboard.text_changed = true;
				
				if (_global_keyboard.selection_begin_offset < _global_keyboard.cursor)
				{
					_global_keyboard.cursor -= _global_keyboard.selection_length;
				}
			}
			
			_global_keyboard.input_text_len = utf8len(_global_keyboard.input_text);
		}
	}
	
	if (keyboard_is_key_down(KEY_BACKSPACE))
	{
		_global_keyboard.keys[KEY_BACKSPACE] = false;
		
		bool is_lctrl_down = _global_keyboard.keys[KEY_LEFT_CONTROL];
		
		if (_global_keyboard.has_selection)
		{
			utf8_str_remove_range(_global_keyboard.input_text, _global_keyboard.selection_begin_offset,
								  _global_keyboard.selection_begin_offset + _global_keyboard.selection_length);
			_global_keyboard.has_selection = false;
			_global_keyboard.text_changed = true;
			
			if (_global_keyboard.selection_begin_offset < _global_keyboard.cursor)
			{
				_global_keyboard.cursor -= _global_keyboard.selection_length;
			}
		}
		else if (is_lctrl_down)
		{
			char *iter = _global_keyboard.input_text;
			s32 index = 0;
			s32 found_index = 0;
			utf8_int32_t br;
			while((iter = utf8codepoint(iter, &br)) && index < _global_keyboard.cursor && br)
			{
				index++;
				if (br == ' ' && index+1 < _global_keyboard.cursor)
				{
					found_index = index;
				}
			}
			
			for (s32 i = found_index; i < _global_keyboard.cursor; i++)
			{
				utf8_str_remove_at(_global_keyboard.input_text, found_index);
			}
			_global_keyboard.cursor = found_index;
			_global_keyboard.text_changed = true;
		}
		else if (_global_keyboard.cursor > 0)
		{
			utf8_str_remove_at(_global_keyboard.input_text, _global_keyboard.cursor-1);
			
			_global_keyboard.cursor--;
			_global_keyboard.text_changed = true;
		}
		
		_global_keyboard.input_text_len = utf8len(_global_keyboard.input_text);
	}
}

inline bool keyboard_is_shortcut_down(s32 shortcut_keys[2])
{
	return keyboard_is_key_down(shortcut_keys[0]) &&
		keyboard_is_key_pressed(shortcut_keys[1]);
}

inline void reset_left_click() {
	_global_mouse.left_state &= ~MOUSE_CLICK;
}
inline void reset_left_down() {
	_global_mouse.left_state &= ~MOUSE_DOWN;
}
inline void reset_right_click() {
	_global_mouse.right_state &= ~MOUSE_CLICK;
}