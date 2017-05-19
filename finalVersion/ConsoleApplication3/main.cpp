#include <SFML/Graphics.hpp>
#include <time.h>
#include <list>
#include <sstream>
#include <windows.h>
#include <fstream>
#include <chrono>
#include <thread>
#include <cstdio>
#include <iostream>


using namespace sf;
//размер окна
const int W = 1366;
const int H = 768;
//один градус в радианах
const float DEGTORAD = 0.017453;
//класс анимации
class Animation
{
public:
	//кадр, скорость изменения кадра
	float Frame, speed;
	Sprite sprite;
    std::vector<IntRect> frames;//вектор кадров
	//конструктор по умолчанию
	Animation(){}
	//конструктор с параметрами
    Animation (Texture &t, int x, int y, int w, int h, int count, float Speed)
	{
	    Frame = 0;
        speed = Speed;
		//пушим кадры в вектор
		for (int i=0;i<count;i++)
         frames.push_back( IntRect(x+i*w, y, w, h));
		//устанавливаем текстуру
		sprite.setTexture(t);
		//центр кадра
		sprite.setOrigin(w/2,h/2);
        sprite.setTextureRect(frames[0]);
	}

	//метод прокрутки кадров
	void update()
	{
    	Frame += speed;
		int n = frames.size();
		if (Frame >= n) Frame -= n;
		if (n>0) sprite.setTextureRect( frames[int(Frame)] );
	}

	bool isEnd()
	{
	  return (Frame+speed)>=frames.size();
	}

};

//класс-родитель для корабля, пуль,комет
class Entity
{
public:
	int bulletSpeed;
float x,y,dx,dy,R,angle;//координаты обьетка, скорость по x и y, радиус, угол поворота
bool life;//жив обьект или нет
std::string name;//имя обьекта
Animation anim;//обЪект класса Animation, для передачи в метод по ссылке

//конструктор по умолчанию
Entity()
{
  life=true;
}

void settings(Animation &a,int X,int Y,float Angle=0,int radius=1)
{
  anim = a;
  x=X; y=Y;
  angle = Angle;
  R = radius;
}

virtual void update(){};

void draw(RenderWindow &app)
{
  anim.sprite.setPosition(x,y);
  anim.sprite.setRotation(angle+90);
  app.draw(anim.sprite);

 // CircleShape circle(R);
 // circle.setFillColor(Color(255,0,0,170));
 // circle.setPosition(x,y);
//circle.setOrigin(R,R);
 // //app.draw(circle);
}

virtual ~Entity(){};
};

//класс комет
class asteroid: public Entity
{
public:
	
  asteroid()//конструктор по умолчанию
  {
	  //задаем рандомную скорость
    dx=rand()%8-4;
    dy=rand()%8-4;
    name="asteroid";//имя кометы
  }

void  update()
  {
	//задаем координаты кометы
   x+=dx;
   y+=dy;
   //если соответствующая координата больше W или H, комета появляется в противоположной части экрана
   if (x>W) x=0; 
   if (x<0) x=W;
   if (y>H) y=0;
   if (y<0) y=H;
  }

};


//класс пуль
class bullet: public Entity
{
public:
  bullet()//конструктор по умолчанию
  {
	  bulletSpeed = 5;
    name="bullet";//имя пули
  }
  //главная функция для пуль
void  update()
  {
	//скорость пуль
   dx = cos(angle*DEGTORAD)*bulletSpeed;
   dy = sin(angle*DEGTORAD)*bulletSpeed;
   x+=dx;
   y+=dy;

   if (x>W || x<0 || y>H || y<0) life=false;//при вылете пули за размеры окна, жизнь пули равна false
  }

};


class Spaceship : public Entity
{
public:
	bool forward;//переменная отвечает за нажатие up
	int scores = 0;//к-во сбитых камней
	bool back;
	Spaceship()//конструктор по умолчанию
	{
		name = "player";//имя игрока(корабля)
	}
	//ф-ия отвечает за движение корабля
	void update()
	{
		if (forward)//если переменная равна true
		{
			//движение в направление носа корабля
			dx +=cos(angle*DEGTORAD)*0.2;
				dy += sin(angle*DEGTORAD)*0.2;
			
		}
		else if (back) {
			dx -= cos(angle*DEGTORAD)*0.2;
			dy -= sin(angle*DEGTORAD)*0.2;

		}
		
		else//иначе
		{
			//замедление
			dx *= 0.99;
			dy *= 0.99;
		}

		int maxSpeed = 5;//максимальная скорость корабля
		float speed = sqrt(dx*dx + dy*dy);
		if (speed > maxSpeed)
		{
			dx *= maxSpeed / speed;
			dy *= maxSpeed / speed;
		}
		
		x += dx;
			y += dy;
		
		if (x > W) x = 0; if (x < 0) x = W;
		if (y > H) y = 0; if (y < 0) y = H;
	}
	

};

