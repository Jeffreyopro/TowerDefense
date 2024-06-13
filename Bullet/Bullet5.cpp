#include <allegro5/base.h>
#include <random>
#include <string>

#include "UI/Animation/DirtyEffect.hpp"
#include "Enemy/Enemy.hpp"
#include "Bullet5.hpp"
#include "Engine/Group.hpp"
#include "Scene/PlayScene.hpp"
#include "Engine/Point.hpp"

class Turret;

Bullet5::Bullet5(Engine::Point position, Engine::Point forwardDirection, float rotation) :
	Bullet("play/bullet-5.png", 300, 3, position, forwardDirection, rotation - ALLEGRO_PI / 2) {
	// TODO: [CUSTOM-TOOL] You can imitate the 2 files: 'FireBullet.hpp', 'FireBullet.cpp' to create a new bullet.
}
void Bullet5::OnExplode(Enemy* enemy) {
	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_int_distribution<std::mt19937::result_type> dist(2, 5);
	getPlayScene()->GroundEffectGroup->AddNewObject(new DirtyEffect("play/dirty-1.png", dist(rng), enemy->Position.x, enemy->Position.y));
}

