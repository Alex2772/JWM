#include <algorithm>
#include <iostream>
#include <jni.h>
#include "impl/Library.hh"
#include "impl/RefCounted.hh"
#include "WindowX11.hh"
#include <GL/gl.h>

namespace jwm {

class LayerGL: public RefCounted {
public:
    WindowX11* fWindow;
    GLXContext _context = nullptr;

    LayerGL() = default;
    ~LayerGL() = default;

    void attach(WindowX11* window) {
        fWindow = jwm::ref(window);

        if (_context == nullptr) {
            _context = glXCreateContext(window->_windowManager.getDisplay(),
                                        window->_windowManager.getVisualInfo(),
                                        nullptr,
                                        true);
                                
        }
        
        glXMakeCurrent(window->_windowManager.getDisplay(),
                       window->_x11Window,
                       _context);

        // this forces vsync      
        using glXSwapIntervalEXT_t = void (*)(Display*, GLXDrawable, int);   
        glXSwapIntervalEXT_t _glXSwapIntervalEXT = reinterpret_cast<glXSwapIntervalEXT_t>(glXGetProcAddress(reinterpret_cast<const GLubyte*>("glXSwapIntervalEXT")));

        if (_glXSwapIntervalEXT) {
            _glXSwapIntervalEXT(window->_windowManager.getDisplay(),
                                window->_x11Window,
                                1);
        }
    }

    void resize(int width, int height) {
        glClearStencil(0);
        glClearColor(0, 0, 0, 255);
        glStencilMask(0xffffffff);
        glClear(GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        glViewport(0, 0, width, height);
    }

    void swapBuffers() {
        glXSwapBuffers(fWindow->_windowManager.getDisplay(), fWindow->_x11Window);
    }

    void close() {
        jwm::unref(&fWindow);
    }
};

} // namespace jwm

// JNI

extern "C" JNIEXPORT jlong JNICALL Java_org_jetbrains_jwm_LayerGL__1nMake
  (JNIEnv* env, jclass jclass) {
    jwm::LayerGL* instance = new jwm::LayerGL();
    return reinterpret_cast<jlong>(instance);
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_LayerGL__1nAttach
  (JNIEnv* env, jobject obj, jobject windowObj) {
    jwm::LayerGL* instance = reinterpret_cast<jwm::LayerGL*>(jwm::classes::Native::fromJava(env, obj));
    jwm::WindowX11* window = reinterpret_cast<jwm::WindowX11*>(jwm::classes::Native::fromJava(env, windowObj));
    instance->attach(window);
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_LayerGL__1nReconfigure
  (JNIEnv* env, jobject obj, jint width, jint height) {
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_LayerGL__1nResize
  (JNIEnv* env, jobject obj, jint width, jint height) {
    jwm::LayerGL* instance = reinterpret_cast<jwm::LayerGL*>(jwm::classes::Native::fromJava(env, obj));
    instance->resize(width, height);
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_LayerGL__1nSwapBuffers
  (JNIEnv* env, jobject obj) {
    jwm::LayerGL* instance = reinterpret_cast<jwm::LayerGL*>(jwm::classes::Native::fromJava(env, obj));
    instance->swapBuffers();
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_LayerGL__1nClose
  (JNIEnv* env, jobject obj) {
    jwm::LayerGL* instance = reinterpret_cast<jwm::LayerGL*>(jwm::classes::Native::fromJava(env, obj));
    instance->close();
}