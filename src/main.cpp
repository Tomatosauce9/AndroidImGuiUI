//
// Created by Ssage on 2022/3/18.
//
#include <main.h>
//由Tomatosauce移植
//由Tomatosauce移植
//由Tomatosauce移植	

int main(){
    printf("Pid is %d\n", getpid());
    screen_config();
    init_screen_x = screen_x + screen_y;
    init_screen_y = screen_y + screen_x;
    if(!init_egl(init_screen_x,init_screen_y)){
        exit(0);
    }
    ImGui_init();
    Init_touch_config();
    for (;;) {
        drawBegin();
        tick();
        drawEnd();
    }
        
    shutdown();
    return 0;
}
//由Tomatosauce移植
//由Tomatosauce移植
//由Tomatosauce移植	