//функция столкновения двух объектов
bool isCollide(Entity *a,Entity *b)
{
  return (b->x - a->x)*(b->x - a->x)+(b->y - a->y)*(b->y - a->y)<(a->R + b->R)*(a->R + b->R);
}




void menu(RenderWindow & window) {
	Texture menuTexture1, menuTexture2, menuTexture3, menuBackground,menuAbout;
	menuBackground.loadFromFile("images/spaceback.jpg");
	menuTexture1.loadFromFile("images/111.png");
	menuTexture2.loadFromFile("images/222.png");
	menuTexture3.loadFromFile("images/333.png");
	menuAbout.loadFromFile("images/about.jpg");
	Sprite menu1(menuTexture1), menu2(menuTexture2), menu3(menuTexture3), menuBg(menuBackground),menuA(menuAbout);
	bool isMenu = 1;
	int menuNum = 0;
	menu1.setPosition(600, 300);
	menu2.setPosition(635, 360);
	menu3.setPosition(645, 420);
	menuBg.setPosition(0, 0);
	menuA.setPosition(383,215);

	//////////////////////////////МЕНЮ///////////////////
	while (isMenu)
	{
		menu1.setColor(Color::White);
		menu2.setColor(Color::White);
		menu3.setColor(Color::White);
		
		menuNum = 0;
		window.clear(Color::Black);

		if (IntRect(600, 300, 221, 48).contains(Mouse::getPosition(window))) { menu1.setColor(Color::Black); menuNum = 1; }
		if (IntRect(635, 360, 136, 48).contains(Mouse::getPosition(window))) { menu2.setColor(Color::Black); menuNum = 2; }
		if (IntRect(645, 420, 101, 48).contains(Mouse::getPosition(window))) { menu3.setColor(Color::Black); menuNum = 3; }


		if (Mouse::isButtonPressed(Mouse::Left))
		{
			if (menuNum == 1) isMenu = false;//если нажали первую кнопку, то выходим из меню 
			if (menuNum == 2) { window.draw(menuA); window.display(); while (!Keyboard::isKeyPressed(Keyboard::Escape)); }
			if (menuNum == 3) { window.close(); isMenu = false; }

		}

		window.draw(menuBg);
		window.draw(menu1);
		window.draw(menu2);
		window.draw(menu3);

		window.display();
	}
}


