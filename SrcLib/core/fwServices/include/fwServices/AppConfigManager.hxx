/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWSERVICES_APPCONFIGMANAGER_HXX__
#define __FWSERVICES_APPCONFIGMANAGER_HXX__

#include <fwCore/base.hpp>

namespace fwData
{
class Object;
}

namespace fwServices
{

template<class CLASSNAME>
SPTR(CLASSNAME) AppConfigManager::getConfigRoot() const
{
    SPTR(::fwData::Object) obj = this->getConfigRoot();
    SPTR(CLASSNAME) rootObject = CLASSNAME::dynamicCast( obj );
    SLM_ASSERT("Unable to cast root object to " << CLASSNAME::classname(), rootObject);
    return rootObject;
}


}

#endif // __FWSERVICES_APPCONFIGMANAGER_HXX__
