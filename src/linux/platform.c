/* 
*  BSD 2-Clause “Simplified” License
*  Copyright (c) 2019, Aldrik Ramaekers, aldrik.ramaekers@protonmail.com
*  All rights reserved.
*/

#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>
#include <X11/Xatom.h>
#include <time.h>
#include <X11/XKBlib.h>
#include <unistd.h>
#include <limits.h>
#include <dirent.h> 
#include <errno.h>
#include <dlfcn.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <X11/cursorfont.h>

#define GET_ATOM(X) window.X = XInternAtom(window.display, #X, False)

struct t_platform_window
{
	Display *display;
	Window parent;
	XVisualInfo *visual_info;
	Colormap cmap;
	Window window;
	GLXContext gl_context;
	XWindowAttributes window_attributes;
	XEvent event;
	char *clipboard_str;
	GC gc;
	s32 clipboard_strlen;
	
	Atom xdnd_req;
	Atom xdnd_source;
	Atom XdndEnter;
	Atom XdndPosition;
	Atom XdndStatus;
	Atom XdndTypeList;
	Atom XdndActionCopy;
	Atom XdndDrop;
	Atom XdndFinished;
	Atom XdndSelection;
	Atom XdndLeave;
	Atom quit;
	Atom PRIMARY;
	Atom CLIPBOARD;
	Atom UTF8_STRING;
	Atom COMPOUND_STRING;
	Atom TARGETS;
	Atom MULTIPLE;
	Atom _NET_WM_STATE;
	
	// shared window properties
	bool icon_loaded;
	bool do_draw;
	backbuffer backbuffer;
	s32 width;
	s32 height;
	bool is_open;
	bool has_focus;
	cursor_type curr_cursor_type;
	cursor_type next_cursor_type;
};

bool platform_get_clipboard(platform_window *window, char *buffer)
{
	char *result;
	unsigned long ressize, restail;
	int resbits;
	Atom bufid = XInternAtom(window->display, "CLIPBOARD", False),
	fmtid = XInternAtom(window->display, "UTF8_STRING", False),
	propid = XInternAtom(window->display, "XSEL_DATA", False),
	incrid = XInternAtom(window->display, "INCR", False);
	XEvent event;
	
	if(window->CLIPBOARD != None) {
		
		if (settings_window && XGetSelectionOwner(window->display, window->CLIPBOARD) == settings_window->window)
		{
			snprintf(buffer, MAX_INPUT_LENGTH, "%s", settings_window->clipboard_str);
			return true;
		}
		else if (XGetSelectionOwner(window->display, window->CLIPBOARD) == 
				 main_window->window)
		{
			snprintf(buffer, MAX_INPUT_LENGTH, "%s", main_window->clipboard_str);
			return true;
		}
	}
	
	XConvertSelection(window->display, bufid, fmtid, propid, window->window, CurrentTime);
	do {
		XNextEvent(window->display, &event);
	} while (event.type != SelectionNotify || event.xselection.selection != bufid);
	
	if (event.xselection.property)
	{
		XGetWindowProperty(window->display, window->window, propid, 0, LONG_MAX/4, False, AnyPropertyType,
						   &fmtid, &resbits, &ressize, &restail, (unsigned char**)&result);
		
		if (fmtid == incrid)
			printf("Buffer is too large and INCR reading is not implemented yet.\n");
		else
			snprintf(buffer, MAX_INPUT_LENGTH, "%s", result);
		
		XFree(result);
		return True;
	}
	else // request failed, e.g. owner can't convert to the target format
		return False;
}

bool platform_set_clipboard(platform_window *window, char *buffer)
{
	if (buffer)
	{
		if(window->CLIPBOARD != None && XGetSelectionOwner(window->display, window->CLIPBOARD) != window->window) {
			XSetSelectionOwner(window->display, window->CLIPBOARD, window->window, CurrentTime);
		}
		
		window->clipboard_strlen = strlen(buffer);
		if(!window->clipboard_str) {
			window->clipboard_str = mem_alloc(window->clipboard_strlen+1);
		} else {
			window->clipboard_str = mem_realloc(window->clipboard_str, window->clipboard_strlen+1);
		}
		string_copyn(window->clipboard_str, buffer, window->clipboard_strlen+1);
		
		return true;
	}
	
	return false;
}

