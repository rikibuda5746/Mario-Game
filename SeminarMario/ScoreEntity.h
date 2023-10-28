#pragma once
#include "Entities.h"
#include "Config.h"
using namespace std;

EntityStatePtr CreateScoreEntity();
EntityPtr createScore();

class ScoreEntity:public Entity
{
public:
	ScoreEntity(EntityStatePtr state) :Entity(state) {}
	virtual void onNotify(Event const& e) override
	{
		if ((e.sender == EventSenders::SENDER_ENTITY_STATE
			&&
			e.type == EventTypes::EVENT_PHYSICS
			&&
			e.code == EventCodes::COLLISION_WITH_ENEMY)
			||
			(e.sender == EventSenders::SENDER_ENTITY_STATE
				&&
				e.type == EventTypes::EVENT_PHYSICS
				&&
				e.code == EventCodes::COLLISION_WITH_FOOD_SCORE))
		{
			if (_state->getGraphics()->update()) {
				_state->Notify(Event{ EventSenders::SENDER_ENTITY_STATE, EventTypes::EVENT_PHYSICS, EventCodes::ADD_LIFE });
			}
		}
	}
};