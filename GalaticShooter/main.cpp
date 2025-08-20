#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"
#include "SFML/System.hpp"
#include "SFML/Audio.hpp"
#include "SFML/Network.hpp"

using namespace std;
using namespace sf;

RenderWindow GameScreen(VideoMode({ 800, 600 }), "Galatic Shooter", Style::Default);

// Galatic Ships
float ShipX = 0.0f;
float ShipY = 250.0f;
float MissileX;
float MissileY;
float EnemyShipX = 700.0f;
float EnemyShipY = 250.0f;

// Textures & Sprites
Texture Ship;
Sprite ShipSprite;
Texture ShipExhaust;
Sprite ShipExhaustSprite;
Texture EnemyShipExhaust;
Sprite EnemyShipExhaustSprite;
Texture Missile;
vector <Sprite> MissileSprite;
Sprite NewMissileSprite;
Texture EnemyShip;
Sprite EnemyShipSprite;
Texture BackGround;
Sprite BackGroundSprite;
Texture Planets;
Sprite PlanetsSprite;
Texture MissileBlast;
Sprite MissileBlastSprite;
Texture EnemyShipExplosion;
Sprite EnemyShipExplosionSprite;

// Clocks
Clock ExhaustAnimation;
int ExhaustFrameCount = 4;
int ExhaustCurrentFrame = 0;
Clock EnemyShipExhaustAnimation;
int EnemyShipExhaustFrameCount = 4;
int EnemyShipExhaustCurrentFrame = 0;
Clock MissileAnimation;
int MissileFrameCount = 5;
int CurrentMissileFrame = 0;
Clock MissileBlastAnimation;
int MissileBlastFrameCount = 5;
int CurrentMissileBlastFrame = 0;
Clock EnemyShipExplosionAnimation;
int EnemyShipExplosionFrameCount = 10;
int CurrentEnemyShipExplosionFrame = 0;
Clock EnemyShipExplosionTimer;
float TotalEnemyShipExplosionDuration = 0.4f;
Clock MissileBlastTimer;
float TotalMissileBlastDuration = 1.0f;

// Sound Assets
SoundBuffer MissileBuffer;
Sound MissileSound;
SoundBuffer ShipBuffer;
Sound ShipSound;

// Required Variables
bool isGameOver = false;
bool isSpacePressed = false;
bool isMissileActive = false;
bool isExplosionActive = false;
bool isMissileBlastActive = false;
int Score = 0;
int FrameWidth = 32;
int FrameHeight = 32;

// Load Game Assets
void GameAssets() {
	BackGround.loadFromFile("Assets/Background.png");
	BackGroundSprite.setTexture(BackGround);
	BackGroundSprite.setScale(800 / BackGround.getSize().x, 600 / BackGround.getSize().y);
	BackGroundSprite.setPosition(0, 0);

	Planets.loadFromFile("Assets/BgPlanets.png");
	PlanetsSprite.setTexture(Planets);
	PlanetsSprite.setPosition(100, 100);
	BackGroundSprite.setScale(800 / Planets.getSize().x, 600 / Planets.getSize().y);

	Ship.loadFromFile("Assets/PlayerShip.png");
	ShipSprite.setTexture(Ship);
	ShipSprite.setScale(1.0f, 1.0f);
	ShipSprite.setPosition(ShipX, ShipY);
	ShipBuffer.loadFromFile("Assets/Sounds/Spaceship.wav");
	ShipSound.setBuffer(ShipBuffer);
	ShipSound.play();
	ShipSound.setLoop(true);
	ShipExhaust.loadFromFile("Assets/Flight.png");
	ShipExhaustSprite.setTexture(ShipExhaust);
	ShipExhaustSprite.setPosition(ShipX - 16.0f, ShipY + 16.0f);

	Missile.loadFromFile("Assets/MissileLaunch.png");
	MissileBuffer.loadFromFile("Assets/Sounds/laser.wav");
	MissileSound.setBuffer(MissileBuffer);
	MissileBlast.loadFromFile("Assets/Blast.png");
	MissileBlastSprite.setTexture(MissileBlast);
	MissileBlastSprite.setScale(1.0f, 1.0f);

	EnemyShip.loadFromFile("Assets/EnemyShip.png");
	EnemyShipSprite.setTexture(EnemyShip);
	EnemyShipSprite.setScale(1.0f, 1.0f);
	EnemyShipSprite.setPosition(EnemyShipX, EnemyShipY);
	EnemyShipExhaust.loadFromFile("Assets/EnemyShipExhaust.png");
	EnemyShipExhaustSprite.setTexture(EnemyShipExhaust);
	EnemyShipExhaustSprite.setPosition(EnemyShipX + 110.0f, EnemyShipY + 32.0f);
	EnemyShipExhaustSprite.setScale(2.0f, 2.0f);
	EnemyShipExplosion.loadFromFile("Assets/EnemyShipBlast.png");
	EnemyShipExplosionSprite.setTexture(EnemyShipExplosion);
	EnemyShipExplosionSprite.setTextureRect(IntRect(0, 0, FrameWidth, FrameHeight));
	EnemyShipExplosionSprite.setScale(1.0f, 1.0f);
}

