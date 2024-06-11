#ifndef MAINTURRET_HPP
#define MAINTURRET_HPP
#include <allegro5/base.h>
#include <list>
#include <string>
#include "Engine/Sprite.hpp"
class Enemy;
class PlayScene;

class MainTurret : public Engine::Sprite{
public:
	float speed = 20;
    int level = 1;
    float coolDown;
    float reload = 0;
    float rotateRadian = 2 * ALLEGRO_PI;
    MainTurret(float x, float y);
    void CreateBullet();
    PlayScene* getPlayScene();
};
#endif // MAINTURRET_HPP
