/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWCAMP_FACTORY_HXX_
#define __FWCAMP_FACTORY_HXX_

namespace fwCamp {


template< typename BaseType, typename ArgType >
BaseType Factory< BaseType (*)(ArgType) >::create(const std::string &name, ArgType arg)
{

    typename FactoryMapType::iterator it = getMap().find(name);
    if(it == getMap().end())
    {
        return 0;
    }
    return it->second(arg);
}

template< typename BaseType, typename ArgType >
void Factory< BaseType (*)(ArgType) >::add(const std::string &name, FactoryType f)
{
    // XXX TODO : maybe check already registered name ?
    getMap().insert(typename FactoryMapType::value_type(name, f));
}

template< typename BaseType, typename ArgType >
typename Factory< BaseType (*)(ArgType) >::FactoryMapType&
Factory< BaseType (*)(ArgType) >::getMap()
{
    static FactoryMapType map;
    return map;
}

} // fwCamp 


#endif //__FWCAMP_FACTORY_HXX_

