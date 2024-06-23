#ifndef WINSCENE_HPP
#define WINSCENE_HPP
#include <allegro5/allegro_audio.h>
#include "Engine/IScene.hpp"
class WinScene final : public Engine::IScene {
	private:
	struct Record {
		std::string name;
		int Rscore;
		std::string date;
		std::string time;
	};
	std::string name;
	float ticks;
	ALLEGRO_SAMPLE_ID bgmId;
	Engine::IObject* pointer;
	static Record parseRecord(const std::string& line);
	static std::string recordToString(const Record& record);
	static bool compareRecords(const Record& a, const Record& b);
public:
	explicit WinScene() = default;
	void Initialize() override;
	void Terminate() override;
	void Update(float deltaTime) override;
	void BackOnClick(int stage);
	void OnKeyDown(int keyCode) override;
};

#endif // WINSCENE_HPP
