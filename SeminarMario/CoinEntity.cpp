#include "CoinEntity.h"
using namespace std;
EntityStatePtr CreateCoinEntity(std::string const& animationFolder)
{
	AnimationPtr animation(new Animation(animationFolder));

	IGraphicsComponentPtr graphicsPtr(
		new SingleAnimationGraphics(animation, true));

	IPhysicsComponentPtr PhysicsPtr(new FixedWidgetPhysics());

	return make_shared<EntityState>(graphicsPtr, PhysicsPtr);
}

EntityPtr createCoin(std::string const& rootAnimationsFolder)
{
	auto stone = CreateCoinEntity(rootAnimationsFolder);
	EntityPtr StoneEntity(new Entity(stone));
	return StoneEntity;
}
