#pragma once
#include "Entities.h"
#include "EntitiesPool.h"
#include "Config.h"
using namespace std;
#include <vector>
EntityStatePtr CreateSlimeEnemy(std::string const& animationFolder);

EntityPtr createSlime(std::string const& rootAnimationsFolder);

class SlimeEntity:public Entity {
private:
	std::vector<EntityPtr>*pool;
	EntityPoolPtr instance;
public:
	SlimeEntity(EntityStatePtr state, std::vector<EntityPtr> *pool, EntityPoolPtr instance) :Entity(state) {
		this->pool =pool;
		this->instance = instance;
	}
	//virtual void onNotify(Event const& e) override {
	//	if (e.sender == EventSenders::SENDER_TIMER
	//		&&
	//		e.type == EventTypes::EVENT_TIMER
	//		&&
	//		e.code == EventCodes::TIME_TICK)
	//	{
	//		_state->update();
	//	}
	//	if (e.sender == EventSenders::SENDER_ENTITY_STATE
	//		&&
	//		e.type == EventTypes::EVENT_PHYSICS
	//		&&
	//		e.code == EventCodes::COLLISION_WITH_ENEMY)
	//	{
	//		for (int i = 0; i < pool->size(); i++)
	//		{
	//			if ((*pool)[i].get()==this)
	//			{
	//				instance->returnEntity((*pool)[i]);
	//				(*pool).erase((*pool).begin() + i);
	//			}
	//		}
	//	}

	//}
};