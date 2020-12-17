#include <dlfcn.h>

void* libX11 = 0;

#define __load_lib_or_exit(_var, _name) _var = dlopen(_name, RTLD_LAZY); if (!_var) { printf("Failed to load %s\n", _name); exit(0); }
#define __load_fnc_or_exit(_ptr, _var) IMP_##_ptr = (DEF_##_ptr)dlsym(_var, #_ptr); if ((uintptr_t)NULL == (uintptr_t)IMP_##_ptr) { printf("Failed to load %s\n", #_ptr); exit(0); }
#define __def_proc(_return, _name, _params) typedef _return (*DEF_##_name)_params; DEF_##_name IMP_##_name;

__def_proc( HGLRC, wglCreateContext, (HDC));

void _lib_loader_init()
{
    __load_lib_or_exit(libX11, "x11");
    __load_fnc_or_exit(wglCreateContext, libX11);
}

void _lib_loader_destroy()
{
    dlclose(libX11);
}