void platform_create_config_directory()
{
	char *env = getenv("HOME");
	char tmp[PATH_MAX];
	snprintf(tmp, PATH_MAX, "%s%s", env, CONFIG_DIRECTORY_LINUX);
	
	if (!platform_directory_exists(tmp))
	{
		mkdir(tmp, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	}
}

char* get_config_save_location(char *buffer)
{
	char *env = getenv("HOME");
	snprintf(buffer, PATH_MAX, "%s%s", env, CONFIG_DIRECTORY_LINUX"/config.txt");
	return buffer;
}

inline void platform_set_cursor(platform_window *window, cursor_type type)
{
	if (window->next_cursor_type != type)
	{
		window->next_cursor_type = type;
	}
}

bool is_platform_in_darkmode()
{
	return false;
}

bool get_active_directory(char *buffer)
{
	char cwd[PATH_MAX];
	if (getcwd(cwd, sizeof(cwd)) != NULL) {
		string_copyn(buffer, cwd, PATH_MAX);
	} else {
		return false;
	}
	return true;
}

bool set_active_directory(char *path)
{
	return !chdir(path);
}

void platform_delete_file(char *path)
{
	remove(path);
}

bool platform_write_file_content(char *path, const char *mode, char *buffer, s32 len)
{
	bool result = false;
	
	FILE *file = fopen(path, mode);
	
	if (!file)
	{
		goto done_failure;
	}
	else
	{
		fwrite(buffer, 1, len, file); // TODO(Aldrik): is this correct?
		//fprintf(file, "%s", buffer);
	}
	
	//done:
	fclose(file);
	done_failure:
	return result;
}

void platform_window_set_title(platform_window *window, char *name)
{
	Atom WM_NAME = XInternAtom(window->display, "WM_NAME", False);
	Atom _NET_WM_NAME = XInternAtom(window->display, "_NET_WM_NAME", False);
	Atom _NET_WM_ICON_NAME = XInternAtom(window->display, "_NET_WM_ICON_NAME", False);
	
	char *list[1] = { (char *) name };
	XTextProperty property;
	
	XStoreName(window->display, window->window, name);
	
	Xutf8TextListToTextProperty(window->display, list, 1, XUTF8StringStyle,
								&property);
	XSetTextProperty(window->display, window->window, &property, WM_NAME);
	XSetTextProperty(window->display, window->window, &property, _NET_WM_NAME);
	XSetTextProperty(window->display, window->window, &property, XA_WM_NAME);
	XSetTextProperty(window->display, window->window, &property, _NET_WM_ICON_NAME);
	XFree(property.value);
}

bool platform_file_exists(char *path)
{
	if(access(path, F_OK) != -1) {
		return 1;
	}
	
	return 0;
}

bool platform_directory_exists(char *path)
{
	DIR* dir = opendir(path);
	if (dir) {
		/* Directory exists. */
		closedir(dir);
		return 1;
	} else if (ENOENT == errno) {
		return 0; // does not exist
	} else {
		return 0; // error opening dir
	}
}


s32 platform_get_file_size(char *path)
{
	FILE *file = fopen(path, "rb");
	if (!file) return -1;
	
	fseek(file, 0 , SEEK_END);
	int length = ftell(file);
	fseek(file, 0, SEEK_SET);
	
	fclose(file);
	return length;
}

file_content platform_read_file_content(char *path, const char *mode)
{
	file_content result;
	result.content = 0;
	result.content_length = 0;
	result.file_error = 0;
	
	FILE *file = fopen(path, mode);
	
	if (!file) 
	{
		if (errno == EMFILE)
			result.file_error = FILE_ERROR_TOO_MANY_OPEN_FILES_PROCESS;
		else if (errno == ENFILE)
			result.file_error = FILE_ERROR_TOO_MANY_OPEN_FILES_SYSTEM;
		else if (errno == EACCES)
			result.file_error = FILE_ERROR_NO_ACCESS;
		else if (errno == EPERM)
			result.file_error = FILE_ERROR_NO_ACCESS;
		else if (errno == ENOENT)
			result.file_error = FILE_ERROR_NOT_FOUND;
		else if (errno == ECONNABORTED)
			result.file_error = FILE_ERROR_CONNECTION_ABORTED;
		else if (errno == ECONNREFUSED)
			result.file_error = FILE_ERROR_CONNECTION_REFUSED;
		else if (errno == ENETDOWN)
			result.file_error = FILE_ERROR_NETWORK_DOWN;
		else if (errno == EREMOTEIO)
			result.file_error = FILE_ERROR_REMOTE_IO_ERROR;
		else if (errno == ESTALE)
			result.file_error = FILE_ERROR_STALE;
		else
		{
			result.file_error = FILE_ERROR_GENERIC;
			printf("ERROR: %d\n", errno);
		}
		
		goto done_failure;
	}
	
	fseek(file, 0 , SEEK_END);
	int length = ftell(file);
	fseek(file, 0, SEEK_SET);
	
	s32 length_to_alloc = length+1;
	
	result.content = mem_alloc(length_to_alloc);
	if (!result.content) goto done;
	
	memset(result.content, 0, length);
	s32 read_result = fread(result.content, 1, length, file);
	if (read_result == 0 && length != 0)
	{
		mem_free(result.content);
		result.content = 0;
		return result;
	}
	
	result.content_length = read_result;
	
	((char*)result.content)[length] = 0;
	
	done:
	fclose(file);
	done_failure:
	return result;
}

inline void platform_destroy_file_content(file_content *content)
{
	assert(content);
	mem_free(content->content);
}

static s32 translate_keycode(platform_window *window, s32 scancode)
{
    s32 keySym;
	
    // Valid key code range is  [8,255], according to the Xlib manual
	
    if (1)
    {
        // Try secondary keysym, for numeric keypad keys
        // Note: This way we always force "NumLock = ON", which is intentional
        // since the returned key code should correspond to a physical
        // location.
        keySym = XkbKeycodeToKeysym(window->display, scancode, 0, 1);
        switch (keySym)
        {
            case XK_KP_0:           return KEY_KP_0;
            case XK_KP_1:           return KEY_KP_1;
            case XK_KP_2:           return KEY_KP_2;
            case XK_KP_3:           return KEY_KP_3;
            case XK_KP_4:           return KEY_KP_4;
            case XK_KP_5:           return KEY_KP_5;
            case XK_KP_6:           return KEY_KP_6;
            case XK_KP_7:           return KEY_KP_7;
            case XK_KP_8:           return KEY_KP_8;
            case XK_KP_9:           return KEY_KP_9;
            case XK_KP_Separator:
            case XK_KP_Decimal:     return KEY_KP_DECIMAL;
            case XK_KP_Equal:       return KEY_KP_EQUAL;
            case XK_KP_Enter:       return KEY_KP_ENTER;
            default:                break;
        }
		
        // Now try primary keysym for function keys (non-printable keys)
        // These should not depend on the current keyboard layout
        keySym = XkbKeycodeToKeysym(window->display, scancode, 0, 0);
    }
	
    switch (keySym)
    {
        case XK_Escape:         return KEY_ESCAPE;
        case XK_Tab:            return KEY_TAB;
        case XK_Shift_L:        return KEY_LEFT_SHIFT;
        case XK_Shift_R:        return KEY_RIGHT_SHIFT;
        case XK_Control_L:      return KEY_LEFT_CONTROL;
        case XK_Control_R:      return KEY_RIGHT_CONTROL;
        case XK_Meta_L:
        case XK_Alt_L:          return KEY_LEFT_ALT;
        case XK_Mode_switch: // Mapped to Alt_R on many keyboards
        case XK_ISO_Level3_Shift: // AltGr on at least some machines
        case XK_Meta_R:
        case XK_Alt_R:          return KEY_RIGHT_ALT;
        case XK_Super_L:        return KEY_LEFT_SUPER;
        case XK_Super_R:        return KEY_RIGHT_SUPER;
        case XK_Menu:           return KEY_MENU;
        case XK_Num_Lock:       return KEY_NUM_LOCK;
        case XK_Caps_Lock:      return KEY_CAPS_LOCK;
        case XK_Print:          return KEY_PRINT_SCREEN;
        case XK_Scroll_Lock:    return KEY_SCROLL_LOCK;
        case XK_Pause:          return KEY_PAUSE;
        case XK_Delete:         return KEY_DELETE;
        case XK_BackSpace:      return KEY_BACKSPACE;
        case XK_Return:         return KEY_ENTER;
        case XK_Home:           return KEY_HOME;
        case XK_End:            return KEY_END;
        case XK_Page_Up:        return KEY_PAGE_UP;
        case XK_Page_Down:      return KEY_PAGE_DOWN;
        case XK_Insert:         return KEY_INSERT;
        case XK_Left:           return KEY_LEFT;
        case XK_Right:          return KEY_RIGHT;
        case XK_Down:           return KEY_DOWN;
        case XK_Up:             return KEY_UP;
        case XK_F1:             return KEY_F1;
        case XK_F2:             return KEY_F2;
        case XK_F3:             return KEY_F3;
        case XK_F4:             return KEY_F4;
        case XK_F5:             return KEY_F5;
        case XK_F6:             return KEY_F6;
        case XK_F7:             return KEY_F7;
        case XK_F8:             return KEY_F8;
        case XK_F9:             return KEY_F9;
        case XK_F10:            return KEY_F10;
        case XK_F11:            return KEY_F11;
        case XK_F12:            return KEY_F12;
        case XK_F13:            return KEY_F13;
        case XK_F14:            return KEY_F14;
        case XK_F15:            return KEY_F15;
        case XK_F16:            return KEY_F16;
        case XK_F17:            return KEY_F17;
        case XK_F18:            return KEY_F18;
        case XK_F19:            return KEY_F19;
        case XK_F20:            return KEY_F20;
        case XK_F21:            return KEY_F21;
        case XK_F22:            return KEY_F22;
        case XK_F23:            return KEY_F23;
        case XK_F24:            return KEY_F24;
        case XK_F25:            return KEY_F25;
		
        // Numeric keypad
        case XK_KP_Divide:      return KEY_KP_DIVIDE;
        case XK_KP_Multiply:    return KEY_KP_MULTIPLY;
        case XK_KP_Subtract:    return KEY_KP_SUBTRACT;
        case XK_KP_Add:         return KEY_KP_ADD;
		
        // These should have been detected in secondary keysym test above!
        case XK_KP_Insert:      return KEY_KP_0;
        case XK_KP_End:         return KEY_KP_1;
        case XK_KP_Down:        return KEY_KP_2;
        case XK_KP_Page_Down:   return KEY_KP_3;
        case XK_KP_Left:        return KEY_KP_4;
        case XK_KP_Right:       return KEY_KP_6;
        case XK_KP_Home:        return KEY_KP_7;
        case XK_KP_Up:          return KEY_KP_8;
        case XK_KP_Page_Up:     return KEY_KP_9;
        case XK_KP_Delete:      return KEY_KP_DECIMAL;
        case XK_KP_Equal:       return KEY_KP_EQUAL;
        case XK_KP_Enter:       return KEY_KP_ENTER;
		
        // Last resort: Check for printable keys (should not happen if the XKB
        // extension is available). This will give a layout dependent mapping
        // (which is wrong, and we may miss some keys, especially on non-US
        // keyboards), but it's better than nothing...
        case XK_a:              return KEY_A;
        case XK_b:              return KEY_B;
        case XK_c:              return KEY_C;
        case XK_d:              return KEY_D;
        case XK_e:              return KEY_E;
        case XK_f:              return KEY_F;
        case XK_g:              return KEY_G;
        case XK_h:              return KEY_H;
        case XK_i:              return KEY_I;
        case XK_j:              return KEY_J;
        case XK_k:              return KEY_K;
        case XK_l:              return KEY_L;
        case XK_m:              return KEY_M;
        case XK_n:              return KEY_N;
        case XK_o:              return KEY_O;
        case XK_p:              return KEY_P;
        case XK_q:              return KEY_Q;
        case XK_r:              return KEY_R;
        case XK_s:              return KEY_S;
        case XK_t:              return KEY_T;
        case XK_u:              return KEY_U;
        case XK_v:              return KEY_V;
        case XK_w:              return KEY_W;
        case XK_x:              return KEY_X;
        case XK_y:              return KEY_Y;
        case XK_z:              return KEY_Z;
        case XK_1:              return KEY_1;
        case XK_2:              return KEY_2;
        case XK_3:              return KEY_3;
        case XK_4:              return KEY_4;
        case XK_5:              return KEY_5;
        case XK_6:              return KEY_6;
        case XK_7:              return KEY_7;
        case XK_8:              return KEY_8;
        case XK_9:              return KEY_9;
        case XK_0:              return KEY_0;
        case XK_space:          return KEY_SPACE;
        case XK_minus:          return KEY_MINUS;
        case XK_equal:          return KEY_EQUAL;
        case XK_bracketleft:    return KEY_LEFT_BRACKET;
        case XK_bracketright:   return KEY_RIGHT_BRACKET;
        case XK_backslash:      return KEY_BACKSLASH;
        case XK_semicolon:      return KEY_SEMICOLON;
        case XK_apostrophe:     return KEY_APOSTROPHE;
        case XK_grave:          return KEY_GRAVE_ACCENT;
        case XK_comma:          return KEY_COMMA;
        case XK_period:         return KEY_PERIOD;
        case XK_slash:          return KEY_SLASH;
        case XK_less:           return KEY_WORLD_1; // At least in some layouts...
        default:                break;
    }
	
    // No matching translation was found
    return KEY_UNKNOWN;
}

static void create_key_tables(platform_window window)
{
	s32 scancode, key;
	char name[XkbKeyNameLength + 1];
	XkbDescPtr desc = XkbGetMap(window.display, 0, XkbUseCoreKbd);
	XkbGetNames(window.display, XkbKeyNamesMask, desc);
	
	// uncomment for layout independant input for games.
#if 0
	for (scancode = desc->min_key_code;  scancode <= desc->max_key_code;  scancode++)
	{
		memcpy(name, desc->names->keys[scancode].name, XkbKeyNameLength);
		name[XkbKeyNameLength] = '\0';
		
		if (strcmp(name, "TLDE") == 0) key = KEY_GRAVE_ACCENT;
		else if (strcmp(name, "AE01") == 0) key = KEY_1;
		else if (strcmp(name, "AE02") == 0) key = KEY_2;
		else if (strcmp(name, "AE03") == 0) key = KEY_3;
		else if (strcmp(name, "AE04") == 0) key = KEY_4;
		else if (strcmp(name, "AE05") == 0) key = KEY_5;
		else if (strcmp(name, "AE06") == 0) key = KEY_6;
		else if (strcmp(name, "AE07") == 0) key = KEY_7;
		else if (strcmp(name, "AE08") == 0) key = KEY_8;
		else if (strcmp(name, "AE09") == 0) key = KEY_9;
		else if (strcmp(name, "AE10") == 0) key = KEY_0;
		else if (strcmp(name, "AE11") == 0) key = KEY_MINUS;
		else if (strcmp(name, "AE12") == 0) key = KEY_EQUAL;
		else if (strcmp(name, "AD01") == 0) key = KEY_Q;
		else if (strcmp(name, "AD02") == 0) key = KEY_W;
		else if (strcmp(name, "AD03") == 0) key = KEY_E;
		else if (strcmp(name, "AD04") == 0) key = KEY_R;
		else if (strcmp(name, "AD05") == 0) key = KEY_T;
		else if (strcmp(name, "AD06") == 0) key = KEY_Y;
		else if (strcmp(name, "AD07") == 0) key = KEY_U;
		else if (strcmp(name, "AD08") == 0) key = KEY_I;
		else if (strcmp(name, "AD09") == 0) key = KEY_O;
		else if (strcmp(name, "AD10") == 0) key = KEY_P;
		else if (strcmp(name, "AD11") == 0) key = KEY_LEFT_BRACKET;
		else if (strcmp(name, "AD12") == 0) key = KEY_RIGHT_BRACKET;
		else if (strcmp(name, "AC01") == 0) key = KEY_A;
		else if (strcmp(name, "AC02") == 0) key = KEY_S;
		else if (strcmp(name, "AC03") == 0) key = KEY_D;
		else if (strcmp(name, "AC04") == 0) key = KEY_F;
		else if (strcmp(name, "AC05") == 0) key = KEY_G;
		else if (strcmp(name, "AC06") == 0) key = KEY_H;
		else if (strcmp(name, "AC07") == 0) key = KEY_J;
		else if (strcmp(name, "AC08") == 0) key = KEY_K;
		else if (strcmp(name, "AC09") == 0) key = KEY_L;
		else if (strcmp(name, "AC10") == 0) key = KEY_SEMICOLON;
		else if (strcmp(name, "AC11") == 0) key = KEY_APOSTROPHE;
		else if (strcmp(name, "AB01") == 0) key = KEY_Z;
		else if (strcmp(name, "AB02") == 0) key = KEY_X;
		else if (strcmp(name, "AB03") == 0) key = KEY_C;
		else if (strcmp(name, "AB04") == 0) key = KEY_V;
		else if (strcmp(name, "AB05") == 0) key = KEY_B;
		else if (strcmp(name, "AB06") == 0) key = KEY_N;
		else if (strcmp(name, "AB07") == 0) key = KEY_M;
		else if (strcmp(name, "AB08") == 0) key = KEY_COMMA;
		else if (strcmp(name, "AB09") == 0) key = KEY_PERIOD;
		else if (strcmp(name, "AB10") == 0) key = KEY_SLASH;
		else if (strcmp(name, "BKSL") == 0) key = KEY_BACKSLASH;
		else if (strcmp(name, "LSGT") == 0) key = KEY_WORLD_1;
		else key = KEY_UNKNOWN;
		
		if ((scancode >= 0) && (scancode < 256))
			keycode_map[scancode] = key;
	}
#endif
	
	for (scancode = 0;  scancode < MAX_KEYCODE;  scancode++)
	{
		// Translate the un-translated key codes using traditional X11 KeySym
		// lookups
		
		keycode_map[scancode] = translate_keycode(&window, scancode);
	}
	
	XkbFreeNames(desc, XkbKeyNamesMask, True);
	XkbFreeKeyboard(desc, 0, True);
}

//CURL *curl;
inline void platform_init(int argc, char **argv)
{
#if 0
	dlerror(); // clear error
	void *x11 = dlopen("libX11.so.6", RTLD_NOW | RTLD_GLOBAL);
	void *randr = dlopen("libXrandr.so", RTLD_NOW | RTLD_GLOBAL);
#endif
	
	setlocale(LC_ALL, "en_US.UTF-8");
	
	XInitThreads();
	
	// get fullpath of the directory the binary is residing in
	binary_path = platform_get_full_path(argv[0]);
	
	platform_create_config_directory();
	
	char buf[MAX_INPUT_LENGTH];
	get_directory_from_path(buf, binary_path);
	string_copyn(binary_path, buf, MAX_INPUT_LENGTH);
	
	//curl = curl_easy_init();
	
	assets_create();
}

inline void platform_destroy()
{
	assets_destroy();
	//curl_easy_cleanup(curl);
	//curl_global_cleanup();
	
#if defined(MODE_DEVELOPER)
	memory_print_leaks();
#endif
}

inline void platform_window_make_current(platform_window *window)
{
	if (global_use_gpu)
		glXMakeCurrent(window->display, window->window, window->gl_context);
}

static void _allocate_backbuffer(platform_window *window)
{
	if (window->backbuffer.buffer) { mem_free(window->backbuffer.buffer); window->backbuffer.buffer = 0; }
	
	window->backbuffer.width = window->width;
	window->backbuffer.height = window->height+1;
	
	s32 bufferMemorySize = (window->backbuffer.width*window->backbuffer.height)*5;
	window->backbuffer.buffer = mem_alloc(bufferMemorySize);
	window->backbuffer.s_image = XCreateImage(
		window->display, 
		DefaultVisual(window->display, DefaultScreen(window->display)),
		DefaultDepth(window->display, DefaultScreen(window->display)), 
		ZPixmap, 0, (char*)window->backbuffer.buffer, 
		window->backbuffer.width, window->backbuffer.height, 32, 0);
}

void platform_window_set_size(platform_window *window, u16 width, u16 height)
{
	XResizeWindow(window->display, window->window, width, height);
	
	if (!global_use_gpu)
		_allocate_backbuffer(window);
}

void platform_window_set_position(platform_window *window, u16 x, u16 y)
{
	XMoveWindow(window->display, window->window, x, y);
}


vec2 platform_get_window_size(platform_window *window)
{
	vec2 res;
	res.x = window->width;
	res.y = window->height;
	return res;
}

void platform_setup_backbuffer(platform_window *window)
{
	if (global_use_gpu)
	{
		static GLXContext share_list = 0;
		
		// get opengl context
		window->gl_context = glXCreateContext(window->display, window->visual_info, 
											  share_list, GL_TRUE);
		
		if (share_list == 0)
			share_list = window->gl_context;
		glXMakeCurrent(window->display, window->window, window->gl_context);
		
	}
	else
	{
		_allocate_backbuffer(window);
	}
}

void platform_setup_renderer()
{
	if (global_use_gpu)
	{
		////// GL SETUP
		glDepthMask(GL_TRUE);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glDepthFunc(GL_LEQUAL);
		glEnable(GL_DEPTH_TEST);
		glAlphaFunc(GL_GREATER, 0.0f);
		glEnable(GL_ALPHA_TEST);
		glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
		glEnable(GL_SAMPLE_ALPHA_TO_ONE);
		glEnable(GL_MULTISAMPLE);
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_SCISSOR_TEST);
		glEnable(GL_BLEND);
		//glEnable(GL_FRAMEBUFFER_SRGB);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_MULTISAMPLE_ARB);
		
		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();
		
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		//glOrtho(0, width, height, 0, -1, 1);
		
		glMatrixMode(GL_MODELVIEW);
		
	}
}

