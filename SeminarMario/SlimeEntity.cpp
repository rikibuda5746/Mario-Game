#include "SlimeEntity.h"
#include "opencv2/opencv.hpp"
#include "Config.h"
#include <memory>
#include <filesystem>
using namespace cv;
using namespace std;
namespace fs = std::filesystem;
EntityStatePtr CreateSlimeEnemy(std::string const& animationFolder)
{
	AnimationPtr animation(new Animation(animationFolder));
	bool isCyclic = true;

	IGraphicsComponentPtr graphicsPtr(
		new SingleAnimationGraphics(animation, isCyclic));

	IPhysicsComponentPtr nonCollidingPhysicsPtr =
		//make_shared<NonCollidingPhysicsDecorator>(
			make_shared<FixedWidgetPhysics>();

	return make_shared<EntityState>(graphicsPtr, nonCollidingPhysicsPtr);
}
EntityPtr createSlime(std::string const& rootAnimationsFolder)
{
	auto slime = CreateSlimeEnemy(rootAnimationsFolder);
	EntityPtr slimeEntity(new Entity(slime));
	return slimeEntity;
}






