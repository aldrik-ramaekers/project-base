#define __load_lib_or_exit(_var, _name) _var = LoadLibraryA(_name); if (!_var) { printf("Failed to load library %s\n", _name); exit(0); }
#define __load_fnc_or_exit(_ptr, _var) IMP_##_ptr = (DEF_##_ptr)GetProcAddress(_var, #_ptr); if ((uintptr_t)NULL == (uintptr_t)IMP_##_ptr) { printf("Failed to load function %s\n", #_ptr); exit(0); }
#define __load_fnc_wgl_or_exit(_ptr) IMP_##_ptr = (DEF_##_ptr)IMP_wglGetProcAddress(#_ptr); if ((uintptr_t)NULL == (uintptr_t)IMP_##_ptr) { printf("Failed to load function %s\n", #_ptr); exit(0); }
#define __load_fnc_wgl(_ptr) IMP_##_ptr = (DEF_##_ptr)IMP_wglGetProcAddress(#_ptr);
#define __def_proc(_return, _name, _params) typedef _return (*DEF_##_name)_params; DEF_##_name IMP_##_name;

//typedef HGLRC (*DEF_wglCreateContext)(HDC); 
//DEF_wglCreateContext IMP_wglCreateContext;
//typedef bool (*DEF_wglShareLists)(HGLRC,HGLRC); 
//DEF_wglShareLists IMP_wglShareLists;

__def_proc( HGLRC, wglCreateContext, (HDC));
__def_proc( bool, wglShareLists, (HGLRC,HGLRC));
__def_proc( bool, wglMakeCurrent, (HDC,HGLRC));
__def_proc( bool, wglDeleteContext, (HGLRC));

__def_proc( PROC, wglGetProcAddress, (LPCSTR));
__def_proc( HDC , wglGetCurrentDC, (void));
__def_proc( GLubyte*, glGetString, (GLenum));

__def_proc( BOOL, wglSwapIntervalEXT, (int));
__def_proc( int, wglGetSwapIntervalEXT, (void));
__def_proc( const char *, wglGetExtensionsStringEXT, (void));
__def_proc( HGLRC, wglCreateContextAttribsARB, (HDC,HGLRC,const int*));


__def_proc( void, glGetBooleanv, (GLenum,GLboolean*));
__def_proc( void, glDepthMask, (GLboolean));
__def_proc( void, glColorMask, (GLboolean,GLboolean,GLboolean,GLboolean));
__def_proc( void, glDepthFunc, (GLenum));
__def_proc( void, glEnable, (GLenum));
__def_proc( void, glAlphaFunc, (GLenum,GLclampf));
__def_proc( void, glBlendFunc, (GLenum,GLenum));
__def_proc( void, glMatrixMode, (GLenum));
__def_proc( void, glLoadIdentity, (void));
__def_proc( void, glViewport, (GLint,GLint,GLsizei,GLsizei));
__def_proc( void, glClearColor, (GLclampf red,GLclampf green,GLclampf blue,GLclampf alpha))
__def_proc( void, glClear, (GLbitfield mask))
__def_proc( void, glRotatef, (GLfloat angle,GLfloat x,GLfloat y,GLfloat z))
__def_proc( void, glBindTexture, (GLenum target,GLuint texture))
__def_proc( void, glBegin, (GLenum mode))
__def_proc( void, glColor4f, (GLfloat red,GLfloat green,GLfloat blue,GLfloat alpha))
__def_proc( void, glTexCoord2i, (GLint s,GLint t))
__def_proc( void, glTexCoord2f, (GLfloat s,GLfloat t))
__def_proc( void, glVertex3i, (GLint x,GLint y,GLint z))
__def_proc( void, glVertex2f, (GLfloat, GLfloat))
__def_proc( void, glEnd, (void))
__def_proc( void, glDisable, (GLenum cap))
__def_proc( void, glPixelStorei, (GLenum pname,GLint param))
__def_proc( void, glScissor, (GLint x,GLint y,GLsizei width,GLsizei height))
__def_proc( void, glGetIntegerv, (GLenum pname,GLint *params))
__def_proc( void, glGenTextures, (GLsizei n,GLuint *textures))
__def_proc( void, glTexImage2D, (GLenum target,GLint level,GLint internalformat,GLsizei width,GLsizei height,GLint border,GLenum format,GLenum type,const GLvoid *pixels))
__def_proc( void, glTexParameteri, (GLenum target,GLenum pname,GLint param))
__def_proc( void, glDeleteTextures, (GLsizei n,const GLuint *textures))
__def_proc( void, glTranslatef, (GLfloat x,GLfloat y,GLfloat z))
__def_proc( void, glOrtho, (GLdouble left,GLdouble right,GLdouble bottom,GLdouble top,GLdouble zNear,GLdouble zFar))
__def_proc( void, glFinish, (void))
__def_proc( void, glLineWidth, (GLfloat))
__def_proc( void, glPushMatrix, ())
__def_proc( void, glPopMatrix, ())

__def_proc( bool, GetSaveFileNameA, (LPOPENFILENAMEA))
__def_proc( bool, GetOpenFileNameA, (LPOPENFILENAMEA))