int main()
{
	srand(time(0));
	RenderWindow app(VideoMode(W, H), "SPACEWAR");
	menu(app);
	app.setFramerateLimit(60);
	Texture t1, t2, t3, t4, t5, t6, t7, t8;
	t1.loadFromFile("images/spaceship.png");
	t2.loadFromFile("images/background.jpg");
	t3.loadFromFile("images/explosions/type_C.png");
	t4.loadFromFile("images/rock.png");
	t5.loadFromFile("images/fire_blue.png");
	t6.loadFromFile("images/rock_small.png");
	t7.loadFromFile("images/explosions/type_B.png");
	t8.loadFromFile("images/spaceback.jpg");
	int Player = 1;


	Clock clock;

	t1.setSmooth(true);
	t2.setSmooth(true);

	Sprite background(t2);
	Sprite gameover(t8);


	Animation sExplosion(t3, 0, 0, 256, 256, 48, 0.5);
	Animation sRock(t4, 0, 0, 64, 64, 16, 0.2);
	Animation sRock_small(t6, 0, 0, 64, 64, 16, 0.2);
	Animation sBullet(t5, 0, 0, 32, 64, 16, 0.8);
	Animation sPlayer(t1, 40, 0, 40, 40, 1, 0);
	Animation sPlayer_go(t1, 40, 40, 40, 40, 1, 0);
	Animation sExplosion_ship(t7, 0, 0, 192, 192, 64, 0.5);

	Font font;
	font.loadFromFile("10530.ttf");
	Text text("", font, 25);
	View view;
	//список указателей на объекты
	std::list<Entity*> entities;
	std::vector <int> bullSp(10,5);
	for (int i = 1; i < bullSp.size(); ++i) {
		bullSp[i] += 3*i;
	}

	

	for (int i = 0; i<10; i++)
	{
		asteroid *a = new asteroid();//выделяем память для asteroid
		a->settings(sRock, rand() % W, rand() % H, rand() % 360, 25);
		entities.push_back(a);//пушим в список
	}
	//выделяем память для Spaceship
	Spaceship *p = new Spaceship();
	p->settings(sPlayer, W/2, H/2, 0, 20);
	entities.push_back(p);//пушим в список

	 //пока открыто окно
	while (app.isOpen())
	{
		app.draw(background);//отрисовываем задний фон

		int timer = clock.getElapsedTime().asSeconds();//переменная для хранения времени в игре в секундах

		Event event;
		while (app.pollEvent(event))
		{
			if (event.type == Event::Closed)
				app.close();

			if (event.type == Event::KeyPressed)
				if (event.key.code == Keyboard::Space)//если нажат пробел
				{
					bullet *b = new bullet();//выделяем память для Bullet
					b->settings(sBullet, p->x, p->y, p->angle, 10);
					entities.push_back(b);
				}
		}

		if (Keyboard::isKeyPressed(Keyboard::Right)) p->angle += 3;
		if (Keyboard::isKeyPressed(Keyboard::Left))  p->angle -= 3;
		if (Keyboard::isKeyPressed(Keyboard::Up)) {
			p->forward = true; p->back = false;
		}
		else if (Keyboard::isKeyPressed(Keyboard::Down)) {
			p->back = true; p->forward = false;
		}

		else {
			p->forward = false;
			p->back = false;
		}

		for (auto a : entities)
			for (auto b : entities)
			{
				if (a->name == "asteroid" && b->name == "bullet")
					if (isCollide(a, b))
					{
						a->life = false;
						b->life = false;
						p->scores++;
						Entity *e = new Entity();
						e->settings(sExplosion, a->x, a->y);
						e->name = "explosion";
						entities.push_back(e);


						for (int i = 0; i<1; i++)
						{
							if (a->R == 15) continue;
							Entity *e = new asteroid();
							e->settings(sRock_small, a->x, a->y, rand() % 360, 15);
							entities.push_back(e);
						}

					}
				if ((p->scores) >=20 ) {
					b->bulletSpeed = 10;
				}
				

				if (a->name == "player" && b->name == "asteroid")
					if (isCollide(a, b))
					{
						std::this_thread::sleep_for(std::chrono::seconds(1));
						std::ofstream fout("records.txt"); // создаём объект класса ofstream для записи и связываем его с файлом cppstudio.txt
						fout << std::endl;
						fout << "Player ";
						fout << Player;
						fout << "    Time:";
						fout << timer;
						fout << " ";
						fout << "Scores:";
						fout << p->scores;
						fout << std::endl;
						fout.close();
						
						
						menu(app);
						Player++;
						b->life = false;
						p->scores = 0;
						Entity *e = new Entity();
						e->settings(sExplosion_ship, a->x, a->y);
						e->name = "explosion";
						entities.push_back(e);
						p->settings(sPlayer, W / 2, H / 2, 0, 20);//появление корабля в середине экрана
						p->dx = 0; p->dy = 0;//начальная 
						clock.restart();//обнуляем время в игре
					}
			}

		//если up не зажата
		if (p->forward)  p->anim = sPlayer_go;
		else   p->anim = sPlayer;//иначе


		for (auto e : entities)
			if (e->name == "explosion")
				if (e->anim.isEnd()) e->life = 0;

		if (rand() % 150 == 0)
		{
			asteroid *a = new asteroid();
			a->settings(sRock, 0, rand() % H, rand() % 360, 25);
			entities.push_back(a);
		}

		for (auto i = entities.begin(); i != entities.end();)
		{
			Entity *e = *i;

			e->update();
			e->anim.update();

			if (e->life == false) { i = entities.erase(i); delete e; }
			else i++;
		}


		for (auto i : entities)
			i->draw(app);

		std::ostringstream playerScoreString, timeString;
		playerScoreString << p->scores;
		timeString << timer;
		text.setString("Your scores:" + playerScoreString.str() + "  Time:" + timeString.str());//задаем строку тексту и вызываем сформированную выше строку методом .str() 
		text.setPosition(view.getCenter().x - 500, view.getCenter().y - 500); //задаем позицию текста
		app.draw(text);//рисуем этот текст
		app.display();
	}

    return 0;
}
