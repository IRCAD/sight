/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>
#include <fwTools/ClassRegistrar.hpp>

#include "fwData/None.hpp"

REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object, ::fwData::None, ::fwData::None );

namespace fwData
{

//------------------------------------------------------------------------------
None::None ()
{
        SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------
None::~None ()
{
        SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------
bool None::isNone(::fwData::Object::sptr object)
{
    return None::dynamicCast(object);
}

} // namespace fwData


