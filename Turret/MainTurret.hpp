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
    int clicked = 0;
	float speed = 20;
    int level = 1;
    float coolDown = 0.1;
    float reload = 0;
    float rotateRadian = 2 * ALLEGRO_PI;
    MainTurret(float x, float y);
    void CreateBullet();
    PlayScene* getPlayScene();
    void Draw() const;
    void Update(float deltaTime);
    Engine::Point forwardDirection = Engine::Point(0, 0);
};
#endif // MAINTURRET_HPP
