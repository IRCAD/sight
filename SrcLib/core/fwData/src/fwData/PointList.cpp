/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>
#include <fwTools/ClassRegistrar.hpp>

#include <fwTools/Factory.hpp>

#include "fwData/PointList.hpp"

REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object, ::fwData::PointList, ::fwData::PointList );

namespace fwData
{

//------------------------------------------------------------------------------

PointList::PointList ()
{
        SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

PointList::~PointList ()
{
        SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

PointList::sptr PointList::clone() const
{
        SLM_TRACE_FUNC();

        PointList::NewSptr pNewPointList;

        // Copy encoding
        for (   PointList::PointListContainer::const_iterator iter = this->m_vPoints.begin();
                        iter != this->m_vPoints.end();
                        ++iter )
        {
                pNewPointList->m_vPoints.push_back( (*iter)->clone() );
        }

        return pNewPointList;
}

//------------------------------------------------------------------------------

PointList & PointList::operator=( const PointList & _pointList )
{
        // Copy encoding
        m_vPoints = _pointList.m_vPoints;

        return *this;
}

//------------------------------------------------------------------------------

} // namespace fwData


