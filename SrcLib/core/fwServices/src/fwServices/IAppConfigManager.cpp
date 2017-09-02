/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwServices/IAppConfigManager.hpp"

#include "fwServices/AppConfigManager.hpp"

namespace fwServices
{

//------------------------------------------------------------------------------

IAppConfigManager::sptr IAppConfigManager::New()
{
    return ::fwServices::AppConfigManager::New();
}

//------------------------------------------------------------------------------

} // namespace fwServices

