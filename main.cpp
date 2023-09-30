//
//  main.cpp
//  snake
//
//  Created by pardo jérémie on 11/02/2022.
//

#include <iostream>
#include <SDL2/SDL.h>

#include "graphical_process.hpp"

int main(int argc, const char * argv[]) {
    GraphicLoop();
    SDL_Quit();
    return 0;
}
