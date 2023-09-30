//
//  graphical_process.cpp
//  snake
//
//  Created by pardo jérémie on 11/02/2022.
//

#include "graphical_process.hpp"
#include "game_process.hpp"
#include <SDL2/SDL.h>
#include <chrono>
#include <fstream>

//### Private constantes, class and functions ###//

//Screen dimension constants
#define WORLD_TILE_IN_PIXEL 18
#define SNAKETEXTURE_TILE_IN_PIXEL 6
#define SNAKETEXTURE_TILE_number 24
#define SCREEN_WIDTH WORLD_TILE_IN_PIXEL * WORLD_XSIZE
#define SCREEN_HEIGHT WORLD_TILE_IN_PIXEL * WORLD_YSIZE
#define SCREEN_FPS 60
#define SCREEN_TICKS_PER_FRAME 1000 / SCREEN_FPS
#define NEWFRUIT_EVERY_X_ANIMATION 24//cannot go above 255

class textureclass {
public:
    //Initializes variables
    textureclass();
    //Deallocates memory
    ~textureclass();
    //Loads image at specified path
    bool loadFromFile(std::string, SDL_Renderer*);
    //Deallocates texture
    void free();
    //Renders texture at given point
    void render(int&, int&, SDL_Renderer*, SDL_Rect* = NULL);
    void renderScaling(int, int, int, int, SDL_Renderer*,SDL_Rect* = NULL);
    //Gets image dimensions
    int getWidth();
    int getHeight();
    //set texture color (srcC = srcC * (color / 255))
    void setcolorMod(uint8_t, uint8_t, uint8_t);
private:
    //The actual hardware texture
    SDL_Texture* mTexture;
    //SDL_Texture* mTexture;
    //Image dimensions
    int mWidth;
    int mHeight;
};


class graphicalsnakeclass  : public snakeclass  {
public:
    graphicalsnakeclass() {}
    void render(uint8_t&, textureclass*, SDL_Renderer*, SDL_Rect*);
private:
    char selectheadclip(uint8_t&, int&);
    char selecttailclip(uint8_t&, int&);
    char selectbodyclip(int&);
};

class graphicalfruitclass  : public fruitclass  {
public:
    graphicalfruitclass() {}
    void render(textureclass*, SDL_Renderer*, SDL_Rect*) ;
};

class graphicalscoreclass  : public scoreclass  {
public:
    graphicalscoreclass() {}
    void render(textureclass*, SDL_Renderer*, SDL_Rect*) ;
};

bool SDLInit();
bool WindowInit(SDL_Window**);
bool SpriteInit(SDL_Surface**);
void ImgsBlit(SDL_Surface*, SDL_Surface*, int&);
bool RendererInit(SDL_Renderer**, SDL_Window*);
void Framcaping (uint32_t, uint32_t);
bool AnimationFrame (uint32_t*, uint32_t&, int&);
void initsnakefruittextureclip (SDL_Rect*);
void initnumbertextureclip (SDL_Rect*);
void initbestscoretextureclip (SDL_Rect*);

//### Content of functions ###//

void fileread (int* scores) {
    std::fstream my_file;
    my_file.open("/Users/pardojeremie/Documents/programmation/C++/snake/snake/score.txt", std::ios::in);
        if (!my_file) {
            //std::cout << "No such file";
            for(int i(0); i<3; i++)
                scores[i] = 0;
        }
        else {
            // Intermediate buffer
            std::string buffer;
            // By default, the >> operator reads word by workd (till whitespace)
            for(int i(0); i<3; i++)
            {
                my_file >> buffer;
                scores[i] = stoi(buffer);
            }
        }
        my_file.close();
};

void filewrite (int* scores) {
    std::fstream my_file;
    my_file.open("/Users/pardojeremie/Documents/programmation/C++/snake/snake/score.txt", std::ios::out);
        if (!my_file) {
            //std::cout << "File not created!";
        }
        else {
            my_file << scores[0] <<std::endl<< scores[1]<<std::endl<< scores[2];
        }
        my_file.close();
};

