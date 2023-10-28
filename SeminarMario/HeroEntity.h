#pragma once

#include "Entities.h"
	EntityPtr createHero(std::string const& rootAnimationsFolder, cv::Rect,EntityPtr lives, EntityPtr coin, EntityPtr scores);
	void Collision(std::vector<EntityPtr> _pool, EntityPtr hero);