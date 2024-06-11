#include <functional>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "UI/Component/Image.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "ScoreboardScene.hpp"
#include "PlayScene.hpp"
#include "Engine/Point.hpp"

void ScoreboardScene::Initialize() {
	int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
	int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
	int halfW = w / 2;
	int halfH = h / 2;
	AddNewObject(new Engine::Label("Scoreboard", "pirulen.ttf", 48, halfW, halfH / 4 + 10, 255, 255, 255, 255, 0.5, 0.5));
	Engine::ImageButton* btn;
	btn = new Engine::ImageButton("win/dirt.png", "win/floor.png", halfW - 200, halfH * 7 / 4 - 50, 400, 100);
	btn->SetOnClickCallback(std::bind(&ScoreboardScene::BackOnClick, this, 2));
	AddNewControlObject(btn);
	AddNewObject(new Engine::Label("Back", "pirulen.ttf", 48, halfW, halfH * 7 / 4, 0, 0, 0, 255, 0.5, 0.5));

    btn = new Engine::ImageButton("win/dirt.png", "win/floor.png", halfW + 300, halfH * 7 / 4 - 50, 400, 100);
	btn->SetOnClickCallback(std::bind(&ScoreboardScene::ReadRecord, this, "next"));
	AddNewControlObject(btn);
	AddNewObject(new Engine::Label("Next page", "pirulen.ttf", 48, halfW + 500, halfH * 7 / 4, 0, 0, 0, 255, 0.5, 0.5));

    btn = new Engine::ImageButton("win/dirt.png", "win/floor.png", halfW - 700, halfH * 7 / 4 - 50, 400, 100);
	btn->SetOnClickCallback(std::bind(&ScoreboardScene::ReadRecord, this, "prev"));
	AddNewControlObject(btn);
	AddNewObject(new Engine::Label("Prev page", "pirulen.ttf", 48, halfW - 500, halfH * 7 / 4, 0, 0, 0, 255, 0.5, 0.5));

    ScoreboardScene::ReadRecord("default");

    bgmInstance = AudioHelper::PlaySample("select.ogg", true, AudioHelper::BGMVolume);
}
void ScoreboardScene::Terminate() {
    currentPage = 0;
    iter.clear();
    scoreboardData.clear();
	AudioHelper::StopSample(bgmInstance);
	bgmInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
	IScene::Terminate();
}
void ScoreboardScene::BackOnClick(int stage) {
	// Change to select scene.
	Engine::GameEngine::GetInstance().ChangeScene("stage-select");
}
void ScoreboardScene::ReadRecord(std::string where) {
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
	int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
	int halfW = w / 2;
	int halfH = h / 2;
    if(where == "default"){
        std::string filename = std::string("Resource/scoreboard") + ".txt";
        std::ifstream fin(filename);
        if (!fin.is_open()) {
            std::cerr << "Error: Couldn't open the file '" << filename << "'\n";
            return;
        }
        std::string line;
        while (std::getline(fin, line)) {
            std::string name, score, date, time;
            std::istringstream iss(line);
            if (iss >> name >> score >> date >> time) {
                scoreboardData.push_back(std::string(name + " " + score + " " + date + " " + time));
            } else {
                std::cerr << "Error: Failed to read name and score from line: " << line << std::endl;
            }
        }
        fin.close();
        // Display the first page of data (first 5 sets)
        size_t startIdx = currentPage * 5;
        size_t endIdx = startIdx + 5;
        if(endIdx > scoreboardData.size()) endIdx = scoreboardData.size();
        int height = 100, count = 0;
        for (size_t i = startIdx; i < endIdx; ++i) {
            iter.push_back(new Engine::Label(scoreboardData[i], "pirulen.ttf", 48, halfW, halfH / 4 + 10 + height, 255, 255, 255, 255, 0.5, 0.5));
            AddNewObject(iter[count]);
            height += 50;
            count++;
        }
        // Increment page number for next page
        if((currentPage+1)*5 < scoreboardData.size()) ++currentPage;
    }
    else if(where =="next"){
        // Display the first page of data (first 5 sets)
        for(int i=0;i<iter.size();i++) RemoveObject(iter[i]->GetObjectIterator());
        iter.clear();
        size_t startIdx = currentPage * 5;
        size_t endIdx = startIdx + 5;
        if(endIdx > scoreboardData.size()) endIdx = scoreboardData.size();
        int height = 100, count = 0;
        for (size_t i = startIdx; i < endIdx; ++i) {
            iter.push_back(new Engine::Label(scoreboardData[i], "pirulen.ttf", 48, halfW, halfH / 4 + 10 + height, 255, 255, 255, 255, 0.5, 0.5));
            AddNewObject(iter[count]);
            height += 50;
            count++;
        }
        // Increment page number for next page
        if((currentPage+1)*5 < scoreboardData.size()) ++currentPage;
    }
    else if(where == "prev"){
        // Display the first page of data (first 5 sets)
        for(int i=0;i<iter.size();i++) RemoveObject(iter[i]->GetObjectIterator());
        iter.clear();
        // Decrement page number for prev page
        if(currentPage > 0) --currentPage;
        size_t startIdx = currentPage * 5;
        size_t endIdx = startIdx + 5;
        if(endIdx > scoreboardData.size()) endIdx = scoreboardData.size();
        int height = 100, count = 0;
        for (size_t i = startIdx; i < endIdx; ++i) {
            iter.push_back(new Engine::Label(scoreboardData[i], "pirulen.ttf", 48, halfW, halfH / 4 + 10 + height, 255, 255, 255, 255, 0.5, 0.5));
            AddNewObject(iter[count]);
            height += 50;
            count++;
        }
    }
}

