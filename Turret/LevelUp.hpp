#ifndef LEVELUP_HPP
#define LEVELUP_HPP
#include "Turret.hpp"
extern int LevelUpPrice;
class LevelUp: public Turret {
public:
    LevelUp(float x, float y);
	void CreateBullet() override;
};
#endif // LEVELUP_HPP
