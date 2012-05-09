/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include "fwMemory/IPolicy.hpp"

namespace fwMemory
{

IPolicy::FactoriesMap IPolicy::s_factories;

IPolicy::sptr IPolicy::createPolicy(std::string name)
{
    IPolicy::sptr policy;
    FactoriesMap::iterator iter = s_factories.find(name);
    if ( iter != s_factories.end())
    {
        PolicyFactoryType &pf = iter->second;
        policy = pf();
    }
    return policy;
}


} // namespace fwMemory


