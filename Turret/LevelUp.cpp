#include <allegro5/base.h>
#include <cmath>
#include <string>

#include "Engine/AudioHelper.hpp"
#include "Bullet/FireBullet.hpp"
#include "Engine/Group.hpp"
#include "LevelUp.hpp"
#include "Scene/PlayScene.hpp"
#include "Engine/Point.hpp"

int LevelUpPrice = 100;
LevelUp::LevelUp(float x, float y) :
	// TODO: [CUSTOM-TOOL] You can imitate the 2 files: 'LevelUp.hpp', 'LevelUp.cpp' to create a new turret.
	Turret("win/dirt.png", "play/LevelUp.png", x, y, 200, LevelUpPrice, 0.5) {
	// Move center downward, since we the turret head is slightly biased upward.
	Anchor.y += 8.0f / GetBitmapHeight();
}
void LevelUp::CreateBullet() {
	Engine::Point diff = Engine::Point(cos(Rotation - ALLEGRO_PI / 2), sin(Rotation - ALLEGRO_PI / 2));
	float rotation = atan2(diff.y, diff.x);
	Engine::Point normalized = diff.Normalize();
	// Change bullet position to the front of the gun barrel.
	getPlayScene()->BulletGroup->AddNewObject(new FireBullet(Position + normalized * 36, diff, rotation));
	AudioHelper::PlayAudio("gun.wav");
}
