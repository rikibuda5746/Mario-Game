#pragma once
#include "Entities.h"
#include "Config.h"
EntityPtr createCoin(std::string const& rootAnimationsFolder);
class CoinEntity:public Entity
{
private:
	std::vector<EntityPtr>* pool;
public:
	CoinEntity(EntityStatePtr state, std::vector<EntityPtr>* pool) :Entity(state) {
		this->pool = pool;
	}
	virtual void onNotify(Event const& e) override {
		if (e.sender == EventSenders::SENDER_TIMER
			&&
			e.type == EventTypes::EVENT_TIMER
			&&
			e.code == EventCodes::TIME_TICK)
		{
			_state->update();
		}
		if (e.sender == EventSenders::SENDER_ENTITY_STATE
			&&
			e.type == EventTypes::EVENT_PHYSICS
			&&
			e.code == EventCodes::COLLISION_WITH_FOOD_SCORE)
		{
			for (int i = 0; i < pool->size(); i++)
			{
				if ((*pool)[i].get() == this)
				{
					(*pool).erase((*pool).begin() + i);
				}
			}
		}
	}
};

