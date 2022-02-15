#include "Framework.h"
#include <cstdlib> 
#include <ctime> 
#include <map>
#include <vector>
#include <memory>
#include <iostream>
#include <functional>
#include <random>

const double max_asteroid_speed = 0.5;
const int spawn_distance_treshold = 40;
const int max_player_speed = 2;
const int big_asteroid_probability_treshold = 3;
const unsigned int reticle_appearence_treshold = 10000;
const unsigned int bullets_appearence_treshold = 2000;

struct SpaceObject {
	Sprite* sprite;
	double x;
	double y;
};

struct Spaceship : SpaceObject {
	double x_acceleration;
	double y_acceleration;
	bool engine_state;
};

struct Asteroid : SpaceObject {
	double x_acceleration;
	double y_acceleration;
	bool small_size;
};

struct Reticle: SpaceObject {
	unsigned int first_appearence;
	unsigned int last_appearence;
	bool drawStatus;
};

struct Bullet : SpaceObject {
	double x_acceleration;
	double y_acceleration;
	unsigned int first_appearence;
	unsigned int last_appearence;
	bool drawStatus;
};

class Rand_double {
public:
	Rand_double(double low, double high)
		:r(std::bind(std::uniform_real_distribution<>(low, high), std::default_random_engine())) {

	}

	double operator()() { return r(); }

private:
	std::function<double()> r;
};

/* Test Framework realization */
class MyFramework : public Framework {

public:

	MyFramework(int windows_x = 1920, int windows_y = 1080, int map_x = 1920, int map_y = 1080, int num_asteroids = 30, int num_ammo = 5) :
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

		Rand_double rd{ 0, max_asteroid_speed };
		//create reticle
		reticle.sprite = createSprite("data\\reticle.png");
		reticle.drawStatus = false;

		// create player
		player.sprite = createSprite("data\\spaceship.png");
		player.x = windows_x / 2 - 1;
		player.y = windows_y / 2 - 1;
		player.x_acceleration = 0;
		player.y_acceleration = 0;
		player.engine_state = false;

		// create asteroids
		srand((unsigned)time(0));
		int x, y;
		double x_acceleration, y_acceleration;
		asteroids.resize(num_asteroids);
		for (int i = 0; i < num_asteroids; ++i) {
			if(rand() % 2 == 1)
				x = player.x + spawn_distance_treshold + rand() % (windows_x - static_cast<int>(player.x) + spawn_distance_treshold);
			else
				x = player.x - spawn_distance_treshold - rand() % (windows_x - static_cast<int>(player.x) - spawn_distance_treshold);
			if (rand() % 2 == 1)
				y = player.y + spawn_distance_treshold + rand() % (windows_y - static_cast<int>(player.y) + spawn_distance_treshold);
			else
				y = player.y - spawn_distance_treshold - rand() % (windows_y - static_cast<int>(player.y) - spawn_distance_treshold);
			x_acceleration = rd();
			y_acceleration = rd();
			if (i % big_asteroid_probability_treshold == 0) {
				asteroids[i].sprite = createSprite("data\\big_asteroid.png");
				asteroids[i].small_size = false;
			}
			else {
				asteroids[i].sprite = createSprite("data\\small_asteroid.png");
				asteroids[i].small_size = true;
			}
			asteroids[i].x = x;
			asteroids[i].y = y;
			asteroids[i].x_acceleration = x_acceleration;
			asteroids[i].y_acceleration = y_acceleration;
		}