int _x11_error_handler(Display *display, XErrorEvent *event)
{
	char tmp[2000];
	XGetErrorText(display, event->error_code, tmp, 2000);
	printf("X11 ERROR: %s\n", tmp);
	
	return 0;
}

platform_window platform_open_window_ex(char *name, u16 width, u16 height, u16 max_w, u16 max_h, u16 min_w, u16 min_h, s32 flags)
{
	bool has_max_size = max_w || max_h;
	
	platform_window window;
	window.width = width;
	window.height = height;
	window.backbuffer.buffer = 0;
	window.gl_context = 0;
	window.icon_loaded = false;
	window.has_focus = true;
	window.curr_cursor_type = CURSOR_DEFAULT;
	window.next_cursor_type = CURSOR_DEFAULT;
	window.clipboard_str = 0;
	window.clipboard_strlen = 0;
	window.do_draw = true;
	window.backbuffer.s_image = 0;
	
	static int att[] =
	{
		GLX_X_RENDERABLE    , True,
		GLX_DRAWABLE_TYPE   , GLX_WINDOW_BIT,
		GLX_RENDER_TYPE     , GLX_RGBA_BIT,
		GLX_X_VISUAL_TYPE   , GLX_TRUE_COLOR,
		GLX_RED_SIZE        , 8,
		GLX_GREEN_SIZE      , 8,
		GLX_BLUE_SIZE       , 8,
		GLX_ALPHA_SIZE      , 8,
		GLX_DEPTH_SIZE      , 24,
		GLX_STENCIL_SIZE    , 8,
		GLX_DOUBLEBUFFER    , True,
		//GLX_SAMPLE_BUFFERS  , 1,
		//GLX_SAMPLES         , 4,
		None
	};
	
	window.display = XOpenDisplay(NULL);
	
	XSetErrorHandler(_x11_error_handler);
	
	if(window.display == NULL) {
		return window;
	}
	
	window.parent = DefaultRootWindow(window.display);
	
	int fbcount;
	GLXFBConfig* fbc = glXChooseFBConfig(window.display, DefaultScreen(window.display), att, &fbcount);
	int best_fbc = -1, worst_fbc = -1, best_num_samp = -1, worst_num_samp = 999;
	
	int i;
	for (i=0; i<fbcount; ++i)
	{
		XVisualInfo *vi = glXGetVisualFromFBConfig(window.display, fbc[i] );
		if ( vi )
		{
			int samp_buf, samples;
			glXGetFBConfigAttrib(window.display, fbc[i], GLX_SAMPLE_BUFFERS, &samp_buf );
			glXGetFBConfigAttrib(window.display, fbc[i], GLX_SAMPLES       , &samples  );
			
			if ( best_fbc < 0 || (samp_buf && samples > best_num_samp))
				best_fbc = i, best_num_samp = samples;
			if ( worst_fbc < 0 || !samp_buf || samples < worst_num_samp )
				worst_fbc = i, worst_num_samp = samples;
		}
		XFree(vi);
	}
	
	GLXFBConfig bestFbc = fbc[best_fbc];
	XFree(fbc);
	
	XVisualInfo *vi = glXGetVisualFromFBConfig(window.display, bestFbc );
	window.visual_info = vi;
	
	if(window.visual_info == NULL) {
		return window;
	}
	
	window.cmap = XCreateColormap(window.display, window.parent, window.visual_info->visual, AllocNone);
	
	// calculate window center
	XRRScreenResources *screens = XRRGetScreenResources(window.display, window.parent);
	XRRCrtcInfo *info = XRRGetCrtcInfo(window.display, screens, screens->crtcs[0]);
	
	s32 center_x = (info->width / 2) - (width / 2);
	s32 center_y = (info->height / 2) - (height / 2);
	
	XRRFreeCrtcInfo(info);
	XRRFreeScreenResources(screens);
	
	XSetWindowAttributes window_attributes;
	window_attributes.colormap = window.cmap;
	window_attributes.border_pixel = 0;
	window_attributes.event_mask = KeyPressMask | KeyReleaseMask | PointerMotionMask |
		ButtonPressMask | ButtonReleaseMask | StructureNotifyMask | FocusChangeMask | LeaveWindowMask;
	
	window.window = XCreateWindow(window.display, window.parent, center_x, center_y, width, height, 0, window.visual_info->depth, InputOutput, window.visual_info->visual, CWColormap | CWEventMask | CWBorderPixel, &window_attributes);
	
	
	XGCValues values;
	window.gc = XCreateGC(window.display, window.window, 0, &values);
	
	XMapWindow(window.display, window.window);
	XFlush(window.display);
	
	XSync(window.display, False);
	
	XSizeHints hints;
	
	if (has_max_size)
		hints.flags = PMaxSize | PMinSize | USPosition;
	else
		hints.flags = PMinSize | USPosition;
	hints.x = center_x;
	hints.y = center_y;
	hints.max_width = width;
	hints.max_height = height;
	hints.min_width = min_w;
	hints.min_height = min_h;
	
	XSetWMNormalHints(window.display, window.window, &hints);
	
	// window name
	{
		Atom WM_NAME = XInternAtom(window.display, "WM_NAME", False);
		Atom _NET_WM_NAME = XInternAtom(window.display, "_NET_WM_NAME", False);
		Atom _NET_WM_ICON_NAME = XInternAtom(window.display, "_NET_WM_ICON_NAME", False);
		
		char *list[1] = { (char *) name };
		XTextProperty property;
		
		XStoreName(window.display, window.window, name);
		
		Xutf8TextListToTextProperty(window.display, list, 1, XUTF8StringStyle,
									&property);
		XSetTextProperty(window.display, window.window, &property, WM_NAME);
		XSetTextProperty(window.display, window.window, &property, _NET_WM_NAME);
		XSetTextProperty(window.display, window.window, &property, XA_WM_NAME);
		XSetTextProperty(window.display, window.window, &property, _NET_WM_ICON_NAME);
		XFree(property.value);
		
		XClassHint class_hint;
		class_hint.res_name = name;
		class_hint.res_class = name;
		XSetClassHint(window.display, window.window, &class_hint);
	}
	
	
	// hide taskbar icon and stay on top
#if 0
	{
		Atom wm_state = XInternAtom(window.display, 
									"_NET_WM_STATE", False);
		Atom taskbar_atom = XInternAtom(window.display, 
										"_NET_WM_STATE_SKIP_TASKBAR", False);
		Atom above_atom = XInternAtom(window.display, 
									  "_NET_WM_STATE_ABOVE", False);
		
		Atom atom_list[2] = {taskbar_atom, above_atom};
		XChangeProperty(window.display, window.window, wm_state, XA_ATOM, 32,
						PropModeReplace, (const unsigned char*)&atom_list, 2);
	}
#endif
	
	{
		XWMHints* win_hints = XAllocWMHints();
		win_hints->flags = StateHint | IconPositionHint;
		win_hints->initial_state = IconicState;
		win_hints->icon_x = 0;
		win_hints->icon_y = 0;
		
		/* pass the hints to the window manager. */
		XSetWMHints(window.display, window.window, win_hints);
		XFree(win_hints);
	}
	
	platform_setup_backbuffer(&window);
	platform_setup_renderer();
	
	window.is_open = true;
	window.width = width;
	window.height = height;
	
	create_key_tables(window);
	
	// recieve window close event
	window.quit = XInternAtom(window.display, "WM_DELETE_WINDOW", False);
	
	GET_ATOM(XdndEnter);
	GET_ATOM(XdndPosition);
	GET_ATOM(XdndStatus);
	GET_ATOM(XdndTypeList);
	GET_ATOM(XdndActionCopy);
	GET_ATOM(XdndDrop);
	GET_ATOM(XdndFinished);
	GET_ATOM(XdndSelection);
	GET_ATOM(XdndLeave);
	GET_ATOM(PRIMARY);
	GET_ATOM(CLIPBOARD);
	GET_ATOM(UTF8_STRING);
	GET_ATOM(COMPOUND_STRING);
	GET_ATOM(TARGETS);
	GET_ATOM(MULTIPLE);
	GET_ATOM(_NET_WM_STATE);
	
	array atoms = array_create(sizeof(Atom));
	array_push(&atoms, &window.quit);
	array_push(&atoms, &window.XdndEnter);
	array_push(&atoms, &window.XdndPosition);
	array_push(&atoms, &window.XdndStatus);
	array_push(&atoms, &window.XdndTypeList);
	array_push(&atoms, &window.XdndActionCopy);
	array_push(&atoms, &window.XdndDrop);
	array_push(&atoms, &window.XdndFinished);
	array_push(&atoms, &window.XdndSelection);
	array_push(&atoms, &window.XdndLeave);
	array_push(&atoms, &window.PRIMARY);
	array_push(&atoms, &window.CLIPBOARD);
	array_push(&atoms, &window.UTF8_STRING);
	array_push(&atoms, &window.COMPOUND_STRING);
	array_push(&atoms, &window.TARGETS);
	array_push(&atoms, &window.MULTIPLE);
	array_push(&atoms, &window._NET_WM_STATE);
	
	XSetWMProtocols(window.display, window.window, atoms.data, atoms.length);
	array_destroy(&atoms);
	
	Atom XdndAware = XInternAtom(window.display, "XdndAware", False);
	Atom xdnd_version = 5;
	XChangeProperty(window.display, window.window, XdndAware, XA_ATOM, 32,
					PropModeReplace, (unsigned char*)&xdnd_version, 1);
	
	
	XFlush(window.display);
	XSync(window.display, True);
	
	return window;
}

