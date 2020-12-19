#include <dlfcn.h>
#include <X11/extensions/Xrandr.h>

void* libX11 = 0;
void* libXRandr = 0;

#define __load_lib(_var, _name) _var = dlopen(_name, RTLD_LAZY);
#define __load_lib_or_exit(_var, _name) _var = dlopen(_name, RTLD_LAZY); if (!_var) { printf("Failed to load library %s\n", _name); exit(0); }
#define __load_fnc_or_exit(_ptr, _var) IMP_##_ptr = (DEF_##_ptr)dlsym(_var, #_ptr); if ((uintptr_t)NULL == (uintptr_t)IMP_##_ptr) { printf("Failed to load function %s\n", #_ptr); exit(0); }
#define __def_proc(_return, _name, _params) typedef _return (*DEF_##_name)_params; DEF_##_name IMP_##_name;

__def_proc( Bool, glXMakeCurrent, (Display *dpy, GLXDrawable drawable, GLXContext ctx))
//__def_proc( XVisualInfo*, glXChooseVisual, ( Display *dpy, int screen, int *attribList))
__def_proc( GLXContext, glXCreateContext, ( Display *dpy, XVisualInfo *vis, GLXContext shareList, Bool direct))
__def_proc( void, glXDestroyContext, (Display *dpy, GLXContext ctx ))
__def_proc( GLXFBConfig*, glXChooseFBConfig, ( Display *dpy, int screen, const int *attribList, int *nitems))
__def_proc( int, glXGetFBConfigAttrib, (Display *dpy, GLXFBConfig config, int attribute, int *value))
__def_proc( XVisualInfo*, glXGetVisualFromFBConfig, (Display *dpy, GLXFBConfig config))
__def_proc( void, glXSwapBuffers, (Display *dpy, GLXDrawable drawable))
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
__def_proc( void, glVertex3i, (GLint x,GLint y,GLint z))
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
__def_proc( void, glViewport, (GLint x, GLint y, GLsizei width, GLsizei height))

__def_proc( XRRCrtcInfo*, XRRGetCrtcInfo, (Display *dpy, XRRScreenResources *resources, RRCrtc crtc))
__def_proc( void, XRRFreeCrtcInfo, (XRRCrtcInfo *crtcInfo))
__def_proc( void, XRRFreeScreenResources, (XRRScreenResources *resources))
__def_proc( XRRScreenResources*, XRRGetScreenResources, (Display *dpy, Window window))

void _lib_loader_init()
{
    __load_lib(libX11, "libGL.so");
    if (!libX11) {
        __load_lib_or_exit(libX11, "libGL.so.1");
    }
    __load_fnc_or_exit(glXMakeCurrent, libX11);
    __load_fnc_or_exit(glXCreateContext, libX11);
    __load_fnc_or_exit(glXDestroyContext, libX11);
    __load_fnc_or_exit(glXChooseFBConfig, libX11);
    __load_fnc_or_exit(glXGetFBConfigAttrib, libX11);
    __load_fnc_or_exit(glXGetVisualFromFBConfig, libX11);
    __load_fnc_or_exit(glXSwapBuffers, libX11);
    __load_fnc_or_exit(glDepthMask, libX11);
    __load_fnc_or_exit(glColorMask, libX11);
    __load_fnc_or_exit(glDepthFunc, libX11);
    __load_fnc_or_exit(glEnable, libX11);
    __load_fnc_or_exit(glAlphaFunc, libX11);
    __load_fnc_or_exit(glBlendFunc, libX11);
    __load_fnc_or_exit(glMatrixMode, libX11);
    __load_fnc_or_exit(glLoadIdentity, libX11);
    __load_fnc_or_exit(glViewport, libX11);
    __load_fnc_or_exit(glClearColor, libX11);
    __load_fnc_or_exit(glClear, libX11);
    __load_fnc_or_exit(glRotatef, libX11);
    __load_fnc_or_exit(glBindTexture, libX11);
    __load_fnc_or_exit(glBegin, libX11);
    __load_fnc_or_exit(glColor4f, libX11);
    __load_fnc_or_exit(glTexCoord2i, libX11);
    __load_fnc_or_exit(glVertex3i, libX11);
    __load_fnc_or_exit(glEnd, libX11);
    __load_fnc_or_exit(glDisable, libX11);
    __load_fnc_or_exit(glPixelStorei, libX11);
    __load_fnc_or_exit(glScissor, libX11);
    __load_fnc_or_exit(glGetIntegerv, libX11);
    __load_fnc_or_exit(glGenTextures, libX11);
    __load_fnc_or_exit(glTexImage2D, libX11);
    __load_fnc_or_exit(glTexParameteri, libX11);
    __load_fnc_or_exit(glDeleteTextures, libX11);
    __load_fnc_or_exit(glTranslatef, libX11);
    __load_fnc_or_exit(glOrtho, libX11);
    __load_fnc_or_exit(glViewport, libX11);

    __load_lib_or_exit(libXRandr, "libXrandr.so");
    __load_fnc_or_exit(XRRGetCrtcInfo, libXRandr);
    __load_fnc_or_exit(XRRFreeCrtcInfo, libXRandr);
    __load_fnc_or_exit(XRRFreeScreenResources, libXRandr);
    __load_fnc_or_exit(XRRGetScreenResources, libXRandr);
}

void _lib_loader_destroy()
{
    dlclose(libX11);
    dlclose(libXRandr);
}