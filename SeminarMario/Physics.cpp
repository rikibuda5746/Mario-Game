#include "Physics.h"
#include <algorithm>

using namespace cv;
using namespace std;

bool checkPixelLevelCollision(IPhysicsComponent const* first, IPhysicsComponentPtr const& other)
{
	cv::Mat const& mask_First = first->getCollisionMask();
	cv::Mat const& mask_Other = other->getCollisionMask();
	if (mask_Other.empty() || mask_First.empty())
		return false;

	Point TL_first = first->getTL();
	Point TL_other = other->getTL();
	
	// ROI = Reagion of Interest
	Rect firstROI(TL_first, TL_first + Point(mask_First.size()));
	Rect othersROI(TL_other, TL_other + Point(mask_Other.size()));
	
	Rect intersection = firstROI & othersROI;
	if (intersection.empty())
		return false;

	Rect intersection_RelativeToMe(
		intersection.tl() - TL_first, intersection.br() - TL_first);

	Rect intersection_RelativeToOther(
		intersection.tl() - TL_other, intersection.br() - TL_other);

	Mat myMaskROI = mask_First(intersection_RelativeToMe);
	Mat othersMaskROI = mask_Other(intersection_RelativeToOther);
	Mat whereMeAndOtherIntersect = myMaskROI.mul(othersMaskROI);

	return countNonZero(whereMeAndOtherIntersect);
}

/// //////////////////////////////////////////

FixedWidgetPhysics::FixedWidgetPhysics()
	:_topLeft(0,0), _mask()
{
}

void FixedWidgetPhysics::reset(cv::Point const& tl)
{
	_topLeft = tl;
}

bool FixedWidgetPhysics::update(cv::Mat const& collisionMask)
{
	_mask = collisionMask;
	return false;
}

bool FixedWidgetPhysics::checkCollision(IPhysicsComponentPtr const& other) const
{
	return checkPixelLevelCollision(this, other);
}

cv::Point const& FixedWidgetPhysics::getTL() const
{
	return _topLeft;
}

cv::Mat const& FixedWidgetPhysics::getCollisionMask() const
{
	return _mask;
}


ConstVelocityPhysics::ConstVelocityPhysics(cv::Point const& velocity):velocity(velocity),_topLeft(0, 0), _mask()
{
}

void ConstVelocityPhysics::reset(cv::Point const& tl)
{
	_topLeft = tl;
}

bool ConstVelocityPhysics::update(cv::Mat const& collisionMask)
{
	_mask = collisionMask;
	_topLeft.x+= velocity.x;
	_topLeft.y+= velocity.y;
	return false;
}


cv::Mat const& ConstVelocityPhysics::getCollisionMask() const
{
	return _mask;
}

bool ConstVelocityPhysics::checkCollision(IPhysicsComponentPtr const& other) const
{
	return checkPixelLevelCollision(this, other);
}

cv::Point const& ConstVelocityPhysics::getTL() const
{
	return _topLeft;
}

JumpPhysics::JumpPhysics(int horizontalVelocity, int initialVerticalVelocity, int gravity, int finalPointY)
	:_initialJumpVelocity(horizontalVelocity,-(initialVerticalVelocity)),_gravity(gravity), finalPointY(finalPointY)
{
}

void JumpPhysics::reset(cv::Point const& tl)
{
	if (finalPointY==-1)
	{
		_jumpStartTL = tl;
	}
	else {
		_jumpStartTL=Point(tl.x,finalPointY);
	}
	_currTL = tl;
	_currVelocity = _initialJumpVelocity;
}

bool JumpPhysics::update(cv::Mat const& collisionMask)
{
		_mask = collisionMask;
		_currTL +=_currVelocity;
		_currVelocity.y+= _gravity;
		if (_currTL.y >= _jumpStartTL.y)
		{
			return _currTL.y = _jumpStartTL.y;
			return true;
		}
		return false;
}

cv::Mat const& JumpPhysics::getCollisionMask() const
{
	return _mask;
}
bool JumpPhysics::checkCollision(IPhysicsComponentPtr const& other) const
{
	return checkPixelLevelCollision(this, other);
}

cv::Point const& JumpPhysics::getTL() const
{
	return _currTL;
}

NonCollidingPhysicsDecorator::NonCollidingPhysicsDecorator(IPhysicsComponentPtr base)
	:_base(base)
{
}

void NonCollidingPhysicsDecorator::reset(cv::Point const& tl)
{
	_base->reset(tl);
}

bool NonCollidingPhysicsDecorator::update(cv::Mat const& collisionMask)
{
	return _base->update(collisionMask);
}

cv::Mat const& NonCollidingPhysicsDecorator::getCollisionMask() const
{
	return cv::Mat();
}

bool NonCollidingPhysicsDecorator::checkCollision(std::shared_ptr<IPhysicsComponent> const& other) const
{
	return false;
}

cv::Point const& NonCollidingPhysicsDecorator::getTL() const
{
	return _base->getTL();
}

BoundedPhysicsDecorator::BoundedPhysicsDecorator(IPhysicsComponentPtr base, cv::Rect bounds):_base(base),_bounds(bounds),currTl(_base->getTL())
{
}

void BoundedPhysicsDecorator::reset(cv::Point const& tl)
{
	_base->reset(tl);
	currTl = _base->getTL();
}

bool BoundedPhysicsDecorator::update(cv::Mat const& collisionMask)
{
	bool flag=_base->update(collisionMask);
	currTl = _base->getTL();
	this->test();
	return flag;
}

cv::Mat const& BoundedPhysicsDecorator::getCollisionMask() const
{
	return _base->getCollisionMask();
}

bool BoundedPhysicsDecorator::checkCollision(std::shared_ptr<IPhysicsComponent> const& other) const
{
	return checkPixelLevelCollision(this,other);
}

cv::Point const& BoundedPhysicsDecorator::getTL() const
{
	return currTl;
}

void BoundedPhysicsDecorator::test()
{

	if (_base->getTL().x > _bounds.x + _bounds.width - _base->getCollisionMask().size().width)
	{
		currTl.x = _base->getTL().x % _bounds.width;

	}
	if (_base->getTL().x <_bounds.x)
	{
		currTl.x = _bounds.width - (_bounds.x - _base->getTL().x) % _bounds.width;
	}
}

//apple
ThrowPhysics::ThrowPhysics(int horizontalVelocity, int initialVerticalVelocity, int gravity)
	:_gravity(gravity), _initialThrowVelocity(horizontalVelocity, -initialVerticalVelocity)
	, _currVelocity(_initialThrowVelocity)
{
	if (gravity < 0 || initialVerticalVelocity < 0)
		throw exception("A positive number was expected");
}
void ThrowPhysics::reset(cv::Point const& tl)
{
	_currTL = tl;
	_currVelocity = _initialThrowVelocity;
}
bool ThrowPhysics::update(cv::Mat const& collisionMask)
{
	_mask = collisionMask;
	_currTL += _currVelocity;
	_currVelocity.y += _gravity;

	return _currTL.y >=0;//return true if the throw is over and false otherwise
}
cv::Mat const& ThrowPhysics::getCollisionMask() const
{
	return _mask;
}
bool ThrowPhysics::checkCollision(IPhysicsComponentPtr const& other) const
{
	return checkPixelLevelCollision(this, other);
}
cv::Point const& ThrowPhysics::getTL() const
{
	return _currTL;
}
cv::Point const& ThrowPhysics::getVelocity() const
{
	return _currVelocity;
}
