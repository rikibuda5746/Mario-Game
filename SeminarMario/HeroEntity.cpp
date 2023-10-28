#include "HeroEntity.h"
#include "opencv2/opencv.hpp"
#include "Config.h"
#include <vector>
#include <memory>
#include <filesystem>
#include "LivesEntity.h"
using namespace cv;
using namespace std;
namespace fs = std::filesystem;

enum HeroStates {
	HERO_IDLE,
	HERO_RUN_RIGHT,
	HERO_RUN_LEFT,
	HERO_DUCK, 
	HERO_STAY_DUCK, 
	HERO_STAND_AFTER_DUCK, 
	HERO_JUMP,
	HERO_IDLE_ON_STONE,
	HERO_JUMP_FROM_STONE
};

EntityStatePtr createHeroState(
	fs::path const& animationFolder,
	HeroStates state, cv::Rect _bounds)
{
	AnimationPtr animation(new Animation(animationFolder.string()));
	bool isNotCyclic =
		state == HeroStates::HERO_DUCK ||
		state == HeroStates::HERO_STAND_AFTER_DUCK ||
		state == HeroStates::HERO_JUMP;
	bool isCyclic = !isNotCyclic;

	IGraphicsComponentPtr graphicsPtr(
		new SingleAnimationGraphics(animation, isCyclic));

	IPhysicsComponentPtr physicsPtr = nullptr;
	Size frameSize = graphicsPtr->getCollisionMask().size();
	IPhysicsComponentPtr boundsnonCollidingPhysicsPtr;
	switch (state)
	{
	case HERO_IDLE:
		physicsPtr.reset(new FixedWidgetPhysics());
		break;
	case HERO_RUN_RIGHT:
		physicsPtr.reset(new ConstVelocityPhysics(Point(frameSize.width / 10, 0)));
		    boundsnonCollidingPhysicsPtr =
			make_shared<BoundedPhysicsDecorator>(physicsPtr, _bounds);
		return make_shared<EntityState>(graphicsPtr, boundsnonCollidingPhysicsPtr);
		break;
	case HERO_RUN_LEFT:
		physicsPtr.reset(new ConstVelocityPhysics(Point(-frameSize.width / 10, 0)));
		boundsnonCollidingPhysicsPtr =
			make_shared<BoundedPhysicsDecorator>(physicsPtr, _bounds);
		return make_shared<EntityState>(graphicsPtr, boundsnonCollidingPhysicsPtr);
		break;
	case HERO_DUCK:
		physicsPtr.reset(new FixedWidgetPhysics());
		break;
	case HERO_STAY_DUCK:
		physicsPtr.reset(new FixedWidgetPhysics());
		break;
	case HERO_STAND_AFTER_DUCK:
		physicsPtr.reset(new FixedWidgetPhysics());
		break;
	case HERO_JUMP:
		physicsPtr.reset(new JumpPhysics(30,50,10));
		boundsnonCollidingPhysicsPtr =
			make_shared<BoundedPhysicsDecorator>(physicsPtr, _bounds);
		return make_shared<EntityState>(graphicsPtr, boundsnonCollidingPhysicsPtr);
		break;
	case HERO_IDLE_ON_STONE:
		physicsPtr.reset(new FixedWidgetPhysics());
		break;
	case HERO_JUMP_FROM_STONE:
		physicsPtr.reset(new JumpPhysics(30, 50, 10,350));
		boundsnonCollidingPhysicsPtr =
			make_shared<BoundedPhysicsDecorator>(physicsPtr, _bounds);
		return make_shared<EntityState>(graphicsPtr, boundsnonCollidingPhysicsPtr);
		break;
	default:
		throw std::exception("Unknown physics state!");
	}

	return make_shared<EntityState>(graphicsPtr, physicsPtr);
}

