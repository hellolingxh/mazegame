#ifdef _DEBUG
#pragma comment(lib,"sfml-graphics-d.lib") 
#pragma comment(lib,"sfml-audio-d.lib") 
#pragma comment(lib,"sfml-system-d.lib") 
#pragma comment(lib,"sfml-window-d.lib") 
#pragma comment(lib,"sfml-network-d.lib") 
#else 
#pragma comment(lib,"sfml-graphics.lib") 
#pragma comment(lib,"sfml-audio.lib") 
#pragma comment(lib,"sfml-system.lib") 
#pragma comment(lib,"sfml-window.lib") 
#pragma comment(lib,"sfml-network.lib") 
#endif 

#include <SFML/Graphics.hpp>
#include <windows.h>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
  
using namespace std;

const string TITLE = "Maze Game Development By Xiaohui Ling Using C++";
const string WALL_IMAGE = "images\\wall.jpg";
const string PLAYER_IMAGE = "images\\player.png";
const string ENEMY_IMAGE = "images\\enemy.png";
const string BOMB_IMAGE = "images\\bomb.png";
const string EXPLODE_IMAGE = "images\\explode.jpg";

const string MAP_DATA = "config\\map.txt";
const string FONT_FILE_PATH = "font\\arial.ttf";

const string GAME_NAME_TIPS = "M A Z E  G A M E";
const string AUTH_TIPS = "development by Xiaohui Ling";
const string EMAIL_TIPS = "Email: C00212235@ITCARLOW.IE";
const string OPTION_1_TIPS = "ONE BOMB AT THE SAME TIME";
const string OPTION_2_TIPS = "MULTIPLE BOMBS AT THE SAME TIME";
const string OTHER_OPTION_1_TIPS = "Pleaes press the key <Enter> to start the game after you have chosen the options.";
const string OTHER_OPTION_2_TIPS = "You could press the letter key <B> to produce a Bomb.";
const string OTHER_OPTION_3_TIPS = "Those keys : <UP, DOWN, LEFT, RIGHT> are direction keys you can move in the game map.";
const string GAME_MODE_1_TIPS = "Mode : one bomb";
const string GAME_MODE_2_TIPS = "Mode : multiple bombs";

const string GAME_OVER_TIPS = "G A M E   O V E R";
const string PLAYER_WIN_TIPS = "Y O U   W I N !";

const int MAP_ROWS = 15; //how many rows in the map
const int MAP_COLS = 30; //how many columns in the map
const float BLOCK_WIDTH = 25.0f;  //each cell width
const float BLOCK_HEIGHT = 25.0f;  //each cell height

/* below configurations you can customize as your favouriate by yourself */

const int BOMB_NUM = 20;  //how many bombs the player can take
const int ENEMY_NUM = 5; //how many enemy in the game
const int BOMB_EXPLODE_GRID_RANGE = 4; //how many cell range of exploded for each direction
const float BOMB_FIRED_WAITING_SECONDS = 4.0f; //how many seconds was exploded after player placed it
const float BOMBING_WAITING_SECONDS = 2.0f; //how many seconds continueously exploding after the bomb exploded
const int PLAYER_TRY_TIMES = 3; //the player's lifes that can play in a game
const float ENEMY_RANDOM_MOVE_SPEED = 0.2f; // this configuration can control the enemy move speed.
const int EXPLODED_DIRECTIONS = 8; // this value could be 4 or 8 indicate exploded directions 4: (up,down,left,right) 8: +(up left, down left, up right, down right)

/* above configurations you can customize as your favouriate by yourself */

enum class BombStatus {
	UNFIRED, FIRED, EXPLODING
};

enum class LifeEntityStatus {
	NORMAL, FAILED, DEADED
};

enum class CellType {
	WALL, CORRIDOR, PLAYER, ENEMY, BOMB
};

enum class GameStatus {
	ENTERING, RUNNING, TERMINATED
};

enum class GameMode {
	ONE_BOMB_ONE_TIMES, MULTIPLE_BOMBS_ONE_TIMES
};

class Cell : public sf::Sprite {
protected:
	CellType type;
	int rowOfGrid;
	int colOfGrid;
	float positionX;
	float positionY;


public:
	Cell() {

	}
	
