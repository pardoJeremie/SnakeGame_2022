//
//  game_process.cpp
//  snake
//
//  Created by pardo jérémie on 11/02/2022.
//

#include "game_process.hpp"


/*### class cordinateclass ###*/

cordinateclass::cordinateclass () {
    x = -1;
    y = -1;
}

int cordinateclass::getx(){
    return x;
}

int cordinateclass::gety(){
    return y;
}

void cordinateclass::setcoord(int newx, int newy) {
    x = newx;
    y = newy;
}


/*### class objectclass ###*/

objectclass::objectclass () {
    state = 0;
}

char objectclass::getstate() {
    return state;
}

void objectclass::setstate(char newstate) {
    state = newstate;
}

/*### class scoreclass ###*/

scoreclass::scoreclass () {
    set(0);
};

int scoreclass::get () {
    return score;
}

void scoreclass::set (int newscore) {
    score = newscore;
    unity = score % 10;
    tens = score / 10 % 10;
    hundred = score /100;
}

void scoreclass::update () {
    set(score+1);
};


/*### class snakeclass ###*/

snakeclass::snakeclass () {}

//tell if the position is in the snake.
bool snakeclass::isinsnake (int newx, int newy) {
    for (auto& segment: snake)
        if (segment.getstate() != 0 && segment.getx() == newx && segment.gety() == newy )
            return true;
    return false;
}

//tell if the position is in the body of the snake.
bool snakeclass::isinbody (int newx, int newy) {
    for (int i(0); i < SNAKE_MAXSIZE; i++)
        if (i != tail && i != head && snake[i].getstate() != 0 && snake[i].getx() == newx && snake[i].gety() == newy )
            return true;
    return false;
}

void snakeclass::addsnake (int newx, int newy) {
    int oldhead = head;
    //set new head position in list
    head++;
    if (head >= SNAKE_MAXSIZE)
        head = 0;
    //set new head orientation
    switch (snake[oldhead].getstate()) {
            /*#S*/
            case 2 :
            case 6 :
            case 8 :
                snake[head].setstate(2);
                break;
            /*#N*/
            case 1 :
            case 10 :
            case 12 :
                snake[head].setstate(1);
                break;
            /*#E*/
            case 4 :
            case 5 :
            case 9 :
                snake[head].setstate(4);
                break;
            /*#O*/
            case 3 :
            case 7 :
            case 11 :
                snake[head].setstate(3);
                break;
            default:
                std::cout<<"error in the state value of the snake head in movesnake"<<std::endl;
    }
    //set new head coordinate
    snake[head].setcoord(newx,newy);
}

bool snakeclass::move (fruitclass* fruits, scoreclass* score) {
    int newx(-1), newy(-1);
    //find new head coordinate
    switch(snake[head].getstate()) {
        /*#S*/
        case 2 :
        case 6 :
        case 8 :
            newx = snake[head].getx();
            newy = snake[head].gety() + 1;
            if (newy >= WORLD_YSIZE) {
                newy = 0;}
            break;
        /*#N*/
        case 1 :
        case 10 :
        case 12 :
            newx = snake[head].getx();
            newy = snake[head].gety() - 1;
            if (newy < 0){
                newy = WORLD_YSIZE - 1;
            }
            break;
        /*#E*/
        case 4 :
        case 5 :
        case 9 :
            newx = snake[head].getx() + 1;
            newy = snake[head].gety();
            if (newx >= WORLD_XSIZE)
                newx = 0;
            break;
        /*#O*/
        case 3 :
        case 7 :
        case 11 :
            newx = snake[head].getx() - 1;
            newy = snake[head].gety();
            if (newx < 0)
                newx = WORLD_XSIZE - 1;
            break;
        default:
            std::cout<<"error in the state value of the snake head in movesnake"<<std::endl;
    }
    //impossibility to move
    if (isinbody(newx, newy))
        return false;
    //set new head
    addsnake(newx, newy);
    if (fruits->isonfruit(newx, newy)) {
        fruits->delet(newx, newy);
        score->update();
        tailmotionless = true;
    }
    else {
        tailmotionless = false;
        //delet old tail
        snake[tail].setstate(0);
        //set new tail position in list
        tail++;
        if (tail >= SNAKE_MAXSIZE)
            tail = 0;
    }
    return true;
}

