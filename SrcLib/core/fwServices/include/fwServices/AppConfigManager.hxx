/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWSERVICES_CONFIGTEMPLATEMANAGER_HXX_
#define _FWSERVICES_CONFIGTEMPLATEMANAGER_HXX_

#include <fwTools/Object.hpp>

namespace fwServices
{

template<class CLASSNAME>
SPTR(CLASSNAME) AppConfigManager::getConfigRoot() const
{
    SPTR(CLASSNAME) myObject = ::boost::dynamic_pointer_cast< CLASSNAME >( getConfigRoot() ) ;
    SLM_ASSERT("Sorry, this object cannot be autocast.", myObject);
    return myObject;
}


}

#endif // _FWSERVICES_CONFIGTEMPLATEMANAGER_HXX_
