#ifndef SPEEDUP_HPP
#define SPEEDUP_HPP
#include "Turret.hpp"
extern int SpeedUpPrice;
class SpeedUp: public Turret {
public:
    SpeedUp(float x, float y);
	void CreateBullet() override;
};
#endif // SpeedUP_HPP
