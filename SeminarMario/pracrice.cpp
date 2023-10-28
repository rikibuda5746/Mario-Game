#include "Animation.h"
#include <opencv2/opencv.hpp>
#include "Timer.h"
#include "HeroEntity.h"
#include "LivesEntity.h"
#include "EntitiesPool.h"
#include "ScoreEntity.h"
#include "AppleEntity.h"
#include "StoneEntity.h"
#include "CoinEntity.h"
#include <vector>
#include "SlimeEntity.h"
using namespace cv;

int main()
{

	Mat background = imread(R"(../Animations/background.png)", IMREAD_UNCHANGED);
	cv::resize(background, background, cv::Size(1500, 600));

	EntityPoolPtr allSlime(new EntitiesPool());
	std::vector<EntityPtr> _pool;

	EntityPtr slimes = createSlime(R"(../Animations/SlimeOrange)");
	EntityPtr slime(new SlimeEntity(slimes->getState(),&_pool, allSlime));
	slime->reset(Point(background.size().width * 2 / 3, background.size().height * 4 / 5));
	EntityPtr slimes2 = createSlime(R"(../Animations/SlimeOrange)");
	EntityPtr slime2(new SlimeEntity(slimes2->getState(), &_pool, allSlime));
	slime2->reset(Point(background.size().width * 1 / 3, background.size().height * 4 / 5));
	allSlime->insert(slime);
	//allSlime->insert(slime2);
	_pool.push_back(allSlime->getNext());
	//_pool.push_back(allSlime->getNext());

	EntityPtr lives = createLive(R"(../Animations/heart)");
    EntityPtr live(new LivesEntity(lives->getState()));
	live->reset(Point(5, 5));
	live->getState()->getGraphics()->reset(4);
	

	EntityPtr score = createScore();
	EntityPtr scores(new ScoreEntity(score->getState()));
	scores->reset(Point(800, 80));

	std::vector<EntityPtr> coinVec;
	EntityPtr coin = createCoin(R"(../Animations/Coin)");
	EntityPtr coins(new CoinEntity(coin->getState(), &coinVec));
	coins->reset(Point(600, 300));
	coinVec.push_back(coins);

	EntityPtr hero = createHero(R"(../Animations/Hero)", cv::Rect(0, 12, background.size().width, background.size().height), live,coins,scores);
	hero->reset(Point(background.size().width / 2, background.size().height * 3/5));

	EntityApplePtr apple = createApple(R"(../Animations/apple)", cv::Rect(0, 12, background.size().width, background.size().height));
	EntityApplePtr apples(new AppleEntity(apple->getState()));

	EntityPtr stone = createStone(R"(../Animations/Stone)");
	stone->reset(Point(500,400));


	
	Timer timer(/*freq. ms=*/100);
	timer.Register(coins);
	timer.Register(slime);
	timer.Register(slime2);
	timer.Register(hero);
	timer.Register(apples);
	timer.Register(stone);
	scores->getState()->Register(live);
	live->getState()->Register(hero);
	apples->getState()->Register(scores);
	apples->getState()->Register(apples);
	apples->getState()->Register(slime);
	apples->getState()->Register(slime2);
	bool isToExit = false;
	while (false == isToExit)
	{

		Mat canvas = background.clone();
		timer.tick();
		Collision(_pool, hero);
	    live->draw(canvas);
		for (int i = 0; i < _pool.size(); i++)
		{
			_pool[i]->draw(canvas);
		}
		for (int i = 0; i < coinVec.size(); i++)
		{
			coinVec[i]->draw(canvas);
		}
		hero->draw(canvas);
		scores->draw(canvas);
		stone->draw(canvas);
		if (apples->getActive())
		{
			if (!apples->getReset())
			{
				apples->reset(hero->getState()->getPhysics()->getTL());
				apples->setReset(true);
			}
			apples->draw(canvas);
			CollisionApple(&_pool, apples,allSlime);
		}
		if(
			stone->getState()->getPhysics()->getTL().y <=
			hero->getState()->getPhysics()->getTL().y +
			hero->getState()->getPhysics()->getCollisionMask().size().height
			&&
			stone->getState()->getPhysics()->getTL().y + 50 >=
			hero->getState()->getPhysics()->getTL().y +
			hero->getState()->getPhysics()->getCollisionMask().size().height
			&&
				stone->getState()->getPhysics()->getTL().x<=
				hero->getState()->getPhysics()->getTL().x
				&&
				stone->getState()->getPhysics()->getTL().x
				+ stone->getState()->getPhysics()->getCollisionMask().size().width >=
				hero->getState()->getPhysics()->getTL().x
			&&
			stone->getState()->getPhysics()->checkCollision(
				hero->getState()->getPhysics()))
		{

			cout << "stone";
			hero->getState()->Notify(Event{ EventSenders::SENDER_ENTITY_STATE, EventTypes::EVENT_PHYSICS, EventCodes::COLLISION_WITH_STONE });
		}

		for (int i = 0; i < coinVec.size(); i++)
		{
			if (hero->getState()->getPhysics()->checkCollision(coinVec[i]->getState()->getPhysics()))
			{
				cout << "i"<<i<<"hell";
				hero->getState()->Notify(Event{ EventSenders::SENDER_ENTITY_STATE, EventTypes::EVENT_PHYSICS, EventCodes::COLLISION_WITH_FOOD_SCORE });
			}
		}
		imshow("test", canvas);
	}

	return 0;

}
