/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWCAMP_FACTORY_HPP__
#define __FWCAMP_FACTORY_HPP__

#include <map>
#include <string>

#include "fwCamp/config.hpp"

namespace fwCamp
{


/**
 *@brief  General template class for fwCamp factory.
 */
template <typename F>
class Factory;


/**
 *@brief Specialization of Factory for function with one parameter.
 */
template< typename BaseType, typename ArgType >
class Factory< BaseType (*)(ArgType) >
{

public:
    typedef BaseType (*FactoryType)(ArgType);

    class Registrar
    {
    public:
        Registrar(const std::string &name, FactoryType f)
        {
            Factory< FactoryType >::add(name, f);
        }
    };



    /**
     * @brief Create a BaseType Object with a name and an object.
     *
     * @param name the name of current object
     * @param the first parameter of template parameter signature.
     * @return the result of the application of FactoryType(arg);
     *
     * This function search in a intern map if the name is registrar and next
     * apply the function represented by the template signature.
     */
    static BaseType create(const std::string &name, ArgType arg);

protected:

    typedef std::map<std::string, FactoryType> FactoryMapType;

    /**
     * @brief add a new entry in the internal map.
     *
     * @param name the new entry key.
     * @param f a function associated with the key.
     */
    static void add(const std::string &name, FactoryType f);


    /**
     * @brief accessor of internal static map
     * @return the internal map.
     */
    static FactoryMapType &getMap();
};

}  // namespace fwCamp

#include "fwCamp/Factory.hxx"
#endif

