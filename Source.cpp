#include <SFML/Graphics.hpp>
#include <conio.h>//добовляет условие, если клавиша не нажата

using namespace sf;

//камера
float offsetX = 0, offsetY = 0;
//
const int H = 18;//кол-во строк
const int W = 66;//кол-во символов в строке карты

const int ts = 77;//размер плиток
const int vr = 60;//размер врага

int lv = 1;
//карта
String TileMap[H] = {
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
"A            A                         A                         A",
"A            A                         A   A       A             A",
"AAAAAAAAAAA  A   A       AAA        A  A  AAAAAAAAAAAAAA      AAAA",
"A            A  AAAAAAAAAAAAAAAAAAAAA  A            A            A",
"A            AA           A            AAAAAAAAAA   A            A",
"A    A  A    A            A            A            AAAA      AAAA",
"A  AAAAAAAAAAAAAAAAAAAAA  A A         AA           AA            A",
"A            A           AA AAAAAAAAAAAA   A        A            A",
"A            A            A            A  AAAAAAAAAAAAAA      AAAA",
"AAAAAAAAAAA  AA  AA A A A A            A            A            A",
"A            A    A       A   A  A  A  AA           A            A",
"A            A    A       AAAAAAAAAAA  A       A    AAAA      AAAA",
"A  A        AA   AA A     A            AAAAAAAAAA   A            A",
"A  AAAAAAAAAAAA   A       A            A            A            A",
"A                 A       A                        AAAAA      AAAA",
"A                AAAAAAAAAA         A    A          AAAAAAAAAAAAAA",
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
};

bool play = false, menu = true, game = false, puskgame = false, pob = false, die = false, pause = false;

class setMenu {
public:
	bool pusklever[5], blv[5];

	setMenu() {
		for (int i = 0; i < 5; i++)
			pusklever[i] = false;

		blv[0] = true;
		for (int i = 1; i < 5; i++)
			blv[i] = false;
	}
};

class Player {
public:
	float dx, dy;
	FloatRect rect;
	bool onGround, rig;
	Sprite sprite;
	float curFrame, damFrame;
	int l, blockdam;
	bool life, damage;
	//отображение персонажа
	Player(Texture& image) {
		sprite.setTexture(image);
		rect = FloatRect(ts, ts * 2, 77 * 0.9, 65 * 0.9);//первоначальные координаты персонажа
		sprite.setTextureRect(IntRect(0, 0, 77, 65));//размер кадров
		sprite.setScale(0.9, 0.9);//размер персонажа
		dx = dy = 0;
		curFrame = 0;
		l = 3;
		life = true;
		damage = false;
		damFrame = 0;
		blockdam = 0;
	}
	//основная функция для персонажа
	void update(float time) {
		rect.left += dx * time;
		Collision(0);
		//гравитация
		if (!onGround)
			dy = dy + 0.0005 * time;
		//
		rect.top += dy * time;
		onGround = false;
		Collision(1);
		//смерть пресонажа
		if (l <= 0) {
			life = false;

			play = false;
			die = true;
		}

		if (pob)
			sprite.setTextureRect(IntRect(0, 0, 77, 65));
		//анимация персонажа
		curFrame += 0.01 * time;
		if (curFrame > 8)
			curFrame -= 7;

		if (dx > 0) {
			if (dy == 0)
				sprite.setTextureRect(IntRect(77 * int(curFrame), 0, 77, 65));
			else
				sprite.setTextureRect(IntRect(0, 81, 45, 65));

			rig = true;
		}
		if (dx < 0) {
			if (dy == 0)
				sprite.setTextureRect(IntRect(77 * int(curFrame) + 77, 0, -77, 65));
			else
				sprite.setTextureRect(IntRect(45, 81, -45, 65));

			rig = false;
		}
		//привязка персонажа к камере
		sprite.setPosition(rect.left - offsetX, rect.top - offsetY);
		dx = 0;
	}
	//взаимодействие персонажа с картой
	void Collision(int dir) {
		for (int i = rect.top / ts; i < (rect.top + rect.height) / ts; i++)
			for (int j = rect.left / ts; j < (rect.left + rect.width) / ts; j++) {
				if (TileMap[i][j] == 'A' || TileMap[i][j] == 'K') { //столкновение персонажа с плитками
					if (dx > 0 && dir == 0)
						rect.left = j * ts - rect.width; 
					if (dx < 0 && dir == 0)
						rect.left = j * ts + ts;
					if (dy > 0 && dir == 1) {
						rect.top = i * ts - rect.height;
						dy = 0;
						onGround = true;
					}
					if (dy < 0 && dir == 1) {
						rect.top = i * ts + ts;
						dy = 0;
					}
				}

				if (TileMap[i][j] == 'B')
					TileMap[i][j] = ' ';

				if (TileMap[i][j] == 'C') {
					TileMap[i][j] = ' ';

					l++;
				}

				if (TileMap[i][j] == '2' || TileMap[i][j] == '3' ||
					TileMap[i][j] == '4' || TileMap[i][j] == '5') {
					rect.left += ts + ts;

					for (int z = 0; z < H; z++)
						TileMap[z][j] = 'A';

					pob = true;
					play = false;
				}
			}
	}
};

