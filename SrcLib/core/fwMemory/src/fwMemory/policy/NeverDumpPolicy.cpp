/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>
#include <fwTools/ClassRegistrar.hpp>

#include "fwMemory/policy/NeverDumpPolicy.hpp"


namespace fwMemory
{
namespace policy
{

REGISTER_BINDING( ::fwMemory::IDumpPolicy, ::fwMemory::policy::NeverDumpPolicy , std::string , "NeverDumpPolicy"  );

//-----------------------------------------------------------------------------

NeverDumpPolicy::NeverDumpPolicy()
{}

//-----------------------------------------------------------------------------

NeverDumpPolicy::~NeverDumpPolicy()
{}

//-----------------------------------------------------------------------------

bool NeverDumpPolicy::apply( ::boost::uint64_t)
{
    SLM_INFO("Apply a never dump policy" ) ;
    return true ;
}

//-----------------------------------------------------------------------------

} //namespace policy
} //namespace fwMemory
