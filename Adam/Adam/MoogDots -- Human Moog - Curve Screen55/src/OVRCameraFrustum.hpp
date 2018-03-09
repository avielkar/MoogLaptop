#ifndef OVRCAMERAFRUSTUM_HPP
#define OVRCAMERAFRUSTUM_HPP
#include "OpenGL.hpp"
#include "OVR_CAPI.h"
#include "OVR_CAPI_GL.h"

/*
 * Rendering class for IR tracking camera bounds (frustum)
 */
class OVRCameraFrustum
{
public:
    ~OVRCameraFrustum();
    void Recalculate(ovrSession session);
    void OnRender();
private:
    GLuint m_vertexArray;
    GLuint m_vertexBuffers[3];
};

#endif