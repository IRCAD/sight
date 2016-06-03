/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwServices/IAppConfigManager.hpp"
#include "fwServices/AppConfigManager.hpp"
#include "fwServices/AppConfigManager2.hpp"

namespace fwServices
{
int IAppConfigManager::s_VERSION = 0;

//------------------------------------------------------------------------------

IAppConfigManager::sptr IAppConfigManager::New()
{
    SLM_ASSERT("Defaut implementation hasn't been chosen yet.", s_VERSION == 1 || s_VERSION == 2);
    if(s_VERSION == 1)
    {
        return ::fwServices::AppConfigManager::New();
    }
    else
    {
        return ::fwServices::AppConfigManager2::New();
    }
}

//------------------------------------------------------------------------------

} // namespace fwServices

