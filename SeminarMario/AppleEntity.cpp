#include "AppleEntity.h"
#include "opencv2/opencv.hpp"
#include <memory>
using namespace cv;
using namespace std;
EntityStatePtr CreateAppleEntity(std::string const& animationFolder, cv::Rect _bounds)
{
	AnimationPtr animation(new Animation(animationFolder));
	IGraphicsComponentPtr graphicsPtr(
		new SingleAnimationGraphics(animation,true));
	IPhysicsComponentPtr physicsPtr = nullptr;
	physicsPtr.reset(new ThrowPhysics(/*Point(-frameSize.width / 10, 0)*/30, 50, 10));
	IPhysicsComponentPtr boundsnonCollidingPhysicsPtr=
		make_shared<BoundedPhysicsDecorator>(physicsPtr, _bounds);

	return make_shared<EntityState>(graphicsPtr, boundsnonCollidingPhysicsPtr);
}


EntityApplePtr createApple(std::string const& rootAnimationsFolder, cv::Rect _bounds)
{
	auto apple = CreateAppleEntity(rootAnimationsFolder,_bounds);
	EntityApplePtr appleEntity(new AppleEntity(apple));
	return appleEntity;
}
void CollisionApple(std::vector<EntityPtr>* _pool,EntityApplePtr apple, EntityPoolPtr instance) {
	for (int i = 0; i < _pool->size(); i++)
	{
		if (apple->getState()->getPhysics()->checkCollision(( * _pool)[i]->getState()->getPhysics())) {
			apple->getState()->Notify(Event{ EventSenders::SENDER_ENTITY_STATE, EventTypes::EVENT_PHYSICS, EventCodes::COLLISION_WITH_ENEMY });
			instance->returnEntity((*_pool)[i]);
         	(*_pool).erase((*_pool).begin() + i);
		}
	}
}
