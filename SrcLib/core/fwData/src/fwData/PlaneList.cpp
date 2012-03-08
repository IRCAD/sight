/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>
#include "fwData/registry/macros.hpp"

#include <fwData/Factory.hpp>

#include "fwData/PlaneList.hpp"

fwDataRegisterMacro( ::fwData::PlaneList );

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

void PlaneList::shallowCopy( PlaneList::csptr _source )
{
    this->::fwData::Object::fieldShallowCopy( _source );

    this->m_vPlanes = _source->m_vPlanes;
}

//------------------------------------------------------------------------------

void PlaneList::deepCopy( PlaneList::csptr _source )
{
    this->::fwData::Object::fieldDeepCopy( _source );

    this->m_vPlanes.clear();
    for (   PlaneList::PlaneListContainer::const_iterator iter = _source->m_vPlanes.begin();
            iter != _source->m_vPlanes.end();
            ++iter )
    {
        Plane::NewSptr newPlane;
        newPlane->deepCopy( *iter );
        this->m_vPlanes.push_back( newPlane );
    }
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


