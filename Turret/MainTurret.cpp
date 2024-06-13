#include <allegro5/base.h>
#include <allegro5/color.h>
#include <allegro5/allegro_primitives.h>
#include <utility>
#include <cmath>
#include <string>
#include <iostream>

#include "Engine/AudioHelper.hpp"
#include "Engine/Group.hpp"
#include "Bullet/FireBullet.hpp"
#include "Bullet/LaserBullet.hpp"
#include "Bullet/Bullet5.hpp"
#include "Bullet/Bullet6.hpp"
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
const int MapWidth = 20, MapHeight = 13;
const int BlockSize = 64;
PlayScene* MainTurret::getPlayScene() {
	return dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetActiveScene());
}
MainTurret::MainTurret(float x, float y) :
	Engine::Sprite("play/turret-1.png", x, y) {
		Velocity = forwardDirection.Normalize() * speed;
		CollisionRadius = 30;
	}

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
			break;
		case 2:
			getPlayScene()->BulletGroup->AddNewObject(new LaserBullet(Position + normalized * 36 - normal * 6, diff, rotation));
			AudioHelper::PlayAudio("laser.wav");
			break;
		case 3:
			getPlayScene()->BulletGroup->AddNewObject(new Bullet5(Position + normalized * 10 - normal * 6, diff, rotation));
			AudioHelper::PlayAudio("laser.wav");
			break;
		case 4:
			getPlayScene()->BulletGroup->AddNewObject(new Bullet6(Position + normalized * 10 - normal * 6, diff, rotation));
			AudioHelper::PlayAudio("laser.wav");
			break;
		case 5:
			getPlayScene()->BulletGroup->AddNewObject(new MissileBullet(Position + normalized * 10 - normal * 6, diff, rotation));
			AudioHelper::PlayAudio("missile.wav");
			break;
		case 6:
			getPlayScene()->BulletGroup->AddNewObject(new NewBullet(Position + normalized * 10 - normal * 6, diff, rotation));
			AudioHelper::PlayAudio("missile.wav");
			break;
		default:
			getPlayScene()->BulletGroup->AddNewObject(new NewBullet(Position + normalized * 10 - normal * 6, diff, rotation));
			AudioHelper::PlayAudio("missile.wav");
			break;
	}
}
void MainTurret::Draw() const {
	Sprite::Draw();
	if (PlayScene::DebugMode) {
		// Draw collision radius.
		al_draw_circle(Position.x, Position.y, CollisionRadius, al_map_rgb(255, 0, 0), 2);
	}
}
void MainTurret::Update(float deltaTime) {
	float prepositionx = Position.x, prepositiony = Position.y;
	Velocity = forwardDirection.Normalize() * speed;
	forwardDirection = Engine::Point(0, 0);
	Sprite::Update(deltaTime);
	if(getPlayScene()->mapState[floor(Position.y/BlockSize)][floor(Position.x/BlockSize)] == 2)  {Position.x = prepositionx; Position.y = prepositiony;}
	if(Position.x < 10) Position.x = prepositionx; if(Position.x >= MapWidth*BlockSize-10) Position.x = prepositionx;
	if(Position.y < 10) Position.y = prepositiony; if(Position.y >= MapHeight*BlockSize-10) Position.y = prepositiony;;
	Engine::Point originRotation = Engine::Point(cos(Rotation - ALLEGRO_PI / 2), sin(Rotation - ALLEGRO_PI / 2));
	Engine::Point targetRotation = (Engine::GameEngine::GetInstance().GetMousePosition() - Position).Normalize();
	float maxRotateRadian = rotateRadian * deltaTime;
	float cosTheta = originRotation.Dot(targetRotation);
	// Might have floating-point precision error.
	if (cosTheta > 1) cosTheta = 1;
	else if (cosTheta < -1) cosTheta = -1;
	float radian = acos(cosTheta);
	Engine::Point rotation;
	if (fabs(radian) <= maxRotateRadian)
		rotation = targetRotation;
	else
		rotation = ((fabs(radian) - maxRotateRadian) * originRotation + maxRotateRadian * targetRotation) / radian;
	// Add 90 degrees (PI/2 radian), since we assume the image is oriented upward.
	Rotation = atan2(rotation.y, rotation.x) + ALLEGRO_PI / 2;
	// Shoot reload.
	reload -= deltaTime;
	if (reload <= 0 && clicked) {
		// shoot.
		reload = coolDown;
		CreateBullet();
	}
	clicked = 0;
}