	float getPositionX(int column) {
		return this->positionY;
	}

	float getPositionY() {
		return this->positionY;
	}

	int getRowOfGrid() {
		return this->rowOfGrid;
	}

	int getColumnOfGrid() {
		return this->colOfGrid;
	}
	
	void setPositionOfGrid(int row, int col) {
		this->rowOfGrid = row;
		this->colOfGrid = col;
		this->positionX = col * BLOCK_WIDTH;
		this->positionY = row * BLOCK_HEIGHT;
		this->setPosition(this->positionX, this->positionY);
	}

	void setCellType(CellType type) {
		this->type = type;
	}

	CellType getCellType() {
		return this->type;
	}
};

class Bomb : public Cell
{
private:
	BombStatus status;
	sf::Clock firedClock;  //timer for placed a bomb
	sf::Clock bombooClock; //timer for xploded continueos time

public:
	Bomb() {
		this->status = BombStatus::UNFIRED; 
		this->type= CellType::BOMB;
		firedClock.restart();
	}

	void setStatus(BombStatus status) {
		this->status = status;
	}
	
	BombStatus getStatus() {
		return this->status;
	}

	float getFiredElapsedSeconds() {
		return this->firedClock.getElapsedTime().asSeconds();
	}

	float getBombooElapsedSeconds() {
		return this->bombooClock.getElapsedTime().asSeconds();
	}

	void exploding() {
		bombooClock.restart();
	}
	
};

class Movable : public Cell {
	private:
		void moveUp() {
			this->setPositionOfGrid(this->getRowOfGrid() - 1, this->getColumnOfGrid());
		}

		void moveDown() {
			this->setPositionOfGrid(this->getRowOfGrid() + 1, this->getColumnOfGrid());
		}

		void moveLeft() {
			this->setPositionOfGrid(this->getRowOfGrid(), this->getColumnOfGrid() - 1);
		}

		void moveRight() {
			this->setPositionOfGrid(this->getRowOfGrid(), this->getColumnOfGrid() + 1);
		}

	public:
		void move(Cell** grid, sf::Keyboard::Key direction) {
			//Get position of the current cell object
			int rowOfGrid = this->getRowOfGrid();
			int colOfGrid = this->getColumnOfGrid();

			if (direction == sf::Keyboard::Up
				&& grid[rowOfGrid - 1][colOfGrid].getCellType() == CellType::CORRIDOR) {
				this->moveUp();
			}
			else if (direction == sf::Keyboard::Down
				&& grid[rowOfGrid + 1][colOfGrid].getCellType() == CellType::CORRIDOR) {
				this->moveDown();
			}
			else if (direction == sf::Keyboard::Left
				&& grid[rowOfGrid][colOfGrid - 1].getCellType() == CellType::CORRIDOR) {
				this->moveLeft();
			}
			else if (direction == sf::Keyboard::Right
				&& grid[rowOfGrid][colOfGrid + 1].getCellType() == CellType::CORRIDOR) {
				this->moveRight();
			}
		}

};

class Player : public Movable
{
private:
	Bomb bombs[BOMB_NUM];
	int bomb_counter;
	int try_times;
	int numOfPlacedBomb;
	bool canPlaceBomb;
	LifeEntityStatus status;

public:
	Player() {
		this->bomb_counter = 0;
		this->try_times = PLAYER_TRY_TIMES;
		this->status = LifeEntityStatus::NORMAL;
		this->canPlaceBomb = true;
		this->type = CellType::PLAYER;
		this->numOfPlacedBomb = 0;
	}

