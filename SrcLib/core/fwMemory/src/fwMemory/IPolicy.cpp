/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include "fwMemory/IPolicy.hpp"

namespace fwMemory
{


//------------------------------------------------------------------------------

IPolicy::FactoryMap &IPolicy::getDefaultPolicyFactories()
{
    static IPolicy::FactoryMap factories;
    return factories;
}

//------------------------------------------------------------------------------

IPolicy::sptr IPolicy::createPolicy(std::string name)
{
    IPolicy::sptr policy;
    IPolicy::FactoryMap &factories = IPolicy::getDefaultPolicyFactories();
    FactoryMap::iterator iter = factories.find(name);
    if ( iter != factories.end())
    {
        PolicyFactoryType &pf = iter->second;
        policy = pf();
    }
    return policy;
}

//------------------------------------------------------------------------------

const IPolicy::FactoryMap &IPolicy::getPolicyFactories()
{
    return IPolicy::getDefaultPolicyFactories();
}


} // namespace fwMemory


