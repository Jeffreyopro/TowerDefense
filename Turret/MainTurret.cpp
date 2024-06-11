#include <allegro5/base.h>
#include <allegro5/color.h>
#include <allegro5/allegro_primitives.h>
#include <utility>
#include <cmath>
#include <string>

#include "Engine/AudioHelper.hpp"
#include "Engine/Group.hpp"
#include "Bullet/FireBullet.hpp"
#include "Bullet/LaserBullet.hpp"
#include "Bullet/MissileBullet.hpp"
#include "Bullet/NewBullet.hpp"
#include "MainTurret.hpp"
#include "Scene/PlayScene.hpp"
#include "Engine/Point.hpp"
#include "Enemy/Enemy.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Group.hpp"
#include "Engine/IObject.hpp"
#include "Engine/IScene.hpp"

PlayScene* MainTurret::getPlayScene() {
	return dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetActiveScene());
}
MainTurret::MainTurret(float x, float y) :
	Engine::Sprite("play/turret-1.png", x, y), coolDown(5) {}

void MainTurret::CreateBullet() {
	Engine::Point diff = Engine::Point(cos(Rotation - ALLEGRO_PI / 2), sin(Rotation - ALLEGRO_PI / 2));
	float rotation = atan2(diff.y, diff.x);
	Engine::Point normalized = diff.Normalize();
	Engine::Point normal = Engine::Point(-normalized.y, normalized.x);
	// Change bullet position to the front of the gun barrel.
	switch(level) {
		case 1:
			getPlayScene()->BulletGroup->AddNewObject(new FireBullet(Position + normalized * 36, diff, rotation));
			AudioHelper::PlayAudio("gun.wav");
		case 2:
			getPlayScene()->BulletGroup->AddNewObject(new LaserBullet(Position + normalized * 36 - normal * 6, diff, rotation));
			AudioHelper::PlayAudio("laser.wav");
		case 3:
			getPlayScene()->BulletGroup->AddNewObject(new MissileBullet(Position + normalized * 10 - normal * 6, diff, rotation));
			AudioHelper::PlayAudio("missile.wav");
		case 4:
			getPlayScene()->BulletGroup->AddNewObject(new NewBullet(Position + normalized * 10 - normal * 6, diff, rotation));
			AudioHelper::PlayAudio("missile.wav");
	}
}
