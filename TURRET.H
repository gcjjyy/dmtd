#ifndef TURRET_H
#define TURRET_H

#include "game.h"
#include "IMAGES.H"

class Disk525Turret : public Turret {
private:
    sprites* turret_image;
    
public:
    Disk525Turret(int x, int y);
    virtual void draw();
    virtual Bullet* fire(Enemy* enemies[], int enemy_count);
};

class XTTurret : public Turret {
private:
    sprites* turret_image;

public:
    XTTurret(int x, int y);
    virtual void draw();
    virtual Bullet* fire(Enemy* enemies[], int enemy_count);
};

class ATTurret : public Turret {
private:
    sprites* turret_image;

public:
    ATTurret(int x, int y);
    virtual void draw();
    virtual Bullet* fire(Enemy* enemies[], int enemy_count);
};

class Disk35Turret : public Turret {
private:
    sprites* turret_image;

public:
    Disk35Turret(int x, int y);
    virtual void draw();
    virtual Bullet* fire(Enemy* enemies[], int enemy_count);
};

class Turret486 : public Turret {
private:
    sprites* turret_image;

public:
    Turret486(int x, int y);
    virtual void draw();
    virtual Bullet* fire(Enemy* enemies[], int enemy_count);
};

class CDTurret : public Turret {
private:
    sprites* turret_image;

public:
    CDTurret(int x, int y);
    virtual void draw();
    virtual Bullet* fire(Enemy* enemies[], int enemy_count);
};

class PentiumTurret : public Turret {
private:
    sprites* turret_image;

public:
    PentiumTurret(int x, int y);
    virtual void draw();
    virtual Bullet* fire(Enemy* enemies[], int enemy_count);
};

#endif