//
// Created by 泓清 on 2022/8/26.
//

//由Tomatosauce移植
//由Tomatosauce移植
//由Tomatosauce移植

#include <draw.h>
#include "custom.hpp"
#include "font.hpp"

// Var
void *handle;// 动态库方案
EGLDisplay display = EGL_NO_DISPLAY;
EGLConfig config;
EGLSurface surface = EGL_NO_SURFACE;
ANativeWindow *native_window;
ANativeWindow *(*createNativeWindow)(const char *surface_name ,uint32_t screen_width ,uint32_t screen_height);
EGLContext context = EGL_NO_CONTEXT;

Screen full_screen;
int Orientation = 0;
int screen_x = 0, screen_y = 0;
int init_screen_x = 0, init_screen_y = 0;
bool g_Initialized = false;

string exec(string command) {
    char buffer[128];
    string result = "";
    // Open pipe to file
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) {
        return "popen failed!";
    }
    // read till end of process:
    while (!feof(pipe)) {
        // use buffer to read and add to result
        if (fgets(buffer, 128, pipe) != nullptr){
            result += buffer;
        }
    }
    pclose(pipe);
    return result;
}

int init_egl(int _screen_x,int _screen_y, bool log){
    void* sy = get_createNativeWindow(); // 适配10-12安卓版本
    createNativeWindow = (ANativeWindow *(*)(const char *, uint32_t, uint32_t))(sy);
    native_window = createNativeWindow("Ssage",_screen_x, _screen_y);
    ANativeWindow_acquire(native_window);
    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (display == EGL_NO_DISPLAY) {
        printf("eglGetDisplay error=%u\n", glGetError());
        return -1;
    }
    if(log){
        printf("eglGetDisplay ok\n");
    }
    if (eglInitialize(display, 0, 0) != EGL_TRUE) {
        printf("eglInitialize error=%u\n", glGetError());
        return -1;
    }
    if(log){
        printf("eglInitialize ok\n");
    }
    EGLint num_config = 0;
    const EGLint attribList[] = {
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL_BLUE_SIZE, 5,   //-->delete
            EGL_GREEN_SIZE, 6,  //-->delete
            EGL_RED_SIZE, 5,    //-->delete
            EGL_BUFFER_SIZE, 32,  //-->new field
            EGL_DEPTH_SIZE, 16,
            EGL_STENCIL_SIZE, 8,
            EGL_NONE
    };
    if (eglChooseConfig(display, attribList, nullptr, 0, &num_config) != EGL_TRUE) {
        printf("eglChooseConfig  error=%u\n", glGetError());
        return -1;
    }
    if(log){
        printf("num_config=%d\n", num_config);
    }
    if (!eglChooseConfig(display, attribList, &config, 1, &num_config)) {
        printf("eglChooseConfig  error=%u\n", glGetError());
        return -1;
    }
    if(log){
        printf("eglChooseConfig ok\n");
    }
    EGLint egl_format;
    eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &egl_format);
    ANativeWindow_setBuffersGeometry(native_window, 0, 0, egl_format);
    const EGLint attrib_list[] = {EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE};
    context = eglCreateContext(display, config, EGL_NO_CONTEXT, attrib_list);
    if (context == EGL_NO_CONTEXT) {
        printf("eglCreateContext  error = %u\n", glGetError());
        return -1;
    }
    if(log){
        printf("eglCreateContext ok\n");
    }
    surface = eglCreateWindowSurface(display, config, native_window, nullptr);
    if (surface == EGL_NO_SURFACE) {
        printf("eglCreateWindowSurface  error = %u\n", glGetError());
        return -1;
    }
    if(log){
        printf("eglCreateWindowSurface ok\n");
    }
    if (!eglMakeCurrent(display, surface, surface, context)) {
        printf("eglMakeCurrent  error = %u\n", glGetError());
        return -1;
    }
    if(log){
        printf("eglMakeCurrent ok\n");
    }
    return 1;
}

void screen_config(){
    std::string window_size = exec("wm size");
    sscanf(window_size.c_str(),"Physical size: %dx%d",&screen_x,&screen_y);
    full_screen.ScreenX = screen_x;
    full_screen.ScreenY = screen_y;
    std::thread *orithread = new std::thread([&] {
        while(true){
            Orientation = atoi(exec("dumpsys input | grep SurfaceOrientation | awk '{print $2}' | head -n 1").c_str());
            if(Orientation == 0 || Orientation == 2){
                screen_x = full_screen.ScreenX;
                screen_y = full_screen.ScreenY;
            }
            if(Orientation == 1 || Orientation == 3){
                screen_x = full_screen.ScreenY;
                screen_y = full_screen.ScreenX;
            }
            std::this_thread::sleep_for(0.5s);
        }
    });
    orithread->detach();
}

