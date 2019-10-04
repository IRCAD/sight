/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#pragma once

#include "fwGui/config.hpp"
#include "fwGui/GuiBaseObject.hpp"

#include <fwData/Composite.hpp>
#include <fwData/location/ILocation.hpp>

#include <filesystem>

#include <string>

namespace fwGui
{
namespace dialog
{
/**
 * @brief   Defines the generic file/folder selector dialog for IHM.
 */
class FWGUI_CLASS_API ILocationDialog : public ::fwGui::GuiBaseObject
{

public:

    fwCoreClassMacro(ILocationDialog, ::fwGui::GuiBaseObject);

    typedef enum
    {
        NONE            = 0,
        READ            = 1 << 1,
        WRITE           = 1 << 2,
        FILE_MUST_EXIST = 1 << 3,
    } Options;

    typedef enum
    {
        SINGLE_FILE,
        FOLDER,
        MULTI_FILES
    } Types;

    typedef std::string FactoryRegistryKeyType;

    /// this *unique* key should  be used *for all* factory for specific LocationDialog(qt,wx,...)
    FWGUI_API static const FactoryRegistryKeyType REGISTRY_KEY;

    FWGUI_API static const std::string SOFTWARE_UI;
    FWGUI_API static const std::string DLG_DEFAULT_LOCATION;

    FWGUI_API virtual ~ILocationDialog();

    FWGUI_API ILocationDialog();

    /// set the title for the dialog
    FWGUI_API virtual void setTitle(const std::string& title);

    /// returns the title for the dialog
    FWGUI_API virtual const std::string& getTitle();

    /// set the initial location for the dialog
    FWGUI_API virtual void setDefaultLocation( ::fwData::location::ILocation::sptr loc);

    /// get the default location for the dialog (from preferences or specified by user)
    FWGUI_API virtual const std::filesystem::path getDefaultLocation();

    /// save the specified default location for the dialog in preferences (if available)
    FWGUI_API virtual void saveDefaultLocation(::fwData::location::ILocation::sptr loc);

    /// set the type of location for the dialog (SINGLE_FILE, FORLDER, MULTI_FILES)
    FWGUI_API virtual void setType( Types type ) = 0;

    /// allow to set option to the file dialog mode=READ/WRITE , check=FILE_MUST_EXIST
    FWGUI_API virtual ILocationDialog& setOption( Options option) = 0;

    /**
     * @brief specify some filtering when browsing files:
     * @param[in] filterName a string that will be displayed as filter name
     * @param[in] wildcardList a string of extension (glob syntax) separated by spaces
     * example : addFilter("images","*.png *.jpg")
     */
    FWGUI_API virtual void addFilter(const std::string& filterName, const std::string& wildcardList ) = 0;

    /**
     * Display the dialog
     * @return the ILocation selected or null sptr if user cancel the operation
     */
    FWGUI_API virtual ::fwData::location::ILocation::sptr show() = 0;

    /// Gets the current extension file selection
    FWGUI_API virtual std::string getCurrentSelection() const = 0;

protected:

    FWGUI_API void readConfig();

    FWGUI_API void writeConfig();

    FWGUI_API ::fwData::Composite::sptr getPreferenceUI();

private:

    std::string m_title;
    ::fwData::location::ILocation::sptr m_defaultLocaction;
};

} //namespace dialog
} // namespace fwGui