inline bool platform_window_is_valid(platform_window *window)
{
	return window->window && window->display;
}

void platform_destroy_window(platform_window *window)
{
	if (global_use_gpu)
	{
		glXMakeCurrent(window->display, None, NULL);
		glXDestroyContext(window->display, window->gl_context);
	}
	else
	{
		if (window->backbuffer.buffer) { mem_free(window->backbuffer.buffer); window->backbuffer.buffer = 0; }
	}
	
	XDestroyWindow(window->display, window->window);
	XCloseDisplay(window->display);
	XFree(window->visual_info);
	mem_free(window->clipboard_str);
	
	window->window = 0;
	window->display = 0;
}

void platform_hide_window_taskbar_icon(platform_window *window)
{
	XClientMessageEvent m;
	memset(&m, 0, sizeof(XClientMessageEvent));
	m.type = ClientMessage;
	m.display = window->display;
	m.window = window->window;
	m.message_type = window->_NET_WM_STATE;
	m.format=32;
	m.data.l[0] = 1;
	m.data.l[1] = XInternAtom(window->display, "_NET_WM_STATE_SKIP_TASKBAR", False);
	m.data.l[2] = None;
	m.data.l[3] = 1;
	m.data.l[4] = 0;
	XSendEvent(window->display, window->window, False, SubstructureRedirectMask | SubstructureNotifyMask, (XEvent*)&m);
	
	XFlush(window->display);
}