void ImGui_init(){
    if (g_Initialized){
        return;
    }
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = NULL;
    ImGui::StyleColorsDark();
    ImGui_ImplAndroid_Init(native_window);
    ImGui_ImplOpenGL3_Init("#version 300 es");
        
    io.Fonts->AddFontFromMemoryTTF((void*)font_binary, sizeof(font_binary), 20.f);
    io.Fonts->AddFontFromMemoryTTF((void*)icons_binary, sizeof(icons_binary), 20.f);
    io.Fonts->AddFontFromMemoryTTF((void*)font_bold_binary, sizeof(font_bold_binary), 20.f);
	io.Fonts->AddFontFromMemoryTTF((void*)OPPOSans_H, OPPOSans_H_size, 20.f, NULL, io.Fonts->GetGlyphRangesChineseFull());

    g_Initialized = true;
}

void drawBegin(){
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplAndroid_NewFrame(init_screen_x,init_screen_y);
    ImGui::NewFrame();
}

void drawEnd(){
    ImGuiIO &io = ImGui::GetIO();
    glViewport(0.0f, 0.0f, (int) io.DisplaySize.x, (int) io.DisplaySize.y);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT); // GL_DEPTH_BUFFER_BIT
    glFlush();
    if (display == EGL_NO_DISPLAY) {
        return;
    }
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    eglSwapBuffers(display, surface);
}