void GraphicLoop(){
    //condition of the graphical loop
    bool quit(false);
    //The window we'll be rendering to
    SDL_Window* window(NULL);
    //init the main renderer
    SDL_Renderer* renderer(NULL);
    //init the texture of the snake and fruits
    textureclass snakeandfruittexture;
    //init number texture
    textureclass numbertexture;
    //init text texture for each menu
    textureclass victorytexttexture;
    textureclass defeattexttexture;
    textureclass mainmenutexttexture;
    textureclass bestscoretexture;
    //init game speed
    uint8_t speed(1);
    uint8_t animationfps[] = {15,22,30};
    int animationticksperframe(1000 / animationfps[speed-1]);
    //init score
    graphicalscoreclass score;
    int bestscore[3];
    fileread(bestscore);
    score.set(bestscore[speed-1]);
    //init pressed key bool variable
    bool enterkeyunpressed(false);
    bool plusminuskeyunpressed(false);
    //init the snake game class;
    graphicalsnakeclass snake;
    snake.reset();
    snake.init();
    graphicalfruitclass fruits;
    //init scene (menu/game/gameover|win) selection
    uint8_t selectedscene(2);
    //init snake animation
    uint8_t animationframe(0);
    //ini fruit add counter
    uint8_t newfruitcunter(0);
    //The frames per second cap timer
    uint32_t startTimer, priorAnimationGetTicks(0);
    //Initialize SDL
    if (SDLInit())
        //init window
        if (WindowInit( &window))
            //init renderer
            if (RendererInit( &renderer, window))
                //init snake texture
                if(snakeandfruittexture.loadFromFile("/Users/pardojeremie/Documents/programmation/C++/snake/snake/snakenewtexture.bmp", renderer) && numbertexture.loadFromFile("/Users/pardojeremie/Documents/programmation/C++/snake/snake/numbertexture.bmp", renderer) && victorytexttexture.loadFromFile("/Users/pardojeremie/Documents/programmation/C++/snake/snake/victorytexttexture.bmp", renderer)&& defeattexttexture.loadFromFile("/Users/pardojeremie/Documents/programmation/C++/snake/snake/defeattexttexture.bmp", renderer)&& mainmenutexttexture.loadFromFile("/Users/pardojeremie/Documents/programmation/C++/snake/snake/mainmenutexttexture.bmp", renderer)
                   && bestscoretexture.loadFromFile("/Users/pardojeremie/Documents/programmation/C++/snake/snake/bestscoretexture.bmp", renderer)
                   ) {
                    //init number texture clips
                    SDL_Rect numberclips[10];
                    initnumbertextureclip(numberclips);
                    //init speed clips
                    SDL_Rect speedclips[3];
                    initbestscoretextureclip(speedclips);
                    //temp change of numbertexture color
                    numbertexture.setcolorMod(230, 230, 230);
                    // init snake and fruits texture clips
                    SDL_Rect snakefruitsclips[SNAKETEXTURE_TILE_number];
                    initsnakefruittextureclip (snakefruitsclips);
                    // keybord event
                    SDL_Event e;
                    //Starting graphical loop
                    while (!quit){
                        startTimer = SDL_GetTicks();
                        //handling keyboard inputs
                        while (SDL_PollEvent(&e)){
                            if (e.type == SDL_QUIT)
                                quit = true;
                           /* if (e.type == SDL_KEYDOWN)
                                quit = true;
                            if (e.type == SDL_MOUSEBUTTONDOWN)
                                quit = true;*/
                            }
                        //keyinput endeling
                        const uint8_t* currentKeyStates(SDL_GetKeyboardState( NULL ));
                        //keyinput by scene
                        switch (selectedscene) {
                            //main game
                            case 0:
                                if(currentKeyStates[ SDL_SCANCODE_UP])
                                    snake.updateheadstate(N);
                                else if(currentKeyStates[ SDL_SCANCODE_DOWN])
                                    snake.updateheadstate(S);
                                else if( currentKeyStates[ SDL_SCANCODE_LEFT])
                                    snake.updateheadstate(O);
                                else if( currentKeyStates[ SDL_SCANCODE_RIGHT])
                                    snake.updateheadstate(E);
                                break;
                            case 1:
                                if(!enterkeyunpressed && !currentKeyStates[ SDL_SCANCODE_RETURN] && !currentKeyStates[ SDL_SCANCODE_RETURN2])
                                    enterkeyunpressed = true;
                                else if(enterkeyunpressed && (currentKeyStates[ SDL_SCANCODE_RETURN] || currentKeyStates[ SDL_SCANCODE_RETURN2])) {
                                    selectedscene = 2;
                                    enterkeyunpressed = false;
                                }
                                break;
                            //main menu + lose win menu
                            default:
                                if(!enterkeyunpressed && !currentKeyStates[ SDL_SCANCODE_RETURN] && !currentKeyStates[ SDL_SCANCODE_RETURN2])
                                    enterkeyunpressed = true;
                                else if(enterkeyunpressed && (currentKeyStates[ SDL_SCANCODE_RETURN] || currentKeyStates[ SDL_SCANCODE_RETURN2])) {
                                    enterkeyunpressed = false;
                                    selectedscene = 0;
                                    snakeandfruittexture.setcolorMod(255, 255, 255);
                                    numbertexture.setcolorMod(40, 40, 40);
                                    score.set(0);
                                    snake.reset();
                                    snake.init();
                                    fruits.reset();
                                    newfruitcunter=0;
                                    animationframe = 0;
                                }
                                if(!plusminuskeyunpressed && !currentKeyStates[ SDL_SCANCODE_DOWN] && !currentKeyStates[ SDL_SCANCODE_UP])
                                    plusminuskeyunpressed = true;
                                else if(plusminuskeyunpressed && currentKeyStates[ SDL_SCANCODE_DOWN] ) {
                                    plusminuskeyunpressed= false;
                                    speed --;
                                    if (speed < 1)
                                    speed = 1;
                                    else
                                        score.set(bestscore[speed-1]);
                                    animationticksperframe = 1000 /  animationfps[speed-1];
                                }
                                else if(plusminuskeyunpressed && currentKeyStates[ SDL_SCANCODE_UP] ) {
                                    plusminuskeyunpressed= false;
                                    speed ++;
                                    if (speed > 3)
                                     speed = 3;
                                    else
                                        score.set(bestscore[speed-1]);
                                    animationticksperframe = 1000 /  animationfps[speed-1];
                                }
                                if(currentKeyStates[ SDL_SCANCODE_ESCAPE])
                                    quit = true;
                        }
                        
                        //game process by scene
                        switch (selectedscene) {
                            //main game
                            case 0:
                                //Snake game animation
                                if (AnimationFrame (&priorAnimationGetTicks, startTimer, animationticksperframe)){
                                    //update animation
                                    animationframe ++;
                                    if (animationframe > 2){
                                        animationframe = 0;
                                        if(!snake.move(&fruits, &score)) {
                                            selectedscene = 1;
                                            numbertexture.setcolorMod(230, 230, 230);
                                            snakeandfruittexture.setcolorMod(30, 30, 30);
                                            animationframe = 2;
                                            //Creat the static rander of victory and defeat menu
                                            //clean render
                                            SDL_SetRenderDrawColor( renderer, 0x00, 0x00, 0x00, 0x00 );
                                            SDL_RenderClear( renderer );
                                            //render texture
                                            snake.render(animationframe, &snakeandfruittexture,renderer, snakefruitsclips);
                                            fruits.render(&snakeandfruittexture,renderer, snakefruitsclips);
                                            //victory condition
                                            if (snake.getsize() == SNAKE_MAXSIZE)
                                                victorytexttexture.renderScaling(32, 107, 135, 66, renderer);
                                            else
                                                defeattexttexture.renderScaling(32, 107, 135, 66, renderer);
                                            score.render(&numbertexture,renderer, numberclips);
                                            //present the render
                                            SDL_RenderPresent( renderer );
                                            if (bestscore[speed-1] < score.get())
                                                bestscore[speed-1] = score.get();
                                            score.set(bestscore[speed-1]);
                                        }
                                    }
                                    
                                    newfruitcunter++;
                                    if (newfruitcunter == NEWFRUIT_EVERY_X_ANIMATION) {
                                        newfruitcunter=0;
                                        fruits.creat(&snake);
                                    }
                                }
                                break;
                            //main menu + lose win menu
                            default:;
                        }
                        
                        //render texture by scene
                        switch (selectedscene) {
                            //main game
                            case 0:
                                //clean render
                                SDL_SetRenderDrawColor( renderer, 0x00, 0x00, 0x00, 0x00 );
                                SDL_RenderClear( renderer );
                                score.render(&numbertexture,renderer, numberclips);
                                snake.render(animationframe, &snakeandfruittexture,renderer, snakefruitsclips);
                                fruits.render(&snakeandfruittexture,renderer, snakefruitsclips);
                                //present the render
                                SDL_RenderPresent( renderer );
                                break;
                            case 2:
                                //clean render
                                SDL_SetRenderDrawColor( renderer, 0x00, 0x00, 0x00, 0x00 );
                                SDL_RenderClear( renderer );
                                score.render(&numbertexture,renderer, numberclips);
                                bestscoretexture.renderScaling(154, 5, 39, 75, renderer, &speedclips[speed-1]);
                                mainmenutexttexture.renderScaling(27, 90, 144, 96,renderer);
                                //present the render
                                SDL_RenderPresent( renderer );
                                break;
                            //lose|win menu + main menu
                            default:;
                        }
                        //scaping frame rate to SCREEN_FPS
                        Framcaping (startTimer,SDL_GetTicks());
                    }
    }
    //save bestscore
    filewrite(bestscore);
    //Destroy window
    snakeandfruittexture.free();
    numbertexture.free();
    victorytexttexture.free();
    defeattexttexture.free();
    mainmenutexttexture.free();
    SDL_DestroyRenderer( renderer );
    SDL_DestroyWindow( window );
    window = NULL;
    renderer = NULL;
    //Quit SDL subsystems
    SDL_Quit();
}

