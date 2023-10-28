#pragma once
#include "Entities.h"
class HeroEntity1:Entity
{
	public:
		HeroEntity1();
		EntityPtr createHero(std::string const& rootAnimationsFolder, cv::Rect);
		void Collision(std::vector<EntityPtr> _pool, EntityPtr hero);
		virtual void onNotify(Event const& e) override;
};

