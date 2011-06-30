/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwMemory/IDumpPolicy.hpp"

namespace fwMemory
{

IDumpPolicy::IDumpPolicy()
{
}

IDumpPolicy::~IDumpPolicy()
{
}

void IDumpPolicy::configure( ::boost::shared_ptr< ::fwRuntime::ConfigurationElement >  cfgElt)
{
    // subclass have
}

} // namespace fwMemory
