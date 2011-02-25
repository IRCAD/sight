/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWRUNTIME_UTILS_GENERICEXECUTABLEFACTORYREGISTRAR_HPP_
#define _FWRUNTIME_UTILS_GENERICEXECUTABLEFACTORYREGISTRAR_HPP_

#include <string>

#include "fwRuntime/ExecutableFactoryRegistrar.hpp"
#include "fwRuntime/utils/GenericExecutableFactory.hpp"


namespace fwRuntime
{

namespace utils
{

/**
 * @brief   Defines a generic template executable factory registrar class.
 * @struct  GenericExecutableFactoryRegistrar
 * @date    2004-2009
 * @author  IRCAD (Research and Development Team).
 */
template< typename E >
struct GenericExecutableFactoryRegistrar : public ExecutableFactoryRegistrar
{

    typedef GenericExecutableFactory< E >   FactoryType;

    /**
     * @brief   Constructor
     */
    GenericExecutableFactoryRegistrar(const std::string& type)
    : ExecutableFactoryRegistrar( ::boost::shared_ptr< FactoryType >( new FactoryType(type) ) )
    {}

};


} // namespace utils

} // namespace fwRuntime


#endif //_FWRUNTIME_UTILS_GENERICEXECUTABLEFACTORYREGISTRAR_HPP_
