#include "camera.h"
#include "mesh.h"
#include "ray.h"
#include "scene.h"
#include "sphere.h"
#include "vec3.h"
#include "renderer.h"
#include <cassert>
#include <iostream>
#include <memory>
#include <random>
#include "renderer.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stb_image.h>

#include <thread>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
using FLOAT = double;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
using vec3f  = vec3<FLOAT>;
using rayf   = ray<FLOAT>;
using hitf   = hit<FLOAT>;
using scenef = scene<FLOAT>;
using meshf  = mesh<FLOAT>;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
static void buildCornellBoxScene( scenef &world )
{
    material redDiffuse{{1.0f, 0.0f, 0.0f, 1.0f}};
    material blueDiffuse{{1.0f, 0.0f, 1.0f, 1.0f}};
    material greenDiffuse{{1.0f, 1.0f, 0.0f, 1.0f}};
    material whiteEmissive{{1.0f, 1.0f, 1.0f, 1.0f}};
    whiteEmissive.setEmissive( 50.0f );

    auto cornellbox = new meshf( "/home/nebula/code/path_tracer/cornellbox.obj" );
    cornellbox->transform(mat44<FLOAT>::makeRotation(90, 2));
    world
        << new sphere<FLOAT>( {0.4f, 0.0f, -0.6f}, 0.75f * 0.5f, redDiffuse )
        << new sphere<FLOAT>( {-0.4f, 0.5f, -0.6f}, 0.75f * 0.5f, blueDiffuse )
        << new sphere<FLOAT>(
               {-0.4f, -0.5f, -0.6f}, 0.75f * 0.5f, greenDiffuse )
        << new sphere<FLOAT>( {0.0f, 0.0f, 1.25f}, 0.75f * 0.5f, whiteEmissive )
        << cornellbox;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class RenderResult
{
public:
    RenderResult( int width, int height ) : p_width( width ), p_height( height )
    {
    }

    std::vector<unsigned char> &ImageBuffer() { return p_imgBuffer; }
    const std::vector<unsigned char> &ImageBuffer() const
    {
        return p_imgBuffer;
    }

    int Width() const { return p_width; }
    int Height() const { return p_height; }

private:
    std::vector<unsigned char> p_imgBuffer;
    int                        p_width  = 0;
    int                        p_height = 0;
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class RenderJob
{
public:
    RenderJob( const renderparams &params, RenderResult &result )
        : rp( params ), renderDevice( cam, rp ), renderResult( result )
    {
        buildCornellBoxScene( world );
    }

    void do_it() { renderDevice.render( world, renderResult.ImageBuffer() ); }

    const RenderResult &GetResult() const { return renderResult; }

    void operator()() { do_it(); }

private:
    renderparams  rp{ 256, 256, 4, 32 };
    camera<FLOAT> cam{ { 2.472f, 0, 0 }, { 0, 0, 0 }, 60 };
    scenef        world;

    renderer<FLOAT> renderDevice;
    RenderResult   &renderResult;
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
static GLuint CreateTexture(const RenderResult &result )
{
    GLuint textureID = 0u;
    glGenTextures( 1, &textureID );
    glBindTexture( GL_TEXTURE_2D, textureID );

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );

    glTexImage2D( GL_TEXTURE_2D,
                  0,
                  GL_RGBA8,
                  result.Width(),
                  result.Height(),
                  0,
                  GL_RGBA,
                  GL_UNSIGNED_BYTE,
                  result.ImageBuffer().data() );
    glBindTexture( GL_TEXTURE_2D, 0 );
    return textureID;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
int main()
{
    if ( !glfwInit() )
        return 1;

    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

    auto window =
        glfwCreateWindow( 1920, 1080, "Pathtracer", nullptr, nullptr );

    if ( !window )
    {
        glfwTerminate();
        return 1;
    }

    glfwSwapInterval( 1 );
    glfwMakeContextCurrent( window );

    if ( GLEW_OK == glewInit() )
        std::cout << glGetString( GL_VERSION ) << std::endl;
    else
        std::cout << "Error initializing opengl context" << std::endl;

    // Callbacks
    /*
    glfwSetKeyCallback( window, KeyCallback );
    glfwSetCursorPosCallback( window, MouseMoveCallback );
    glfwSetMouseButtonCallback( window, MouseButtonCallback );
    glfwSetScrollCallback( window, MouseScrollCallback );
    glfwSetFramebufferSizeCallback( window, WindowResizeCallback );
    */

       // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable
    // Keyboard Controls io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; //
    // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL( window, true );
    const char *glsl_version = "#version 130";
    ImGui_ImplOpenGL3_Init( glsl_version );

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can
    // also load multiple fonts and use ImGui::PushFont()/PopFont() to select
    // them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you
    // need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please
    // handle those errors in your application (e.g. use an assertion, or
    // display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and
    // stored into a texture when calling
    // ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame
    // below will call.
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use
    // Freetype for higher quality font rendering.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string
    // literal you need to write a double backslash \\ !
    // io.Fonts->AddFontDefault();
    // io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
    // io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    // io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    // io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    // ImFont* font =
    // io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f,
    // NULL, io.Fonts->GetGlyphRangesJapanese()); IM_ASSERT(font != NULL);

    GLuint textureID = 0;

    double updateTime = 0.0f;

    renderparams rp{ 256, 256, 4, 32 };
    RenderResult result( rp.width(), rp.height() );
    std::thread  renderThread( RenderJob( rp, result ) );

    while ( !glfwWindowShouldClose( window ) )
    {
        double time = glfwGetTime();
        if ( time - updateTime > 1.0 )
        {
            updateTime = time;

            if ( textureID != 0 )
            {
                glDeleteTextures( 1, &textureID );
                textureID = 0;
            }

            textureID  = CreateTexture( result );
        }

        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 1. Show the big demo window (Most of the sample code is in
        // ImGui::ShowDemoWindow()! You can browse its code to learn more about
        // Dear ImGui!).
        bool show_demo_window = false;
        if ( show_demo_window )
            ImGui::ShowDemoWindow( &show_demo_window );

        if ( textureID != 0 )
        {
            ImGui::Begin( "OpenGL Texture Text" );
            // ImGui::Text( "pointer = %p", textureID );
            // ImGui::Image( (void *)(intptr_t)textureID, ImVec2( 540, 960 ) );
            ImGui::Image( (void *)(intptr_t)textureID, ImVec2( 256, 256 ) );
            ImGui::End(); 
        }

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize( window, &display_w, &display_h );
        glViewport( 0, 0, display_w, display_h );
        glClearColor( 1.0, 1.0, 1.0, 1.0 );
        glClear( GL_COLOR_BUFFER_BIT );
        ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );

        glfwSwapBuffers( window );
    }

    if ( window )
        glfwDestroyWindow( window );

    renderThread.join();
    return 0;
}
