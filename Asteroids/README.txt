Hello!

Your task is to implement *simple* asteroids-like game
(please, see Asteroids (1979) arcade game for a reference if you like)

Asteroids should be spawned randomly on a map but not too close to the player (make a threshold). They have random impulse that set the movement
Asteroids can not pass through each other and should fly apart after collision (look Brownian motion for reference)
There are two types of asteroids: big and small
Once the asteroid touches player, the player dies and game restarts

You should be able to move your avatar with left/right/up/down buttons. Input gives to player a maximum speed, after input is released speed should decreases over time (inertia).
Have a reticle following mouse button
Player can shoot (e.g. by pressing left mouse button).
Once it is pressed, a bullet should move at the given direction.
If bullet is intersected with small asteroid, both asteroid and bullet should be destroyed.
If bullet is intersected with big asteroid, big asteroid break up into 2 small asteroids and bullet should be destroyed.
If there are no ammo, then oldest bullet should disappear and new one should be spawned

Player should always be in the center of a screen (i.e. if it moves map is recentered). You can add extra threshold, but it is not necessary

Everything "wrap around" the screen. This means if player goes past the edge of the map, he will reappear on the opposite side traveling the same direction. 
The same goes fo the asteoids and player bullets.

***
Additional improvements.
When asteroid is destroyed one of the special abilities can appear on the same place with some probability. Player can hold just one ability, the last one that player took.
Ability can be once activated for the limited period of time by pressing RMB. Abilities are: 
1)Power shield - it appears around player. If asteroid touches the circle, it continue moving in it's direction but do not get closer to player less then the radius of shield.
2)Homing missile - it follows an asteroid that was under mouse cursor when RMB was pressed. 
3)Auto shooting of nearby asteroids
***

Window size, map size, number of bullets and asteroids should be arbitrary and it should be possible to set it from the command-line, e.g.:

game.exe -window 800x600 -map 1000x1000 -num_asteroids 10 -num_ammo 3 -ability_probability 0.3

size of maps is in pixels (same as sprite size)

Functionality provided by Framework should be enough to do all required drawing.
You can load TGA, JPG and PNG images as sprites with it.
(there are some textures provided, you can use them if you wish)
Please, see example project for a template implementation

When implementing application take special care about performance and memeory usage.

Try to use data oriented programming approach.

Good luck!
Bye.

--