void platform_handle_events(platform_window *window, mouse_input *mouse, keyboard_input *keyboard)
{
	mouse->left_state &= ~MOUSE_CLICK;
	mouse->right_state &= ~MOUSE_CLICK;
	mouse->left_state &= ~MOUSE_DOUBLE_CLICK;
	mouse->right_state &= ~MOUSE_DOUBLE_CLICK;
	mouse->left_state &= ~MOUSE_RELEASE;
	mouse->right_state &= ~MOUSE_RELEASE;
	memset(keyboard->input_keys, 0, MAX_KEYCODE);
	mouse->move_x = 0;
	mouse->move_y = 0;
	mouse->scroll_state = 0;
	keyboard->text_changed = false;
	window->do_draw = true;
	
	XClientMessageEvent m;
	
	s32 pending_events = XPending(window->display);
	for (s32 i = 0; i < pending_events; i++)
	{
		XNextEvent(window->display, &window->event);
		if (window->event.type == ClientMessage)
		{
			static int xdnd_version=0;
			
			if ((Atom)window->event.xclient.data.l[0] == window->quit) {
				window->is_open = false;
			}
			
			if (window->event.xclient.message_type == window->XdndDrop)
			{
				if (window->xdnd_req == None) {
					/* say again - not interested! */
					memset(&m, 0, sizeof(XClientMessageEvent));
					m.type = ClientMessage;
					m.display = window->event.xclient.display;
					m.window = window->event.xclient.data.l[0];
					m.message_type = window->XdndFinished;
					m.format=32;
					m.data.l[0] = window->window;
					m.data.l[1] = 0;
					m.data.l[2] = None; /* fail! */
					XSendEvent(window->display, window->event.xclient.data.l[0], False, NoEventMask, (XEvent*)&m);
				} else {
					/* convert */
					if(xdnd_version >= 1) {
						XConvertSelection(window->display, window->XdndSelection, window->xdnd_req, window->PRIMARY, window->window, window->event.xclient.data.l[2]);
					} else {
						printf("time to find the time.\n");
						//XConvertSelection(window->display, window->XdndSelection, window->xdnd_req, window->PRIMARY, window->xwindow, CurrentTime);
					}
				}
			}
		}
		else if (window->event.type == LeaveNotify)
		{
			mouse->x = MOUSE_OFFSCREEN;
			mouse->y = MOUSE_OFFSCREEN;
		}
		else if (window->event.type == ConfigureNotify)
		{
			XConfigureEvent xce = window->event.xconfigure;
			window->width = xce.width;
			window->height = xce.height;
			
			if (!global_use_gpu)
				_allocate_backbuffer(window);
			else
				glViewport(0, 0, window->width, window->height);
		}
		else if (window->event.type == FocusIn)
		{
			window->has_focus = true;
		}
		else if (window->event.type == FocusOut)
		{
			mouse->x = MOUSE_OFFSCREEN;
			mouse->y = MOUSE_OFFSCREEN;
			window->has_focus = false;
			
			// if windows loses focus, set all keys to not pressed
			memset(keyboard->keys, 0, MAX_KEYCODE);
		}
		else if (window->event.type == MotionNotify)
		{
			s32 x = mouse->x;
			s32 y = mouse->y;
			
			mouse->total_move_x += window->event.xmotion.x - mouse->x;
			mouse->total_move_y += window->event.xmotion.y - mouse->y;
			
			mouse->x = window->event.xmotion.x;
			mouse->y = window->event.xmotion.y;
			
			mouse->move_x = mouse->x - x;
			mouse->move_y = mouse->y - y;
		}
		else if (window->event.type == ButtonPress)
		{
			Time ev_time = window->event.xbutton.time;
			static Time last_ev_time = 0;
			
			bool is_left_down = window->event.xbutton.button == Button1;
			bool is_right_down = window->event.xbutton.button == Button3;
			bool is_middle_down = window->event.xbutton.button == Button2;
			bool scroll_up = window->event.xbutton.button == Button4;
			bool scroll_down = window->event.xbutton.button == Button5;
			
			if (scroll_up)
				mouse->scroll_state = SCROLL_UP;
			if (scroll_down)
				mouse->scroll_state = SCROLL_DOWN;
			
			if (is_left_down)
			{
				if (ev_time - last_ev_time < 200)
				{
					mouse->left_state |= MOUSE_DOUBLE_CLICK;
				}
				
				mouse->left_state |= MOUSE_DOWN;
				mouse->left_state |= MOUSE_CLICK;
				
				mouse->total_move_x = 0;
				mouse->total_move_y = 0;
				last_ev_time = ev_time;
			}
			if (is_right_down)
			{
				mouse->right_state |= MOUSE_DOWN;
				mouse->right_state |= MOUSE_CLICK;
			}
		}
		else if (window->event.type == ButtonRelease)
		{
			bool is_left_up = window->event.xbutton.button == Button1;
			bool is_right_up = window->event.xbutton.button == Button3;
			bool is_middle_up = window->event.xbutton.button == Button2;
			
			if (is_left_up)
			{
				mouse->left_state = MOUSE_RELEASE;
			}
			if (is_right_up)
			{
				mouse->right_state = MOUSE_RELEASE;
			}
		}
		else if(window->event.type == KeyPress) 
		{
			s32 key = window->event.xkey.keycode;
			
			keyboard->keys[keycode_map[key]] = true;
			keyboard->input_keys[keycode_map[key]] = true;
			
			// https://gist.github.com/rickyzhang82/8581a762c9f9fc6ddb8390872552c250
			//printf("state: %d\n", window->event.xkey.state);
			
			// remove key control key from mask so it doesnt block input
			window->event.xkey.state &= ~ControlMask;
			
			// replace capslock with shiftkey else keylookup returns 0...
			if (window->event.xkey.state == 2)
				window->event.xkey.state = 1;
			
			KeySym ksym = XLookupKeysym(&window->event.xkey, window->event.xkey.state);
			
			if (keyboard->take_input)
			{
				char *ch = 0;
				switch(ksym)
				{
					case XK_space: ch = " "; break;
					case XK_exclam: ch = "!"; break;
					case XK_quotedbl: ch = "\""; break;
					case XK_numbersign: ch = "#"; break;
					case XK_dollar: ch = "$"; break;
					case XK_percent: ch = "%"; break;
					case XK_ampersand: ch = "&"; break;
					case XK_apostrophe: ch = "`"; break;
					case XK_parenleft: ch = "("; break;
					case XK_parenright: ch = ")"; break;
					case XK_asterisk: ch = "*"; break;
					case XK_plus: ch = "+"; break;
					case XK_comma: ch = ","; break;
					case XK_minus: ch = "-"; break;
					case XK_period: ch = "."; break;
					case XK_slash: ch = "/"; break;
					case XK_0: ch = "0"; break;
					case XK_1: ch = "1"; break;
					case XK_2: ch = "2"; break;
					case XK_3: ch = "3"; break;
					case XK_4: ch = "4"; break;
					case XK_5: ch = "5"; break;
					case XK_6: ch = "6"; break;
					case XK_7: ch = "7"; break;
					case XK_8: ch = "8"; break;
					case XK_9: ch = "9"; break;
					
					case XK_colon: ch = ":"; break;
					case XK_semicolon: ch = ";"; break;
					case XK_less: ch = "<"; break;
					case XK_equal: ch = "="; break;
					case XK_greater: ch = ">"; break;
					case XK_question: ch = "?"; break;
					case XK_at: ch = "@"; break;
					case XK_bracketleft: ch = "["; break;
					case XK_backslash: ch = "\\"; break;
					case XK_bracketright: ch = "]"; break;
					case XK_asciicircum: ch = "^"; break;
					case XK_underscore: ch = "_"; break;
					case XK_grave: ch = "`"; break;
					case XK_braceleft: ch = "{"; break;
					case XK_bar: ch = "|"; break;
					case XK_braceright: ch = "}"; break;
					case XK_asciitilde: ch = "~"; break;
				}
				
				if ((ksym >= XK_A && ksym <= XK_Z) || (ksym >= XK_a && ksym <= XK_z))
				{
					ch = XKeysymToString(ksym);
				}
				
				if (ch && keyboard->input_mode == INPUT_NUMERIC)
				{
					if (!(*ch >= 48 && *ch <= 57))
					{
						ch = 0;
					}
				}
				
				keyboard_handle_input_string(window, keyboard, ch);
			}
		}
		else if (window->event.type == KeyRelease)
		{
			s32 key = window->event.xkey.keycode;
			keyboard->keys[keycode_map[key]] = false;
			
			KeySym ksym = XLookupKeysym(&window->event.xkey, 0);
		}
		else if (window->event.type == SelectionClear)
		{
			window->clipboard_str = 0;
			window->clipboard_strlen = 0;
		}
		else if (window->event.type == SelectionRequest)
		{
			Atom formats[] = {window->UTF8_STRING, window->COMPOUND_STRING, XA_STRING};
			Atom targets[] = {window->TARGETS, window->MULTIPLE, window->UTF8_STRING, window->COMPOUND_STRING, XA_STRING};
			int formatCount = sizeof(formats) / sizeof(formats[0]);
			
			XSelectionEvent event = {.type = SelectionNotify, .selection = window->event.xselectionrequest.selection, .target = window->event.xselectionrequest.target, .display = window->event.xselectionrequest.display, .requestor = window->event.xselectionrequest.requestor, .time = window->event.xselectionrequest.time};
			
			if(window->event.xselectionrequest.target == window->TARGETS) {
				XChangeProperty(window->display, window->event.xselectionrequest.requestor, window->event.xselectionrequest.property, XA_ATOM, 32, PropModeReplace, (unsigned char*)targets, sizeof(targets) / sizeof(targets[0]));
				
				event.property = window->event.xselectionrequest.property;
			} else {
				event.property = None;
				int i;
				for(i = 0; i < formatCount; i++) {
					if(window->event.xselectionrequest.target == formats[i]) {
						XChangeProperty(window->display, window->event.xselectionrequest.requestor, window->event.xselectionrequest.property, window->event.xselectionrequest.target, 8, PropModeReplace, (unsigned char*)window->clipboard_str, window->clipboard_strlen);
						
						event.property = window->event.xselectionrequest.property;
						break;
					}
				}
			}
			
			XSendEvent(window->display, window->event.xselectionrequest.requestor, False, 0, (XEvent*)&event);
			XFlush(window->display);
		}
	}
}

