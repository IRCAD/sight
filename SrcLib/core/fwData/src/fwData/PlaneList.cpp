/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>
#include <fwTools/ClassRegistrar.hpp>

#include <fwTools/Factory.hpp>

#include "fwData/PlaneList.hpp"

REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object, ::fwData::PlaneList, ::fwData::PlaneList );

namespace fwData
{

//------------------------------------------------------------------------------

PlaneList::PlaneList ()
{
	SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

PlaneList::~PlaneList ()
{
	SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

PlaneList::sptr PlaneList::clone() const
{
	SLM_TRACE_FUNC();

	PlaneList::NewSptr pNewPlaneList;

	// Copy encoding
	for ( 	PlaneList::PlaneListContainer::const_iterator iter = this->m_vPlanes.begin();
			iter != this->m_vPlanes.end();
			++iter )
	{
		pNewPlaneList->m_vPlanes.push_back( (*iter)->clone() );
	}

	return pNewPlaneList;
}

//------------------------------------------------------------------------------

PlaneList & PlaneList::operator=( const PlaneList & _planetList )
{
	// Copy encoding
	m_vPlanes = _planetList.m_vPlanes;

	return *this;
}

//------------------------------------------------------------------------------

void PlaneList::deleteDuplicatedPlan(void)
{
	::fwData::PlaneList::PlaneListContainer::iterator iter = this->m_vPlanes.begin();
	::fwData::PlaneList::PlaneListContainer::iterator iter2 ;
	if(this->m_vPlanes.size() > 1)
	{
		for ( iter = this->m_vPlanes.begin(); iter != this->m_vPlanes.end(); ++iter )
		{
			PlaneList::PlaneListContainer::iterator iter2 =iter+1;
			while(iter2 != this->m_vPlanes.end())
			{
				if(*(iter->get()) == *(iter2->get()))
					this->m_vPlanes.erase(iter2);
				else
					++iter2;
			}
		}
	}
}

} // namespace fwData


