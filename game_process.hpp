//
//  game_process.hpp
//  snake
//
//  Created by pardo jérémie on 11/02/2022.
//

#ifndef game_process_hpp
#define game_process_hpp

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#define WORLD_XSIZE 11
#define WORLD_YSIZE 11
#define FRUIT_MAXNUMBER 2
#define SNAKE_MAXSIZE WORLD_XSIZE * WORLD_YSIZE
#define SNAKE_INITIAL_X 0
#define SNAKE_INITIAL_Y 0

enum CardDirEnum { N,E,S,O };

class fruitclass;
class snakeclass;

class cordinateclass {
public:
    cordinateclass ();
    int getx();
    int gety();
    void setcoord( int, int);
private:
    int x;//x coordinate
    int y;//y coordinate
};

class objectclass : public cordinateclass {
public:
    objectclass ();
    char getstate();
    void setstate( char);
private:
    char state;//the state of an object
};


class scoreclass {
public:
    scoreclass ();
    int get ();
    void set (int);
    void update ();
protected:
    int score;
    uint8_t unity;
    uint8_t tens;
    uint8_t hundred;
};

class snakeclass {
public:
    snakeclass ();
    //tell if the position is in the snake.
    bool isinsnake (int, int);
    //move snake (If the snake eat a fruit, add a snake block and delet say fruit| if snake inbody return false);
    bool move ( fruitclass*, scoreclass*);
    void updateheadstate ( CardDirEnum);
    int getsize ();
    void reset ();
    void init ();
protected:
    int head;
    int tail;
    bool tailmotionless;
    objectclass snake[SNAKE_MAXSIZE];
    /* The states of the parts of the snake are their orientations:
     0 - does not exist
     1 - SN
     2 - NS
     3 - EO
     4 - OE
     5 - SE        N
     6 - ES     O -|- E
     7 - SO        S
     8 - OS
     9 - NE
     10 - EN
     11 - NO
     12 - ON
     */
private:
    void addsnake (int, int);
    //tell if the position is in the body of the snake.
    bool isinbody (int, int);
};

class fruitclass {
public:
    fruitclass ();
    //tell if the position is on a fruit.
    bool isonfruit (int, int);
    void creat (snakeclass*);
    void delet (int, int);
    void reset ();
protected:
    objectclass fruits[FRUIT_MAXNUMBER];
    /* The states of a fruit are:
     0 - does not exist
     1 - exist */
private:
    int numberoffruits ();
};

#endif /* game_process_hpp */
