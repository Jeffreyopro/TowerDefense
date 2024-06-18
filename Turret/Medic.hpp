#ifndef MEDIC_HPP
#define MEDIC_HPP
#include "Turret.hpp"
extern int MedicPrice;
class Medic: public Turret {
public:
    Medic(float x, float y);
	void CreateBullet() override;
};
#endif // MEDIC_HPP
