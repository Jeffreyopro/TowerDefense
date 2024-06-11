#ifndef NEWBULLET_HPP
#define NEWBULLET_HPP
#include <allegro5/base.h>
#include <list>

#include "Bullet.hpp"

class Enemy;
class Turret;
namespace Engine {
struct Point;
}  // namespace Engine

class NewBullet : public Bullet {
protected:
	const float rotateRadian = 2 * ALLEGRO_PI;
	std::list<Bullet*>::iterator lockedBulletIterator;
public:
	explicit NewBullet(Engine::Point position, Engine::Point forwardDirection, float rotation);
	void Update(float deltaTime) override;
	void OnExplode(Enemy* enemy) override;
};
#endif // NEWBULLET_HPP