	void placeBomb(Cell** grid, sf::Texture& bomb_texture, GameMode mode) {
		
		bool hasPlacedSamePosition = false;
		for (int i = 0; i < bomb_counter; i++) {
			if (this->getGlobalBounds().intersects(bombs[i].getGlobalBounds())) {
				hasPlacedSamePosition = true;
				break;
			}
		}
		if (!hasPlacedSamePosition && this->numOfPlacedBomb < BOMB_NUM && this->canPlaceBomb) {
			Bomb bomb;
			bomb.setTexture(bomb_texture);
			int row = this->getRowOfGrid();
			int col = this->getColumnOfGrid();
			bomb.setPositionOfGrid(row, col);
			bomb.exploding(); // start the bombing clock;
			bombs[bomb_counter] = bomb;
			bomb_counter++;
			
			grid[row][col].setCellType(CellType::BOMB);
			if(mode == GameMode::ONE_BOMB_ONE_TIMES)
				this->canPlaceBomb = false;
			this->numOfPlacedBomb++;
		}
		
	}
	void setPlayerPosition() {
		this->setPositionOfGrid(1, 1);
	}

	Bomb removeBomb(int index) {
		Bomb bomb = this->bombs[index];
		
		for (int i = index; i < this->bomb_counter - 1; i++) {
			this->bombs[i] = this->bombs[i + 1];
		}
		this->bomb_counter--;
		this->canPlaceBomb = true;
		return bomb;

	}

	void loser() {
		
		this->reduceTryTimeOnce();

		if (this->getHadTryTimes() > 0)
			this->setStatus(LifeEntityStatus::FAILED);
		else
			this->setStatus(LifeEntityStatus::DEADED);

	}

	int getBombCounter() {
		return this->bomb_counter;
	}

	Bomb getBomb(int index) {
		return this->bombs[index];
	}

	int getHadTryTimes() {
		return this->try_times;
	}

	void reduceTryTimeOnce() {
		if(this->try_times > 0)
			this->try_times--;
	}

	void setStatus(LifeEntityStatus status) {
		this->status = status;
	}

	LifeEntityStatus getStatus() {
		return this->status;
	}

	int getNumOfPlacedBomb() {
		return this->numOfPlacedBomb;
	}

};

class Enemy : public Movable
{
private:
	LifeEntityStatus status;
	sf::Clock randomClock;
	int seq;

public:
	Enemy() {
		this->status = LifeEntityStatus::NORMAL;
		this->type = CellType::ENEMY;
		this->randomClock.restart();
	}

	void setSeq(int seq) {
		this->seq = seq;
	}

	void randomPosition(Cell* corridors, int corridorQuantity, int randomNum) {
		
		int index = randomNum % corridorQuantity;

		Cell cell = corridors[index];
		int row = cell.getRowOfGrid();
		int col = cell.getColumnOfGrid();
		this->rowOfGrid = row;
		this->colOfGrid = col;

		this->setPositionOfGrid(row, col);
		
	}

	void setStatus(LifeEntityStatus status) {
		this->status = status;
	}

	LifeEntityStatus getStatus() {
		return this->status;
	}

	void randomMove(Cell** grid, int randomNum) {
		//if the time betwwen start point and current point of the time less than 1 seconds then reutrn directly.
		//else the time more than 1 second then timer restart.
		if (this->randomClock.getElapsedTime().asSeconds() < ENEMY_RANDOM_MOVE_SPEED)
			return;
		else
			this->randomClock.restart();
		
		int directionNum = randomNum % 4; //produce the random number from 0 to 3. 
		sf::Keyboard::Key direction;
		
		switch (directionNum) {
			case 0:
				direction = sf::Keyboard::Up;
				break;
			case 1:
				direction = sf::Keyboard::Down;
				break;
			case 2:
				direction = sf::Keyboard::Left;
				break;
			case 3:
				direction = sf::Keyboard::Right;
				break;
			default:
				direction = sf::Keyboard::Up;
		}

		this->move(grid, direction);
	}
};