void ShipMovement() {
	if (Keyboard::isKeyPressed(Keyboard::W)) {
		ShipY -= 5.0f;
	}
	if (Keyboard::isKeyPressed(Keyboard::S)) {
		ShipY += 5.0f;
	}
	if (Keyboard::isKeyPressed(Keyboard::A)) {
		ShipX -= 5.0f;
	}
	if (Keyboard::isKeyPressed(Keyboard::D)) {
		ShipX += 5.0f;
	}
	ShipSprite.setPosition(ShipX, ShipY);
	ShipExhaustSprite.setPosition(ShipX - 16.0f, ShipY + 16.0f);

	if (Keyboard::isKeyPressed(Keyboard::Space)) {
		if (!isSpacePressed) {
			isSpacePressed = true;
			NewMissileSprite.setTexture(Missile);
			NewMissileSprite.setScale(1.0f, 1.0f);
			NewMissileSprite.setTextureRect(IntRect(0, 0, FrameWidth, FrameHeight));
			NewMissileSprite.setPosition(ShipX + 32.0f, ShipY + 16.0f);
			MissileSound.play();
			MissileSprite.push_back(NewMissileSprite);
		}
	}
	else {
		isSpacePressed = false;
	}

	if (ExhaustAnimation.getElapsedTime().asMilliseconds() > 100) {
		ExhaustCurrentFrame = (ExhaustCurrentFrame + 1) % ExhaustFrameCount;
		ShipExhaustSprite.setTextureRect(IntRect(ExhaustCurrentFrame * FrameWidth, 0, FrameWidth, FrameHeight));
		ExhaustAnimation.restart();
	}
}

void MissileAnim() {
	if (MissileAnimation.getElapsedTime().asMilliseconds() > 100) {
		CurrentMissileFrame = (CurrentMissileFrame + 1) % MissileFrameCount;
		MissileAnimation.restart();
	}

	for (auto& missile : MissileSprite) {
		missile.move(10.0f, 0);
		missile.setTextureRect(IntRect(CurrentMissileFrame * FrameWidth, 0, FrameWidth, FrameHeight));
	}
}

void ExplosionAnimation() {
	if (isExplosionActive) {
		if (EnemyShipExplosionAnimation.getElapsedTime().asMilliseconds() > 100) {
			CurrentEnemyShipExplosionFrame++;

			if (CurrentEnemyShipExplosionFrame >= EnemyShipExplosionFrameCount) {
				isExplosionActive = false;
			}
			else {
				EnemyShipExplosionSprite.setTextureRect(
					IntRect(CurrentEnemyShipExplosionFrame * FrameWidth, 0, FrameWidth, FrameHeight));
			}

			EnemyShipExplosionAnimation.restart();
		}
	}

	if (isMissileBlastActive) {
		if (MissileBlastAnimation.getElapsedTime().asMilliseconds() > 100) {
			CurrentMissileBlastFrame = (CurrentMissileBlastFrame + 1) % MissileBlastFrameCount;
			MissileBlastSprite.setTextureRect(IntRect(CurrentMissileBlastFrame * FrameWidth, 0, FrameWidth, FrameHeight));
			MissileBlastAnimation.restart();
		}
		if (MissileBlastTimer.getElapsedTime().asSeconds() > TotalMissileBlastDuration) {
			isMissileBlastActive = false;
		}
	}
}


