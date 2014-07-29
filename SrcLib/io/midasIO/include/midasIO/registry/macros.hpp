/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __MIDASIO_REGISTRY_MACROS_HPP__
#define __MIDASIO_REGISTRY_MACROS_HPP__

#include <boost/preprocessor/cat.hpp>

#include "midasIO/registry/detail.hpp"

namespace midasIO
{

namespace registry
{

#define MidasIOConfigurationRegisterMacro( classname, objectKey )                                  \
    static ::midasIO::IConfiguration::Registrar< classname >                  \
    BOOST_PP_CAT(s__factory__record__object__, __LINE__) ( objectKey ) ;

#define responseHandlerRegisterMacro( classname, objectKey )                                  \
    static ::midasIO::IResponseHandler::Registrar< classname >                  \
    BOOST_PP_CAT(s__factory__record__object__, __LINE__) ( objectKey ) ;


} // namespace registry

} // namespace midasIO

#endif /*__MIDASIO_REGISTRY_MACROS_HPP__*/

