#ifndef TOWERBASE_HPP
#define TOWERBASE_HPP
#include "Turret.hpp"

class TowerBase: public Turret {
public:
	static const int Price;
    TowerBase(float x, float y);
    void CreateBullet() override;
};
#endif // TOWERBASE_HPP
