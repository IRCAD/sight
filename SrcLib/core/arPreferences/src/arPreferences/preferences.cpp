/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "arPreferences/preferences.hpp"

#include <fwData/Composite.hpp>
#include <fwData/String.hpp>

#include <fwServices/registry/ObjectService.hpp>

namespace arPreferences
{

const std::string s_VIDEO_DIR_PREF = "VIDEO_DIR_PREF";

//-----------------------------------------------------------------------------

std::string getVideoDir()
{
    std::string videoDirectory;
    auto preferencesServicesList = ::fwServices::OSR::getServices("::fwPreferences::IPreferences");
    if(!preferencesServicesList.empty())
    {
        ::fwServices::IService::sptr prefService = *preferencesServicesList.begin();
        ::fwData::Composite::sptr prefComposite  = prefService->getObject< ::fwData::Composite >();

        ::fwData::Composite::IteratorType iterVideoDir = prefComposite->find( s_VIDEO_DIR_PREF );
        const bool videoFound = (iterVideoDir  != prefComposite->end());
        if (videoFound)
        {
            ::fwData::String::sptr videoDir = ::fwData::String::dynamicCast(iterVideoDir->second);
            SLM_ERROR_IF("Wrong type of preference : '" + s_VIDEO_DIR_PREF + "' parameter must be a string",
                         !videoDir);
            videoDirectory = videoDir->value();
        }
    }
    return videoDirectory;
}

//-----------------------------------------------------------------------------

} // namespace arPreferences
