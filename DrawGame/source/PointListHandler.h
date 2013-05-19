#pragma once

#include "PointList.h"

#include <XnCppWrapper.h>
#include <XnList.h>
#include <XnHash.h>
#include <cassert>

//map<XnUserID, queue<XnPoint3D>>
class PointListHandler
{
	public:
		XN_DECLARE_DEFAULT_HASH(XnUserID, PointList*, XnUserTrailHash);
		
	private:
		XnUserTrailHash hash;
		unsigned short maxPointListLength;

	public:
		class ConstIterator
		{
			private:
				XnUserTrailHash::ConstIterator iterator;
			public:
				ConstIterator(XnUserTrailHash::ConstIterator base);

				ConstIterator& operator++();
				XnBool operator==(const ConstIterator& other) const;
				XnBool operator!=(const ConstIterator& other) const;

				XnUserID GetKey() const;
				const PointList& GetPointList() const;
		};

	public:
		PointListHandler(unsigned short maxLength);
		~PointListHandler();

		PointList& Add(XnUserID id);
		PointList* Find(XnUserID id) const;
		void Remove(XnUserID id);

		ConstIterator begin() const;
		const ConstIterator	end() const;
};