EntityPtr createHero(std::string const & rootAnimationsFolder,cv::Rect _bounds,EntityPtr lives,EntityPtr coin, EntityPtr scores)
{
	fs::path root = rootAnimationsFolder;
	auto idleFolder = root / "idle";

	auto idle = createHeroState(root / "idle", HeroStates::HERO_IDLE,_bounds);
	auto idleOnstone = createHeroState(root / "idle", HeroStates::HERO_IDLE_ON_STONE, _bounds);
	auto runRight = createHeroState(root / "runRight", HeroStates::HERO_RUN_RIGHT, _bounds);
	auto jump = createHeroState(root / "jump", HeroStates::HERO_JUMP, _bounds);
	auto jumpFromStone = createHeroState(root / "jump", HeroStates::HERO_JUMP_FROM_STONE, _bounds);
	auto runLeft = createHeroState(root / "runLeft", HeroStates::HERO_RUN_LEFT, _bounds);
	auto duckDown = createHeroState(root / "duckDown", HeroStates::HERO_DUCK, _bounds);
	auto duckStay = createHeroState(root / "duckStay", HeroStates::HERO_STAY_DUCK, _bounds);
	auto standAfterDuck= createHeroState(root / "standAfterDuck", HeroStates::HERO_STAND_AFTER_DUCK, _bounds);

	idle->addState(Event{ EventSenders::SENDER_KEYBOARD, EventTypes::EVENT_KEY_PRESSED, EventCodes::KEY_RIGHT }, runRight);
	idle->addState(Event{ EventSenders::SENDER_KEYBOARD, EventTypes::EVENT_KEY_PRESSED, EventCodes::KEY_LEFT }, runLeft);
	idle->addState(Event{ EventSenders::SENDER_KEYBOARD, EventTypes::EVENT_KEY_PRESSED, EventCodes::KEY_DOWN }, duckDown);
	idle->addState(Event{ EventSenders::SENDER_KEYBOARD, EventTypes::EVENT_KEY_PRESSED, EventCodes::KEY_UP }, jump);
	runRight->addState(Event{ EventSenders::SENDER_KEYBOARD, EventTypes::EVENT_KEY_PRESSED, EventCodes::KEY_LEFT }, idle);
	runRight->addState(Event{ EventSenders::SENDER_KEYBOARD, EventTypes::EVENT_KEY_PRESSED, EventCodes::KEY_UP }, jump);
	runRight->addState(Event{ EventSenders::SENDER_KEYBOARD, EventTypes::EVENT_KEY_PRESSED, EventCodes::KEY_DOWN }, duckDown);
	jump->addState(Event{ EventSenders::SENDER_ENTITY_STATE, EventTypes::EVENT_PHYSICS, EventCodes::ENTITY_PHYSICS_FINISHED }, idle);
	jump->addState(Event{ EventSenders::SENDER_ENTITY_STATE, EventTypes::EVENT_PHYSICS, EventCodes::COLLISION_WITH_STONE }, idleOnstone);
	runLeft->addState(Event{ EventSenders::SENDER_KEYBOARD, EventTypes::EVENT_KEY_PRESSED, EventCodes::KEY_RIGHT}, idle);
	duckDown->addState(Event{ EventSenders::SENDER_ENTITY_STATE, EventTypes::EVENT_GRAPHICS, EventCodes::ENTITY_FINISHED_ANIMATION }, duckStay);
	duckStay->addState(Event{ EventSenders::SENDER_KEYBOARD, EventTypes::EVENT_KEY_PRESSED, EventCodes::KEY_UP }, standAfterDuck);
	standAfterDuck->addState(Event{ EventSenders::SENDER_ENTITY_STATE, EventTypes::EVENT_GRAPHICS, EventCodes::ENTITY_FINISHED_ANIMATION }, idle);
	idleOnstone->addState(Event{ EventSenders::SENDER_KEYBOARD, EventTypes::EVENT_KEY_PRESSED, EventCodes::KEY_UP }, jumpFromStone);
	jumpFromStone->addState(Event{ EventSenders::SENDER_ENTITY_STATE, EventTypes::EVENT_PHYSICS, EventCodes::ENTITY_PHYSICS_FINISHED }, idle);
	
	EntityPtr hero(new Entity(idle));

	idle->Register(hero);
	idle->Register(lives);
	runRight->Register(hero);
	runRight->Register(lives);
	jump->Register(hero);
	jump->Register(lives);
	runLeft->Register(hero);
	runLeft->Register(lives);
	duckDown->Register(hero);
	duckDown->Register(lives);
	duckStay->Register(hero);
	duckStay->Register(lives);
	standAfterDuck->Register(hero);
	standAfterDuck->Register(lives);
	jumpFromStone->Register(hero);
	jumpFromStone->Register(lives);
	jumpFromStone->Register(coin);
	jumpFromStone->Register(scores);
	idleOnstone->Register(hero);
	idleOnstone->Register(lives);
	return hero;
}


void Collision(std::vector<EntityPtr> _pool,EntityPtr hero) {
	for (int i = 0; i < _pool.size(); i++)
	{
		if (hero->getState()->getPhysics()->checkCollision(_pool[i]->getState()->getPhysics())){
			hero->getState()->Notify(Event{ EventSenders::SENDER_ENTITY_STATE, EventTypes::EVENT_PHYSICS, EventCodes::COLLISION_WITH_ENEMY });
			hero->getState()->getPhysics()->reset(Point(hero->getState()->getPhysics()->getTL().x - 150, hero->getState()->getPhysics()->getTL().y));
		}
	}
}