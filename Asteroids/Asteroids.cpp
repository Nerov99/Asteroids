#include "Framework.h"
#include <cstdlib> 
#include <ctime> 
#include <map>
#include <vector>
#include <memory>
using namespace std;

const int max_asteroid_speed = 3;
const int max_player_speed = 3;
const int big_asteroid_treshold = 3;

/* Test Framework realization */
class MyFramework : public Framework {

public:

	MyFramework(int windows_x = 900, int windows_y = 900, int map_x = 1000, int map_y = 1000, int num_asteroids = 30, int num_ammo = 3) :
		windows_x{ windows_x }, windows_y{ windows_y }, map_x{ map_x }, map_y{ map_y }, num_asteroids{ num_asteroids },
		num_ammo{ num_ammo }
	{

	}

	virtual void PreInit(int& width, int& height, bool& fullscreen)
	{
		width = windows_x;
		height = windows_y;
		fullscreen = false;
	}

	virtual bool Init() {
		player.insert({ {{windows_x / 2, windows_y / 2}, {0, 0}}, createSprite("data\\spaceship.png") });
		// create and randomly spawn asteroids
		srand((unsigned)time(0));
		for (int i = 0, x, y, speed_x, speed_y; i < num_asteroids; ++i) {
			x = rand() % windows_x + windows_x / 10;
			y = rand() % windows_y + windows_y / 10;
			speed_x = rand() / max_asteroid_speed;
			speed_y = rand() / max_asteroid_speed;
			if (i % big_asteroid_treshold == 0)
				asteroids.insert({ {{x, y}, {speed_x, speed_y}}, createSprite("data\\big_asteroid.png") });
			asteroids.insert({ {{x, y}, {speed_x, speed_y}}, createSprite("data\\small_asteroid.png") });
		}

		for (int i = 0; i < num_ammo; ++i) {
			bullets.insert({ { {windows_x / 2, windows_y / 2}, {0, 0}}, createSprite("data\\bullet.png") });
		}

		return true;
	}

	virtual void Close() {

	}

	void checkForBounds(pair<int, int>& position) {
		position.first += speed_x;
		if (position.first < 0)
			position.first = map_x - 1;
		else if (position.first >= map_x)
			position.first = 0;
		position.second += speed_y;
		if (position.second < 0)
			position.second = map_y - 1;
		else if (position.second >= map_y)
			position.second = 0;
	}

	void setPlayerSpeedX(int speed_x) {
		auto p = player.begin();
		p->first.second.first = speed_x;
	}

	void setPlayerSpeedY(int speed_y) {

	}

	void showReticle(bool state) {
	}

	virtual bool Tick() {
		drawTestBackground();

		// draw asteroids
		auto p = asteroids.begin();
		for (int i = 0; i < num_asteroids; ++i, ++p) {
			checkForBounds({ p->first.first.first, p->first.first.second });
			drawSprite(p->second, p->first.first.first, p->first.first.second);
		}

		auto px = player.begin();
		checkForBounds({Players_x, Players_y});
		drawSprite(px->second, px->first.first.first, px->first.first.first);
		
		
		return false;
	}

	virtual void onMouseMove(int x, int y, int xrelative, int yrelative) {
		showReticle(true);
	}

	virtual void onMouseButtonClick(FRMouseButton button, bool isReleased) {
		if (button == FRMouseButton::LEFT) {

		}
	}

	virtual void onKeyPressed(FRKey k) {
		if (k == FRKey::UP) {
			setPlayerSpeedY(-1 * max_player_speed);
			Tick();
		}
		else if (k == FRKey::DOWN) {
			setPlayerSpeedY(max_player_speed);
			Tick();
		}
		else if (k == FRKey::LEFT) {
			setPlayerSpeedX(-1 * max_player_speed);
			Tick();
		}
		else if (k == FRKey::RIGHT) {
			setPlayerSpeedX(max_player_speed);
			Tick();
		}
	}

	virtual void onKeyReleased(FRKey k) {
		auto p = player.begin();
		while (p->first.second.first < 0) {
			++p->first.second.first;
			Tick();
		}
		while (speed_x > 0) {
			--speed_x;
			Tick();
		}
		while (speed_y < 0) {
			++speed_y;
			Tick();
		}
		while (speed_y > 0) {
			--speed_y;
			Tick();
		}
			
	}

	virtual const char* GetTitle() override
	{
		return "asteroids";
	}

private:
	int num_ammo;
	int num_asteroids;
	int map_x, map_y;
	int windows_x, windows_y;
	map<pair<pair<int, int>, pair<int, int>>, Sprite*> player;
	map<pair<pair<int, int>, pair<int, int>>, Sprite*> asteroids;
	map<pair<pair<int, int>, pair<int, int>>, Sprite*> bullets;
};

int main(int argc, char* argv[])
{
	return run(new MyFramework());
}