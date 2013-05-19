#pragma once  

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

		ConstIterator begin() const;
		const ConstIterator	end() const;
};
