#include "Framework.h"
#include <cstdlib> 
#include <ctime> 
#include <map>
#include <vector>
#include <memory>
#include <iostream>
using namespace std;

const int max_asteroid_speed = 2;
const int max_player_speed = 2;
const int big_asteroid_treshold = 3;
const unsigned int reticle_appearence_treshold = 10000;

struct SpaceObject {
	Sprite* sprite;
	double x;
	double y;
	double x_acceleration;
	double y_acceleration;
};

struct Reticle: SpaceObject {
	unsigned int first_appearence;
	unsigned int last_appearence;
	bool drawStatus;
};

/* Test Framework realization */
class MyFramework : public Framework {

public:

	MyFramework(int windows_x = 1920, int windows_y = 1080, int map_x = 1920, int map_y = 1080, int num_asteroids = 30, int num_ammo = 3) :
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

		reticle.sprite = createSprite("data\\reticle.png");
		reticle.drawStatus = false;
		reticle.x = 10;
		reticle.y = 10;

		// create player
		player.sprite = createSprite("data\\spaceship.png");
		player.x = windows_x / 2 - 1;
		player.y = windows_y / 2 - 1;
		player.x_acceleration = 0;
		player.y_acceleration = 0;

		// create and randomly spawn asteroids
		srand((unsigned)time(0));
		double x, y, x_acceleration, y_acceleration;
		asteroids.resize(num_asteroids);
		for (int i = 0; i < num_asteroids; ++i) {
			x = rand() % windows_x + windows_x / 10;
			y = rand() % windows_y + windows_y / 10;
			x_acceleration = rand() % max_asteroid_speed + rand() % max_asteroid_speed / 10.;
			y_acceleration = rand() % max_asteroid_speed + rand() % max_asteroid_speed / 10.;
			if (i % big_asteroid_treshold == 0)
				asteroids[i].sprite = createSprite("data\\big_asteroid.png");
			else
				asteroids[i].sprite = createSprite("data\\small_asteroid.png");
			asteroids[i].x = x;
			asteroids[i].y = y;
			asteroids[i].x_acceleration = x_acceleration;
			asteroids[i].y_acceleration = y_acceleration;
		}

		bullets.resize(num_asteroids);
		for (int i = 0; i < num_ammo; ++i) {
			bullets[i].sprite = createSprite("data\\bullet.png");
			bullets[i].x = windows_x / 2 - 1;
			bullets[i].y = windows_y / 2 - 1;
			bullets[i].x_acceleration = 0;
			bullets[i].y_acceleration = 0;
		}

		return true;
	}

	virtual void Close() {

	}

	void checkForBounds(SpaceObject& object) {
		if (object.x < 0)
			object.x = map_x - 1;
		else if (object.x >= map_x)
			object.x = 0;
		if (object.y < 0)
			object.y = map_y - 1;
		else if (object.y >= map_y)
			object.y = 0;
	}

	bool checkForCollisions(const SpaceObject& object1, const SpaceObject& object2) {
		int object1_tlx, object1_tly, object1_w, object1_h;
		int object2_tlx, object2_tly, object2_w, object2_h;
		getSpriteSize(object1.sprite, object1_w, object1_h);
		getSpriteSize(object2.sprite, object2_w, object2_h);
		object1_tlx = object1.x - object1_w / 2;
		object1_tly = object1.y - object1_h / 2;
		object2_tlx = object2.x - object2_w / 2;
		object2_tly = object2.y - object2_h / 2;
		if (object1_tlx < object2_tlx + object2_w &&
			object1_tlx + object1_w > object2_w &&
			object1_tly < object2_tly + object2_h &&
			object1_h + object1_tly > object2_tly)
			return true;
		return false;
	}

	void showReticle(bool state) {
	}

	virtual bool Tick() {

		drawTestBackground();

		// draw asteroids
		for (int i = 0; i < num_asteroids; ++i) {
			asteroids[i].x += asteroids[i].x_acceleration;
			asteroids[i].y += asteroids[i].y_acceleration;
			checkForBounds(asteroids[i]);
			drawSprite(asteroids[i].sprite, asteroids[i].x, asteroids[i].y);
		}

		// draw player
		player.x += player.x_acceleration;
		player.y += player.y_acceleration;
		checkForBounds(player);
		drawSprite(player.sprite, player.x, player.y);
		
		for (int i = 0; i < num_asteroids; ++i) {
			if (checkForCollisions(asteroids[i], player))
				cout << "OUPS" << endl;
		}

		
		if (reticle.drawStatus) {
			reticle.last_appearence = getTickCount();
			drawSprite(reticle.sprite, reticle.x, reticle.y);
			if (reticle.last_appearence - reticle.first_appearence > reticle_appearence_treshold)
				reticle.drawStatus = false;
		}
		//draw bullets
		
		
		return false;
	}

	virtual void onMouseMove(int x, int y, int xrelative, int yrelative) {
		reticle.x = x;
		reticle.y = y;
		reticle.first_appearence = getTickCount();
		reticle.drawStatus = true;
	}

	virtual void onMouseButtonClick(FRMouseButton button, bool isReleased) {
		if (button == FRMouseButton::LEFT) {

		}
	}

	virtual void onKeyPressed(FRKey k) {
		if (k == FRKey::UP) {
			player.y_acceleration = -1 * max_player_speed;
		}
		else if (k == FRKey::DOWN) {
			player.y_acceleration = max_player_speed;
		}
		else if (k == FRKey::LEFT) {
			player.x_acceleration = -1 * max_player_speed;
		}
		else if (k == FRKey::RIGHT) {
			player.x_acceleration = max_player_speed;
		}
	}

	virtual void onKeyReleased(FRKey k) {
		if (k == FRKey::UP) {
			for (; player.y_acceleration < 0; player.y_acceleration += 0.05);
		}
		else if (k == FRKey::DOWN) {
			for (; player.y_acceleration > 0; player.y_acceleration -= 0.05);
		}
		else if (k == FRKey::LEFT) {
			for (; player.x_acceleration < 0; player.x_acceleration += 0.05);
		}
		else if (k == FRKey::RIGHT) {
			for (; player.x_acceleration > 0; player.x_acceleration -= 0.05);
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
	Reticle reticle;
	SpaceObject player;
	vector<SpaceObject> asteroids;
	vector<SpaceObject> bullets;
};

int main(int argc, char* argv[])
{
	return run(new MyFramework());
}