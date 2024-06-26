#include <allegro5/allegro.h>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <functional>
#include <vector>
#include <queue>
#include <string>
#include <memory>
#include <iostream>
#include <random>

#include "Engine/AudioHelper.hpp"
#include "UI/Animation/DirtyEffect.hpp"
#include "Enemy/Enemy.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Group.hpp"
#include "UI/Component/Label.hpp"
#include "Turret/LaserTurret.hpp"
#include "Turret/LevelUp.hpp"
#include "Turret/Medic.hpp"
#include "Turret/SpeedUp.hpp"
#include "Turret/MissileTurret.hpp"
#include "Turret/TowerBase.hpp"
#include "Turret/MainTurret.hpp"
#include "UI/Animation/Plane.hpp"
#include "Enemy/PlaneEnemy.hpp"
#include "PlayScene.hpp"
#include "Engine/Resources.hpp"
#include "Enemy/SoldierEnemy.hpp"
#include "Enemy/TankEnemy.hpp"
#include "Enemy/NewEnemy.hpp"
#include "Turret/TurretButton.hpp"
#include "Engine/Collider.hpp"
int generateRandomNumber();
int Score;
float speedup; float speedupbuff;
int random_spawn = 0;
int KeyCodeDetect[4]; // wsad
TurretButton* storage[3]; // store btn pointer
bool PlayScene::DebugMode = false;
const std::vector<Engine::Point> PlayScene::directions = { Engine::Point(-1, 0), Engine::Point(0, -1), Engine::Point(1, 0), Engine::Point(0, 1) };
const int PlayScene::MapWidth = 20, PlayScene::MapHeight = 13;
const int PlayScene::BlockSize = 64;
const float PlayScene::DangerTime = 7.61;
Engine::Point SpawnGridPoint = Engine::Point(-1, 0);
const Engine::Point PlayScene::EndGridPoint = Engine::Point(MapWidth, MapHeight - 1);
const std::vector<int> PlayScene::code = { ALLEGRO_KEY_UP, ALLEGRO_KEY_UP, ALLEGRO_KEY_DOWN, ALLEGRO_KEY_DOWN,
									ALLEGRO_KEY_LEFT, ALLEGRO_KEY_LEFT, ALLEGRO_KEY_RIGHT, ALLEGRO_KEY_RIGHT,
									ALLEGRO_KEY_B, ALLEGRO_KEY_A, ALLEGRO_KEYMOD_SHIFT, ALLEGRO_KEY_ENTER };