inline void platform_show_alert(char *title, char *message)
{
	char command[MAX_INPUT_LENGTH];
	snprintf(command, MAX_INPUT_LENGTH, "notify-send \"%s\" \"%s\"", title, message);
	platform_run_command(command);
}

inline void platform_window_swap_buffers(platform_window *window)
{
	// set cursor if changed
	if (window->curr_cursor_type != window->next_cursor_type)
	{
		int cursor_shape = 0;
		switch(window->next_cursor_type)
		{
			case CURSOR_DEFAULT: cursor_shape = XC_arrow; break;
			case CURSOR_POINTER: cursor_shape = XC_hand1; break;
			case CURSOR_DRAG: cursor_shape = XC_sb_h_double_arrow; break;
			case CURSOR_TEXT: cursor_shape = XC_xterm; break;
		}
		Cursor cursor = XCreateFontCursor(window->display, cursor_shape);
		XDefineCursor(window->display, window->window, cursor);
		window->curr_cursor_type = window->next_cursor_type;
	}
	
	if (!global_use_gpu)
	{
		s32 pixel_count = window->backbuffer.width * window->backbuffer.height;
		for (s32 i = 0; i < pixel_count; i++)
		{
			u8 *buffer_entry = window->backbuffer.buffer + (i*5);
			memcpy(window->backbuffer.buffer + (i*4), buffer_entry, 4);
		}
		
		XPutImage(window->display, window->window, window->gc, window->backbuffer.s_image, 0, 0, 0, 0, window->backbuffer.width, window->backbuffer.height);
		XFlush(window->display);
	}
	else
	{
		glXSwapBuffers(window->display, window->window);
	}
}

