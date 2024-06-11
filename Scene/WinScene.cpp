#include <functional>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <vector>
#include <algorithm>


#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "UI/Component/Image.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "PlayScene.hpp"
#include "Engine/Point.hpp"
#include "WinScene.hpp"
struct Record {
    std::string name;
    int Rscore;
    std::string date;
    std::string time;
};
// Function to parse a line into a Record struct
Record parseRecord(const std::string& line) {
    std::istringstream iss(line);
    Record record;
    iss >> record.name >> record.Rscore >> record.date >> record.time;
    return record;
}
// Function to convert a Record struct into a formatted string
std::string recordToString(const Record& record) {
    std::ostringstream oss;
    oss << record.name << " " << record.Rscore << " " << record.date << " " << record.time;
    return oss.str();
}
// Function to compare Records based on score in descending order
bool compareRecords(const Record& a, const Record& b) {
    return a.Rscore > b.Rscore;
}

void WinScene::Initialize() {
	ticks = 0;
	int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
	int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
	int halfW = w / 2;
	int halfH = h / 2;
	AddNewObject(new Engine::Image("win/benjamin-sad.png", halfW, halfH, 0, 0, 0.5, 0.5));
	AddNewObject(new Engine::Label("You Win!", "pirulen.ttf", 48, halfW, halfH / 4 -10, 255, 255, 255, 255, 0.5, 0.5));
    pointer = new Engine::Label(name , "pirulen.ttf", 48, halfW, halfH / 4  + 50, 255, 255, 255, 255, 0.5, 0.5);
    AddNewObject(pointer);
	Engine::ImageButton* btn;
	btn = new Engine::ImageButton("win/dirt.png", "win/floor.png", halfW - 200, halfH * 7 / 4 - 50, 400, 100);
	btn->SetOnClickCallback(std::bind(&WinScene::BackOnClick, this, 2));
	AddNewControlObject(btn);
	AddNewObject(new Engine::Label("Back", "pirulen.ttf", 48, halfW, halfH * 7 / 4, 0, 0, 0, 255, 0.5, 0.5));
	bgmId = AudioHelper::PlayAudio("win.wav");
}
void WinScene::Terminate() {
    std::ifstream inFile("Resource/scoreboard.txt");
    std::vector<Record> records;
    // Read existing records from the file
    std::string line;
    while (std::getline(inFile, line)) records.push_back(parseRecord(line));
    inFile.close();
    // Insert new data
    Record newRecord;
    newRecord.name = name;
    name = "";
    newRecord.Rscore = Score;
    // Get current date and time for new record
    auto now = std::chrono::system_clock::now();
    auto currentTime = std::chrono::system_clock::to_time_t(now);
    std::tm* timeInfo = std::localtime(&currentTime);
    std::ostringstream oss;
    oss << std::put_time(timeInfo, "%Y/%m/%d %H:%M:%S");
    size_t space_pos = oss.str().find(' ');
    newRecord.date = oss.str().substr(0, space_pos);  
    newRecord.time = oss.str().substr(space_pos + 1); 
    records.push_back(newRecord);
    // Sort records in descending order based on score
    std::sort(records.begin(), records.end(), compareRecords);
    // Write sorted records back to the file
    std::ofstream outFile("Resource/scoreboard.txt");
    for (const auto& record : records) outFile << recordToString(record) << std::endl;
    outFile.close();

	IScene::Terminate();
	AudioHelper::StopBGM(bgmId);
}
void WinScene::Update(float deltaTime) {
	ticks += deltaTime;
	if (ticks > 4 && ticks < 100 &&
		dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetScene("play"))->MapId == 2) {
		ticks = 100;
		bgmId = AudioHelper::PlayBGM("happy.ogg");
	}
}
void WinScene::BackOnClick(int stage) {
	// Change to select scene.
	Engine::GameEngine::GetInstance().ChangeScene("stage-select");
}
void WinScene::OnKeyDown(int keyCode) {
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
	int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
	int halfH = h / 2;
	IScene::OnKeyDown(keyCode);
    RemoveObject(pointer->GetObjectIterator());
	if (ALLEGRO_KEY_A <= keyCode && keyCode <= ALLEGRO_KEY_Z) name += (char)(keyCode+64);
	else if (ALLEGRO_KEY_0 <= keyCode && keyCode <= ALLEGRO_KEY_9) name += (char)(keyCode+21);
    else if(keyCode == ALLEGRO_KEY_BACKSPACE) if(name.size()>0) name.erase(name.size()-1);
    pointer = new Engine::Label(name , "pirulen.ttf", 48, halfW, halfH / 4 + 50, 255, 255, 255, 255, 0.5, 0.5);
    AddNewObject(pointer);
}