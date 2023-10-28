#include "LivesEntity.h"
#include "opencv2/opencv.hpp"
#include "Config.h"
#include <memory>
#include <filesystem>
using namespace cv;
using namespace std;



EntityStatePtr CreateLivesEntity(std::string const& animationFolder)
{
	AnimationPtr animation(new Animation(animationFolder));

	IGraphicsComponentPtr graphicsPtr(
		new LivesGraphics(animation));

	IPhysicsComponentPtr nonCollidingPhysicsPtr =
		make_shared<NonCollidingPhysicsDecorator>(
			make_shared<FixedWidgetPhysics>());

	return make_shared<EntityState>(graphicsPtr, nonCollidingPhysicsPtr);
}


EntityPtr createLive(std::string const& rootAnimationsFolder)
{
	auto lives = CreateLivesEntity(rootAnimationsFolder);
	EntityPtr livesEntity(new Entity(lives));
	return livesEntity;
}
