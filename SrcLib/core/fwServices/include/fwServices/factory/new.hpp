/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWSERVICES_FACTORY_NEW_HPP__
#define __FWSERVICES_FACTORY_NEW_HPP__

#include <string>
#include <boost/make_shared.hpp>

#include "fwServices/config.hpp"

namespace fwServices
{

class IService;

namespace factory
{

template<class CLASSNAME > SPTR( CLASSNAME )  New()
{
    SPTR(CLASSNAME) srv = ::boost::make_shared< CLASSNAME >();

    return srv;
}

} // namespace factory

} // namespace fwServices

#endif /* __FWSERVICES_FACTORY_NEW_HPP__ */