u64 platform_get_time(time_type time_type, time_precision precision)
{
	s32 type = CLOCK_REALTIME;
	switch(time_type)
	{
		case TIME_FULL: type = CLOCK_REALTIME; break;
		case TIME_THREAD: type = CLOCK_THREAD_CPUTIME_ID; break;
		case TIME_PROCESS: type = CLOCK_PROCESS_CPUTIME_ID; break;
	}
	
	struct timespec tms;
	if (clock_gettime(type,&tms)) {
		return -1;
	}
	
	long result = 0;
	
	if (precision == TIME_NS)
	{
		result = tms.tv_sec * 1000000000;
		result += tms.tv_nsec;
		if (tms.tv_nsec % 1000 >= 500) {
			++result;
		}
	}
	else if (precision == TIME_US)
	{
		result = tms.tv_sec * 1000000;
		result += tms.tv_nsec/1000;
		if (tms.tv_nsec % 1000 >= 500) {
			++result;
		}
	}
	else if (precision == TIME_MILI_S)
	{
		result = tms.tv_sec * 1000;
		result += tms.tv_nsec/1000000;
		if (tms.tv_nsec % 1000 >= 500) {
			++result;
		}
	}
	else if (precision == TIME_S)
	{
		result = tms.tv_sec;
		result += tms.tv_nsec/1000000000;
		if (tms.tv_nsec % 1000 >= 500) {
			++result;
		}
	}
	
	return result;
}

inline s32 platform_get_cpu_count()
{
	return (int)sysconf(_SC_NPROCESSORS_ONLN);
}

inline s32 platform_get_memory_size()
{
	uint64_t aid = (uint64_t) sysconf(_SC_PHYS_PAGES);
	aid         *= (uint64_t) sysconf(_SC_PAGESIZE);
	aid         /= (uint64_t) (1024 * 1024);
	return (int)(aid);
}

void platform_show_message(platform_window *window, char *message, char *title)
{
	char command[MAX_INPUT_LENGTH];
	snprintf(command, MAX_INPUT_LENGTH, "zenity --info --text=\"%s\" --title=\"%s\" --width=240", message, title);
	FILE *f = popen(command, "r");
}

static void* platform_open_file_dialog_thread(void *data)
{
	struct open_dialog_args *args = data;
	
	FILE *f;
	
	char current_val[MAX_INPUT_LENGTH];
	string_copyn(current_val, args->buffer, MAX_INPUT_LENGTH);
	
	char file_filter[MAX_INPUT_LENGTH];
	file_filter[0] = 0;
	if (args->file_filter)
		snprintf(file_filter, MAX_INPUT_LENGTH, "--file-filter=\"%s\"", args->file_filter);
	
	char start_path[MAX_INPUT_LENGTH];
	start_path[0] = 0;
	if (args->start_path)
		snprintf(start_path, MAX_INPUT_LENGTH, "--filename=\"%s\"", args->start_path);
	
	
	char command[MAX_INPUT_LENGTH];
	
	if (args->type == OPEN_FILE)
	{
		snprintf(command, MAX_INPUT_LENGTH, "zenity --file-selection %s %s", file_filter, start_path);
	}
	else if (args->type == OPEN_DIRECTORY)
	{
		snprintf(command, MAX_INPUT_LENGTH, "zenity --file-selection --directory %s %s", file_filter, start_path);
	}
	else if (args->type == SAVE_FILE)
	{
		snprintf(command, MAX_INPUT_LENGTH, "zenity --file-selection --save --confirm-overwrite %s %s", file_filter, start_path);
	}
	
	f = popen(command, "r");
	
	char buffer[MAX_INPUT_LENGTH];
	char *result = fgets(buffer, MAX_INPUT_LENGTH, f);
	
	if (!result)
		return 0;
	
	// replace newlines with 0, we only want one file path
	s32 len = strlen(buffer);
	for (s32 x = 0; x < len; x++)
	{
		if (buffer[x] == '\n') buffer[x] = 0;
	}
	
	if (strcmp(buffer, current_val) != 0 && strcmp(buffer, "") != 0)
	{
		string_copyn(args->buffer, buffer, MAX_INPUT_LENGTH);
		s32 len = strlen(args->buffer);
		args->buffer[len] = 0;
	}
	
	return 0;
}

void *platform_open_file_dialog_block(void *arg)
{
	thread thr = thread_start(platform_open_file_dialog_thread, arg);
	thread_join(&thr);
	mem_free(arg);
	return 0;
}

