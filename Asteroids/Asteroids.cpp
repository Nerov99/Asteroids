// Not implemented features:
// - Player should always be in the center of a screen (i.e. if it moves map is recentered). You can add extra threshold, but it is not necessary
// - Window size, map size, number of bullets and asteroids should be possible to set from the command-line
// - All aditional features
#include "Framework.h"
#include <cstdlib> 
#include <ctime> 
#include <vector>
#include <memory>
#include <functional>
#include <random>

const double max_asteroid_speed = 0.5;
const unsigned short int spawn_distance_treshold = 100;
const unsigned short int max_player_speed = 2;
const double player_acceleration_multiplier = 60;
const unsigned short int big_asteroid_probability_treshold = 3;
const unsigned short int reticle_appearence_treshold = 10000;
const unsigned short int bullets_appearence_treshold = 2000;

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

	double operator()() {
		return r();
	}

private:
	std::function<double()> r;
};

/* Test Framework realization */
class MyFramework : public Framework {

public:

	MyFramework(int windows_x = 1920, int windows_y = 1080, int map_x = 1920, int map_y = 1080, int num_asteroids = 30, int num_ammo = 7) :
		windows_x{ windows_x }, windows_y{ windows_y }, map_x{ map_x }, map_y{ map_y }, num_asteroids{ num_asteroids }, num_ammo{ num_ammo }
	{}

	virtual void PreInit(int& width, int& height, bool& fullscreen)
	{
		width = windows_x;
		height = windows_y;
		fullscreen = false;
	}

	virtual bool Init() {
		createBackground();
		createPlayer();
		createReticle();
		createAsteroids();
		createBullets();
		return true;
	}

	virtual void Close() {

	}

	virtual bool Tick() {

		drawBackground();

		drawAsteroids();

		drawPlayer();

		drawReticle();

		drawBullets();

		collisionBetweenAsteroidsAndBullets();
		
		collisionsBetweenAsteroids();

		collisionsBetweenAsteroidsAndPlayer();

		return false;
	}

	virtual void onMouseMove(int x, int y, int xrelative, int yrelative) {
		reticle->x = x;
		reticle->y = y;
		reticle->first_appearence = getTickCount();
		reticle->drawStatus = true;
	}

	virtual void onMouseButtonClick(FRMouseButton button, bool isReleased) {
		static bool state = false;
		int k = 0;
		if (button == FRMouseButton::LEFT && state == false) {
			state = true;
			if (bullets_count >= num_ammo) {
				for (size_t i = 0; i < bullets.size(); ++i) {
					if (bullets[i]->drawStatus == true)
						if (bullets[k]->first_appearence > bullets[i]->first_appearence)
							k = i;
				}
				bullets[k]->drawStatus = false;
				--bullets_count;
			}
			for (size_t i = 0; i < bullets.size(); ++i) {
				if (bullets[i]->drawStatus == false) {
					k = i;
					break;
				}
			}
			bullets[k]->x = player->x;
			bullets[k]->y = player->y;
			bullets[k]->drawStatus = true;
			bullets[k]->first_appearence = getTickCount();
			bullets[k]->x_acceleration = (reticle->x - player->x) / player->x;
			bullets[k]->y_acceleration = (reticle->y - player->y) / player->y;
			++bullets_count;
			}
		else if (button == FRMouseButton::LEFT && state == true)
			state = false;
	}

	virtual void onKeyPressed(FRKey k) {
		if (k == FRKey::UP) {
			player->y_acceleration = -1 * max_player_speed;
		}
		if (k == FRKey::DOWN) {
			player->y_acceleration = max_player_speed;
		}
		if (k == FRKey::LEFT) {
			player->x_acceleration = -1 * max_player_speed;
		}
		if (k == FRKey::RIGHT) {
			player->x_acceleration = max_player_speed;
		}
		player->engine_state = true;
	}

