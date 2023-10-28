#include "EntitiesPool.h"

EntitiesPool::EntitiesPool()
{
}

void EntitiesPool::insert(EntityPtr whom)
{
	_pool.push_back(whom);
	_isInUse.push_back(false);
}

EntityPtr EntitiesPool::getNext()
{
	for (int i = 0; i < _pool.size(); i++)
	{
		if (!_isInUse[i])
		{
			_isInUse[i] = true;
			return _pool[i];
		}
	}
	throw std::exception("all entities are in use. ");
}

void EntitiesPool::returnEntity(EntityPtr toFree)
{
	int i;
	for (i = 0; i < _pool.size() && _pool[i].get() != toFree.get(); i++);
		_isInUse[i] = false;
}