void platform_list_files_block(array *list, char *start_dir, array filters, bool recursive, memory_bucket *bucket, bool include_directories, bool *is_cancelled, search_info *info)
{
	assert(list);
	
	s32 len = 0;
	char *matched_filter = 0;
	
	char *subdirname_buf;
	if (bucket)
		subdirname_buf = memory_bucket_reserve(bucket, MAX_INPUT_LENGTH);
	else
		subdirname_buf = mem_alloc(MAX_INPUT_LENGTH);
	
	DIR *d;
	struct dirent *dir;
	d = opendir(start_dir);
	if (d) {
		set_active_directory(start_dir);
		while ((dir = readdir(d)) != NULL) {
			if (*is_cancelled) break;
			set_active_directory(start_dir);
			
			if (dir->d_type == DT_DIR)
			{
				if ((strcmp(dir->d_name, ".") == 0) || (strcmp(dir->d_name, "..") == 0))
					continue;
				
				if (include_directories)
				{
					if ((len = filter_matches(&filters, dir->d_name, 
											  &matched_filter)) && len != -1)
					{
						char *buf;
						if (bucket)
							buf = memory_bucket_reserve(bucket, MAX_INPUT_LENGTH);
						else
							buf = mem_alloc(MAX_INPUT_LENGTH);
						
						//realpath(dir->d_name, buf);
						snprintf(buf, MAX_INPUT_LENGTH, "%s%s",start_dir, dir->d_name);
						
						found_file f;
						f.path = buf;
						
						if (bucket)
							f.matched_filter = memory_bucket_reserve(bucket, len+1);
						else
							f.matched_filter = mem_alloc(len+1);
						
						string_copyn(f.matched_filter, matched_filter, len+1);
						
						mutex_lock(&list->mutex);
						array_push_size(list, &f, sizeof(found_file));
						mutex_unlock(&list->mutex);
					}
				}
				
				if (recursive)
				{
					if (info) info->dir_count++;
					
					string_copyn(subdirname_buf, start_dir, MAX_INPUT_LENGTH);
					string_appendn(subdirname_buf, dir->d_name, MAX_INPUT_LENGTH);
					string_appendn(subdirname_buf, "/", MAX_INPUT_LENGTH);
					
					// do recursive search
					platform_list_files_block(list, subdirname_buf, filters, recursive, bucket, include_directories, is_cancelled, info);
				}
			}
			// we handle DT_UNKNOWN for file systems that do not support type lookup.
			else if (dir->d_type == DT_REG || dir->d_type == DT_UNKNOWN)
			{
				if (info) info->file_count++;
				
				// check if name matches pattern
				if ((len = filter_matches(&filters, dir->d_name, 
										  &matched_filter)) && len != -1)
				{
					char *buf;
					if (bucket)
						buf = memory_bucket_reserve(bucket, MAX_INPUT_LENGTH);
					else
						buf = mem_alloc(MAX_INPUT_LENGTH);
					
					//realpath(dir->d_name, buf);
					snprintf(buf, MAX_INPUT_LENGTH, "%s%s",start_dir, dir->d_name);
					
					found_file f;
					f.path = buf;
					
					if (bucket)
						f.matched_filter = memory_bucket_reserve(bucket, len+1);
					else
						f.matched_filter = mem_alloc(len+1);
					
					string_copyn(f.matched_filter, matched_filter, len+1);
					
					mutex_lock(&list->mutex);
					array_push_size(list, &f, sizeof(found_file));
					mutex_unlock(&list->mutex);
					
				}
			}
		}
		closedir(d);
	}
	
	if (!bucket)
		mem_free(subdirname_buf);
}

char *platform_get_full_path(char *file)
{
	char *buf = mem_alloc(PATH_MAX);
	buf[0] = 0;
	
	char *result = realpath(file, buf);
	
	if (!result)
	{
		buf[0] = 0;
		return buf;
	}
	
	return buf;
}

inline u64 string_to_u64(char *str)
{
	return (u64)strtoull(str, 0, 10);
}

inline u32 string_to_u32(char *str)
{
	return (u32)strtoul(str, 0, 10);
}

inline u16 string_to_u16(char *str)
{
	return (u16)strtoul(str, 0, 10);
}

inline u8 string_to_u8(char *str)
{
	return (u8)strtoul(str, 0, 10);
}

inline s64 string_to_s64(char *str)
{
	return (s64)strtoll(str, 0, 10);
}

inline s32 string_to_s32(char *str)
{
	return (u32)strtol(str, 0, 10);
}

inline s16 string_to_s16(char *str)
{
	return (s16)strtol(str, 0, 10);
}

inline s8 string_to_s8(char *str)
{
	return (s8)strtol(str, 0, 10);
}

inline s8 string_to_f32(char *str)
{
	return (f32)atof(str);
}

inline s8 string_to_f64(char *str)
{
	return (f64)strtod(str, NULL);
}

inline void platform_open_url(char *url)
{
	char buffer[MAX_INPUT_LENGTH];
	snprintf(buffer, MAX_INPUT_LENGTH, "xdg-open %s", url);
	platform_run_command(buffer);
}

inline void platform_run_command(char *command)
{
	s32 result = system(command);
}

void platform_set_icon(platform_window *window, image *img)
{
	if (!img->loaded) return;
	if (!window->icon_loaded)
		window->icon_loaded = true;
	else
		return;
	
	s32 w = img->width;
	s32 h = img->height;
	
	s32 nelements = (w * h) + 2;
	
	unsigned long data[nelements];
	int i = 0;
	(data)[i++] = w;
	(data)[i++] = h;
	
	for (s32 y = 0; y < h; y++)
	{
		for (s32 x = 0; x < w; x++)
		{
			s32 *pixel = (s32*)(&((data)[i++]));
			
			s32 img_pixel = *(((s32*)img->data+(x+(y*w))));
			
#if 0
			// 0xAABBGGRR
			s32 a = (img_pixel>>24) & 0x000000FF;
			s32 b = (img_pixel>>16) & 0x000000FF;
			s32 g = (img_pixel>> 8) & 0x000000FF;
			s32 r = (img_pixel>> 0) & 0x000000FF;
			
			//s32 c = (r << 24) | (g << 16) | (b << 8) | (a << 0);
			s32 c = (a << 24) | (r << 16) | (g << 8) | (b << 0);
#endif
			s32 c = img_pixel;
			*pixel = c;
		}
	}
	
	Atom property = XInternAtom(window->display, "_NET_WM_ICON", 0);
	Atom cardinal = XInternAtom(window->display, "CARDINAL", False);
	
	int result = XChangeProperty(window->display, window->window, 
								 property, cardinal, 32, PropModeReplace, 
								 (unsigned char *)data, nelements);
}

#if 0
uint write_cb(char *in, uint size, uint nmemb, char *buffer)
{
	string_appendn(buffer, in, MAX_INPUT_LENGTH);
	return size * nmemb;
}

bool platform_send_http_request(char *url, char *params, char *response_buffer)
{
	string_copyn(response_buffer, "", MAX_INPUT_LENGTH);
	
	char fullurl[200];
	sprintf(fullurl, "https://%s/%s", url, params);
	curl_easy_setopt(curl, CURLOPT_URL,fullurl);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
	curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
	curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, response_buffer);
	CURLcode res = curl_easy_perform(curl);
	if (res != CURLE_OK) return false;
	
	return true;
}

bool platform_get_mac_address(char *buffer, s32 buf_size)
{
	struct ifaddrs *ifaddr=NULL;
    struct ifaddrs *ifa = NULL;
    int i = 0;
	
    if (getifaddrs(&ifaddr) == -1)
    {
		return false;
	}
    else
    {
		for ( ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
		{
			if ( (ifa->ifa_addr) && (ifa->ifa_addr->sa_family == AF_PACKET) )
			{
				struct sockaddr_ll *s = (struct sockaddr_ll*)ifa->ifa_addr;
				for (i=0; i <s->sll_halen; i++)
				{
					buffer += sprintf(buffer, "%02x", (s->sll_addr[i]));
					
					if (i+1!=s->sll_halen)
					{
						buffer += sprintf(buffer, "%c", '-');
					}
				}
				
				freeifaddrs(ifaddr);
				return true;
			}
		}
    }
	
	return false;
}
#endif