void tick() {
    ImVec4 clear_color = ImVec4(72 / 255.f, 79 / 255.f, 81 / 255.f, 1.00f);
    ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, { 0, 0 } );
    ImGui::Begin( "Tomato", 0, ImGuiWindowFlags_NoDecoration );
    {
        ImGui::PopStyleVar( );

        ImGuiWindow* window = ImGui::GetCurrentWindow( );
        ImDrawList* draw = ImGui::GetWindowDrawList( );
        ImVec2 p = ImGui::GetWindowPos( );
		//设置窗口大小
        ImGui::SetWindowSize( { 900, 700 } );

        static bool navbar_collapsed = true;
        static float navbar_width = 0.f; navbar_width = ImLerp( navbar_width, navbar_collapsed ? 0.f : 1.f, 0.04f );
        content_anim = ImLerp( content_anim, 1.f, 0.04f );

        ImGui::PushItemFlag( ImGuiItemFlags_Disabled, !navbar_collapsed );
        ImGui::PushStyleVar( ImGuiStyleVar_Alpha, content_anim * ( navbar_width > 0.005f ? 1 / navbar_width / 2 : 1.f ) );
        ImGui::SetCursorPos( { 81, 25 } );
        ImGui::BeginGroup( );
        ImGui::PushFont( ImGui::GetIO( ).Fonts->Fonts[3] );
        ImGui::Text( "Tomatosauce移植" );        

        ImGui::BeginChild( "Main", { window->Size.x - 81, window->Size.y - window->DC.CursorPos.y + window->Pos.y }, 0, ImGuiWindowFlags_NoBackground );
        {
            switch ( tab ) {
                case 0: {
                    ImGui::BeginChild( "子标签", { ImGui::GetWindowWidth( ) - 50, 40 }, 1 );
                    {
                        ImGui::SetCursorPos( { 15, 0 } );

                        if ( custom::subtab( "子标签 1##subtab", subtab == 0 ) ) subtab = 0; ImGui::SameLine( 0, 20 );
                        if ( custom::subtab( "子标签 2##subtab", subtab == 1 ) ) subtab = 1; ImGui::SameLine( 0, 20 );
                        if ( custom::subtab( "子标签 3##subtab", subtab == 2 ) ) subtab = 2;
                    }
                    ImGui::EndChild( );

                    ImGui::BeginChild( "Main_child", { ImGui::GetWindowWidth( ), ImGui::GetWindowHeight( ) - 55 }, 0, ImGuiWindowFlags_NoBackground );
                    {
                        ImGui::BeginGroup( );
                        {
                            custom::begin_child( "窗口 1", { ( ImGui::GetWindowWidth( ) - 45 ) / 2 - ImGui::GetStyle( ).ItemSpacing.x / 2, 0 } );
                            {
                                static bool enabled = true;
                                static int slider = 0, combo = 0;
                                static char text[64];
                                static float col[4];

                                ImGui::Checkbox( "勾选", &enabled );
                                ImGui::SliderInt( "整数滑动条", &slider, 0, 100, "%d%%" );
                                ImGui::Combo( "类型", &combo, "类型 1\0\r类型 2\0\r类型 3\0\0" );
                                ImGui::InputText( "文字", text, 64 );
                                ImGui::ColorEdit4( "颜色", col, ImGuiColorEditFlags_NoBorder | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_DisplayHex );
                                if(ImGui::Button("关闭"))
                                    exit(0);
                            }
                            custom::end_child( );

                        }
                        ImGui::EndGroup( );
                        ImGui::SameLine( 0, 15 );
                        ImGui::BeginGroup( );
                        {
                            custom::begin_child( "窗口 2", { ( ImGui::GetWindowWidth( ) - 45 ) / 2 - ImGui::GetStyle( ).ItemSpacing.x / 2, 0 } );
                            {
                                static bool enabled = true;
                                static int slider = 0, combo = 0;
                                static char text[64];
                                static float col[4];

                                ImGui::Checkbox( "勾选##3", &enabled );
                                ImGui::SliderInt( "整数滑动条##3", &slider, 0, 100, "%d%%" );
                                ImGui::Combo( "类型##3", &combo, "类型 1\0\r类型 2\0\r类型 3\0\0" );
                                ImGui::InputText( "文字##3", text, 64 );
                                ImGui::ColorEdit4( "颜色##3", col, ImGuiColorEditFlags_NoBorder | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_DisplayHex );
                                ImGui::Button( "按纽##3" );
                            }
                            custom::end_child( );

                        }
                        ImGui::EndGroup( );
                        
                        ImGui::BeginGroup( );
                        {
                            custom::begin_child( "窗口 3", { ( ImGui::GetWindowWidth( ) - 45 ) / 2 - ImGui::GetStyle( ).ItemSpacing.x / 2, 0 } );
                            {
                                static bool enabled = true;
                                static float slider = 0;
                                static int combo = 0;
                                static char text[64];
                                static float col[4];

                                ImGui::Checkbox( "勾选##4", &enabled );
                                ImGui::SliderFloat( "浮点数滑动条##4", &slider, 0, 100, "%f%%" );
                                ImGui::Combo( "类型##4", &combo, "类型 1\0\r类型 2\0\r类型 3\0\0" );
                                ImGui::InputText( "文字##4", text, 64 );
                                ImGui::ColorEdit4( "颜色##4", col, ImGuiColorEditFlags_NoBorder | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_DisplayHex );
                                ImGui::Button( "按纽##4" );
                            }
                            custom::end_child( );

                        }
                        ImGui::EndGroup( );
                        
                        ImGui::Spacing( );
                    }
                    ImGui::EndChild( );
                } break;
                case 1: {
                	ImGui::Text("菜单 1");
                } break;
                case 2: {
                	ImGui::Text("菜单 2");
                } break;
                case 3: {
                	ImGui::Text("菜单 3");
                } break;
                case 4: {
                	ImGui::Text("菜单 4");
                } break;
                
            }//switch
        }
        ImGui::EndChild( );

        ImGui::EndGroup( );
        ImGui::PopStyleVar( );
        ImGui::PopItemFlag( );

        ImGui::SetCursorPos( { 0, 0 } );

        ImGui::BeginChild( "左选单", { 50 + 100 * navbar_width, window->Size.y }, 0, ImGuiWindowFlags_NoBackground );
        {
            ImGui::GetWindowDrawList( )->AddRectFilled( p, p + ImGui::GetWindowSize().y , ImGui::GetColorU32( ImGuiCol_ChildBg ), ImGui::GetStyle( ).WindowRounding, ImDrawFlags_RoundCornersLeft );
            ImGui::GetWindowDrawList( )->AddRectFilled( { p.x + ImGui::GetWindowWidth() - 1, p.y }, p + ImGui::GetWindowSize().x, ImGui::GetColorU32( ImGuiCol_Border ) );

            ImGui::SetCursorPosY( 87 );

            ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, { 0, 16 } );
            if ( custom::tab( "A", "Tab 1", tab == 0 ) ) tab = 0;
            if ( custom::tab( "B", "Tab 2", tab == 1 ) ) tab = 1;
            if ( custom::tab( "C", "Tab 3", tab == 2 ) ) tab = 2;
            if ( custom::tab( "D", "Tab 4", tab == 3 ) ) tab = 3;
            if ( custom::tab( "E", "Tab 5", tab == 4 ) ) tab = 4;
            ImGui::PopStyleVar( );
        }
        ImGui::EndChild( );

        ImGui::SetCursorPos( { 41.5f + 100 * navbar_width, 47 } );
        if ( custom::collapse_button( navbar_collapsed ) ) navbar_collapsed = !navbar_collapsed;
    }
    ImGui::End();
    ImGui::PopFont( );
}

void shutdown(){
    if (!g_Initialized){
        return;
    }
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplAndroid_Shutdown();
    ImGui::DestroyContext();
    if (display != EGL_NO_DISPLAY){
        eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (context != EGL_NO_CONTEXT){
            eglDestroyContext(display, context);
        }
        if (surface != EGL_NO_SURFACE){
            eglDestroySurface(display, surface);
        }
        eglTerminate(display);
    }
    display = EGL_NO_DISPLAY;
    context = EGL_NO_CONTEXT;
    surface = EGL_NO_SURFACE;
    ANativeWindow_release(native_window);
}

//由Tomatosauce移植
//由Tomatosauce移植
//由Tomatosauce移植