class Game {

private:
	Cell** grid;
	Enemy enemies[ENEMY_NUM];
	int wallQuantity;
	int corridorQuantity;
	Cell corridors[MAP_ROWS*MAP_COLS];
	Player player;
	int enemy_counter;
	GameStatus status;
	GameMode gameMode;
	Cell availableExplodedCells[BOMB_EXPLODE_GRID_RANGE * EXPLODED_DIRECTIONS];
	int availableExplodedCellsCounter;

private:
	void loadMapData() {
		//init the map for maze
		this->grid = new Cell*[MAP_ROWS];
		int wallQuantityCounter = 0;
		int corridorQuantityCounter = 0;

		string line, item;
		ifstream myfile(MAP_DATA);

		if (myfile.is_open()) {
			int r = 0;

			while (getline(myfile, line)) {

				istringstream ss(line); 
				grid[r] = new Cell[MAP_COLS];
				int c = 0;
				while (getline(ss, item, ',')) {

					Cell cell;
					cell.setPositionOfGrid(r, c);
					if (item == "1") {
						cell.setCellType(CellType::WALL);
						wallQuantityCounter++;
					}
					else {
						cell.setCellType(CellType::CORRIDOR);
						corridors[corridorQuantity] = cell;
						corridorQuantity++;
					}

					grid[r][c] = cell;
					c++;
				}

				r++;
			}

			this->wallQuantity = wallQuantityCounter;
			this->corridorQuantity = corridorQuantity;
		}else {
			cout << "Reading the map data file occur error." << endl;
		}

		myfile.close();

	}
	// up direction exploded
	void addAvailableExplodedCellsUpDirection(int row, int col) {
		for (int c = 1; c <= BOMB_EXPLODE_GRID_RANGE && (row - c) >= 0; c++) {
			Cell cell = grid[row - c][col];
			if (cell.getCellType() == CellType::CORRIDOR) {
				this->availableExplodedCells[this->availableExplodedCellsCounter++] = cell;
			}else
				break;
		}

	}
	// down direction exploded
	void addAvailableExplodedCellsDownDirection(int row, int col) {
		for (int c = 1; c <= BOMB_EXPLODE_GRID_RANGE && (row + c) <= MAP_ROWS * BLOCK_HEIGHT; c++) {
			Cell cell = grid[row + c][col];
			if (cell.getCellType() == CellType::CORRIDOR) {
				this->availableExplodedCells[this->availableExplodedCellsCounter++] = cell;
			}else
				break;
		}
	}
	// left direction exploded
	void addAvailableExplodedCellsLeftDirection(int row, int col) {
		for (int c = 1; c <= BOMB_EXPLODE_GRID_RANGE && (col - c) >= 0; c++) {
			Cell cell = grid[row][col - c];
			if (cell.getCellType() == CellType::CORRIDOR) {
				this->availableExplodedCells[this->availableExplodedCellsCounter++] = cell;
			}else
				break;
		}
	}
	// right direction exploded
	void addAvailableExplodedCellsRightDirection(int row, int col) {
		for (int c = 1; c <= BOMB_EXPLODE_GRID_RANGE && (col + c) <= MAP_COLS * BLOCK_WIDTH; c++) {
			Cell cell = grid[row][col + c];
			if (cell.getCellType() == CellType::CORRIDOR) {
				this->availableExplodedCells[this->availableExplodedCellsCounter++] = cell;
			}else
				break;
		}
	}
	// up left direction exploded
	void addAvailableExplodedCellsUpLeftDirection(int row, int col) {
		for (int c = 1; c <= BOMB_EXPLODE_GRID_RANGE && (col - c) >= 0 && (row - c) >= 0; c++) {
			Cell cell = grid[row - c][col - c];
			if (cell.getCellType() == CellType::CORRIDOR) {
				this->availableExplodedCells[this->availableExplodedCellsCounter++] = cell;
			}else
				break;
		}
	}
	// down left direction exploded
	void addAvailableExplodedCellsDownLeftDirection(int row, int col) {
		for (int c = 1; c <= BOMB_EXPLODE_GRID_RANGE && (col - c) >= 0 && (row + c) <= MAP_ROWS * BLOCK_HEIGHT; c++) {
			Cell cell = grid[row + c][col - c];
			if (cell.getCellType() == CellType::CORRIDOR) {
				this->availableExplodedCells[this->availableExplodedCellsCounter++] = cell;
			}else
				break;
		}
	}
	// up right direction exploded
	void addAvailableExplodedCellsUpRightDirection(int row, int col) {
		for (int c = 1; c <= BOMB_EXPLODE_GRID_RANGE && (col + c) <= MAP_COLS * BLOCK_WIDTH && (row - c) >= 0; c++) {
			Cell cell = grid[row - c][col + c];
			if (cell.getCellType() == CellType::CORRIDOR) {
				this->availableExplodedCells[this->availableExplodedCellsCounter++] = cell;
			}else
				break;
		}
	}
	// down right direction exploded
	void addAvailableExplodedCellsDownRightDirection(int row, int col) {
		for (int c = 1; c <= BOMB_EXPLODE_GRID_RANGE && (col + c) <= MAP_COLS * BLOCK_WIDTH && (row + c) <= MAP_ROWS * BLOCK_HEIGHT; c++) {
			Cell cell = grid[row + c][col + c];
			if (cell.getCellType() == CellType::CORRIDOR) {
				this->availableExplodedCells[this->availableExplodedCellsCounter++] = cell;
			}else
				break;
		}
	}

public:
	Game() {
		loadMapData();
		this->enemy_counter = 0;
		this->status = GameStatus::ENTERING;
		this->gameMode = GameMode::ONE_BOMB_ONE_TIMES;
		this->availableExplodedCellsCounter = 0;
	}