bool SDLInit() {
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
        std::cout << "SDL could not initialize! SDL_Error:"<<  SDL_GetError()<< std::endl;
        return false;
    }
    return true;
}

bool WindowInit( SDL_Window** window) {
    *window = SDL_CreateWindow( "Snake", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
    if( *window == NULL ) {
        std::cout <<  "Window could not be created! SDL_Error:"<< SDL_GetError() << std::endl;
        return false;
    }
    return true;
}

bool RendererInit( SDL_Renderer** renderer,SDL_Window* window) {
    //Create renderer for window
    *renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED );
    if( renderer == NULL ) {
        std::cout << "Renderer could not be created! SDL Error:"<< SDL_GetError() << std::endl;
        return false;
    }
    else {
        //Initialize renderer color
        SDL_SetRenderDrawColor( *renderer, 0xFF, 0xFF, 0xFF, 0xFF );
    }
    return true;
}

void Framcaping ( uint32_t startTimer, uint32_t endTimer) {
    uint32_t dif( endTimer - startTimer);
    if( dif < SCREEN_TICKS_PER_FRAME )
        SDL_Delay( SCREEN_TICKS_PER_FRAME - dif);
}

bool AnimationFrame ( uint32_t* priorAnimationGetTicks, uint32_t& actualGetTicks,int& ticksperframe) {
    uint32_t dif(actualGetTicks - *priorAnimationGetTicks);
    if( dif > ticksperframe ) {
        *priorAnimationGetTicks = actualGetTicks;
        return true;
    }
    return false;
}


