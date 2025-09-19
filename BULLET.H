#ifndef BULLET_H
#define BULLET_H

#include "game.h"
#include "IMAGES.H"

class BasicBullet : public Bullet {
private:
    sprites* bullet_image;
    int bullet_index;  // 1 or 2 for BULLET1.PCX or BULLET2.PCX
public:
    BasicBullet(float x, float y, Enemy* target, int damage, int index = 1);
    virtual void draw();

    // 실제 스프라이트 크기 리턴
    virtual int getSpriteWidth() const {
        return bullet_image ? bullet_image->x : 2;
    }
    virtual int getSpriteHeight() const {
        return bullet_image ? bullet_image->y : 2;
    }
};

class SniperBullet : public Bullet {
private:
    sprites* bullet_image;
public:
    SniperBullet(float x, float y, Enemy* target, int damage);
    virtual void draw();

    // 실제 스프라이트 크기 리턴
    virtual int getSpriteWidth() const {
        return bullet_image ? bullet_image->x : 2;
    }
    virtual int getSpriteHeight() const {
        return bullet_image ? bullet_image->y : 2;
    }
};

class RapidBullet : public Bullet {
private:
    sprites* bullet_image;
    int bullet_index;  // 4 or 5 for BULLET4.PCX or BULLET5.PCX
public:
    RapidBullet(float x, float y, Enemy* target, int damage, int index = 4);
    virtual void draw();

    // 실제 스프라이트 크기 리턴
    virtual int getSpriteWidth() const {
        return bullet_image ? bullet_image->x : 2;
    }
    virtual int getSpriteHeight() const {
        return bullet_image ? bullet_image->y : 2;
    }
};

class MultiBullet : public Bullet {
private:
    float base_vx, base_vy;
    float angle_offset;
    int bullet_index;
    sprites* bullet_image;
public:
    MultiBullet(float x, float y, Enemy* target, int damage, int index);
    virtual void update(float deltaTime);
    virtual void draw();

    // 실제 스프라이트 크기 리턴
    virtual int getSpriteWidth() const {
        return bullet_image ? bullet_image->x : 2;
    }
    virtual int getSpriteHeight() const {
        return bullet_image ? bullet_image->y : 2;
    }
};

class PiercingBullet : public Bullet {
private:
    int pierce_count;
    int max_pierce;
    sprites* bullet_image;
public:
    PiercingBullet(float x, float y, Enemy* target, int damage);
    virtual void update(float deltaTime);
    virtual void draw();
    int checkPiercingCollision(Enemy* enemies[], int enemy_count);
    int getPierceCount() const { return pierce_count; }
    int getMaxPierce() const { return max_pierce; }
    int canPierce() const { return pierce_count < max_pierce; }

    // 실제 스프라이트 크기 리턴
    virtual int getSpriteWidth() const {
        return bullet_image ? bullet_image->x : 2;
    }
    virtual int getSpriteHeight() const {
        return bullet_image ? bullet_image->y : 2;
    }
};

#endif