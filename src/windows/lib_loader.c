HMODULE libOpengl32 = 0;

#define __load_lib_or_exit(_var, _name) _var = LoadLibraryA(_name); if (!_var) { printf("Failed to load %s\n", _name); exit(0); }
#define __load_fnc_or_exit(_ptr, _var, _name) IMP_##_ptr = (DEF_##_ptr)GetProcAddress(_var, _name); if ((uintptr_t)NULL == (uintptr_t)IMP_##_ptr) { printf("Failed to load %s\n", _name); exit(0); }
#define __def_proc(_return, _name, _params) typedef _return APIENTRY (*DEF_##_name)_params; DEF_##_name IMP_##_name = 0;

__def_proc(HGLRC, wglCreateContext, (HDC));
__def_proc(WINBOOL, wglShareLists, (HGLRC,HGLRC));
__def_proc(WINBOOL, wglMakeCurrent, (HDC,HGLRC));
__def_proc(WINBOOL, wglDeleteContext, (HGLRC));
__def_proc(void, glDepthMask, (GLboolean));
__def_proc(void, glColorMask, (GLboolean,GLboolean,GLboolean,GLboolean));
__def_proc(void, glDepthFunc, (GLenum));
__def_proc(void, glEnable, (GLenum));
__def_proc(void, glAlphaFunc, (GLenum,GLclampf));
__def_proc(void, glBlendFunc, (GLenum,GLenum));
__def_proc(void, glMatrixMode, (GLenum));
__def_proc(void, glLoadIdentity, (void));
__def_proc(void, glViewport, (GLint,GLint,GLsizei,GLsizei));

void _lib_loader_init()
{
    __load_lib_or_exit(libOpengl32, "opengl32");
    __load_fnc_or_exit(wglCreateContext, libOpengl32, "wglCreateContext");
    __load_fnc_or_exit(wglShareLists, libOpengl32, "wglShareLists");
    __load_fnc_or_exit(wglMakeCurrent, libOpengl32, "wglMakeCurrent");
    __load_fnc_or_exit(wglDeleteContext, libOpengl32, "wglDeleteContext");
    __load_fnc_or_exit(glDepthMask, libOpengl32, "glDepthMask");
    __load_fnc_or_exit(glColorMask, libOpengl32, "glColorMask");
    __load_fnc_or_exit(glDepthFunc, libOpengl32, "glDepthFunc");
    __load_fnc_or_exit(glEnable, libOpengl32, "glEnable");
    __load_fnc_or_exit(glAlphaFunc, libOpengl32, "glAlphaFunc");
    __load_fnc_or_exit(glBlendFunc, libOpengl32, "glBlendFunc");
    __load_fnc_or_exit(glMatrixMode, libOpengl32, "glMatrixMode");
    __load_fnc_or_exit(glLoadIdentity, libOpengl32, "glLoadIdentity");
    __load_fnc_or_exit(glViewport, libOpengl32, "glViewport");
}

void _lib_loader_destroy()
{
    // Libraries will be fried automatically on program exit.
    // https://docs.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-loadlibrarya#remarks

    FreeLibrary(libOpengl32);
}