textureclass::textureclass() {
    //Initialize
    mTexture = NULL;
    mWidth = 0;
    mHeight = 0;
}

//Deallocates memory
textureclass::~textureclass() {
    //Deallocate
    free();
}

//Loads image at specified path
bool textureclass::loadFromFile( std::string path, SDL_Renderer* renderer ) {
    //Get rid of preexisting texture
    free();

    //The final texture
    SDL_Texture* newTexture( NULL);

    //Load image at specified path
    SDL_Surface* loadedSurface( SDL_LoadBMP(path.c_str()));
    if( loadedSurface == NULL )
    {
        std::cout << "Unable to load sprite! SDL Error:"<< SDL_GetError() << std::endl;
    }
    else
    {
        //Color key image
        SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, 0, 0xFF, 0xFF ));

        //Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface( renderer, loadedSurface );
        if( newTexture == NULL )
        {
            std::cout << "Unable to create texture from"<< path.c_str()<<"! SDL Error:"<< SDL_GetError() << std::endl;
        }
        else
        {
            //Get image dimensions
            mWidth = loadedSurface->w;
            mHeight = loadedSurface->h;
        }

        //Get rid of old loaded surface
        SDL_FreeSurface( loadedSurface);
    }

    //Return success
    mTexture = newTexture;
    return mTexture != NULL;
}

//Deallocates texture
void textureclass::free() {
    //Free texture if it exists
    if( mTexture != NULL)
    {
        SDL_DestroyTexture( mTexture);
        mTexture = NULL;
        mWidth = 0;
        mHeight = 0;
    }
}


