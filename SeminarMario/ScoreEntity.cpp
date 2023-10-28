#include "ScoreEntity.h"
#include "opencv2/opencv.hpp"
#include "Config.h"
#include <memory>
using namespace cv;
using namespace std;
EntityStatePtr CreateScoreEntity()
{

	IGraphicsComponentPtr graphicsPtr(
		new ScoresGraphics(4,2));

	IPhysicsComponentPtr nonCollidingPhysicsPtr =
		make_shared<NonCollidingPhysicsDecorator>(
			make_shared<FixedWidgetPhysics>());

	return make_shared<EntityState>(graphicsPtr, nonCollidingPhysicsPtr);
}

EntityPtr createScore()
{
	auto score = CreateScoreEntity();
	EntityPtr scoreEntity(new Entity(score));
	return scoreEntity;
}