void EnemyShipMovement() {
	int Min = 1, Max = 500;
	float CalculateEnemyShipY = Min + rand() % (Max - Min + 1);

	EnemyShipSprite.move(-3.0f, 0.0f);
	EnemyShipExhaustSprite.move(-3.0f, 0.0f);

	if (EnemyShipSprite.getPosition().x < -100) {
		EnemyShipY = CalculateEnemyShipY;
		EnemyShipSprite.setPosition(750.0f, EnemyShipY);
		EnemyShipExhaustSprite.setPosition(860.0f, EnemyShipY + 32.0f);
	}

	if (EnemyShipExhaustAnimation.getElapsedTime().asMilliseconds() > 100) {
		EnemyShipExhaustCurrentFrame = (EnemyShipExhaustCurrentFrame + 1) % EnemyShipExhaustFrameCount;
		EnemyShipExhaustSprite.setTextureRect(IntRect(EnemyShipExhaustCurrentFrame * FrameWidth, 0, FrameWidth, FrameHeight));
		EnemyShipExhaustAnimation.restart();
	}

	if (!MissileSprite.empty()) {
		auto& LatestMissile = MissileSprite.back();
		if (LatestMissile.getGlobalBounds().intersects(EnemyShipSprite.getGlobalBounds())) {
			Vector2f ExplosionPosition = EnemyShipSprite.getPosition();
			EnemyShipExplosionSprite.setPosition(ExplosionPosition);
			MissileBlastSprite.setPosition(LatestMissile.getPosition());

			isExplosionActive = true;
			isMissileBlastActive = true;

			CurrentEnemyShipExplosionFrame = 0;
			EnemyShipExplosionSprite.setTextureRect(IntRect(CurrentEnemyShipExplosionFrame * FrameWidth, 0, FrameWidth, FrameHeight));
			MissileBlastSprite.setTextureRect(IntRect(0, 0, FrameWidth, FrameHeight));

			CurrentEnemyShipExplosionFrame = 0;
			CurrentMissileBlastFrame = 0;

			EnemyShipExplosionAnimation.restart();
			MissileBlastAnimation.restart();
			EnemyShipExplosionTimer.restart();
			MissileBlastTimer.restart();

			MissileSprite.pop_back();

			EnemyShipY = CalculateEnemyShipY;
			EnemyShipSprite.setPosition(750.0f, EnemyShipY);
			EnemyShipExhaustSprite.setPosition(860.0f, EnemyShipY + 32.0f);
		}
	}
}

int main() {
	GameScreen.setFramerateLimit(60);
	GameAssets();
	srand(time(0));

	while (GameScreen.isOpen()) {
		Event UserResponse;
		while (GameScreen.pollEvent(UserResponse)) {
			if (UserResponse.type == Event::Closed) {
				GameScreen.close();
			}
		}

		if (!isGameOver) {
			ShipMovement();
			EnemyShipMovement();
		}

		MissileAnim();
		ExplosionAnimation();

		GameScreen.clear();
		GameScreen.draw(BackGroundSprite);
		GameScreen.draw(PlanetsSprite);
		GameScreen.draw(ShipSprite);
		GameScreen.draw(ShipExhaustSprite);

		for (const auto& m : MissileSprite) {
			GameScreen.draw(m);
		}

		if (isMissileBlastActive) {
			GameScreen.draw(MissileBlastSprite);
		}
		if (!isExplosionActive) {
			GameScreen.draw(EnemyShipSprite);
			GameScreen.draw(EnemyShipExhaustSprite);
		}

		if (isExplosionActive) {
			GameScreen.draw(EnemyShipExplosionSprite);
		}

		GameScreen.display();
	}
	return 0;
}