__def_proc( int, GetDeviceCaps, (HDC hdc, int index))
__def_proc( HPEN, CreatePen, (int iStyle,int cWidth,COLORREF color))
__def_proc( HBRUSH, CreateBrushIndirect, (CONST LOGBRUSH *plbrush))
__def_proc( HGDIOBJ, SelectObject, (HDC hdc,HGDIOBJ h))
__def_proc( bool, Rectangle, (HDC hdc,int left,int top,int right,int bottom))
__def_proc( bool, DeleteObject, (HGDIOBJ ho))
__def_proc( int, ChoosePixelFormat, (HDC hdc,CONST PIXELFORMATDESCRIPTOR *ppfd))
__def_proc( int, DescribePixelFormat, (HDC hdc,int iPixelFormat,UINT nBytes,LPPIXELFORMATDESCRIPTOR ppfd))
__def_proc( bool, SetPixelFormat, (HDC hdc,int format,CONST PIXELFORMATDESCRIPTOR *ppfd))
__def_proc( int, StretchDIBits, (HDC ,int ,int ,int ,int ,int ,int ,int ,int ,CONST VOID *,CONST BITMAPINFO *,UINT ,DWORD ))
__def_proc( bool, SwapBuffers, (HDC))

__def_proc(bool, PlaySoundA, (LPCSTR, HMODULE, DWORD))

void _lib_loader_init()
{
    HMODULE libOpengl32 = 0;
    HMODULE libComdlg32 = 0;
    HMODULE libGdi32 = 0;
	HMODULE libWinmm = 0;

    __load_lib_or_exit(libOpengl32, "opengl32");
    __load_fnc_or_exit(wglCreateContext, libOpengl32);
    __load_fnc_or_exit(wglShareLists, libOpengl32);
    __load_fnc_or_exit(wglMakeCurrent, libOpengl32);
    __load_fnc_or_exit(wglDeleteContext, libOpengl32);
    __load_fnc_or_exit(wglGetProcAddress, libOpengl32);
	__load_fnc_or_exit(wglGetCurrentDC, libOpengl32);
	__load_fnc_or_exit(glGetString, libOpengl32);

	__load_fnc_or_exit(glGetBooleanv, libOpengl32);
    __load_fnc_or_exit(glDepthMask, libOpengl32);
    __load_fnc_or_exit(glColorMask, libOpengl32);
    __load_fnc_or_exit(glDepthFunc, libOpengl32);
    __load_fnc_or_exit(glEnable, libOpengl32);
    __load_fnc_or_exit(glAlphaFunc, libOpengl32);
    __load_fnc_or_exit(glBlendFunc, libOpengl32);
    __load_fnc_or_exit(glMatrixMode, libOpengl32);
    __load_fnc_or_exit(glLoadIdentity, libOpengl32);
    __load_fnc_or_exit(glViewport, libOpengl32);
    __load_fnc_or_exit(glClearColor, libOpengl32);
    __load_fnc_or_exit(glClear, libOpengl32);
    __load_fnc_or_exit(glRotatef, libOpengl32);
    __load_fnc_or_exit(glBindTexture, libOpengl32);
    __load_fnc_or_exit(glBegin, libOpengl32);
    __load_fnc_or_exit(glColor4f, libOpengl32);
    __load_fnc_or_exit(glTexCoord2i, libOpengl32);
	__load_fnc_or_exit(glTexCoord2f, libOpengl32);
    __load_fnc_or_exit(glVertex3i, libOpengl32);
	__load_fnc_or_exit(glVertex2f, libOpengl32);
    __load_fnc_or_exit(glEnd, libOpengl32);
    __load_fnc_or_exit(glDisable, libOpengl32);
    __load_fnc_or_exit(glPixelStorei, libOpengl32);
    __load_fnc_or_exit(glScissor, libOpengl32);
    __load_fnc_or_exit(glGetIntegerv, libOpengl32);
    __load_fnc_or_exit(glGenTextures, libOpengl32);
    __load_fnc_or_exit(glTexImage2D, libOpengl32);
    __load_fnc_or_exit(glTexParameteri, libOpengl32);
    __load_fnc_or_exit(glDeleteTextures, libOpengl32);
    __load_fnc_or_exit(glTranslatef, libOpengl32);
    __load_fnc_or_exit(glOrtho, libOpengl32);
    __load_fnc_or_exit(glFinish, libOpengl32);
    __load_fnc_or_exit(glLineWidth, libOpengl32);
    __load_fnc_or_exit(glPushMatrix, libOpengl32);
    __load_fnc_or_exit(glPopMatrix, libOpengl32);

    __load_lib_or_exit(libComdlg32, "comdlg32");
    __load_fnc_or_exit(GetSaveFileNameA, libComdlg32);
    __load_fnc_or_exit(GetOpenFileNameA, libComdlg32);

    __load_lib_or_exit(libGdi32, "gdi32");
    __load_fnc_or_exit(CreatePen, libGdi32);
    __load_fnc_or_exit(CreateBrushIndirect, libGdi32);
    __load_fnc_or_exit(SelectObject, libGdi32);
    __load_fnc_or_exit(Rectangle, libGdi32);
    __load_fnc_or_exit(DeleteObject, libGdi32);
    __load_fnc_or_exit(ChoosePixelFormat, libGdi32);
    __load_fnc_or_exit(DescribePixelFormat, libGdi32);
    __load_fnc_or_exit(SetPixelFormat, libGdi32);
    __load_fnc_or_exit(StretchDIBits, libGdi32);
    __load_fnc_or_exit(SwapBuffers, libGdi32);
	__load_fnc_or_exit(GetDeviceCaps, libGdi32);

	__load_lib_or_exit(libWinmm, "Winmm");
	__load_fnc_or_exit(PlaySoundA, libWinmm);
}