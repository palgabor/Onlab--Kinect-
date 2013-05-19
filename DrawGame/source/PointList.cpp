#include "PointList.h"

PointList::ConstIterator::ConstIterator(XnList::ConstIterator base):iterator(base)
{
	
}

PointList::ConstIterator& PointList::ConstIterator::operator++()
{
	++iterator;
	return *this;
}

XnBool PointList::ConstIterator::operator==(const PointList::ConstIterator& other) const
{
	return iterator == other.iterator;
}

XnBool PointList::ConstIterator::operator!=(const PointList::ConstIterator& other) const
{
	return iterator != other.iterator;
}

const XnPoint3D& PointList::ConstIterator::operator*() const
{
	return *static_cast<XnPoint3D*>(*iterator);
}



PointList::PointList(unsigned short maxLength):lengthOfList(0),maxLengthOfList(maxLength)
{
	assert(maxLength > 0);
}

PointList::~PointList()
{
	// Pop all points from the list
	XnPoint3D* point;
	while((point = Pop()))
	{
		delete point;
	}
}

PointList::ConstIterator PointList::begin() const
{
	return ConstIterator(list.begin());
}
		
const PointList::ConstIterator PointList::end() const
{
	return ConstIterator(list.end());
}

XnPoint3D* PointList::Pop()
{
	void* point = NULL;

	if(list.IsEmpty())
	{
		return NULL;
	}

	if(list.Remove(list.rbegin(), point) != XN_STATUS_OK)
	{
		printf("Unable to remove point from points list.");
		exit(1);
	}
	
	lengthOfList--;
	
	assert(point);
	assert(lengthOfList);
	assert(lengthOfList < maxLengthOfList);
	
	return static_cast<XnPoint3D*>(point);
}


void PointList::Push(const XnPoint3D& position)
{
	XnPoint3D* point;

	if(lengthOfList == maxLengthOfList)
	{
		//Point list is full, delete the oldest element.
		point = Pop();
		assert(point);
	}
	else
	{
		//Point list is not full.
		assert(lengthOfList < maxLengthOfList);
		point = new XnPoint3D;
	}

	*point = position;
	if(list.AddFirst(point) != XN_STATUS_OK)
	{
		printf("Unable to add point to the list");
		exit(1);
	}
	
	lastPoint = position;
	isHandClose[lengthOfList] = position.Z < HAND_CLOSE;
	lengthOfList++;
}

const XnPoint3D& PointList::GetLastPoint() const
{
	return lastPoint;
}

const bool* PointList::GetHandCloseVector() const
{
	return isHandClose;
}