		// create bullets
		bullets.resize(num_ammo);
		bullets_count = 0;
		for (int i = 0; i < num_ammo; ++i) {
			bullets[i].sprite = createSprite("data\\bullet.png");
			bullets[i].x = windows_x / 2 - 1;
			bullets[i].y = windows_y / 2 - 1;
			bullets[i].x_acceleration = 0;
			bullets[i].y_acceleration = 0;
			bullets[i].drawStatus = false;
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

	// TODO
	// This function will return right acceleration for bullets (equal speed in all direction and independent from reticle distance to player)
	// Function, probably, should be realized by finding common point between circle(center = player and radius = bullets speed) and line(player to reticle)
	// Will be implemented in next releaze
	std::pair<double, double> getBulletAcceleration(double x0, double y0, double x1, double y1) {
		return {0, 0};
	}

	bool checkForCollisions(const SpaceObject& object1, const SpaceObject& object2) {
		int object2_w, object2_h, object1_w, object1_h;
		double object1_tlx, object1_tly, object2_tlx, object2_tly;
		getSpriteSize(object1.sprite, object1_w, object1_h);
		getSpriteSize(object2.sprite, object2_w, object2_h);
		object1_tlx = object1.x - object1_w / 2;
		object1_tly = object1.y - object1_h / 2;
		object2_tlx = object2.x - object2_w / 2;
		object2_tly = object2.y - object2_h / 2;
		if (object1_tlx < object2_tlx + object2_w &&
			object1_tlx + object1_w > object2_tlx &&
			object1_tly < object2_tly + object2_h &&
			object1_tly + object1_h > object2_tly)
			return true;
		return false;
	}

	virtual bool Tick() {

		drawTestBackground();

		// draw asteroids
		for (int i = 0; i < asteroids.size(); ++i) {
			asteroids[i].x += asteroids[i].x_acceleration;
			asteroids[i].y += asteroids[i].y_acceleration;
			checkForBounds(asteroids[i]);
			drawSprite(asteroids[i].sprite, asteroids[i].x, asteroids[i].y);
		}

		// draw player
		if (player.engine_state == false && (player.x_acceleration != 0 || player.y_acceleration != 0)) {
			if (player.x_acceleration < 0) {
				if (player.x_acceleration > -0.001 && player.x_acceleration < 0.001)
					player.x_acceleration = 0;
				player.x_acceleration += max_player_speed / 40.;
			}
			else if (player.x_acceleration > 0) {
				if (player.x_acceleration > -0.001 && player.x_acceleration < 0.001)
					player.x_acceleration = 0;
				player.x_acceleration -= max_player_speed / 40.;
			}
			if (player.y_acceleration < 0) {
				if (player.y_acceleration > -0.001 && player.y_acceleration < 0.001)
					player.y_acceleration = 0;
				player.y_acceleration += max_player_speed / 40.;
			}
			else if (player.y_acceleration > 0) {
				if (player.y_acceleration > -0.001 && player.y_acceleration < 0.001)
					player.y_acceleration = 0;
				player.y_acceleration -= max_player_speed / 40.;
			}
		}
		player.x += player.x_acceleration;
		player.y += player.y_acceleration;
		checkForBounds(player);
		drawSprite(player.sprite, player.x, player.y);

		// draw reticle
		if (reticle.drawStatus) {
			reticle.last_appearence = getTickCount();
			drawSprite(reticle.sprite, reticle.x, reticle.y);
			if (reticle.last_appearence - reticle.first_appearence > reticle_appearence_treshold)
				reticle.drawStatus = false;
		}

		//draw bullets
		for (int i = 0; i < bullets.size() ; ++i) {
			if (bullets[i].drawStatus) {
				bullets[i].x += bullets[i].x_acceleration;
				bullets[i].y += bullets[i].y_acceleration;
				checkForBounds(bullets[i]);
				drawSprite(bullets[i].sprite, bullets[i].x, bullets[i].y);
			}
			else {
				bullets[i].x = player.x;
				bullets[i].y = player.y;
			}
		}

		// check for collisions between asteroids and bullets
		for (int i = 0; i < asteroids.size(); ++i) {
			for (int k = 0; k < bullets.size(); ++k) {
				if (bullets[k].drawStatus == true && checkForCollisions(asteroids[i], bullets[k])) {
					if (asteroids[i].small_size == true) {
						asteroids.erase(asteroids.begin() + i);
					}
					else {

						Asteroid temp_asteroid;
						Rand_double rd{ 0, max_asteroid_speed };
						double x, y, x_acceleration, y_acceleration;
						x = asteroids[i].x;
						y = asteroids[i].y;
						x_acceleration = rd();
						y_acceleration = rd();
						temp_asteroid.sprite = createSprite("data\\small_asteroid.png");
						temp_asteroid.small_size = true;
						temp_asteroid.x = x;
						temp_asteroid.y = y;
						temp_asteroid.x_acceleration = x_acceleration;
						temp_asteroid.y_acceleration = y_acceleration;
						asteroids.push_back(temp_asteroid);

						x = asteroids[i].x + 1;
						y = asteroids[i].y + 1;
						x_acceleration = rd();
						y_acceleration = rd();
						temp_asteroid.sprite = createSprite("data\\small_asteroid.png");
						temp_asteroid.small_size = true;
						temp_asteroid.x = x;
						temp_asteroid.y = y;
						temp_asteroid.x_acceleration = -1 * x_acceleration;
						temp_asteroid.y_acceleration = -1 * y_acceleration;
						asteroids.push_back(temp_asteroid);

						destroySprite(asteroids[i].sprite);
						asteroids.erase(asteroids.begin() + i);
					}
					bullets[k].drawStatus = false;
					--bullets_count;
				}
			}
		}

		// check for collisions between asteroids
		for (int i = 0; i < asteroids.size(); ++i) {
			for (int k = 0; k < asteroids.size(); ++k) {
				if (i != k && checkForCollisions(asteroids[i], asteroids[k]) == true) {
					srand((unsigned)time(0));
					int variant = rand() % 5;
					if (variant == 0) {
						asteroids[i].x_acceleration = -1 * asteroids[i].x_acceleration;
						asteroids[i].y_acceleration = -1 * asteroids[i].y_acceleration;
						asteroids[k].x_acceleration = -1 * asteroids[k].x_acceleration;
						asteroids[k].y_acceleration = -1 * asteroids[k].y_acceleration;
					} else
					if (variant == 1) {
						asteroids[i].y_acceleration = -1 * asteroids[i].y_acceleration;
						asteroids[k].x_acceleration = -1 * asteroids[k].x_acceleration;
						asteroids[k].y_acceleration = -1 * asteroids[k].y_acceleration;
					} else
					if (variant == 2) {
						asteroids[i].x_acceleration = -1 * asteroids[i].x_acceleration;
						asteroids[k].x_acceleration = -1 * asteroids[k].x_acceleration;
						asteroids[k].y_acceleration = -1 * asteroids[k].y_acceleration;
					} else
					if (variant == 3) {
						asteroids[i].x_acceleration = -1 * asteroids[i].x_acceleration;
						asteroids[i].y_acceleration = -1 * asteroids[i].y_acceleration;
						asteroids[k].y_acceleration = -1 * asteroids[k].y_acceleration;
					} else
					if (variant == 4) {
						asteroids[i].x_acceleration = -1 * asteroids[i].x_acceleration;
						asteroids[i].y_acceleration = -1 * asteroids[i].y_acceleration;
						asteroids[k].x_acceleration = -1 * asteroids[k].x_acceleration;
					}
					asteroids[i].x += asteroids[i].x_acceleration;
					asteroids[i].y += asteroids[i].y_acceleration;
					asteroids[k].x += asteroids[k].x_acceleration;
					asteroids[k].y += asteroids[k].y_acceleration;
				}
			}
		}

		// check for collisions between asteroids and player
		/*for (int i = 0; i < asteroids.size(); ++i) {
			if (checkForCollisions(asteroids[i], player))
				Init();
		}
		*/
		return false;
	}

	virtual void onMouseMove(int x, int y, int xrelative, int yrelative) {
		reticle.x = x;
		reticle.y = y;
		reticle.first_appearence = getTickCount();
		reticle.drawStatus = true;
	}

	virtual void onMouseButtonClick(FRMouseButton button, bool isReleased) {
		static bool state = false;
		int k = 0;
		if (button == FRMouseButton::LEFT && state == false) {
			state = true;
			if (bullets_count >= num_ammo) {
				for (int i = 0; i < bullets.size(); ++i) {
					if (bullets[i].drawStatus == true)
						if (bullets[k].first_appearence > bullets[i].first_appearence)
							k = i;
				}
				bullets[k].drawStatus = false;
				--bullets_count;
			}
			for (int i = 0; i < bullets.size(); ++i) {
				if (bullets[i].drawStatus == false) {
					k = i;
					break;
				}
			}
			bullets[k].x = player.x;
			bullets[k].y = player.y;
			bullets[k].drawStatus = true;
			bullets[k].first_appearence = getTickCount();
			bullets[k].x_acceleration = (reticle.x - player.x) / player.x;
			bullets[k].y_acceleration = (reticle.y - player.y) / player.y;
			++bullets_count;
			}
		else if (button == FRMouseButton::LEFT && state == true)
			state = false;
	}

	virtual void onKeyPressed(FRKey k) {
		if (k == FRKey::UP) {
			player.y_acceleration = -1 * max_player_speed;
		}
		if (k == FRKey::DOWN) {
			player.y_acceleration = max_player_speed;
		}
		if (k == FRKey::LEFT) {
			player.x_acceleration = -1 * max_player_speed;
		}
		if (k == FRKey::RIGHT) {
			player.x_acceleration = max_player_speed;
		}
		player.engine_state = true;
	}

	virtual void onKeyReleased(FRKey k) {
		player.engine_state = false;
	}

	virtual const char* GetTitle() override
	{
		return "asteroids";
	}

private:
	int num_ammo;
	int bullets_count;
	int num_asteroids;
	int map_x, map_y;
	int windows_x, windows_y;
	Reticle reticle;
	Spaceship player;
	std::vector<Bullet> bullets;
	std::vector<Asteroid> asteroids;
};

int main(int argc, char* argv[])
{
	/*int windows_x, windows_y;
	int map_x, map_y;
	int num_asteroids, num_ammo;
	for (int i = 0; i < argc; ++i) {
		if (argv[i] == "-window") {
			windows_x = ;
			windows_y = ;
		}
		else if (argv[i] == "-map") {
			map_x = ;
			map_y = ;
		}
		else if (argv[i] == "-num_asteroids") {
			num_asteroids = ;
		}
		else if (argv[i] == "-num_ammo") {
			num_ammo = ;
		}
	}*/
	return run(new MyFramework());
}