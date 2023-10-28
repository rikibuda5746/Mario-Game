#pragma once
#include "Entities.h"
#include "Config.h"
using namespace std;

	 EntityStatePtr CreateLivesEntity(std::string const& animationFolder);
	 EntityPtr createLive(std::string const& rootAnimationsFolder);

	 class LivesEntity : public Entity
	 {
	 public:
		 LivesEntity(EntityStatePtr state) :Entity(state) {}
		 virtual void onNotify(Event const& e) override
		 {

			 //collision
			 if (e.sender == EventSenders::SENDER_ENTITY_STATE
				 &&
				 e.type == EventTypes::EVENT_PHYSICS
				 &&
				 e.code == EventCodes::COLLISION_WITH_ENEMY)
			 {

				 if (_state->getGraphics()->update())
				 {
					 _state->Notify(Event{ EventSenders::SENDER_ENTITY_STATE, EventTypes::EVENT_GRAPHICS, EventCodes::GAME_OVER});
				 }
			 }
			 //add life
			 if (e.sender == EventSenders::SENDER_ENTITY_STATE
				 &&
				 e.type == EventTypes::EVENT_PHYSICS
				 &&
				 e.code == EventCodes::ADD_LIFE)
			 {
				 _state->getGraphics()->reset(4);
			 }
		 }
	 };