void snakeclass::updateheadstate (CardDirEnum  dir) {
    switch(snake[head].getstate()) {
        /*S#*/
        case 1 :
        case 5 :
        case 7 :
            switch(dir) {
                case N :
                    snake[head].setstate(1);
                    break;
                case E :
                    snake[head].setstate(5);
                    break;
                case O :
                    snake[head].setstate(7);
                    break;
                default:;
            }
            break;
        /*N#*/
        case 2 :
        case 9 :
        case 11 :
            switch(dir) {
                case S :
                    snake[head].setstate(2);
                    break;
                case E :
                    snake[head].setstate(9);
                    break;
                case O :
                    snake[head].setstate(11);
                    break;
                default:;
            }
            break;
        /*E#*/
        case 10 :
        case 6 :
        case 3 :
            switch(dir) {
                case N :
                    snake[head].setstate(10);
                    break;
                case S :
                    snake[head].setstate(6);
                    break;
                case O :
                    snake[head].setstate(3);
                    break;
                default:;
            }
            break;
        /*O#*/
        case 12 :
        case 8 :
        case 4 :
            switch(dir) {
                case N :
                    snake[head].setstate(12);
                    break;
                case S :
                    snake[head].setstate(8);
                    break;
                case E :
                    snake[head].setstate(4);
                    break;
                default:;
            }
            break;
        default:
            std::cout<<"error in the state value of the snake head in updatesnakeheadstate"<<std::endl;
    }
}

int snakeclass::getsize () {
    return head - tail;
}

void snakeclass::reset () {
    for
        (auto& segment: snake)
        segment.setstate(0);
}

void snakeclass::init () {
    //set head EN
    head = 8;
    snake[8].setstate(10);
    snake[8].setcoord(WORLD_XSIZE/2+1,WORLD_YSIZE/2-1);
    //set bodysegment
    //SO
    snake[7].setstate(7);
    snake[7].setcoord(WORLD_XSIZE/2+2,WORLD_YSIZE/2-1);
    //ON
    snake[6].setstate(12);
    snake[6].setcoord(WORLD_XSIZE/2+2,WORLD_YSIZE/2);
    //OE
    snake[5].setstate(4);
    snake[5].setcoord(WORLD_XSIZE/2+1,WORLD_YSIZE/2);
    //SE
    snake[4].setstate(5);
    snake[4].setcoord(WORLD_XSIZE/2,WORLD_YSIZE/2);
    //EN
    snake[3].setstate(10);
    snake[3].setcoord(WORLD_XSIZE/2,WORLD_YSIZE/2+1);
    //SO
    snake[2].setstate(7);
    snake[2].setcoord(WORLD_XSIZE/2+1,WORLD_YSIZE/2+1);
    //set SN
    snake[1].setstate(1);
    snake[1].setcoord(WORLD_XSIZE/2+1,WORLD_YSIZE/2+2);
    //set tail SN
    tail = 0;
    snake[0].setstate(1);
    snake[0].setcoord(WORLD_XSIZE/2+1,WORLD_YSIZE/2+3);
    tailmotionless = false;
}


/*### class fruitclass ###*/

fruitclass::fruitclass () {}

//tell if the position is on a fruit.
bool fruitclass::isonfruit (int newx, int newy) {
    for (auto& fruit:fruits)
        if (fruit.getstate() != 0 && fruit.getx() == newx && fruit.gety() == newy )
            return true;
    return false;
}

void fruitclass::delet (int newx, int newy) {
    for (auto& fruit:fruits)
        if (fruit.getx() == newx && fruit.gety() == newy ) {
            fruit.setstate(0);
            break;
        }
}

void fruitclass::creat (snakeclass* snake) {
    int newpossibleposition = SNAKE_MAXSIZE /*is equal to WORLD_XSIZE * WORLD_YSIZE*/ - snake->getsize() - numberoffruits();
    //test to see if the world have some void spaces that can accommodate a new fruit
    if (newpossibleposition != 0) {
        int posnewfruit = rand() % newpossibleposition;
        //test for void spaces where a fruit can be created
        for (int x(0); x < WORLD_XSIZE;x++)
            for (int y(0); y < WORLD_YSIZE; y++) {
                if (posnewfruit !=0 && !snake->isinsnake(x, y) && !isonfruit(x, y))
                    posnewfruit--; //iterate void spaces until reaching the void space n°posnewfruit
                else if (!snake->isinsnake(x, y) && !isonfruit(x, y)) //fvoid space n°posnewfruit found
                    for (auto& fruit:fruits)//found a empty spot in the fruits list and create the new fruit
                        if (fruit.getstate() == 0) {
                            fruit.setcoord(x, y);
                            fruit.setstate(1);
                            goto endloop; //end the function as the new fruit was created
                        }
            }
    }
    endloop:;
}
void fruitclass::reset () {
    for (auto& fruit:fruits)
        fruit.setstate(0);
}
 
int fruitclass::numberoffruits () {
    int res = 0;
    for (auto& fruit:fruits)
        if (fruit.getstate() != 0)
            res++;
    return res;
}
