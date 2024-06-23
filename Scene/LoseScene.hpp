#ifndef LOSESCENE_HPP
#define LOSESCENE_HPP
#include <allegro5/allegro_audio.h>
#include <memory>
#include "Engine/IScene.hpp"
class LoseScene final : public Engine::IScene {
private:
	struct Record {
		std::string name;
		int Rscore;
		std::string date;
		std::string time;
	};
	std::string name;
	Engine::IObject* pointer;
	std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> bgmInstance;
	static Record parseRecord(const std::string& line);
	static std::string recordToString(const Record& record);
	static bool compareRecords(const Record& a, const Record& b);
public:
	explicit LoseScene() = default;
	void Initialize() override;
	void Terminate() override;
	void BackOnClick(int stage);
	void OnKeyDown(int keyCode) override;
};

#endif // LOSESCENE_HPP