//Renders texture at given point
void textureclass::render( int& x, int& y, SDL_Renderer* renderer, SDL_Rect* clip) {
    //Set rendering space and render to screen
    SDL_Rect renderQuad = { x, y, mWidth, mHeight};

    //Set clip rendering dimensions
    if( clip != NULL)
    {
        renderQuad.w = clip->w;
        renderQuad.h = clip->h;
    }
    

    //Render to screen
    SDL_RenderCopy( renderer, mTexture, clip, &renderQuad);
}

void textureclass::renderScaling( int x, int y,int scalingw, int scalingh,SDL_Renderer* renderer,SDL_Rect* clip) {
    //Set rendering space and render to screen
    SDL_Rect renderQuad = { x, y, scalingw, scalingh};

    //Set clip rendering dimensions
    if( clip != NULL)
    {
        renderQuad.w = scalingw;
        renderQuad.h = scalingh;
    }
    

    //Render to screen
    SDL_RenderCopy( renderer, mTexture, clip, &renderQuad);
}

//Gets image dimensions
int textureclass::getWidth() {
    return mWidth;
}


int textureclass::getHeight() {
    return mHeight;
}

void textureclass::setcolorMod( uint8_t r,uint8_t g, uint8_t b) {
    SDL_SetTextureColorMod( mTexture, r, g, b);
}

void initsnakefruittextureclip ( SDL_Rect* cliplist) {
    for (int i(0); i < SNAKETEXTURE_TILE_number; i++){
        cliplist[i].x = 0;
        cliplist[i].y = i* SNAKETEXTURE_TILE_IN_PIXEL;
        cliplist[i].h = SNAKETEXTURE_TILE_IN_PIXEL;
        cliplist[i].w = SNAKETEXTURE_TILE_IN_PIXEL;
    }
}

void initnumbertextureclip ( SDL_Rect* cliplist) {
    for (int i(0); i < 10; i++){
        cliplist[i].x = 0;
        cliplist[i].y = i* 5;
        cliplist[i].h = 5;
        cliplist[i].w = 3;
    }
}

void initbestscoretextureclip ( SDL_Rect* cliplist) {
    for (int i(0); i < 3; i++){
        cliplist[i].x = 0;
        cliplist[i].y = i* 23;
        cliplist[i].h = 23;
        cliplist[i].w = 13;
    }
}
void graphicalsnakeclass::render( uint8_t& animationFrame, textureclass* gametexture, SDL_Renderer* renderer, SDL_Rect* cliplist) {
    for (int i(0); i < SNAKE_MAXSIZE; i++)
        if( snake[i].getstate() != 0) {
            if ( i == head)
                gametexture->renderScaling( snake[i].getx() * WORLD_TILE_IN_PIXEL,snake[i].gety() * WORLD_TILE_IN_PIXEL,WORLD_TILE_IN_PIXEL,WORLD_TILE_IN_PIXEL, renderer, &cliplist[selectheadclip( animationFrame,i)]);
            else if (i == tail)
                gametexture->renderScaling( snake[i].getx() * WORLD_TILE_IN_PIXEL,snake[i].gety() * WORLD_TILE_IN_PIXEL,WORLD_TILE_IN_PIXEL,WORLD_TILE_IN_PIXEL, renderer, &cliplist[selecttailclip( animationFrame,i)]);
            else
                gametexture->renderScaling( snake[i].getx() * WORLD_TILE_IN_PIXEL,snake[i].gety() * WORLD_TILE_IN_PIXEL,WORLD_TILE_IN_PIXEL,WORLD_TILE_IN_PIXEL, renderer, &cliplist[ selectbodyclip(i)]);
        }
}

