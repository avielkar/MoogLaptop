#ifndef APPLICATION_INCLUDED
#define APPLICATION_INCLUDED

#include "InputHandlers.hpp"
#include "renderer/OpenGL.hpp"
#include "renderer/Texture.hpp"

/*
 * main application 
 */
class Application
{
public:
    Application::Application() : m_running(true)
    {
    }

    ~Application();

    void OnStart();
    void OnRender();

    inline bool Running() const  { return m_running; }
    inline void Terminate()      { m_running = false; }

    void OnKeyPress(KeyCode key);
private:
    bool m_running;

    // rendered quad data
    GLuint m_vertexBuffer;
    GLuint m_colorBuffer;
    GLuint m_texcoordBuffer;
    GLuint m_vertexArray;
    Texture *m_texture;

};

#endif
