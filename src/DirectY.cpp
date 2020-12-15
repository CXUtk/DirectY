// DirectY.cpp : 定义应用程序的入口点。
//

#include "DirectY.h"
#include "framework/system/DYWindow.h"
#include "framework/system/Win32.h"
#include <ctime>
#include <cstdio>
#include "Main.h"


int main() {
    std::shared_ptr<DYWindow> window = std::make_shared<Win32Window>();
    window->Init();

    Main main(window);
    auto lastT = clock();
    auto lastFrameTime = clock();
    int cnt = 0;
    // 主消息循环:
    while (true) {
        main.Run();
        cnt++;
        auto curtime = clock();
        if (curtime - lastT > CLOCKS_PER_SEC) {
            lastT = curtime;
            printf("FPS: %d\n", cnt);
            cnt = 0;
        }
        do {
            window->PollEvents();
        } while (clock() - lastFrameTime < 16);
        lastFrameTime = clock();
    }
    return 0;
}
