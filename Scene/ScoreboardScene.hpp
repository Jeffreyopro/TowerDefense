#ifndef SCOREBOARDSCENE_HPP
#define SCOREBOARDSCENE_HPP
#include <allegro5/allegro_audio.h>
#include <memory>
#include "Engine/IScene.hpp"

class ScoreboardScene final : public Engine::IScene {
private:
	std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> bgmInstance;
    std::vector<std::string> scoreboardData;
    std::vector<Engine::IObject*> iter;
    size_t currentPage = 0;
public:
	explicit ScoreboardScene() = default;
	void Initialize() override;
	void Terminate() override;
	void BackOnClick(int stage);
    void ReadRecord(std::string where);
};

#endif // SCOREBOARDSCENE_HPP