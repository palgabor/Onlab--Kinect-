#pragma once  

#include "Constants.h"

#include <XnCppWrapper.h>
#include <XnList.h>
#include <XnHash.h>
#include <cassert>

class PointList
{
	private:
		XnList list;
		unsigned short lengthOfList, maxLengthOfList;
		XnPoint3D lastPoint;
		bool isHandClose[SIZE_OF_POINT_LIST];
	
		XnPoint3D* Pop();
	
	public:
		class ConstIterator
		{
			private:
				XnList::ConstIterator iterator;
			public:
				ConstIterator(XnList::ConstIterator	base);

				ConstIterator&	operator++();
				XnBool operator==(const ConstIterator& other) const;
				XnBool operator!=(const ConstIterator& other) const;

				const XnPoint3D& operator*() const;
		};
	
	
		PointList(unsigned short maxLength);
		~PointList();

		
		void Push(const XnPoint3D&	position);
		const XnPoint3D& GetLastPoint() const;
		const bool* GetHandCloseVector() const;

		ConstIterator begin() const;
		const ConstIterator	end() const;
};