	virtual void onKeyReleased(FRKey k) {
		player->engine_state = false;
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
	std::unique_ptr<SpaceObject> background;
	std::unique_ptr<Reticle> reticle;
	std::unique_ptr<Spaceship> player;
	std::vector<std::unique_ptr<Bullet>> bullets;
	std::vector<std::unique_ptr<Asteroid>> asteroids;

	void createBackground() {
		background = std::make_unique<SpaceObject>();
		background->sprite = createSprite("data\\background.png");
	}

	void createAsteroids() {
		Rand_double rd{ 0, max_asteroid_speed };
		srand((unsigned)time(0));
		asteroids.resize(num_asteroids);
		for (size_t i = 0; i < num_asteroids; ++i) {
			asteroids[i] = std::make_unique<Asteroid>();
			if (rand() % 2 == 1)
				asteroids[i]->x = player->x + spawn_distance_treshold + rand() % (windows_x - static_cast<int>(player->x) + spawn_distance_treshold);
			else
				asteroids[i]->x = player->x - spawn_distance_treshold - rand() % (windows_x - static_cast<int>(player->x) - spawn_distance_treshold);
			if (rand() % 2 == 1)
				asteroids[i]->y = player->y + spawn_distance_treshold + rand() % (windows_y - static_cast<int>(player->y) + spawn_distance_treshold);
			else
				asteroids[i]->y = player->y - spawn_distance_treshold - rand() % (windows_y - static_cast<int>(player->y) - spawn_distance_treshold);
			asteroids[i]->x_acceleration = rd();
			asteroids[i]->y_acceleration = rd();
			if (i % big_asteroid_probability_treshold == 0) {
				asteroids[i]->sprite = createSprite("data\\big_asteroid.png");
				asteroids[i]->small_size = false;
			}
			else {
				asteroids[i]->sprite = createSprite("data\\small_asteroid.png");
				asteroids[i]->small_size = true;
			}
		}
	}

	void createPlayer() {
		player = std::make_unique<Spaceship>();
		player->sprite = createSprite("data\\spaceship.png");
		player->x = windows_x / 2 - 1;
		player->y = windows_y / 2 - 1;
		player->x_acceleration = 0;
		player->y_acceleration = 0;
		player->engine_state = false;
	}

	void createReticle() {
		reticle = std::make_unique<Reticle>();
		reticle->sprite =createSprite("data\\reticle.png");
		reticle->drawStatus = false;
	}

	void createBullets() {
		bullets.resize(num_ammo);
		bullets_count = 0;
		for (size_t i = 0; i < num_ammo; ++i) {
			bullets[i] = std::make_unique<Bullet>();
			bullets[i]->sprite = createSprite("data\\bullet.png");
			bullets[i]->x = windows_x / 2 - 1;
			bullets[i]->y = windows_y / 2 - 1;
			bullets[i]->x_acceleration = 0;
			bullets[i]->y_acceleration = 0;
			bullets[i]->drawStatus = false;
		}
	}

	template<class T>
	void checkForBounds(const std::unique_ptr<T>& object) {
		if (object->x < 0)
			object->x = map_x - 1;
		else if (object->x >= map_x)
			object->x = 0;
		if (object->y < 0)
			object->y = map_y - 1;
		else if (object->y >= map_y)
			object->y = 0;
	}

	template<class T1, class T2>
	bool checkForCollisions(const std::unique_ptr<T1>& object1, const std::unique_ptr<T2>& object2) {
		int object2_w, object2_h, object1_w, object1_h;
		// tlx - top left x, tly - top left y
		double object1_tlx, object1_tly, object2_tlx, object2_tly;

		getSpriteSize(object1->sprite, object1_w, object1_h);
		getSpriteSize(object2->sprite, object2_w, object2_h);

		object1_tlx = object1->x - object1_w / 2;
		object1_tly = object1->y - object1_h / 2;
		object2_tlx = object2->x - object2_w / 2;
		object2_tly = object2->y - object2_h / 2;

		if (object1_tlx < object2_tlx + object2_w &&
			object1_tlx + object1_w > object2_tlx &&
			object1_tly < object2_tly + object2_h &&
			object1_tly + object1_h > object2_tly)
			return true;

		return false;
	}

	void drawBackground() {
		int w, h;
		getSpriteSize(background->sprite, w, h);
		int pos_x = 0;
		int pos_y = 0;
		for (size_t i = 0; i < windows_y / h + 1; ++i) {
			for (size_t k = 0; k < windows_x / w + 1; ++k) {
				drawSprite(background->sprite, pos_x, pos_y);
				pos_x += w;
			}
			pos_x = 0;
			pos_y += h;
		}
	}

	void drawAsteroids() {
		for (size_t i = 0; i < asteroids.size(); ++i) {
			asteroids[i]->x += asteroids[i]->x_acceleration;
			asteroids[i]->y += asteroids[i]->y_acceleration;
			checkForBounds(asteroids[i]);
			drawSprite(asteroids[i]->sprite, asteroids[i]->x, asteroids[i]->y);
		}
	}

	void getPlayerAcceleration() {
		if (player->engine_state == false && (player->x_acceleration != 0 || player->y_acceleration != 0)) {

			if (player->x_acceleration > -0.001 && player->x_acceleration < 0.001)
				player->x_acceleration = 0;

			if (player->y_acceleration > -0.001 && player->y_acceleration < 0.001)
				player->y_acceleration = 0;

			if (player->x_acceleration < 0) {
				player->x_acceleration += max_player_speed / player_acceleration_multiplier;
			}
			else if (player->x_acceleration > 0) {
				player->x_acceleration -= max_player_speed / player_acceleration_multiplier;
			}
			if (player->y_acceleration < 0) {
				player->y_acceleration += max_player_speed / player_acceleration_multiplier;
			}
			else if (player->y_acceleration > 0) {
				player->y_acceleration -= max_player_speed / player_acceleration_multiplier;
			}
		}
	}

	void drawPlayer() {
		getPlayerAcceleration();
		player->x += player->x_acceleration;
		player->y += player->y_acceleration;
		checkForBounds(player);
		drawSprite(player->sprite, player->x, player->y);
	}

	void drawReticle() {
		if (reticle->drawStatus) {
			reticle->last_appearence = getTickCount();
			drawSprite(reticle->sprite, reticle->x, reticle->y);
			if (reticle->last_appearence - reticle->first_appearence > reticle_appearence_treshold)
				reticle->drawStatus = false;
		}
	}

	void drawBullets() {
		for (size_t i = 0; i < bullets.size(); ++i) {
			if (bullets[i]->drawStatus) {
				bullets[i]->x += bullets[i]->x_acceleration;
				bullets[i]->y += bullets[i]->y_acceleration;
				checkForBounds(bullets[i]);
				drawSprite(bullets[i]->sprite, bullets[i]->x, bullets[i]->y);
			}
			else {
				bullets[i]->x = player->x;
				bullets[i]->y = player->y;
			}
		}
	}

	void collisionBetweenAsteroidsAndBullets() {
		for (size_t i = 0; i < asteroids.size(); ++i) {
			for (size_t k = 0; k < bullets.size(); ++k) {
				if (i >= asteroids.size())
					break;
				if (bullets[k]->drawStatus == true && checkForCollisions(asteroids[i], bullets[k])) {
					if (asteroids[i]->small_size == true) {
						destroySprite(asteroids[i]->sprite);
						asteroids.erase(asteroids.begin() + i);
					}
					else {
						Rand_double rd{ 0, max_asteroid_speed };

						// change current asteroid
						destroySprite(asteroids[i]->sprite);
						asteroids[i]->sprite = createSprite("data\\small_asteroid.png");
						asteroids[i]->small_size = true;

						// create another asteroid
						std::unique_ptr<Asteroid> temp_asteroid = std::make_unique<Asteroid>();
						temp_asteroid->sprite = createSprite("data\\small_asteroid.png");
						temp_asteroid->small_size = true;
						temp_asteroid->x = asteroids[i]->x + 1;
						temp_asteroid->y = asteroids[i]->y + 1;
						temp_asteroid->x_acceleration = -1 * rd();
						temp_asteroid->y_acceleration = -1 * rd();
						asteroids.push_back(std::move(temp_asteroid));

					}
					bullets[k]->drawStatus = false;
					--bullets_count;
				}
			}
		}
	}

	void collisionsBetweenAsteroids() {
		for (size_t i = 0; i < asteroids.size(); ++i) {
			for (size_t k = 0; k < asteroids.size(); ++k) {
				if (i != k && checkForCollisions(asteroids[i], asteroids[k]) == true) {
					srand((unsigned)time(0));
					int variant = rand() % 5;
					if (variant == 0) {
						asteroids[i]->x_acceleration = -1 * asteroids[i]->x_acceleration;
						asteroids[i]->y_acceleration = -1 * asteroids[i]->y_acceleration;
						asteroids[k]->x_acceleration = -1 * asteroids[k]->x_acceleration;
						asteroids[k]->y_acceleration = -1 * asteroids[k]->y_acceleration;
					}
					else
						if (variant == 1) {
							asteroids[i]->y_acceleration = -1 * asteroids[i]->y_acceleration;
							asteroids[k]->x_acceleration = -1 * asteroids[k]->x_acceleration;
							asteroids[k]->y_acceleration = -1 * asteroids[k]->y_acceleration;
						}
						else
							if (variant == 2) {
								asteroids[i]->x_acceleration = -1 * asteroids[i]->x_acceleration;
								asteroids[k]->x_acceleration = -1 * asteroids[k]->x_acceleration;
								asteroids[k]->y_acceleration = -1 * asteroids[k]->y_acceleration;
							}
							else
								if (variant == 3) {
									asteroids[i]->x_acceleration = -1 * asteroids[i]->x_acceleration;
									asteroids[i]->y_acceleration = -1 * asteroids[i]->y_acceleration;
									asteroids[k]->y_acceleration = -1 * asteroids[k]->y_acceleration;
								}
								else
									if (variant == 4) {
										asteroids[i]->x_acceleration = -1 * asteroids[i]->x_acceleration;
										asteroids[i]->y_acceleration = -1 * asteroids[i]->y_acceleration;
										asteroids[k]->x_acceleration = -1 * asteroids[k]->x_acceleration;
									}
					asteroids[i]->x += asteroids[i]->x_acceleration;
					asteroids[i]->y += asteroids[i]->y_acceleration;
					asteroids[k]->x += asteroids[k]->x_acceleration;
					asteroids[k]->y += asteroids[k]->y_acceleration;
				}
			}
		}
	}

	void collisionsBetweenAsteroidsAndPlayer() {
		for (size_t i = 0; i < asteroids.size(); ++i) {
			if (checkForCollisions(asteroids[i], player))
				Init();
		}
	}

	// TODO
	// This function will return right acceleration for bullets (equal speed in all direction and independent from reticle distance to player)
	// Function, probably, should be realized by finding common point between circle(center = player and radius = bullets speed) and line(player to reticle)
	// Will be implemented in next releaze:)
	std::pair<double, double> getBulletAcceleration(double x0, double y0, double x1, double y1) {
		return { 0, 0 };
	}
};

int main(int argc, char* argv[])
{
	return run(new MyFramework());
}