char graphicalsnakeclass::selectheadclip(uint8_t& animationFrame, int& idSnakehead) {
    char clip (SNAKETEXTURE_TILE_number - 1);
    switch(snake[idSnakehead].getstate()) {
        /*SN*/
        case 1 :
            switch(animationFrame) {
                case 0 :
                    clip = 0;
                    break;
                case 1 :
                    clip = 1;
                    break;
                case 2 :
                    clip = 4;
                    break;
                default:;
            }
            break;
        /*NS*/
        case 2 :
            switch(animationFrame) {
                case 0 :
                    clip = 2;
                    break;
                case 1 :
                    clip = 3;
                    break;
                case 2 :
                    clip = 4;
                    break;
                default:;
            }
            break;
        /*EO*/
        case 3 :
            switch(animationFrame) {
                case 0 :
                    clip = 7;
                    break;
                case 1 :
                    clip = 8;
                    break;
                case 2 :
                    clip = 9;
                    break;
                default:;
            }
            break;
        /*OE*/
        case 4 :
            switch(animationFrame) {
                case 0 :
                    clip = 5;
                    break;
                case 1 :
                    clip = 6;
                    break;
                case 2 :
                    clip = 9;
                    break;
                default:;
            }
            break;
        /*SE*/
        case 5 :
            switch(animationFrame) {
                case 0 :
                    clip = 0;
                    break;
                case 1 :
                    clip = 11;
                    break;
                case 2 :
                    clip = 19;
                    break;
                default:;
            }
            break;
        /*ES*/
        case 6 :
            switch(animationFrame) {
                case 0 :
                    clip = 7;
                    break;
                case 1 :
                    clip = 17;
                    break;
                case 2 :
                    clip = 19;
                    break;
                default:;
            }
            break;
        /*SO*/
        case 7 :
            switch(animationFrame) {
                case 0 :
                    clip = 0;
                    break;
                case 1 :
                    clip = 10;
                    break;
                case 2 :
                    clip = 18;
                    break;
                default:;
            }
            break;
        /*OS*/
        case 8 :
            switch(animationFrame) {
                case 0 :
                    clip = 5;
                    break;
                case 1 :
                    clip = 15;
                    break;
                case 2 :
                    clip = 18;
                    break;
                default:;
            }
            break;
        /*NE*/
        case 9 :
            switch(animationFrame) {
                case 0 :
                    clip = 2;
                    break;
                case 1 :
                    clip = 13;
                    break;
                case 2 :
                    clip = 21;
                    break;
                default:;
            }
            break;
        /*EN*/
        case 10 :
            switch(animationFrame) {
                case 0 :
                    clip = 7;
                    break;
                case 1 :
                    clip = 16;
                    break;
                case 2 :
                    clip = 21;
                    break;
                default:;
            }
            break;
        /*NO*/
        case 11 :
            switch(animationFrame) {
                case 0 :
                    clip = 2;
                    break;
                case 1 :
                    clip = 12;
                    break;
                case 2 :
                    clip = 20;
                    break;
                default:;
            }
            break;
        /*ON*/
        case 12 :
            switch(animationFrame) {
                case 0 :
                    clip = 5;
                    break;
                case 1 :
                    clip = 14;
                    break;
                case 2 :
                    clip = 20;
                    break;
                default:;
            }
            break;
        default:;
    }
    return clip;
}

