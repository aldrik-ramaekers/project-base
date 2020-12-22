#include <dlfcn.h>
#include <X11/extensions/Xrandr.h>

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

__def_proc( int, pthread_create, (pthread_t *__restrict __newthread, const pthread_attr_t *__restrict __attr, void *(*__start_routine) (void *), void *__restrict __arg))
__def_proc( void, pthread_exit, (void *__retval))
__def_proc( int, pthread_join, (pthread_t __th, void **__thread_return))
__def_proc( int, pthread_tryjoin_np, (pthread_t __th, void **__thread_return))
__def_proc( int, pthread_detach, (pthread_t __th))
__def_proc( int, pthread_mutexattr_init, (pthread_mutexattr_t *__attr))
__def_proc( int, pthread_mutexattr_destroy, (pthread_mutexattr_t *__attr))
__def_proc( int, pthread_mutexattr_settype, (pthread_mutexattr_t *__attr, int __kind))

__def_proc( int, pthread_attr_init, (pthread_attr_t *__attr))
__def_proc( int, pthread_attr_destroy, (pthread_attr_t *__attr))
__def_proc( int, pthread_cancel, (pthread_t __th))
__def_proc( int, pthread_mutex_init, (pthread_mutex_t *__mutex, const pthread_mutexattr_t *__mutexattr))
__def_proc( int, pthread_mutex_lock, (pthread_mutex_t *__mutex))
__def_proc( int, pthread_mutex_trylock, (pthread_mutex_t *__mutex))
__def_proc( int, pthread_mutex_unlock, (pthread_mutex_t *__mutex))
__def_proc( int, pthread_mutex_destroy, (pthread_mutex_t *__mutex))

void _lib_loader_init()
{
    void* libGL = 0;
    void* libXRandr = 0;
    void* libpthread = 0;

    __load_lib(libGL, "libGL.so");
    if (!libGL) {
        __load_lib_or_exit(libGL, "libGL.so.1");
    }
    __load_fnc_or_exit(glXMakeCurrent, libGL);
    __load_fnc_or_exit(glXCreateContext, libGL);
    __load_fnc_or_exit(glXDestroyContext, libGL);
    __load_fnc_or_exit(glXChooseFBConfig, libGL);
    __load_fnc_or_exit(glXGetFBConfigAttrib, libGL);
    __load_fnc_or_exit(glXGetVisualFromFBConfig, libGL);
    __load_fnc_or_exit(glXSwapBuffers, libGL);
    __load_fnc_or_exit(glDepthMask, libGL);
    __load_fnc_or_exit(glColorMask, libGL);
    __load_fnc_or_exit(glDepthFunc, libGL);
    __load_fnc_or_exit(glEnable, libGL);
    __load_fnc_or_exit(glAlphaFunc, libGL);
    __load_fnc_or_exit(glBlendFunc, libGL);
    __load_fnc_or_exit(glMatrixMode, libGL);
    __load_fnc_or_exit(glLoadIdentity, libGL);
    __load_fnc_or_exit(glViewport, libGL);
    __load_fnc_or_exit(glClearColor, libGL);
    __load_fnc_or_exit(glClear, libGL);
    __load_fnc_or_exit(glRotatef, libGL);
    __load_fnc_or_exit(glBindTexture, libGL);
    __load_fnc_or_exit(glBegin, libGL);
    __load_fnc_or_exit(glColor4f, libGL);
    __load_fnc_or_exit(glTexCoord2i, libGL);
    __load_fnc_or_exit(glVertex3i, libGL);
    __load_fnc_or_exit(glEnd, libGL);
    __load_fnc_or_exit(glDisable, libGL);
    __load_fnc_or_exit(glPixelStorei, libGL);
    __load_fnc_or_exit(glScissor, libGL);
    __load_fnc_or_exit(glGetIntegerv, libGL);
    __load_fnc_or_exit(glGenTextures, libGL);
    __load_fnc_or_exit(glTexImage2D, libGL);
    __load_fnc_or_exit(glTexParameteri, libGL);
    __load_fnc_or_exit(glDeleteTextures, libGL);
    __load_fnc_or_exit(glTranslatef, libGL);
    __load_fnc_or_exit(glOrtho, libGL);
    __load_fnc_or_exit(glViewport, libGL);

    __load_lib_or_exit(libXRandr, "libXrandr.so");
    __load_fnc_or_exit(XRRGetCrtcInfo, libXRandr);
    __load_fnc_or_exit(XRRFreeCrtcInfo, libXRandr);
    __load_fnc_or_exit(XRRFreeScreenResources, libXRandr);
    __load_fnc_or_exit(XRRGetScreenResources, libXRandr);

    __load_lib(libpthread, "libpthread.so");
    if (!libpthread) {
        __load_lib_or_exit(libpthread, "libpthread.so.0");
    }
    __load_fnc_or_exit(pthread_create, libpthread);
    __load_fnc_or_exit(pthread_exit, libpthread);
    __load_fnc_or_exit(pthread_join, libpthread);
    __load_fnc_or_exit(pthread_tryjoin_np, libpthread);
    __load_fnc_or_exit(pthread_detach, libpthread);
    __load_fnc_or_exit(pthread_mutexattr_init, libpthread);
    __load_fnc_or_exit(pthread_mutexattr_destroy, libpthread);
    __load_fnc_or_exit(pthread_mutexattr_settype, libpthread);
    __load_fnc_or_exit(pthread_attr_init, libpthread);
    __load_fnc_or_exit(pthread_attr_destroy, libpthread);
    __load_fnc_or_exit(pthread_cancel, libpthread);
    __load_fnc_or_exit(pthread_mutex_init, libpthread);
    __load_fnc_or_exit(pthread_mutex_lock, libpthread);
    __load_fnc_or_exit(pthread_mutex_trylock, libpthread);
    __load_fnc_or_exit(pthread_mutex_unlock, libpthread);
    __load_fnc_or_exit(pthread_mutex_destroy, libpthread);
}