	Player& setupPlayer(sf::Texture& texture) {
		this->player.setTexture(texture);

		this->player.setPlayerPosition();
		
		return this->player;
	}

	Enemy* createEnemies(sf::Texture& texture) {
		for (int i = 0; i < ENEMY_NUM; i++) {
			Enemy enemy;
			enemy.setTexture(texture);
			enemy.randomPosition(this->getCorridors(), this->getCorridorQuantity(), this->createRandomNum()); 
			
			this->enemies[i] = enemy;
			this->enemy_counter++;
		}

		return this->enemies;
	}

	Enemy* getEnemies() {
		return this->enemies;
	}
	
	int createRandomNum() {
		LARGE_INTEGER seed;
		QueryPerformanceFrequency(&seed);
		QueryPerformanceCounter(&seed);
		srand(seed.QuadPart);

		return rand();
	}

	Cell* getAvailablExplodedCells(int row, int col) {
		
		this->availableExplodedCellsCounter = 0;
		
		//up direction cell of the bomb current position.
		this->addAvailableExplodedCellsUpDirection(row, col);
		//down direction cell of the bomb current position.
		this->addAvailableExplodedCellsDownDirection(row, col);
		//left direction cell of the bomb current position.
		this->addAvailableExplodedCellsLeftDirection(row, col);
		//right direction cell of the bomb current position.
		this->addAvailableExplodedCellsRightDirection(row, col);

		if (EXPLODED_DIRECTIONS > 4) { //if the exploded direction more than 4, then using 8 direction exploded range.
			//up left direction cell of the bomb current position
			this->addAvailableExplodedCellsUpLeftDirection(row, col);
			//up right direction cell of the bomb current position
			this->addAvailableExplodedCellsUpRightDirection(row, col);
			//down left direction cell of the bomb current position
			this->addAvailableExplodedCellsUpLeftDirection(row, col);
			//down right direction cell of the bomb current position
			this->addAvailableExplodedCellsDownRightDirection(row, col);

		}
		
		return this->availableExplodedCells;
	
	}

	int getAvailableExplodedCellsCounter() {
		return this->availableExplodedCellsCounter;
	}

	int getEnemyCounter() {
		return this->enemy_counter;
	}

	Enemy removeEnemy(int index) {
		Enemy enemy = enemies[index];

		for (int i = index; i < this->enemy_counter - 1; i++) {
			enemies[i] = enemies[i + 1];
		}
		this->enemy_counter--;
		return enemy;
	}

	string getPlayerTips() {
		ostringstream player_tips;
		player_tips << "Player : " << this->player.getHadTryTimes() << "/" << PLAYER_TRY_TIMES;
		return player_tips.str();
	}

	string getBombTips() {
		ostringstream bomb_tips;
		bomb_tips << "Bomb : " <<  (BOMB_NUM - this->player.getNumOfPlacedBomb()) << "/" << BOMB_NUM;
		return bomb_tips.str();
	}

	string getEnemyTips() {
		ostringstream enemy_tips;
		enemy_tips << "Score : " << (ENEMY_NUM - this->enemy_counter )<< "/" << ENEMY_NUM;
		return enemy_tips.str();
	}
	