class Pula {
public:
	float dx, dy;
	FloatRect rect;//для указания координат пули
	int go;
	Sprite sprite;
	//первоначальноеотображение пули
	Pula(Texture& image) {
		sprite.setTexture(image);
		dx = dy = 0;
		go = 0;
	}
	//сама функция пули
	void update(float time) {
		rect.left += dx * time;
		Collision(0);

		rect.top += dy * time;
		Collision(1);

		sprite.setPosition(rect.left - offsetX, rect.top - offsetY);
	}
	//для взаимодействия с картой
	void Collision(int dir) {
		for (int i = rect.top / ts; i < (rect.top + rect.height) / ts; i++)
			for (int j = rect.left / ts; j < (rect.left + rect.width) / ts; j++)
				if (TileMap[i][j] == 'A')
					if ((dx > 0 && dir == 0) || (dx < 0 && dir == 0))
						go = 0;
	}
};

class Enemy {
public:
	float dx, dy;
	FloatRect rect;
	Sprite sprite;
	float curFrame;
	bool life;
	//ии для врагов
	void set(Texture& image, int x, int y) {
		sprite.setTexture(image);
		sprite.setScale(0.38, 0.38);
		rect = FloatRect(x, y, ts, ts);

		dx = 0.1;
		curFrame = 0;
		life = false;
	}
	//основная функция
	void update(float time) {
		if (!pause)
			rect.left += dx * time;

		Collision();
		//анимация
		curFrame += time * 0.005;
		if (curFrame > 5)
			curFrame -= 5;
		
		if (life) {
			if (dx > 0)
				sprite.setTextureRect(IntRect(165 * int(curFrame), 0, 165, 204));
			else if (dx < 0)
				sprite.setTextureRect(IntRect(165 * int(curFrame) + 165, 0, -165, 204));
		}
		else
			sprite.setTextureRect(IntRect(0, 0, 0, 0));

		sprite.setPosition(rect.left - offsetX, rect.top - offsetY);
	}
	//взаимодействие с картой
	void Collision() {
		for (int i = rect.top / ts; i < (rect.top + rect.height) / ts; i++)
			for (int j = rect.left / ts; j < (rect.left + rect.width) / ts; j++)
				if (TileMap[i][j] == 'A') {
					if (dx > 0) {
						rect.left = j * ts - rect.width;
						dx *= -1;
					}
					else if (dx < 0) {
						rect.left = j * ts + ts;
						dx *= -1;
					}
				}
	}
};

class Boss {
public:
	float dx, dy;
	FloatRect rect;
	Sprite sprite;
	float curFrame;
	bool life, rig;
	int l;

	void set(Texture& image, int x, int y) {
		sprite.setTexture(image);
		sprite.setScale(1.5, 1.5);
		rect = FloatRect(x, y, ts, ts);

		dy = 0.2;
		curFrame = 0;
		life = true;
		rig = false;
		l = 10;
	}

	void update(float time) {
		if (!pause)
			rect.top += dy * time;

		Collision();

		if (life && l == 0) {
			life = false;

			sprite.setTextureRect(IntRect(0, 0, 0, 0));

			pob = true;
			play = false;
		}

		sprite.setPosition(rect.left - offsetX, rect.top - offsetY);
	}

	void Collision() {
		for (int i = rect.top / ts; i < (rect.top + rect.height) / ts; i++)
			for (int j = rect.left / ts; j < (rect.left + rect.width) / ts; j++)
				if (TileMap[i][j] == 'A')
				{
					if (dy > 0) {
						rect.top = i * ts - rect.height;
						dy *= -1;
					}
					else if (dy < 0) {
						rect.top = i * ts + ts;
						dy *= -1;
					}
				}
	}
};

class PulaBoss {
public:
	float dx, dy;
	FloatRect rect;
	int go;
	Sprite sprite;

	void set(Texture& image, int x, int y) {
		sprite.setTexture(image);
		dx = dy = 0;
		go = 0;
	}

	void update(float time) {
		if (!pause)
			rect.left += dx * time;
		Collision();

		if (!pause)
			rect.top += dy * time;

		sprite.setPosition(rect.left - offsetX, rect.top - offsetY);
	}

	void Collision() {
		for (int i = rect.top / ts; i < (rect.top + rect.height) / ts; i++)
			for (int j = rect.left / ts; j < (rect.left + rect.width) / ts; j++)
				if (TileMap[i][j] == 'A') {
					if (dx > 0) {
						rect.left = j * ts - rect.width;
						go = 0;
					}
					else if (dx < 0) {
						rect.left = j * ts + ts;
						go = 0;
					}
				}
	}
};