Engine::Point PlayScene::GetClientSize() {
	return Engine::Point(MapWidth * BlockSize, MapHeight * BlockSize);
}
MainTurret* main_turret; 
void PlayScene::Initialize() {
	// TODO: [HACKATHON-3-BUG] (1/5): There's a bug in this file, which crashes the game when you lose. Try to find it.
	// TODO: [HACKATHON-3-BUG] (2/5): Find out the cheat code to test.
    // TODO: [HACKATHON-3-BUG] (2/5): It should generate a Plane, and add 10000 to the money, but it doesn't work now.
	mapState.clear();
	keyStrokes.clear();
	memset(KeyCodeDetect, 0, sizeof(KeyCodeDetect));
	ticks = 0;
	deathCountDown = -1;
	lives = 5;
	money = 1000;
	speedup = 0; 
	speedupbuff = 0;
	Score = 0;
	SpeedMult = 1;
	LevelUpPrice = 100;
	SpeedUpPrice = 50;
	// Add groups from bottom to top.
	main_turret = new MainTurret(500, 500);
	AddNewObject(TileMapGroup = new Group());
	AddNewObject(GroundEffectGroup = new Group());
	AddNewObject(DebugIndicatorGroup = new Group());
	AddNewObject(TowerGroup = new Group());
	AddNewObject(EnemyGroup = new Group());
	AddNewObject(BulletGroup = new Group());
	AddNewObject(EffectGroup = new Group());
	// Should support buttons.
	AddNewControlObject(UIGroup = new Group());
	ReadMap();
	ReadEnemyWave();
	mapDistance = CalculateBFSDistance();
	ConstructUI();
	imgTarget = new Engine::Image("play/target.png", 0, 0);
	imgTarget->Visible = false;
	preview = nullptr;
	UIGroup->AddNewObject(imgTarget);
	// Preload Lose Scene
	deathBGMInstance = Engine::Resources::GetInstance().GetSampleInstance("astronomia.ogg");
	Engine::Resources::GetInstance().GetBitmap("lose/benjamin-happy.png");
	// Start BGM.
	bgmId = AudioHelper::PlayBGM("play.ogg");
}
void PlayScene::Terminate() {
	memset(KeyCodeDetect, 0, sizeof(KeyCodeDetect));
	AudioHelper::StopBGM(bgmId);
	AudioHelper::StopSample(deathBGMInstance);
	deathBGMInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
	delete main_turret;
	IScene::Terminate();
}
void PlayScene::Update(float deltaTime) {
	if(speedupbuff>=2) speedupbuff = 0;
	else speedupbuff += speedup;
	// If we use deltaTime directly, then we might have Bullet-through-paper problem.
	// Reference: Bullet-Through-Paper
	main_turret->Update(deltaTime);
	if (SpeedMult == 0)
		deathCountDown = -1;
	else if (deathCountDown != -1)
		SpeedMult = 1;
	// Calculate danger zone.
	std::vector<float> reachEndTimes;
	for (auto& it : EnemyGroup->GetObjects()) {
		reachEndTimes.push_back(dynamic_cast<Enemy*>(it)->reachEndTime);
	}
	// Can use Heap / Priority-Queue instead. But since we won't have too many enemies, sorting is fast enough.
	std::sort(reachEndTimes.begin(), reachEndTimes.end());
	float newDeathCountDown = -1;
	int danger = lives;
	for (auto& it : reachEndTimes) {
		if (it <= DangerTime) {
			danger--;
			if (danger <= 0) {
				// Death Countdown
				float pos = DangerTime - it;
				if (it > deathCountDown) {
					// Restart Death Count Down BGM.
					AudioHelper::StopSample(deathBGMInstance);
					if (SpeedMult != 0)
						deathBGMInstance = AudioHelper::PlaySample("astronomia.ogg", false, AudioHelper::BGMVolume, pos);
				}
				float alpha = pos / DangerTime;
				alpha = std::max(0, std::min(255, static_cast<int>(alpha * alpha * 255)));
				dangerIndicator->Tint = al_map_rgba(255, 255, 255, alpha);
				newDeathCountDown = it;
				break;
			}
		}
	}
	deathCountDown = newDeathCountDown;
	for (auto& it : EnemyGroup->GetObjects()) {
		Enemy* enemy = dynamic_cast<Enemy*>(it);
		if (!enemy->Visible)
			continue;
		if (Engine::Collider::IsCircleOverlap(main_turret->Position, main_turret->CollisionRadius, enemy->Position, enemy->CollisionRadius)) {
			enemy->Hit(100000);
			Hit();
			return;
	}
	dynamic_cast<Enemy*>(it)->UpdatePath(mapDistance);
	}
	if (SpeedMult == 0)
		AudioHelper::StopSample(deathBGMInstance);
	if (deathCountDown == -1 && lives > 0) {
		AudioHelper::StopSample(deathBGMInstance);
		dangerIndicator->Tint.a = 0;
	}
	if (SpeedMult == 0)
		deathCountDown = -1;
	for (int i = 0; i < SpeedMult; i++) {
		IScene::Update(deltaTime);
		// Check if we should create new enemy.
		ticks += deltaTime;
		if (enemyWaveData.empty()) {
			if (EnemyGroup->GetObjects().empty()) {
				// Free resources.
				/*delete TileMapGroup;
				delete GroundEffectGroup;
				delete DebugIndicatorGroup;
				delete TowerGroup;
				delete EnemyGroup;
				delete BulletGroup;
				delete EffectGroup;
				delete UIGroup;
				delete imgTarget;*/
				Engine::GameEngine::GetInstance().ChangeScene("win");
			}
			continue;
		}
		auto current = enemyWaveData.front();
		if (ticks < current.second)
			continue;
		ticks -= current.second;
		enemyWaveData.pop_front();
		random_spawn = generateRandomNumber();
		switch(random_spawn){
			case 0:
				PlayScene::SpawnGridPoint = Engine::Point(-1, 0);
				break;
			case 1:
				PlayScene::SpawnGridPoint = Engine::Point(-1, MapHeight);
				break;
			case 2:
				PlayScene::SpawnGridPoint = Engine::Point(MapWidth, 0);
				break;
			case 3:
				PlayScene::SpawnGridPoint = Engine::Point(MapWidth, MapHeight);
				break;
		}
		Engine::Point SpawnCoordinate = Engine::Point(SpawnGridPoint.x * BlockSize + BlockSize / 2, SpawnGridPoint.y * BlockSize + BlockSize / 2);
		Enemy* enemy;
		for(int i=0 ;  i< 1+speedupbuff;i++){
		switch (current.first) {
		case 1:
			EnemyGroup->AddNewObject(enemy = new SoldierEnemy(SpawnCoordinate.x, SpawnCoordinate.y));
			break;
		case 2:
			EnemyGroup->AddNewObject(enemy = new PlaneEnemy(SpawnCoordinate.x, SpawnCoordinate.y));
			break;
		case 3:
			EnemyGroup->AddNewObject(enemy = new TankEnemy(SpawnCoordinate.x, SpawnCoordinate.y));
			break;
		case 4:
			EnemyGroup->AddNewObject(enemy = new NewEnemy(SpawnCoordinate.x, SpawnCoordinate.y));
			break;
        // TODO: [CUSTOM-ENEMY]: You need to modify 'Resource/enemy1.txt', or 'Resource/enemy2.txt' to spawn the 4th enemy.
        //         The format is "[EnemyId] [TimeDelay] [Repeat]".
        // TODO: [CUSTOM-ENEMY]: Enable the creation of the enemy.
		default:
			continue;
		}
		enemy->UpdatePath(mapDistance);
		// Compensate the time lost.
		enemy->Update(ticks);}
	}
	mapDistance = CalculateBFSDistance();
	if (preview) {
		preview->Position = Engine::GameEngine::GetInstance().GetMousePosition();
		// To keep responding when paused.
		preview->Update(deltaTime);
	}
}	
void PlayScene::Draw() const {
	IScene::Draw();
	main_turret->Draw();
	if (DebugMode) {
		// Draw reverse BFS distance on all reachable blocks.
		for (int i = 0; i < MapHeight; i++) {
			for (int j = 0; j < MapWidth; j++) {
				if (mapDistance[i][j] != -1) {
					// Not elegant nor efficient, but it's quite enough for debugging.
					Engine::Label label(std::to_string(mapDistance[i][j]), "pirulen.ttf", 32, (j + 0.5) * BlockSize, (i + 0.5) * BlockSize);
					label.Anchor = Engine::Point(0.5, 0.5);
					label.Draw();
				}
			}
		}
	}
}
void PlayScene::OnMouseDown(int button, int mx, int my) {
	if ((button & 1) && !imgTarget->Visible && preview) {
		// Cancel turret construct.
		UIGroup->RemoveObject(preview->GetObjectIterator());
		preview = nullptr;
	}
	main_turret->clicked = 1;
	IScene::OnMouseDown(button, mx, my);
}
void PlayScene::OnMouseMove(int mx, int my) {
	IScene::OnMouseMove(mx, my);
	const int x = mx / BlockSize;
	const int y = my / BlockSize;
	if (!preview || x < 0 || x >= MapWidth || y < 0 || y >= MapHeight) {
		imgTarget->Visible = false;
		return;
	}
	imgTarget->Visible = true;
	imgTarget->Position.x = x * BlockSize;
	imgTarget->Position.y = y * BlockSize;
}
void PlayScene::OnMouseUp(int button, int mx, int my) {
	IScene::OnMouseUp(button, mx, my);
	if (!imgTarget->Visible)
		return;
	const int x = mx / BlockSize;
	const int y = my / BlockSize;
	if (button & 1) {
		if (mapState[y][x] != TILE_OCCUPIED) {
			if (!preview)
				return;
			// Check if valid.
			if (!CheckSpaceValid(x, y)) {
				Engine::Sprite* sprite;
				GroundEffectGroup->AddNewObject(sprite = new DirtyEffect("play/target-invalid.png", 1, x * BlockSize + BlockSize / 2, y * BlockSize + BlockSize / 2));
				sprite->Rotation = 0;
				return;
			}
			// Purchase.
			EarnMoney(-preview->GetPrice());
			// Remove Preview.
			preview->GetObjectIterator()->first = false;
			UIGroup->RemoveObject(preview->GetObjectIterator());
			// Construct real turret.
			preview->Position.x = x * BlockSize + BlockSize / 2;
			preview->Position.y = y * BlockSize + BlockSize / 2;
			if(preview->GetPrice()==100) preview->Enabled = false;
			else preview->Enabled = true;
			preview->Preview = false;
			preview->Tint = al_map_rgba(255, 255, 255, 255);
			TowerGroup->AddNewObject(preview);
			// To keep responding when paused.
			preview->Update(0);
			// Remove Preview.
			preview = nullptr;

			mapState[y][x] = TILE_OCCUPIED;
			OnMouseMove(mx, my);
		}
	}
}
void PlayScene::OnKeyDown(int keyCode) {
	IScene::OnKeyDown(keyCode);
	if (keyCode == ALLEGRO_KEY_TAB) {
		DebugMode = !DebugMode;
	}
	else {
		keyStrokes.push_back(keyCode);
		//std::printf("%d\n", keyStrokes.size());
		if (keyStrokes.size() > code.size())
			keyStrokes.pop_front();
		if (keyCode == ALLEGRO_KEY_ENTER && keyStrokes.size() == code.size()) {
			auto it = keyStrokes.begin();
			for (int c : code) {
				if (!((*it == c) ||
					(c == ALLEGRO_KEYMOD_SHIFT &&
					(*it == ALLEGRO_KEY_LSHIFT || *it == ALLEGRO_KEY_RSHIFT))))
					return;
				++it;
			}
			// cheat mode
			PlayScene::EarnMoney(10000);
			EffectGroup->AddNewObject(new Plane());
		}
	}
	if (keyCode == ALLEGRO_KEY_R) {
		// Hotkey for LevelUp.
		UIBtnClicked(0);
	}
	else if (keyCode == ALLEGRO_KEY_F) {
		// Hotkey for SpeedUp.
		UIBtnClicked(1);
	}
	else if (keyCode == ALLEGRO_KEY_V) {
		// Hotkey for Medic.
		UIBtnClicked(2);
	}
	else if (keyCode == ALLEGRO_KEY_E) {
		// Hotkey for TowerBase.
		UIBtnClicked(3);
	}
	else if (keyCode == ALLEGRO_KEY_W) {
		KeyCodeDetect[0] = 1; 
		main_turret->forwardDirection = Engine::Point(-KeyCodeDetect[2]+KeyCodeDetect[3], -KeyCodeDetect[0]+KeyCodeDetect[1]);
	}
	else if (keyCode == ALLEGRO_KEY_S) {
		KeyCodeDetect[1] = 1;
		main_turret->forwardDirection = Engine::Point(-KeyCodeDetect[2]+KeyCodeDetect[3], -KeyCodeDetect[0]+KeyCodeDetect[1]);
	}
	else if (keyCode == ALLEGRO_KEY_A) {
		KeyCodeDetect[2] = 1;
		main_turret->forwardDirection = Engine::Point(-KeyCodeDetect[2]+KeyCodeDetect[3], -KeyCodeDetect[0]+KeyCodeDetect[1]);
	}
	else if (keyCode == ALLEGRO_KEY_D) {
		KeyCodeDetect[3] = 1;
		main_turret->forwardDirection = Engine::Point(-KeyCodeDetect[2]+KeyCodeDetect[3], -KeyCodeDetect[0]+KeyCodeDetect[1]);
	}
	else if (keyCode == ALLEGRO_KEY_P) {
		main_turret->forwardDirection = Engine::Point(-KeyCodeDetect[2]+KeyCodeDetect[3], -KeyCodeDetect[0]+KeyCodeDetect[1]);
	}
	// TODO: [CUSTOM-TURRET]: Make specific key to create the turret.
	else if (keyCode >= ALLEGRO_KEY_0 && keyCode <= ALLEGRO_KEY_9) {
		// Hotkey for Speed up.
		SpeedMult = keyCode - ALLEGRO_KEY_0;
	}
}
void PlayScene::OnKeyUp(int keyCode) {
	if (keyCode == ALLEGRO_KEY_W) {
		KeyCodeDetect[0] = 0; 
	}
	else if (keyCode == ALLEGRO_KEY_S) {
		KeyCodeDetect[1] = 0;
	}
	else if (keyCode == ALLEGRO_KEY_A) {
		KeyCodeDetect[2] = 0;
	}
	else if (keyCode == ALLEGRO_KEY_D) {
		KeyCodeDetect[3] = 0;
	}
}
void PlayScene::Hit() {
	lives--;
	UILives->Text = std::string("Life ") + std::to_string(lives);
	if (lives <= 0) {
		// MARK lose-scene
		Engine::GameEngine::GetInstance().ChangeScene("lose");
	}
}
int PlayScene::GetMoney() const {
	return money;
}
void PlayScene::EarnMoney(int money) {
	if(money>0) EarnScore(money*3);
	this->money += money;
	UIMoney->Text = std::string("$") + std::to_string(this->money);
}
void PlayScene::EarnScore(int score) {
	Score += score;
	UIScore->Text = std::string("score->") + std::to_string(Score);
}
void PlayScene::ReadMap() {
	std::string filename = std::string("Resource/map") + std::to_string(MapId) + ".txt";
	// Read map file.
	char c;
	std::vector<bool> mapData;
	std::ifstream fin(filename);
	while (fin >> c) {
		switch (c) {
		case '0': mapData.push_back(false); break;
		case '1': mapData.push_back(true); break;
		case '\n':
		case '\r':
			if (static_cast<int>(mapData.size()) / MapWidth != 0)
				throw std::ios_base::failure("Map data is corrupted.");
			break;
		default: throw std::ios_base::failure("Map data is corrupted.");
		}
	}
	fin.close();
	// Validate map data.
	if (static_cast<int>(mapData.size()) != MapWidth * MapHeight)
		throw std::ios_base::failure("Map data is corrupted.");
	// Store map in 2d array.
	mapState = std::vector<std::vector<TileType>>(MapHeight, std::vector<TileType>(MapWidth));
	for (int i = 0; i < MapHeight; i++) {
		for (int j = 0; j < MapWidth; j++) {
			const int num = mapData[i * MapWidth + j];
			mapState[i][j] = num ? TILE_FLOOR : TILE_DIRT;
			if (num)
				TileMapGroup->AddNewObject(new Engine::Image("play/floor.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
			else
				TileMapGroup->AddNewObject(new Engine::Image("play/dirt.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
		}
	}
}
void PlayScene::ReadEnemyWave() {
    // TODO: [HACKATHON-3-BUG] (3/5): Trace the code to know how the enemies are created.
    // TODO: [HACKATHON-3-BUG] (3/5): There is a bug in these files, which let the game only spawn the first enemy, try to fix it
    std::string filename = std::string("Resource/enemy") + std::to_string(MapId) + ".txt";
	// Read enemy file.
	float type, wait, repeat;
	enemyWaveData.clear();
	std::ifstream fin(filename);
    while (fin >> type >> wait >> repeat) {
        if (fin.fail()) {
            std::cerr << "Error: Failed to read enemy data from file." << std::endl;
            break;
        }
        for (int i = 0; i < repeat; i++) {
            enemyWaveData.emplace_back(type, wait);
            std::cout << "Added enemy of type " << type << " with wait " << wait << std::endl;
	}
	}
	fin.close();
}
void PlayScene::ConstructUI() {
	// Background
	UIGroup->AddNewObject(new Engine::Image("play/sand.png", 1280, 0, 320, 832));
	// Text
	UIGroup->AddNewObject(new Engine::Label(std::string("Stage ") + std::to_string(MapId), "pirulen.ttf", 32, 1294, 0));
	UIGroup->AddNewObject(UIMoney = new Engine::Label(std::string("$") + std::to_string(money), "pirulen.ttf", 24, 1294, 48));
	UIGroup->AddNewObject(UIScore = new Engine::Label(std::string("score->") + std::to_string(Score), "pirulen.ttf", 24, 1400, 48));
	UIGroup->AddNewObject(UILives = new Engine::Label(std::string("Life ") + std::to_string(lives), "pirulen.ttf", 24, 1294, 88));
	// Button 1 
	auto btn = new TurretButton("play/floor.png", "play/dirt.png",
		Engine::Sprite("win/dirt.png", 1294, 136, 0, 0, 0, 0),
		Engine::Sprite("play/LevelUp.png", 1294, 136, 0, 0, 0, 0)
		, 1294, 136, LevelUpPrice);
	// Reference: Class Member Function Pointer and std::bind.
	btn->SetOnClickCallback(std::bind(&PlayScene::UIBtnClicked, this, 0));
	UIGroup->AddNewControlObject(btn);
	storage[0] = btn;

	// Button 2
	btn = new TurretButton("play/floor.png", "play/dirt.png",
		Engine::Sprite("play/dirt.png", 1370, 136, 0, 0, 0, 0),
		Engine::Sprite("play/SpeedUp.png", 1370, 136, 0, 0, 0, 0)
		, 1370, 136, SpeedUpPrice);
	btn->SetOnClickCallback(std::bind(&PlayScene::UIBtnClicked, this, 1));
	UIGroup->AddNewControlObject(btn);
	storage[1] = btn;
	// Button 3
	btn = new TurretButton("play/floor.png", "play/dirt.png",
		Engine::Sprite("play/dirt.png", 1446, 136, 0, 0, 0, 0),
		Engine::Sprite("play/Medic.png", 1446, 136, 0, 0, 0, 0)
		, 1446, 136, MedicPrice);
	btn->SetOnClickCallback(std::bind(&PlayScene::UIBtnClicked, this, 2));
	UIGroup->AddNewControlObject(btn); 
	
	// TODO: [CUSTOM-TURRET]: Create a button to support constructing the turret.
	btn = new TurretButton("play/floor.png", "play/dirt.png",
	Engine::Sprite("play/sand.png", 1522, 136, 0, 0, 0, 0),
	Engine::Sprite("play/tower-base.png", 1522, 136, 0, 0, 0, 0)
	, 1522, 136, TowerBase::Price);
	btn->SetOnClickCallback(std::bind(&PlayScene::UIBtnClicked, this, 3));
	UIGroup->AddNewControlObject(btn);

	int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
	int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
	int shift = 135 + 25;
	dangerIndicator = new Engine::Sprite("play/benjamin.png", w - shift, h - shift);
	dangerIndicator->Tint.a = 0;
	UIGroup->AddNewObject(dangerIndicator);
}

void PlayScene::UIBtnClicked(int id) {
	if (preview) {
		UIGroup->RemoveObject(preview->GetObjectIterator());
	}
    // TODO: [CUSTOM-TURRET]: On callback, create the turret.
	if (id == 0 && money >= LevelUpPrice) {
		speedup += 0.01;
		main_turret->level++;
		EarnMoney(-LevelUpPrice);
		if(main_turret->level!=6) LevelUpPrice *= 1.5;
		else LevelUpPrice = 10000000;
		storage[0]->money = LevelUpPrice;
		/* 
		UIGroup->RemoveControlObject(storage[0]->GetControlIterator(), dynamic_cast<IObject*>(storage[0])->GetObjectIterator());
		auto btn = new TurretButton("play/floor.png", "play/dirt.png",
		Engine::Sprite("win/dirt.png", 1294, 136, 0, 0, 0, 0),
		Engine::Sprite("play/LevelUp.png", 1294, 136 , 0, 0, 0, 0)
		, 1294, 136, LevelUpPrice);
		// Reference: Class Member Function Pointer and std::bind.
		btn->SetOnClickCallback(std::bind(&PlayScene::UIBtnClicked, this, id)); //problem
		UIGroup->AddNewControlObject(btn);
		storage[0] = btn;
		*/
		//std::cout << "level" << main_turret->level << std::endl;
	}
	else if (id == 1 && money >= SpeedUpPrice) {
		speedup += 0.01;
		main_turret->speed *= 1.3;
		EarnMoney(-SpeedUpPrice);
		if(main_turret->speed<=100) SpeedUpPrice *= 1.5;
		else SpeedUpPrice = 10000000;
		storage[1]->money = SpeedUpPrice;
	}
	
	else if (id == 2 && money >= MedicPrice) {
		lives++;
		UILives->Text = std::string("Life ") + std::to_string(lives);
		EarnMoney(-MedicPrice);
	}
	else if (id == 3 && money >= TowerBase::Price){
		preview = new TowerBase(0, 0);
	}
	if (!preview)
		return;
	preview->Position = Engine::GameEngine::GetInstance().GetMousePosition();
	preview->Tint = al_map_rgba(255, 255, 255, 200);
	preview->Enabled = false;
	preview->Preview = true;
	UIGroup->AddNewObject(preview);
	OnMouseMove(Engine::GameEngine::GetInstance().GetMousePosition().x, Engine::GameEngine::GetInstance().GetMousePosition().y);
}

bool PlayScene::CheckSpaceValid(int x, int y) {
	if (x < 0 || x >= MapWidth || y < 0 || y >= MapHeight)
		return false;
	auto map00 = mapState[y][x];
	mapState[y][x] = TILE_OCCUPIED;
	std::vector<std::vector<int>> map = CalculateBFSDistance();
	mapState[y][x] = map00;
	if (map[0][0] == -1 || map[MapHeight - 1][0] == -1 || map[0][MapWidth - 1] == -1 || map[MapHeight - 1][MapWidth - 1] == -1)
		return false;
	for (auto& it : EnemyGroup->GetObjects()) {
		Engine::Point pnt;
		pnt.x = floor(it->Position.x / BlockSize);
		pnt.y = floor(it->Position.y / BlockSize);
		if (pnt.x < 0) pnt.x = 0;
		if (pnt.x >= MapWidth) pnt.x = MapWidth - 1;
		if (pnt.y < 0) pnt.y = 0;
		if (pnt.y >= MapHeight) pnt.y = MapHeight - 1;
		if (map[pnt.y][pnt.x] == -1)
			return false;
	}
	// All enemy have path to exit.
	mapState[y][x] = TILE_OCCUPIED;
	mapDistance = map;
	for (auto& it : EnemyGroup->GetObjects())
		dynamic_cast<Enemy*>(it)->UpdatePath(mapDistance);
	return true;
}
std::vector<std::vector<int>> PlayScene::CalculateBFSDistance() {
	// Reverse BFS to find path.
	std::vector<std::vector<int>> map(MapHeight, std::vector<int>(std::vector<int>(MapWidth, -1)));
	std::queue<Engine::Point> que;
	// Push end point.
	// BFS from end point.
	que.push(Engine::Point(floor(main_turret->Position.x/BlockSize), floor(main_turret->Position.y/BlockSize)));
	map[floor(main_turret->Position.y/BlockSize)][floor(main_turret->Position.x/BlockSize)] = 0;
	while (!que.empty()) {
		Engine::Point p = que.front();
		if (p.y > 0 && map[p.y - 1][p.x] == -1 && mapState[p.y - 1][p.x] == TILE_DIRT) {
			que.push(Engine::Point(p.x, p.y - 1));
			map[p.y - 1][p.x] = map[p.y][p.x] + 1;
		}
		if(p.x > 0 && map[p.y][p.x - 1] == -1 && mapState[p.y][p.x - 1] == TILE_DIRT){
			que.push(Engine::Point(p.x - 1, p.y));
			map[p.y][p.x - 1] = map[p.y][p.x] + 1;
		}
		if (p.y < MapHeight - 1 && map[p.y + 1][p.x] == -1 && mapState[p.y + 1][p.x] == TILE_DIRT) {
			que.push(Engine::Point(p.x, p.y + 1));
			map[p.y + 1][p.x] = map[p.y][p.x] + 1;
		}
		if(p.x < MapWidth - 1 && map[p.y][p.x + 1] == -1 && mapState[p.y][p.x + 1] == TILE_DIRT){
			que.push(Engine::Point(p.x + 1, p.y));
			map[p.y][p.x + 1] = map[p.y][p.x] + 1;
		}
		
		que.pop();
		// TODO: [BFS PathFinding] (1/1): Implement a BFS starting from the most right-bottom block in the map.
		//               For each step you should assign the corresponding distance to the most right-bottom block.
		//               mapState[y][x] is TILE_DIRT if it is empty.
	}
	return map;
}
int generateRandomNumber() {
    // Initialize random number generation
    std::random_device rd;  // Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<> distrib(0, 3); // Define the range [0, 3] inclusive
    return distrib(gen); // Generate a random number within the defined range
}
