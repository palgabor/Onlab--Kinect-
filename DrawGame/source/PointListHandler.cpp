#include "PointListHandler.h"

PointListHandler::ConstIterator::ConstIterator(PointListHandler::XnUserTrailHash::ConstIterator base):iterator(base)
{
	
}

PointListHandler::ConstIterator& PointListHandler::ConstIterator::operator++()
{
	++iterator;
	return *this;
}

XnBool PointListHandler::ConstIterator::operator==(const PointListHandler::ConstIterator& other) const
{
	return iterator == other.iterator;
}

XnBool PointListHandler::ConstIterator::operator!=(const PointListHandler::ConstIterator& other) const
{
	return iterator != other.iterator;
}

XnUserID PointListHandler::ConstIterator::GetKey() const
{
	return iterator.Key();
}

const PointList& PointListHandler::ConstIterator::GetPointList() const
{
	return *iterator.Value();
}


PointListHandler::PointListHandler(unsigned short maxLength):maxPointListLength(maxLength)
{
	assert(maxPointListLength > 0);
}

PointListHandler::~PointListHandler()
{
	// Delete all points
	while(hash.IsEmpty())
	{
		PointList* pointList;

		if(hash.Remove(hash.begin().Key(), pointList) != XN_STATUS_OK)
		{
			printf("Unable to remove from hash");
			exit(1);
		}
		assert(pointList);

		delete static_cast<PointList*>(pointList);
	}
}

PointListHandler::ConstIterator PointListHandler::begin() const
{
	return ConstIterator(hash.begin());
}

const PointListHandler::ConstIterator PointListHandler::end() const
{
	return ConstIterator(hash.end());
}

PointList& PointListHandler::Add(XnUserID id)
{
	PointList* temp = NULL;

	// An existing user should not be added again
	assert(hash.Get(id, temp) == XN_STATUS_NO_MATCH);

	PointList* pointList = new PointList(maxPointListLength);

	if(hash.Set(id, pointList) != XN_STATUS_OK)
	{
		printf("Unable to add to hash");
		exit(1);
	}

	return *pointList;
}

PointList* PointListHandler::Find(XnUserID id) const
{
	PointList* pointList;

	switch(hash.Get(id, pointList))
	{
		case XN_STATUS_OK:
			return pointList;

		case XN_STATUS_NO_MATCH:
			return NULL;

		default:
			printf("Unable to search hash");
			exit(1);
	}
}

void PointListHandler::Remove(XnUserID id)
{
	PointList* pointList = NULL;

	if(hash.Remove(id, pointList) != XN_STATUS_OK)
	{
		printf("Unable to remove from hash");
		exit(1);
	}
	assert(pointList);

	delete pointList;
}
