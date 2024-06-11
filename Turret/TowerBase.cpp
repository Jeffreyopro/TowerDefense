#include <allegro5/base.h>
#include <cmath>
#include <string>

#include "Engine/AudioHelper.hpp"
#include "Engine/Group.hpp"
#include "TowerBase.hpp"
#include "Scene/PlayScene.hpp"
#include "Engine/Point.hpp"

const int TowerBase::Price = 5;
TowerBase::TowerBase(float x, float y) :
	Turret("play/sand.png", "play/tower-base.png", x, y, 1000, Price, 5) {
}
void TowerBase::CreateBullet() {}
