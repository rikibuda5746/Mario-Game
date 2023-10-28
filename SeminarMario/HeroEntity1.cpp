#include "HeroEntity1.h"
#include "opencv2/opencv.hpp"
#include "Config.h"
#include <vector>
#include <memory>
#include <filesystem>

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
	HERO_JUMP
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
		physicsPtr.reset(new JumpPhysics(5, 50, 10));
		boundsnonCollidingPhysicsPtr =
			make_shared<BoundedPhysicsDecorator>(physicsPtr, _bounds);
		return make_shared<EntityState>(graphicsPtr, boundsnonCollidingPhysicsPtr);
		break;
	default:
		throw std::exception("Unknown physics state!");
	}

	return make_shared<EntityState>(graphicsPtr, physicsPtr);
}

EntityPtr createHero(std::string const& rootAnimationsFolder, cv::Rect _bounds)
{
	fs::path root = rootAnimationsFolder;
	auto idleFolder = root / "idle";

	auto idle = createHeroState(root / "idle", HeroStates::HERO_IDLE, _bounds);
	auto runRight = createHeroState(root / "runRight", HeroStates::HERO_RUN_RIGHT, _bounds);
	auto jump = createHeroState(root / "jump", HeroStates::HERO_JUMP, _bounds);
	auto runLeft = createHeroState(root / "runLeft", HeroStates::HERO_RUN_LEFT, _bounds);
	auto duckDown = createHeroState(root / "duckDown", HeroStates::HERO_DUCK, _bounds);
	auto duckStay = createHeroState(root / "duckStay", HeroStates::HERO_STAY_DUCK, _bounds);
	auto standAfterDuck = createHeroState(root / "standAfterDuck", HeroStates::HERO_STAND_AFTER_DUCK, _bounds);

	idle->addState(Event{ EventSenders::SENDER_KEYBOARD, EventTypes::EVENT_KEY_PRESSED, EventCodes::KEY_RIGHT }, runRight);
	idle->addState(Event{ EventSenders::SENDER_KEYBOARD, EventTypes::EVENT_KEY_PRESSED, EventCodes::KEY_LEFT }, runLeft);
	idle->addState(Event{ EventSenders::SENDER_KEYBOARD, EventTypes::EVENT_KEY_PRESSED, EventCodes::KEY_DOWN }, duckDown);
	idle->addState(Event{ EventSenders::SENDER_KEYBOARD, EventTypes::EVENT_KEY_PRESSED, EventCodes::KEY_UP }, jump);
	runRight->addState(Event{ EventSenders::SENDER_KEYBOARD, EventTypes::EVENT_KEY_PRESSED, EventCodes::KEY_LEFT }, idle);
	runRight->addState(Event{ EventSenders::SENDER_KEYBOARD, EventTypes::EVENT_KEY_PRESSED, EventCodes::KEY_UP }, jump);
	runRight->addState(Event{ EventSenders::SENDER_KEYBOARD, EventTypes::EVENT_KEY_PRESSED, EventCodes::KEY_DOWN }, duckDown);
	jump->addState(Event{ EventSenders::SENDER_ENTITY_STATE, EventTypes::EVENT_PHYSICS, EventCodes::ENTITY_PHYSICS_FINISHED }, idle);
	runLeft->addState(Event{ EventSenders::SENDER_KEYBOARD, EventTypes::EVENT_KEY_PRESSED, EventCodes::KEY_RIGHT }, idle);
	duckDown->addState(Event{ EventSenders::SENDER_ENTITY_STATE, EventTypes::EVENT_GRAPHICS, EventCodes::ENTITY_FINISHED_ANIMATION }, duckStay);
	duckStay->addState(Event{ EventSenders::SENDER_KEYBOARD, EventTypes::EVENT_KEY_PRESSED, EventCodes::KEY_UP }, standAfterDuck);
	standAfterDuck->addState(Event{ EventSenders::SENDER_ENTITY_STATE, EventTypes::EVENT_GRAPHICS, EventCodes::ENTITY_FINISHED_ANIMATION }, idle);

	EntityPtr hero(new Entity(idle));

	idle->Register(hero);
	runRight->Register(hero);
	jump->Register(hero);
	runLeft->Register(hero);
	duckDown->Register(hero);
	duckStay->Register(hero);
	standAfterDuck->Register(hero);
	return hero;
}


void Collision(std::vector<EntityPtr> _pool, EntityPtr hero) {
	for (int i = 0; i < _pool.size(); i++)
	{
		if (hero->getState()->getPhysics()->checkCollision(_pool[i]->getState()->getPhysics()))
		{

			hero->getState()->Notify(Event{ EventSenders::SENDER_ENTITY_STATE, EventTypes::EVENT_PHYSICS, EventCodes::COLLISION_WITH_ENEMY });
		}
	}
}

void HeroEntity1::onNotify(Event const& e)
{
	auto newStateCandidate = _state->tryModifyState(e);
	
	if (newStateCandidate)
	{
		newStateCandidate->reset(_state->getPhysics()->getTL());
		_state = newStateCandidate;
	}
}
