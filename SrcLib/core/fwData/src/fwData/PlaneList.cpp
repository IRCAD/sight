/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>

#include "fwData/registry/macros.hpp"
#include "fwData/Exception.hpp"
#include "fwData/PlaneList.hpp"

fwDataRegisterMacro( ::fwData::PlaneList );

namespace fwData
{

//------------------------------------------------------------------------------

PlaneList::PlaneList(::fwData::Object::Key key)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

PlaneList::~PlaneList()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void PlaneList::shallowCopy(const Object::csptr &_source )
{
    PlaneList::csptr other = PlaneList::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );
    this->fieldShallowCopy( _source );

    this->m_vPlanes = other->m_vPlanes;
}

//------------------------------------------------------------------------------

void PlaneList::cachedDeepCopy(const Object::csptr &_source, DeepCopyCacheType &cache)
{
    PlaneList::csptr other = PlaneList::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );
    this->fieldDeepCopy( _source, cache );

    this->m_vPlanes.clear();
    for (   PlaneList::PlaneListContainer::const_iterator iter = other->m_vPlanes.begin();
            iter != other->m_vPlanes.end();
            ++iter )
    {
        Plane::sptr newPlane;
        newPlane = ::fwData::Object::copy(*iter, cache);
        this->m_vPlanes.push_back( newPlane );
    }
}

//------------------------------------------------------------------------------

void PlaneList::deleteDuplicatedPlan(void)
{
    ::fwData::PlaneList::PlaneListContainer::iterator iter = this->m_vPlanes.begin();
    ::fwData::PlaneList::PlaneListContainer::iterator iter2;
    if(this->m_vPlanes.size() > 1)
    {
        for ( iter = this->m_vPlanes.begin(); iter != this->m_vPlanes.end(); ++iter )
        {
            PlaneList::PlaneListContainer::iterator iter2 = iter+1;
            while(iter2 != this->m_vPlanes.end())
            {
                if(*(iter->get()) == *(iter2->get()))
                {
                    this->m_vPlanes.erase(iter2);
                }
                else
                {
                    ++iter2;
                }
            }
        }
    }
}

} // namespace fwData


