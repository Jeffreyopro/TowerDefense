#ifndef FIREBULLET_HPP
#define FIREBULLET_HPP
#include "Bullet.hpp"

class Enemy;
class Turret;
namespace Engine {
struct Point;
}  // namespace Engine

class FireBullet : public Bullet {
public:
	explicit FireBullet(Engine::Point position, Engine::Point forwardDirection, float rotation);
	void OnExplode(Enemy* enemy) override;
};
#endif // FIREBULLET_HPP