char graphicalsnakeclass::selecttailclip(uint8_t& animationFrame, int& idSnaketail) {
    char clip(SNAKETEXTURE_TILE_number - 1);
    if(tailmotionless)
        switch(snake[idSnaketail].getstate()) {
            /*#S*/
            case 2 :
            case 6 :
            case 8 :
                clip = 0;
                break;
            /*#N*/
            case 1 :
            case 10 :
            case 12:
                clip = 2;
                break;
            /*#E*/
            case 4 :
            case 5 :
            case 9 :
                clip = 7;
                break;
            /*#O*/
            case 3 :
            case 7 :
            case 11 :
                clip = 5;
                break;
            default:;
        }
    else
        switch(snake[idSnaketail].getstate()) {
            /*SN*/
            case 2 :
                switch(animationFrame) {
                    case 2 :
                        clip = 0;
                        break;
                    case 1 :
                        clip = 1;
                        break;
                    case 0 :
                        clip = 4;
                        break;
                    default:;
                }
                break;
            /*NS*/
            case 1 :
                switch(animationFrame) {
                    case 2 :
                        clip = 2;
                        break;
                    case 1 :
                        clip = 3;
                        break;
                    case 0 :
                        clip = 4;
                        break;
                    default:;
                }
                break;
            /*EO*/
            case 4 :
                switch(animationFrame) {
                    case 2 :
                        clip = 7;
                        break;
                    case 1 :
                        clip = 8;
                        break;
                    case 0 :
                        clip = 9;
                        break;
                    default:;
                }
                break;
            /*OE*/
            case 3 :
                switch(animationFrame) {
                    case 2 :
                        clip = 5;
                        break;
                    case 1 :
                        clip = 6;
                        break;
                    case 0 :
                        clip = 9;
                        break;
                    default:;
                }
                break;
            /*SE*/
            case 6 :
                switch(animationFrame) {
                    case 2 :
                        clip = 0;
                        break;
                    case 1 :
                        clip = 11;
                        break;
                    case 0 :
                        clip = 19;
                        break;
                    default:;
                }
                break;
            /*ES*/
            case 5 :
                switch(animationFrame) {
                    case 2 :
                        clip = 7;
                        break;
                    case 1 :
                        clip = 17;
                        break;
                    case 0 :
                        clip = 19;
                        break;
                    default:;
                }
                break;
            /*SO*/
            case 8 :
                switch(animationFrame) {
                    case 2 :
                        clip = 0;
                        break;
                    case 1 :
                        clip = 10;
                        break;
                    case 0 :
                        clip = 18;
                        break;
                    default:;
                }
                break;
            /*OS*/
            case 7 :
                switch(animationFrame) {
                    case 2 :
                        clip = 5;
                        break;
                    case 1 :
                        clip = 15;
                        break;
                    case 0 :
                        clip = 18;
                        break;
                    default:;
                }
                break;
            /*NE*/
            case 10 :
                switch(animationFrame) {
                    case 2 :
                        clip = 2;
                        break;
                    case 1 :
                        clip = 13;
                        break;
                    case 0 :
                        clip = 21;
                        break;
                    default:;
                }
                break;
            /*EN*/
            case 9 :
                switch(animationFrame) {
                    case 2 :
                        clip = 7;
                        break;
                    case 1 :
                        clip = 16;
                        break;
                    case 0 :
                        clip = 21;
                        break;
                    default:;
                }
                break;
            /*NO*/
            case 12 :
                switch(animationFrame) {
                    case 2 :
                        clip = 2;
                        break;
                    case 1 :
                        clip = 12;
                        break;
                    case 0 :
                        clip = 20;
                        break;
                    default:;
                }
                break;
            /*ON*/
            case 11 :
                switch(animationFrame) {
                    case 2 :
                        clip = 5;
                        break;
                    case 1 :
                        clip = 14;
                        break;
                    case 0 :
                        clip = 20;
                        break;
                    default:;
                }
                break;
            default:;
        }
        return clip;
    }

    char graphicalsnakeclass::selectbodyclip(int& idSnakebody) {
        char clip( SNAKETEXTURE_TILE_number - 1);
        switch(snake[idSnakebody].getstate()) {
            /*SN*/
            case 1 :
            /*NS*/
            case 2 :
                clip = 4;
                break;
            /*EO*/
            case 3 :
            /*OE*/
            case 4 :
                clip = 9;
                break;
            /*SE*/
            case 5 :
            /*ES*/
            case 6 :
                clip = 19;
                break;
            /*SO*/
            case 7 :
                clip = 18;
                break;
            /*OS*/
            case 8 :
                clip = 18;
                break;
            /*NE*/
            case 9 :
            /*EN*/
            case 10 :
                clip = 21;
                break;
            /*NO*/
            case 11 :
            /*ON*/
            case 12 :
                clip = 20;
                break;
            default:;
        }
    return clip;
}


void graphicalfruitclass::render(textureclass* gametexture, SDL_Renderer* renderer, SDL_Rect* cliplist) {
        for (auto& fruit:fruits)
            if(fruit.getstate() != 0)
                    gametexture->renderScaling(fruit.getx() * WORLD_TILE_IN_PIXEL,fruit.gety() * WORLD_TILE_IN_PIXEL,WORLD_TILE_IN_PIXEL,WORLD_TILE_IN_PIXEL, renderer, &cliplist[22]);
}

void graphicalscoreclass::render(textureclass* gametexture, SDL_Renderer* renderer, SDL_Rect* cliplist) {
    gametexture->renderScaling(5, 5, 45, 75, renderer, &cliplist[hundred]);
    gametexture->renderScaling(55, 5, 45, 75, renderer, &cliplist[tens]);
    gametexture->renderScaling(105, 5, 45, 75, renderer, &cliplist[unity]);
}
