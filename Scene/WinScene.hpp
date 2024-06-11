#ifndef WINSCENE_HPP
#define WINSCENE_HPP
#include <allegro5/allegro_audio.h>
#include "Engine/IScene.hpp"

class WinScene final : public Engine::IScene {
private:
	std::string name;
	float ticks;
	ALLEGRO_SAMPLE_ID bgmId;
	Engine::IObject* pointer;
public:
	explicit WinScene() = default;
	void Initialize() override;
	void Terminate() override;
	void Update(float deltaTime) override;
	void BackOnClick(int stage);
	void OnKeyDown(int keyCode) override;
};

#endif // WINSCENE_HPP