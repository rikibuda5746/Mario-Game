#pragma once

#include "Entities.h"
#include "Config.h"
#include "EntitiesPool.h"

class AppleEntity:public Entity
{
private:
	bool isActive;
	bool isReset;
public:
	AppleEntity(EntityStatePtr state) :Entity(state) {}
	virtual void onNotify(Event const& e) override
	{
		if (e.sender == EventSenders::SENDER_TIMER
			&&
			e.type == EventTypes::EVENT_TIMER
			&&
			e.code == EventCodes::TIME_TICK)
		{
			_state->update();
		}

		if (e.sender == EventSenders::SENDER_KEYBOARD
			&&
			e.type == EventTypes::EVENT_KEY_PRESSED
			&&
			e.code == EventCodes::KEY_THROW)
		{
			this->setActive(true);
		}
		if ((e.sender == EventSenders::SENDER_ENTITY_STATE
			&&
			e.type == EventTypes::EVENT_GRAPHICS
			&&
			e.code == EventCodes::ENTITY_FINISHED_ANIMATION) ||
			(e.sender == EventSenders::SENDER_ENTITY_STATE
			&&
			e.type == EventTypes::EVENT_PHYSICS
			&&
			e.code == EventCodes::COLLISION_WITH_ENEMY))
		{
			this->setActive(false);
			this->setReset(false);
		}
	}
	bool getActive()const {
		return isActive;
	}
	void setActive(bool isActive) {
		this->isActive = isActive;
	}
	bool getReset()const {
		return isReset;
	}
	void setReset(bool isReset) {
		this->isReset = isReset;
	}
};

typedef std::shared_ptr< AppleEntity> EntityApplePtr;
EntityApplePtr createApple(std::string const& rootAnimationsFolder, cv::Rect);
void CollisionApple(std::vector<EntityPtr>* _pool, EntityApplePtr apple, EntityPoolPtr instance);
