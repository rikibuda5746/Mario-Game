#pragma once

#include <opencv2/opencv.hpp>

class IPhysicsComponent
{
public:
	// start over the physics from a given tl=topLeft.
	virtual void reset(cv::Point const & tl) = 0;
	
	// update physics to the next position, 
	// with current "collisionMask" = the "shape" of the current non-transparent-pixels.
	// it's called "collisionMask" since only if a non-transparent pixel is colliding it's considered a collision.
	// 
	// return value is true if the physics would like to be replaced 
	// (physics states that "it has fulfilled its duty and has no reason to be here anymore").
	virtual bool update(cv::Mat const& collisionMask) = 0;
	
	// return the current collisionMaks
	virtual cv::Mat const & getCollisionMask() const = 0;

	// check collision with other entitie's physics.
	virtual bool checkCollision(std::shared_ptr<IPhysicsComponent> const& other) const = 0;

	// return const ref to a current top left.
	// WARNING! you return a reference here, which is actually a POINTER!
	// therefore, you must return a reference to a member, not to a temporary variable.
	virtual cv::Point const& getTL() const = 0;
};

// using shorter, more convinient, name:
typedef std::shared_ptr<IPhysicsComponent> IPhysicsComponentPtr;

// physics of non-moving. implemented as an example.
class FixedWidgetPhysics : public IPhysicsComponent
{
private:
	cv::Point _topLeft;
	cv::Mat _mask;
public:
	FixedWidgetPhysics();
	// Inherited via IPhysicsComponent
	virtual void reset(cv::Point const& tl);
	virtual bool update(cv::Mat const& collisionMask) override;
	virtual cv::Mat const& getCollisionMask() const override;
	virtual bool checkCollision(IPhysicsComponentPtr const& other) const override;
	virtual cv::Point const& getTL() const override;
};
 
class ConstVelocityPhysics : public IPhysicsComponent
{
private:
	cv::Point _topLeft;
	cv::Mat _mask;
	cv::Point velocity;
public:

	ConstVelocityPhysics(cv::Point const & velocity);
	// Inherited via IPhysicsComponent
	virtual void reset(cv::Point const& tl);
	virtual bool update(cv::Mat const& collisionMask);
	virtual cv::Mat const& getCollisionMask() const override;
	virtual bool checkCollision(IPhysicsComponentPtr const& other) const;
	virtual cv::Point const& getTL() const;
};

// Liskov Open Closed Principle
// Decorator Design Pattern
class NonCollidingPhysicsDecorator : public IPhysicsComponent
{
private:
	IPhysicsComponentPtr _base;

public:
	NonCollidingPhysicsDecorator(IPhysicsComponentPtr base);

	// Inherited via IPhysicsComponent
	virtual void reset(cv::Point const& tl) override;
	virtual bool update(cv::Mat const& collisionMask) override;
	virtual cv::Mat const& getCollisionMask() const override;
	virtual bool checkCollision(std::shared_ptr<IPhysicsComponent> const& other) const override;
	virtual cv::Point const& getTL() const override;

};

class BoundedPhysicsDecorator : public IPhysicsComponent
{
private:
	cv::Rect _bounds;
	IPhysicsComponentPtr _base;
	cv::Point currTl;
public:
	BoundedPhysicsDecorator(IPhysicsComponentPtr base, cv::Rect _bounds);
	virtual void reset(cv::Point const& tl) override;
	virtual bool update(cv::Mat const& collisionMask) override;
	virtual cv::Mat const& getCollisionMask() const override;
	virtual bool checkCollision(std::shared_ptr<IPhysicsComponent> const& other) const override;
	virtual cv::Point const& getTL() const override;
	void test();
};

// physics of moving with const velocity in X direction, while moving "up and down" on the y axis.
class JumpPhysics : public IPhysicsComponent
{
private:
	cv::Point _initialJumpVelocity; // the velocity (in X and Y axes) at the start of the jump
	int _gravity; // by how much the Y axis velocity recudes at each frame ? (speed in Y reduces because of gravity)
	cv::Point _jumpStartTL; // the point where jump started. the TL with which reset(...) was called
	cv::Point _currTL; // current top left
	cv::Mat _mask; // current mask. the one with which getCollisionMask(...) was called
	// current velocity = how to move _currTL . next _currTL = _currTL + _currVelocity.
	// note! at each update, _currVelocity.y = _currVelocity.y - gravity.
	cv::Point _currVelocity;
	int finalPointY;
public:
	/// <param name="horizontalVelocity">positive or negative - x axis velocity</param>
	/// <param name="initialq1	`assssssssssssssedfgmkVelocity">positive number in pixels</param>
	/// <param name="gravity">positive number in pixels</param>
	JumpPhysics(int horizontalVelocity, int initialVerticalVelocity, int gravity, int finalPointY = -1);
	virtual void reset(cv::Point const& tl);
	virtual bool update(cv::Mat const& collisionMask);
	virtual cv::Mat const& getCollisionMask() const override;
	virtual bool checkCollision(IPhysicsComponentPtr const& other) const;
	virtual cv::Point const& getTL() const;
};


class  ThrowPhysics : public IPhysicsComponent
{
private:
	cv::Point _initialThrowVelocity; // the velocity (in X and Y axes) at the start of the jump
	int _gravity; // by how much the Y axis velocity recudes at each frame ? (speed in Y reduces because of gravity)
	cv::Point _currTL; // current top left
	cv::Mat _mask; // current mask. the one with which getCollisionMask(...) was called
	// current velocity = how to move _currTL . next _currTL = _currTL + _currVelocity.
	// note! at each update, _currVelocity.y = _currVelocity.y - gravity.
	cv::Point _currVelocity;
	int _bottom = 30;
public:
	/// <param name="horizontalVelocity">positive or negative - x axis velocity</param>
	/// <param name="initialq1	`assssssssssssssedfgmkVelocity">positive number in pixels</param>
	/// <param name="gravity">positive number in pixels</param>
	ThrowPhysics(int horizontalVelocity, int initialVerticalVelocity, int gravity);
	virtual void reset(cv::Point const& tl);
	virtual bool update(cv::Mat const& collisionMask);
	virtual cv::Mat const& getCollisionMask() const override;
	virtual bool checkCollision(IPhysicsComponentPtr const& other) const;
	virtual cv::Point const& getTL() const;
	cv::Point const& getVelocity() const;
};
