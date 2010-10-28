/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwXML/policy/NeverSplitPolicy.hpp"

namespace fwXML
{

bool NeverSplitPolicy::split(::boost::shared_ptr< ::fwTools::Object > obj)
{
    return false;
}

//-----------------------------------------------------------------------------

NeverSplitPolicy::~NeverSplitPolicy() {}

}// namespace fwXML
