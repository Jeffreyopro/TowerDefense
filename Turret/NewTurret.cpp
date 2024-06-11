#include <allegro5/base.h>
#include <cmath>
#include <string>

#include "Engine/AudioHelper.hpp"
#include "Engine/Group.hpp"
#include "Bullet/NewBullet.hpp"
#include "NewTurret.hpp"
#include "Scene/PlayScene.hpp"
#include "Engine/Point.hpp"

const int NewTurret::Price = 500;
NewTurret::NewTurret(float x, float y) :
	Turret("play/tower-base.png", "play/turret-4.png", x, y, 1000, Price, 5) {
}
void NewTurret::CreateBullet() {
	Engine::Point diff = Engine::Point(cos(Rotation - ALLEGRO_PI / 2), sin(Rotation - ALLEGRO_PI / 2));
	float rotation = atan2(diff.y, diff.x);
	Engine::Point normalized = diff.Normalize();
	Engine::Point normal = Engine::Point(-normalized.y, normalized.x);
	// Change bullet position to the front of the gun barrel.
	getPlayScene()->BulletGroup->AddNewObject(new NewBullet(Position + normalized * 10 - normal * 6, diff, rotation, this));
	getPlayScene()->BulletGroup->AddNewObject(new NewBullet(Position + normalized * 10 + normal * 6, diff, rotation, this));
	AudioHelper::PlayAudio("missile.wav");
}
