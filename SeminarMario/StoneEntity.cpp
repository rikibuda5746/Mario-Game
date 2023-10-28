#include "StoneEntity.h"
using namespace std;

EntityStatePtr CreateStoneEntity(std::string const& animationFolder)
{
	AnimationPtr animation(new Animation(animationFolder));

	IGraphicsComponentPtr graphicsPtr(
		new SingleAnimationGraphics(animation,true));

	IPhysicsComponentPtr nonCollidingPhysicsPtr =
		//make_shared<NonCollidingPhysicsDecorator>(
			make_shared<FixedWidgetPhysics>();

	return make_shared<EntityState>(graphicsPtr, nonCollidingPhysicsPtr);
}
EntityPtr createStone(std::string const& rootAnimationsFolder)
{
	auto stone = CreateStoneEntity(rootAnimationsFolder);
	EntityPtr StoneEntity(new Entity(stone));
	return StoneEntity;
}