	bool isGameOver() {
		
		if (this->player.getHadTryTimes() <= 0
			|| (this->player.getNumOfPlacedBomb() == BOMB_NUM && this->player.getBombCounter() == 0))
			return true;

		return false;
	}

	bool isWin(){
		if (this->getEnemyCounter() <= 0)
			return true;
		return false;
	}
	
	void setStatus(GameStatus status) {
		this->status = status;
	}

	GameStatus getStatus() {
		return this->status;
	}

	void setGameMode(GameMode mode) {
		this->gameMode = mode;
	}

	GameMode getGameMode() {
		return this->gameMode;
	}

	Player& getPlayer() {
		return this->player;
	}

	int getWallQuantity() {
		return this->wallQuantity;
	}

	int getCorridorQuantity() {
		return this->corridorQuantity;
	}

	Cell* getCorridors() {
		return this->corridors;
	}

	Cell** getMapGrid() {
		return this->grid;
	}

};


int main() {
	
	sf::RenderWindow window(sf::VideoMode(BLOCK_WIDTH * MAP_COLS, BLOCK_HEIGHT * (MAP_ROWS + 2)), TITLE);
	
	Game game;
	Cell** grid = game.getMapGrid();

	srand(time(NULL));

	sf::Texture wall_texture;
	wall_texture.loadFromFile(WALL_IMAGE, sf::IntRect(0, 0, BLOCK_WIDTH, BLOCK_HEIGHT));

	sf::Texture corridor_texture;

	sf::Texture player_texture;
	player_texture.loadFromFile(PLAYER_IMAGE);
	
	sf::Texture enemy_texture;
	enemy_texture.loadFromFile(ENEMY_IMAGE);

	sf::Texture bomb_texture;
	bomb_texture.loadFromFile(BOMB_IMAGE);

	sf::Texture explode_texture;
	explode_texture.loadFromFile(EXPLODE_IMAGE);

	sf::Font font;
	font.loadFromFile(FONT_FILE_PATH);

	Player& player = game.setupPlayer(player_texture);

	Enemy* enemies = game.createEnemies(enemy_texture);

	sf::Text player_text(game.getPlayerTips(), font, 15);
	player_text.setPosition(0, (MAP_ROWS + 1)*BLOCK_HEIGHT);

	sf::Text bomb_text(game.getBombTips(), font, 15);
	bomb_text.setPosition(BLOCK_WIDTH * 5, (MAP_ROWS + 1)*BLOCK_HEIGHT);

	sf::Text enemy_text(game.getEnemyTips(), font, 15);
	enemy_text.setPosition(BLOCK_WIDTH * 10, (MAP_ROWS + 1)*BLOCK_HEIGHT);

	sf::Text mode_text;
	mode_text.setFont(font);
	mode_text.setCharacterSize(15);
	mode_text.setPosition(BLOCK_WIDTH * 15, (MAP_ROWS + 1)*BLOCK_HEIGHT);

	sf::Text game_over_text(GAME_OVER_TIPS, font, 50);
	game_over_text.setPosition(BLOCK_WIDTH * 7, (MAP_ROWS - 10)*BLOCK_HEIGHT);

	sf::Text win_game_text(PLAYER_WIN_TIPS, font, 50);
	win_game_text.setPosition(BLOCK_WIDTH * 7, (MAP_ROWS - 10)*BLOCK_HEIGHT);

	sf::Text game_name_text(GAME_NAME_TIPS, font, 50);
	game_name_text.setPosition(BLOCK_WIDTH * 7, (MAP_ROWS - 12)*BLOCK_HEIGHT);

	sf::Text auth_text(AUTH_TIPS, font, 13);
	auth_text.setPosition(BLOCK_WIDTH * 11, (MAP_ROWS - 9)*BLOCK_HEIGHT);

	sf::Text option_1_text(OPTION_1_TIPS, font, 15);
	option_1_text.setPosition(BLOCK_WIDTH * 11, (MAP_ROWS - 6) * BLOCK_HEIGHT);
	option_1_text.setFillColor(sf::Color::Green);

	sf::Text option_2_text(OPTION_2_TIPS, font, 15);
	option_2_text.setPosition(BLOCK_WIDTH * 11, (MAP_ROWS - 5) * BLOCK_HEIGHT);
	option_2_text.setFillColor(sf::Color::Green);

	sf::Text other_option_1_text(OTHER_OPTION_1_TIPS, font, 12);
	other_option_1_text.setPosition(BLOCK_WIDTH * 7, (MAP_ROWS - 3) * BLOCK_HEIGHT);

	sf::Text other_option_2_text(OTHER_OPTION_2_TIPS, font, 12);
	other_option_2_text.setPosition(BLOCK_WIDTH * 7, (MAP_ROWS - 2) * BLOCK_HEIGHT);
	
	sf::Text other_option_3_text(OTHER_OPTION_3_TIPS, font, 12);
	other_option_3_text.setPosition(BLOCK_WIDTH * 7, (MAP_ROWS - 1) * BLOCK_HEIGHT);

	sf::Text email_text(EMAIL_TIPS, font, 10);
	email_text.setPosition(0, (MAP_ROWS + 1) * BLOCK_HEIGHT);

	sf::Texture narrow_texture;
	narrow_texture.loadFromFile(BOMB_IMAGE);
	sf::Sprite narrow_shape;
	narrow_shape.setTexture(narrow_texture);
	narrow_shape.setPosition(BLOCK_WIDTH * 9.7f, (MAP_ROWS - 6)*BLOCK_HEIGHT);

	// set timePerFrame to 1 60th of a second. 60 frames per second

	sf::Time timePerFrame = sf::seconds(2.0f / 60.0f);


	sf::Time timeSinceLastUpdate = sf::Time::Zero;

	// the clock object keeps the time.
	sf::Clock clock;

	clock.restart();

	//show the window if the window status is open.
	while (window.isOpen()) {

		//Get the event of the window we have created
		sf::Event event;
		//if the events of the window are triggered
		if (window.pollEvent(event)) {
			//if the close event of the window are triggered.
			if (event.type == sf::Event::Closed) {
				window.close(); //closed the window and then exit the program.
			}
		}

		if (game.getStatus() == GameStatus::ENTERING) {
			window.clear();
			window.draw(game_name_text);
			window.draw(auth_text);
			window.draw(email_text);
			window.draw(option_1_text);
			window.draw(option_2_text);
		    window.draw(other_option_1_text);
			window.draw(other_option_2_text);
			window.draw(other_option_3_text);
			
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
				narrow_shape.setPosition(BLOCK_WIDTH * 9.7f, (MAP_ROWS - 6)*BLOCK_HEIGHT);
			}else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
				narrow_shape.setPosition(BLOCK_WIDTH * 9.7f, (MAP_ROWS - 5)*BLOCK_HEIGHT);
			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
				if (narrow_shape.getPosition().y == (MAP_ROWS - 6) * BLOCK_HEIGHT) {
					game.setGameMode(GameMode::ONE_BOMB_ONE_TIMES);
				}else {
					game.setGameMode(GameMode::MULTIPLE_BOMBS_ONE_TIMES);
				}

				game.setStatus(GameStatus::RUNNING);
			}
			window.draw(narrow_shape);

			window.display();
			continue;
		}

		if (game.getStatus() == GameStatus::TERMINATED) {
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
				return EXIT_SUCCESS;
		}

		//if (game.isGameOver())
			//continue;
		
		if (player.getStatus() == LifeEntityStatus::FAILED) {
			player.setPlayerPosition();
			player.setStatus(LifeEntityStatus::NORMAL);
		}

		//add to the time since last update and restart the clock
		timeSinceLastUpdate += clock.restart();

		//update every 60th of a second
		//only when the time since last update is greater than 1/60 update the world.

		if (timeSinceLastUpdate > timePerFrame)
		{

			if(!game.isGameOver())
				for (int i = 0; i < game.getEnemyCounter(); i++) {
					enemies[i].randomMove(grid, game.createRandomNum());
					if (enemies[i].getGlobalBounds().intersects(player.getGlobalBounds())) // the player will be killed at once.
						player.loser();

				}
			
			// get keyboard input.
			if (!game.isGameOver() && (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)
				|| sf::Keyboard::isKeyPressed(sf::Keyboard::Down)
				|| sf::Keyboard::isKeyPressed(sf::Keyboard::Left)
				|| sf::Keyboard::isKeyPressed(sf::Keyboard::Right))) 		

				player.move(grid, event.key.code);			
				
			if (!game.isGameOver() && sf::Keyboard::isKeyPressed(sf::Keyboard::B))
				player.placeBomb(grid, bomb_texture, game.getGameMode());

			//clear the canvas of the window
			window.clear();

			for (int row = 0; row < MAP_ROWS; row++) {

				for (int col = 0; col < MAP_COLS; col++) {
					Cell cell = grid[row][col];
					sf::Sprite sprite_cell;

					if (cell.getCellType() == CellType::WALL) {
						sprite_cell.setTexture(wall_texture);
					}
					else if (cell.getCellType() == CellType::CORRIDOR) {
						sprite_cell.setTexture(corridor_texture);
					}

					sprite_cell.setPosition(col*BLOCK_WIDTH, row*BLOCK_HEIGHT);

					window.draw(sprite_cell);
				}

			}

			for (int i = 0; i < player.getBombCounter(); i++) {
				Bomb bomb = player.getBomb(i);
				if (bomb.getStatus() == BombStatus::UNFIRED) {
					if (bomb.getFiredElapsedSeconds() > BOMB_FIRED_WAITING_SECONDS) {
						bomb.setStatus(BombStatus::FIRED);
					}

					window.draw(bomb);
				}

				if (bomb.getStatus() == BombStatus::FIRED) {
					float a = bomb.getBombooElapsedSeconds();
					if (bomb.getBombooElapsedSeconds() > (BOMB_FIRED_WAITING_SECONDS + BOMBING_WAITING_SECONDS))
						bomb.setStatus(BombStatus::EXPLODING);

						int row = bomb.getRowOfGrid();
						int col = bomb.getColumnOfGrid();
						Cell* availableExplodedCells = game.getAvailablExplodedCells(row, col);

						for (int idx = 0; idx < game.getAvailableExplodedCellsCounter(); idx++) {
							Cell cell = availableExplodedCells[idx];
							cell.setTexture(explode_texture);

							if (cell.getGlobalBounds().intersects(player.getGlobalBounds()))
								player.loser();

							for (int j = 0; j < game.getEnemyCounter(); j++) {
								if (cell.getGlobalBounds().intersects(enemies[j].getGlobalBounds())) {
									enemies[j].setStatus(LifeEntityStatus::DEADED);
									game.removeEnemy(j);
								}
							}

							window.draw(cell);
						}
						
				}

				if (bomb.getStatus() == BombStatus::EXPLODING) {
					player.removeBomb(i);
					grid[bomb.getRowOfGrid()][bomb.getColumnOfGrid()].setCellType(CellType::CORRIDOR);// release the grid where space of bomb when exploding the bomb
				}

			}

			for (int i = 0; i < game.getEnemyCounter(); i++) {
				window.draw(game.getEnemies()[i]);
			}

			if (player.getStatus() == LifeEntityStatus::NORMAL)
				window.draw(player);

			player_text.setString(game.getPlayerTips());
			window.draw(player_text);

			bomb_text.setString(game.getBombTips());
			window.draw(bomb_text);

			if (game.getGameMode() == GameMode::ONE_BOMB_ONE_TIMES)
				mode_text.setString(GAME_MODE_1_TIPS);
			else
				mode_text.setString(GAME_MODE_2_TIPS);

			window.draw(mode_text);

			enemy_text.setString(game.getEnemyTips());
			window.draw(enemy_text);

			if (game.isWin()) {
				window.draw(win_game_text);
				game.setStatus(GameStatus::TERMINATED);
			}else if (game.isGameOver()) {
				window.draw(game_over_text);
				game.setStatus(GameStatus::TERMINATED);
			}

			window.display();

			timeSinceLastUpdate = sf::Time::Zero;
		}
		
	}
	return EXIT_SUCCESS;
}
