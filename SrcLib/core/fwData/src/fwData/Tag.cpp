/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>
#include "fwData/registry/macros.hpp"

#include <fwData/Factory.hpp>

#include "fwData/Tag.hpp"

REGISTER_DATA( ::fwData::Tag );
namespace fwData
{
//------------------------------------------------------------------------------

Tag::Tag () :
m_size(0.5)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

Tag::~Tag ()
{
    SLM_TRACE_FUNC();
    if(m_pointList)
    {
        m_pointList->getRefPoints().clear();
    }
}

//------------------------------------------------------------------------------

} // namespace fwData