int main() {
	RenderWindow window(VideoMode(1000, 500), "Platformer Ptichka!");//создание окна
	//текстура фона
	Texture f;
	f.loadFromFile("C:/Users/torti/Desktop/курсовая/картинки/Play/fonn.png");
	Sprite fon(f);
	//текстура персонажа
	Texture Picture;
	Picture.loadFromFile("C:/Users/torti/Desktop/курсовая/картинки/Play/birddd.png");
	Player p(Picture);
	int frame = 0;
	//текстура плиток
	Texture t2;
	t2.loadFromFile("C:/Users/torti/Desktop/курсовая/картинки/Play/plat.png");
	Sprite plat(t2);
	//текстура пули
	Texture t1;
	t1.loadFromFile("C:/Users/torti/Desktop/курсовая/картинки/Play/pula.png");
	Pula pu(t1);
	//текстура жизней
	Texture tl;
	tl.loadFromFile("C:/Users/torti/Desktop/курсовая/картинки/Play/life.png");
	Sprite life[12];//отображение кол-ва жизней
	for (int i = 0; i < 12; i++) {
		life[i].setTexture(tl);
		life[i].setScale(1.5, 1.5);
		life[i].setPosition(5 + 50 * i, 5);
	}
	//текстура врага
	Texture t3;
	t3.loadFromFile("C:/Users/torti/Desktop/курсовая/картинки/Play/enemy.png");
	Enemy enemy[vr];
	enemy[0].set(t3, 4 * ts, 13 * ts);
	for (int i = 1; i < 4; i++)
		enemy[i].set(t3, (18 + (i - 1)) * ts, 3 * ts);
	for (int i = 4; i < 9; i++)
		enemy[i].set(t3, (28 + (i - 4)) * ts, 3 * ts);
	for (int i = 9; i < 16; i++)
		enemy[i].set(t3, (29 + (i - 9)) * ts, 7 * ts);
	for (int i = 16; i < 26; i++)
		enemy[i].set(t3, (27 + (i - 16)) * ts, 16 * ts);
	for (int i = 26; i < 36; i++)
		enemy[i].set(t3, (42 + (i - 26)) * ts, 16 * ts);
	for (int i = 36; i < 44; i++)
		enemy[i].set(t3, (40 + (i - 36)) * ts, 12 * ts);
	for (int i = 44; i < 53; i++)
		enemy[i].set(t3, (44 + (i - 44)) * ts, 8 * ts);
	for (int i = 53; i < 60; i++)
		enemy[i].set(t3, (44 + (i - 53)) * ts, 2 * ts);

	int enemydam = 0, bossdam = 0;
	//тестура босса
	Texture t4;
	t4.loadFromFile("C:/Users/torti/Desktop/курсовая/картинки/Play/Boss.png");
	Boss boss;
	boss.set(t4, 59 * ts, ts);//размер босса
	//текстура пули босса
	Texture t5;
	t5.loadFromFile("C:/Users/torti/Desktop/курсовая/картинки/Play/pulaBoss.png");
	PulaBoss puBos;
	puBos.set(t5, 59 * ts, ts);
	//отображение жизней босса
	Sprite lifeBoss[10];
	for (int i = 0; i < 10; i++) {
		lifeBoss[i].setTexture(t4);
		lifeBoss[i].setScale(0.5, 0.5);
		lifeBoss[i].setPosition(60 * i, 60);
	}
	//текстура "уровень"
	Texture nl;
	nl.loadFromFile("C:/Users/torti/Desktop/курсовая/картинки/Play/nadlev.png");
	Sprite nadlev(nl);
	nadlev.setPosition(700, 0);
	//текстура номера уровня
	Texture sc;
	sc.loadFromFile("C:/Users/torti/Desktop/курсовая/картинки/Play/score.png");
	Sprite score(sc);
	score.setTextureRect(IntRect(0, 0, 27, 36));
	score.setPosition(950, 5);
	//текстура названия игры
	Texture m1;
	m1.loadFromFile("C:/Users/torti/Desktop/курсовая/картинки/Menu/naz.png");
	Sprite naz(m1);
	naz.setPosition(200, 10);
	//текстура клавиши "играть"
	Texture m2;
	m2.loadFromFile("C:/Users/torti/Desktop/курсовая/картинки/Menu/butplay.png");
	Sprite butplay(m2);
	butplay.setPosition(200, 235);
	//текстура клавиши "выход"
	Texture m3;
	m3.loadFromFile("C:/Users/torti/Desktop/курсовая/картинки/Menu/exit.png");
	Sprite exit(m3);
	exit.setPosition(600, 235);
	//текстура клавиши "уровни" в меню
	Texture m4;
	m4.loadFromFile("C:/Users/torti/Desktop/курсовая/картинки/Menu/nazurav.png");
	Sprite nazurav(m4);
	nazurav.setPosition(350, 10);
	//текстура "номер уровня" в меню
	Texture m5;
	m5.loadFromFile("C:/Users/torti/Desktop/курсовая/картинки/Menu/butlev.png");
	Sprite butlev[5];
	for (int i = 0; i < 5; i++) {
		butlev[i].setTexture(m5);
		butlev[i].setPosition(150 + 150 * i, 230);
		butlev[i].setTextureRect(IntRect(0, 0, 80, 80));
	}
	butlev[0].setTextureRect(IntRect(0, 80, 80, 80));
	//текстура клавиши "назад"
	Texture m6;
	m6.loadFromFile("C:/Users/torti/Desktop/курсовая/картинки/Menu/back.png");
	Sprite back(m6);
	back.setPosition(150, 400);

	setMenu sm;

	//текстура "победа"
	Texture m7;
	m7.loadFromFile("C:/Users/torti/Desktop/курсовая/картинки/Menu/win.png");
	Sprite win(m7);
	win.setPosition(300, 100);
	//текстура "уровень пройден"
	Texture m8;
	m8.loadFromFile("C:/Users/torti/Desktop/курсовая/картинки/Menu/urpro.png");
	Sprite urpro(m8);
	urpro.setPosition(300, 100);
	//текстура клавиши "следующий уровень"
	Texture m9;
	m9.loadFromFile("C:/Users/torti/Desktop/курсовая/картинки/Menu/sledur.png");
	Sprite sledur(m9);
	sledur.setPosition(500, 250);
	//текстура клавиши "начать заново"
	Texture m10;
	m10.loadFromFile("C:/Users/torti/Desktop/курсовая/картинки/Menu/naczan.png");
	Sprite naczan(m10);
	naczan.setPosition(200, 250);
	//текстура клавиши "меню"
	Texture m11;
	m11.loadFromFile("C:/Users/torti/Desktop/курсовая/картинки/Menu/butmen.png");
	Sprite butmen(m11);
	butmen.setPosition(400, 350);
	//тексура "конец игры"
	Texture m12;
	m12.loadFromFile("C:/Users/torti/Desktop/курсовая/картинки/Menu/ki.png");
	Sprite ki(m12);
	ki.setPosition(400, 100);
	//текстура "пауза"
	Texture m13;
	m13.loadFromFile("C:/Users/torti/Desktop/курсовая/картинки/Menu/nadpause.png");
	Sprite nadpause(m13);
	nadpause.setPosition(400, 100);
	//текстура клавиши "продолжить"
	Texture m14;
	m14.loadFromFile("C:/Users/torti/Desktop/курсовая/картинки/Menu/nadprod.png");
	Sprite nadprod(m14);
	nadprod.setPosition(600, 250);

	Clock clock;//установка времени

	while (window.isOpen())
	{
		float time = clock.getElapsedTime().asMicroseconds();
		clock.restart();
		time = time / 800;

		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
				window.close();

			if (event.type == Event::MouseButtonPressed)
				if (event.key.code == Mouse::Left) {
					Vector2i pos = Mouse::getPosition(window);

					if (menu) {
						if (pos.x >= 600 && pos.x <= 774 &&
							pos.y >= 235 && pos.y <= 306)
							window.close();

						if (pos.x >= 200 && pos.x <= 392 &&
							pos.y >= 235 && pos.y <= 302) {
							menu = false;
							game = true;
						}
					}

					else if (game) {
						if (pos.x >= 150 && pos.x <= 273 &&
							pos.y >= 400 && pos.y <= 448) {
							game = false;
							menu = true;
						}

						for (int i = 0; i < 5; i++)
							if (pos.x >= 150 + 150 * i && pos.x <= 230 + 150 * i &&
								pos.y >= 230 && pos.y <= 310 && sm.blv[i]) {
								puskgame = true;
								sm.pusklever[i] = true;
							}
					}

					else if (pob || die || pause) {
						if (pos.x >= 400 && pos.x <= 685 &&
							pos.y >= 350 && pos.y <= 382) {
							pob = false;
							menu = true;
							play = false;
							die = false;
							p.damage = false;
							pause = false;
							enemydam = 0;
							p.blockdam = 0;
							bossdam = 0;

							p.l = 3;
							p.life = true;

							p.sprite.setTextureRect(IntRect(0, 0, 77, 65));

							for (int i = 0; i < 5; i++)
								sm.pusklever[i] = false;

							offsetX = 0;
							offsetY = 0;

							p.rect.left = 77;
							p.rect.top = 172.5;
						}

						if (pos.x >= 200 && pos.x <= 428 &&
							pos.y >= 250 && pos.y <= 291) {
							pob = false;
							puskgame = true;
							die = false;
							p.damage = false;
							pause = false;
							enemydam = 0;
							p.blockdam = 0;
							bossdam = 0;

							if (lv == 5)
								offsetY = 0;
						}

						if (pos.x >= 600 && pos.x <= 872 &&
							pos.y >= 250 && pos.y <= 290 && pause)
							pause = false;

						else if (pos.x >= 500 && pos.x <= 856 &&
							pos.y >= 250 && pos.y <= 293 && lv < 5 && !die) {
							pob = false;
							puskgame = true;
							die = false;
							p.damage = false;
							pause = false;
							enemydam = 0;
							p.blockdam = 0;
							bossdam = 0;

							sm.pusklever[lv - 1] = false;
							sm.pusklever[lv] = true;
						}
					}
				}
		}

		if (pob)
			sm.blv[lv] = true;

		if (sm.blv[1])
			butlev[1].setTextureRect(IntRect(80, 80, 80, 80));
		if (sm.blv[2])
			butlev[2].setTextureRect(IntRect(80 * 2, 80, 80, 80));
		if (sm.blv[3])
			butlev[3].setTextureRect(IntRect(80 * 3, 80, 80, 80));
		if (sm.blv[4])
			butlev[4].setTextureRect(IntRect(0, 80 * 2, 80, 80));

		if (puskgame) {
			puskgame = false;
			game = false;

			if (sm.pusklever[0]) {
				lv = 1;

				offsetX = 0;
				offsetY = 0;

				p.rect.left = 77;
				p.rect.top = 172.5;

				enemy[0].life = true;

				for (int i = 4; i < 7; i++)
					TileMap[9][i] = 'B';
				TileMap[16][7] = 'C';
				for (int i = 15; i < 17; i++)
					TileMap[i][13] = '2';
			}
			if (sm.pusklever[1]) {
				lv = 2;

				p.rect.left = 1098.39;
				p.rect.top = 1250.5;

				offsetX = p.rect.left;
				offsetY = p.rect.top - 365;

				for (int i = 1; i < 4; i++)
					enemy[i].life = true;

				for (int i = 15; i < 17; i++)
					TileMap[i][13] = 'A';
				TileMap[9][14] = 'B';
				for (int i = 17; i < 19; i++)
					TileMap[9][i] = 'B';
				TileMap[9][22] = 'B';
				for (int i = 19; i < 22; i++)
					TileMap[6][i] = 'C';
				TileMap[3][14] = 'B';
				for (int i = 1; i < 3; i++)
					TileMap[i][26] = '3';
			}
			if (sm.pusklever[2]) {
				lv = 3;

				p.rect.left = 2079;
				p.rect.top = 172.5;

				offsetX = p.rect.left + 500;
				offsetY = 0;

				for (int i = 4; i < 9; i++)
					enemy[i].life = true;
				for (int i = 9; i < 16; i++)
					enemy[i].life = true;
				for (int i = 16; i < 26; i++)
					enemy[i].life = true;

				for (int i = 1; i < 3; i++)
					TileMap[i][26] = 'A';
				for (int i = 28; i < 30; i++)
					TileMap[2][i] = 'B';
				for (int i = 31; i < 33; i++)
					TileMap[2][i] = 'B';
				for (int i = 34; i < 36; i++)
					TileMap[2][i] = 'B';
				for (int i = 29; i < 32; i++)
					TileMap[6][i] = 'B';
				for (int i = 33; i < 36; i++)
					TileMap[6][i] = 'B';
				for (int i = 27; i < 29; i++)
					TileMap[11][i] = 'C';
				for (int i = 31; i < 33; i++)
					TileMap[11][i] = 'B';
				for (int i = 34; i < 36; i++)
					TileMap[11][i] = 'C';
				for (int i = 27; i < 31; i++)
					TileMap[14][i] = 'B';
				for (int i = 32; i < 36; i++)
					TileMap[14][i] = 'B';
				for (int i = 15; i < 17; i++)
					TileMap[i][39] = '4';
			}
			if (sm.pusklever[3]) {
				lv = 4;

				p.rect.left = 3080;
				p.rect.top = 1250.5;

				offsetX = p.rect.left;
				offsetY = p.rect.top - 365;

				for (int i = 26; i < 36; i++)
					enemy[i].life = true;
				for (int i = 36; i < 44; i++)
					enemy[i].life = true;
				for (int i = 44; i < 53; i++)
					enemy[i].life = true;
				for (int i = 53; i < 60; i++)
					enemy[i].life = true;

				for (int i = 15; i < 17; i++)
					TileMap[i][39] = 'A';
				TileMap[15][41] = 'C';
				for (int i = 42; i < 46; i++)
					TileMap[11][i] = 'B';
				TileMap[8][47] = 'C';
				TileMap[8][48] = 'B';
				for (int i = 43; i < 46; i++)
					TileMap[4][i] = 'B';
				for (int i = 1; i < 3; i++)
					TileMap[i][52] = '5';
			}
			if (sm.pusklever[4]) {
				lv = 5;

				boss.life = true;
				boss.l = 10;

				p.rect.left = 4112.69;
				p.rect.top = 172.5;

				for (int i = 1; i < 3; i++)
					TileMap[i][52] = 'A';
				for (int i = 0; i < 4; i++) {
					TileMap[4 + 3 * i][53] = 'B';
					TileMap[5 + 3 * i][54] = 'C';
				}
				for (int i = 0; i < 5; i++) {
					TileMap[2 + 3 * i][63] = 'C';
					TileMap[1 + 3 * i][64] = 'B';
				}
			}
			//изначальное кол-во жизней игрока
			p.l = 3;
			p.life = true;

			play = true;
		}
		
		if (p.life && !p.damage && play && !pause) {
			//проверка на нажатие клавиш
			if (!_kbhit())
				if (p.dy == 0) {
					if (p.rig)
						p.sprite.setTextureRect(IntRect(0, 0, 77, 65));
					else if (!p.rig)
						p.sprite.setTextureRect(IntRect(77, 0, -77, 65));
				}
			//подключения правой и левой стрелочки для упраления движения
			if (Keyboard::isKeyPressed(Keyboard::Left))
				p.dx = -0.3;
			if (Keyboard::isKeyPressed(Keyboard::Right))
				p.dx = 0.3;
			//подключение стрелочки для прыжка
			if (Keyboard::isKeyPressed(Keyboard::Up))
				if (p.onGround) {
					p.dy = -0.5;
					//фреймы для прыжка
					if (p.rig)
						p.sprite.setTextureRect(IntRect(0, 81, 45, 65));
					else if (!p.rig)
						p.sprite.setTextureRect(IntRect(45, 81, -45, 65));

					p.onGround = false;
				}
			if (Keyboard::isKeyPressed(Keyboard::Space))//подключение пробела для стрельбы
				if (p.onGround && pu.go == 0) {
					frame = 100;

					if (p.rig) {
						pu.go = 1;

						pu.rect.left = p.rect.left + 67;
						pu.rect.top = p.rect.top + 20;
					}
					else {
						pu.go = 2;

						pu.rect.left = p.rect.left - 12;
						pu.rect.top = p.rect.top + 18;
					}
				}
			//подключение эскейп как паузу
			if (Keyboard::isKeyPressed(Keyboard::Escape))
				pause = true;
		}

		if (pu.go == 0) {
			pu.rect.left = p.rect.left;
			pu.rect.top = p.rect.top;
		}
		else if (pu.go == 1)
			pu.dx = 2;
		else if (pu.go == 2)
			pu.dx = -2;
		//отображение текстур поза стрельбы
		if (frame != 0)
			if (p.dy == 0) {
				if (p.rig)
					p.sprite.setTextureRect(IntRect(68, 80, 69, 61));
				else if (!p.rig)
					p.sprite.setTextureRect(IntRect(68 + 69, 80, -69, 61));

				frame--;
			}

		if (!enemy[0].life) {
			TileMap[11][3] = ' ';
			TileMap[12][3] = ' ';
		}
		else if (enemy[0].life) {
			TileMap[11][3] = 'K';
			TileMap[12][3] = 'K';
		}
		if (!enemy[1].life && !enemy[2].life && !enemy[3].life) {
			TileMap[1][25] = ' ';
			TileMap[2][25] = ' ';
		}
		else if (enemy[1].life && enemy[2].life && enemy[3].life) {
			TileMap[1][25] = 'K';
			TileMap[2][25] = 'K';
		}
		if (!enemy[4].life && !enemy[5].life && !enemy[6].life && !enemy[7].life && !enemy[8].life) {
			TileMap[1][36] = ' ';
			TileMap[2][36] = ' ';
		}
		else if (enemy[4].life && enemy[5].life && enemy[6].life && enemy[7].life && enemy[8].life) {
			TileMap[1][36] = 'K';
			TileMap[2][36] = 'K';
		}
		if (!enemy[9].life && !enemy[10].life && !enemy[10].life && !enemy[11].life && !enemy[12].life &&
			!enemy[13].life && !enemy[14].life && !enemy[15].life) {
			TileMap[5][28] = ' ';
			TileMap[6][28] = ' ';
		}
		else if (enemy[9].life && enemy[10].life && enemy[10].life && enemy[11].life && enemy[12].life &&
			enemy[13].life && enemy[14].life && enemy[15].life) {
			TileMap[5][28] = 'K';
			TileMap[6][28] = 'K';
		}
		if (!enemy[16].life && !enemy[17].life && !enemy[18].life && !enemy[19].life && !enemy[20].life &&
			!enemy[21].life && !enemy[22].life && !enemy[23].life && !enemy[24].life && !enemy[25].life) {
			TileMap[15][38] = ' ';
			TileMap[16][38] = ' ';
		}
		else if (enemy[16].life && enemy[17].life && enemy[18].life && enemy[19].life && enemy[20].life &&
			enemy[21].life && enemy[22].life && enemy[23].life && enemy[24].life && enemy[25].life) {
			TileMap[15][38] = 'K';
			TileMap[16][38] = 'K';
		}
		if (!enemy[26].life && !enemy[27].life && !enemy[28].life && !enemy[29].life && !enemy[30].life &&
			!enemy[31].life && !enemy[32].life && !enemy[33].life && !enemy[34].life && !enemy[35].life) {
			TileMap[13][49] = ' ';
			TileMap[13][50] = ' ';
			TileMap[13][51] = ' ';
		}
		else if (enemy[26].life && enemy[27].life && enemy[28].life && enemy[29].life && enemy[30].life &&
			enemy[31].life && enemy[32].life && enemy[33].life && enemy[34].life && enemy[35].life) {
			TileMap[13][49] = 'K';
			TileMap[13][50] = 'K';
			TileMap[13][51] = 'K';
		}
		if (!enemy[36].life && !enemy[37].life && !enemy[38].life && !enemy[39].life && !enemy[40].life &&
			!enemy[41].life && !enemy[42].life && !enemy[43].life) {
			TileMap[9][40] = ' ';
			TileMap[9][41] = ' ';
		}
		else if (enemy[36].life && enemy[37].life && enemy[38].life && enemy[39].life && enemy[40].life &&
			enemy[41].life && enemy[42].life && enemy[43].life) {
			TileMap[9][40] = 'K';
			TileMap[9][41] = 'K';
		}
		if (!enemy[44].life && !enemy[45].life && !enemy[46].life && !enemy[47].life &&
			!enemy[48].life && !enemy[49].life && !enemy[50].life) {
			TileMap[5][49] = ' ';
			TileMap[5][50] = ' ';
			TileMap[5][51] = ' ';
		}
		else if (enemy[44].life && enemy[45].life && enemy[46].life && enemy[47].life &&
			enemy[48].life && enemy[49].life && enemy[50].life) {
			TileMap[5][49] = 'K';
			TileMap[5][50] = 'K';
			TileMap[5][51] = 'K';
		}
		if (!enemy[53].life && !enemy[54].life && !enemy[55].life && !enemy[56].life && !enemy[57].life &&
			!enemy[58].life && !enemy[59].life)
			TileMap[1][51] = ' ';
		else if (enemy[53].life && enemy[54].life && enemy[55].life && enemy[56].life && enemy[57].life &&
			enemy[58].life && enemy[59].life)
			TileMap[1][51] = 'K';

		if (boss.life) {
			if (boss.rect.left < p.rect.left) {
				boss.sprite.setTextureRect(IntRect(0, 0, 102, 51));
				boss.rig = true;
			}
			else if (boss.rect.left > p.rect.left) {
				boss.sprite.setTextureRect(IntRect(102, 0, -102, 51));
				boss.rig = false;
			}

			if (boss.rect.top > p.rect.top && boss.rect.top < p.rect.top + p.rect.height &&
				puBos.go == 0 && lv == 5) {
				if (boss.rig)
					puBos.go = 1;
				else
					puBos.go = 2;
			}

			if (puBos.go == 0) {
				if (boss.rig) {
					puBos.rect.left = boss.rect.left + 153;
					puBos.rect.top = boss.rect.top;
				}
				else if (!boss.rig) {
					puBos.rect.left = boss.rect.left;
					puBos.rect.top = boss.rect.top;
				}
			}
			else if (puBos.go == 1)
				puBos.dx = 1;
			else if (puBos.go == 2)
				puBos.dx = -1;

			if (boss.rect.left < pu.rect.left && boss.rect.left + 153 > pu.rect.left && boss.life &&
				boss.rect.top < pu.rect.top && boss.rect.top + 77 > pu.rect.top && pu.go != 0) {
				boss.l--;

				pu.go = 0;
			}

			if (p.rect.intersects(boss.rect) && !p.damage) {

				p.dy = -0.5;
				p.l--;

				p.damage = true;

				bossdam = 1;
			}

			if (p.rect.left < puBos.rect.left && p.rect.left + 77 > puBos.rect.left && p.life && !p.damage &&
				p.rect.top < puBos.rect.top && p.rect.top + 65 > puBos.rect.top && puBos.go != 0) {
				p.l--;
				p.damage = true;

				bossdam = 1;
			}
		}

		for (int i = 0; i < vr; i++)
			if (enemy[i].life) {
				if (enemy[i].rect.left < pu.rect.left && enemy[i].rect.left + 10 > pu.rect.left &&
					enemy[i].rect.top < pu.rect.top && enemy[i].rect.top + 50 > pu.rect.top && pu.go != 0) {
					enemy[i].life = false;

					pu.go = 0;
				}

				if (p.rect.intersects(enemy[i].rect) && !p.damage) {
					p.l--;

					p.dy = -0.5;
					p.damage = true;

					if (i == 0)
						enemydam = 1;
					else if (i >= 1 && i < 4)
						enemydam = 2;
					else if (i >= 4 && i < 9)
						enemydam = 3;
					else if (i >= 9 && i < 16)
						enemydam = 4;
					else if (i >= 16 && i < 26)
						enemydam = 5;
					else if (i >= 26 && i < 36)
						enemydam = 6;
					else if (i >= 36 && i < 44)
						enemydam = 7;
					else if (i >= 44 && i < 53)
						enemydam = 8;
					else if (i >= 53 && i < 60)
						enemydam = 9;
				}
			}

		if (p.damage) {
			p.damFrame += 0.006;

			switch (int(p.damFrame))
			{
			case 1:
				if (p.rig)
					p.sprite.setTextureRect(IntRect(229, 72, 72, 65));
				else
					p.sprite.setTextureRect(IntRect(301, 72, -72, 65));
				break;
			case 2:
				if (p.rig)
					p.sprite.setTextureRect(IntRect(308, 95, 76, 34));
				else
					p.sprite.setTextureRect(IntRect(384, 95, -76, 34));
				break;
			case 3:
				if (p.rig)
					p.sprite.setTextureRect(IntRect(391, 101, 80, 31));
				else
					p.sprite.setTextureRect(IntRect(471, 101, -80, 31));
				break;
			case 4:
				if (p.l > 0) {
					if (enemydam != 0)
						switch (enemydam)
						{
						case 1:
							p.rect.left = 931.7;
							p.rect.top = 942.5;
							break;
						case 2:
							p.rect.left = 1312.52;
							p.rect.top = 172.5;
							break;
						case 3:
							p.rect.left = 2093.68;
							p.rect.top = 172.5;
							break;
						case 4:
							p.rect.left = 2933.7;
							p.rect.top = 480.5;
							break;
						case 5:
							p.rect.left = 2775.62;
							p.rect.top = 1173.5;
							break;
						case 6:
							p.rect.left = 3176.79;
							p.rect.top = 1173.5;
							break;
						case 7:
							p.rect.left = 3615.72;
							p.rect.top = 865.5;
							break;
						case 8:
							p.rect.left = 3330.41;
							p.rect.top = 557.5;
							break;
						case 9:
							p.rect.left = 3309.64;
							p.rect.top = 95.5;
							break;
						}
					else if (p.blockdam != 0)
						switch (p.blockdam)
						{
						case 1:
							p.rect.left = 931.7;
							p.rect.top = 942.5;
							break;
						case 2:
							p.rect.left = 584.22;
							p.rect.top = 711.5;
							break;
						case 3:
							p.rect.left = 1362.86;
							p.rect.top = 711.5;
							break;
						case 4:
							p.rect.left = 2173.41;
							p.rect.top = 865.5;
							break;
						case 5:
							p.rect.left = 3626.1;
							p.rect.top = 865.5;
							break;
						case 6:
							p.rect.left = 3311.82;
							p.rect.top = 557.5;
							break;
						case 7:
							p.rect.left = 4081;
							p.rect.top = 172.5;
							offsetY = 0;
							break;
						}
					else if (bossdam == 1) {
						p.rect.left = 4081;
						p.rect.top = 172.5;
						offsetY = 0;
					}

					if (p.rig)
						p.sprite.setTextureRect(IntRect(0, 0, 77, 65));
					else if (!p.rig)
						p.sprite.setTextureRect(IntRect(77, 0, -77, 65));

					enemydam = 0;
					p.blockdam = 0;
					bossdam = 0;

					p.damage = false;
					p.damFrame = 0;
				}
				break;
			}
		}

		score.setTextureRect(IntRect(27 * (lv - 1), 0, 27, 36));
		if (p.rect.left > 500 && p.rect.left < 4582)
			offsetX = p.rect.left - 500;
		if (p.rect.top > 250 && p.rect.top < 1135)
			offsetY = p.rect.top - 250;
		//чтобы работало и отображалось
		p.update(time);
		pu.update(time);
		for (int i = 0; i < vr; i++)
			enemy[i].update(time);
		boss.update(time);
		puBos.update(time);
		window.clear(Color::White);//очистка экрана

		fon.setPosition(-offsetX, -offsetY);
		window.draw(fon);
		// отображение плиток на карте
		for (int i = 0; i < H; i++)
			for (int j = 0; j < W; j++) {
				if (TileMap[i][j] == 'A')
					plat.setTextureRect(IntRect(0, 0, ts, ts));
				if (TileMap[i][j] == 'B')
					plat.setTextureRect(IntRect(ts, 0, ts, ts));
				if (TileMap[i][j] == 'C')
					plat.setTextureRect(IntRect(ts * 2, 0, ts, ts));
				if (TileMap[i][j] == 'X')
					plat.setTextureRect(IntRect(ts * 3, 0, ts, ts));
				if (TileMap[i][j] == 'F')
					plat.setTextureRect(IntRect(ts * 4, 0, ts, ts));
				if (TileMap[i][j] == '2')
					plat.setTextureRect(IntRect(0, ts, ts, ts));
				if (TileMap[i][j] == '3')
					plat.setTextureRect(IntRect(ts, ts, ts, ts));
				if (TileMap[i][j] == '4')
					plat.setTextureRect(IntRect(ts * 2, ts, ts, ts));
				if (TileMap[i][j] == '5')
					plat.setTextureRect(IntRect(ts * 3, ts, ts, ts));
				if (TileMap[i][j] == 'K')
					plat.setTextureRect(IntRect(ts * 4, 0, ts, ts));
				if (TileMap[i][j] == ' ')
					continue;

				plat.setPosition(j * ts - offsetX, i * ts - offsetY);
				window.draw(plat);
			}
		//отображение текстур
		window.draw(p.sprite);
		if (pu.go != 0)
			window.draw(pu.sprite);
		for (int i = 0; i < vr; i++)
			window.draw(enemy[i].sprite);
		window.draw(boss.sprite);
		if (puBos.go != 0)
			window.draw(puBos.sprite);

		if (play || pob || die || pause) {
			for (int i = 0; i < p.l; i++)
				window.draw(life[i]);
			for (int i = 0; i < boss.l; i++)
				if (lv == 5)
					window.draw(lifeBoss[i]);
			window.draw(nadlev);
			window.draw(score);
			//отображение текстур
			if (pob || die || pause) {
				if (pob && !die && !pause) {
					if (lv < 5)
						window.draw(urpro);
					else
						window.draw(win);
				}
				else if (!pob && die && !pause)
					window.draw(ki);
				else if (!pob && !die && pause)
					window.draw(nadpause);

				if (lv < 5 && !die && !pause)
					window.draw(sledur);
				else if (pause)
					window.draw(nadprod);

				window.draw(naczan);
				window.draw(butmen);
			}
		}
		//отображение кнопок и текста в меню
		if (menu) {
			window.draw(naz);
			window.draw(butplay);
			window.draw(exit);
		}
		//отображение номера уровня
		if (game) {
			window.draw(nazurav);
			for (int i = 0; i < 5; i++)
				window.draw(butlev[i]);
			window.draw(back);
		}
		window.display();
	}
	return 0;
}