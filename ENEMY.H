#ifndef ENEMY_H
#define ENEMY_H

#include "game.h"
#include "IMAGES.H"

class BasicEnemy : public Enemy {
public:
    BasicEnemy(float x, float y);
    virtual void draw();
    virtual void draw(int mode);
};

class FastEnemy : public Enemy {
public:
    FastEnemy(float x, float y);
    virtual void draw();
    virtual void draw(int mode);
};

class StrongEnemy : public Enemy {
public:
    StrongEnemy(float x, float y);
    virtual void draw();
    virtual void draw(int mode);
};

class StageEnemy : public Enemy {
public:
    StageEnemy(float x, float y, int stage);
    virtual void draw();
    virtual void draw(int mode);
    char* getName() const;

    // 실제 스프라이트 크기 리턴
    virtual int getSpriteWidth() const {
        return sprite_image ? sprite_image->x : 8;
    }
    virtual int getSpriteHeight() const {
        return sprite_image ? sprite_image->y : 8;
    }

private:
    int stage_num;
    char* name;
    sprites* sprite_image;
};

#endif