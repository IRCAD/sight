/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "gui/config.hpp"
#include "gui/GuiBaseObject.hpp"

#include <data/Composite.hpp>
#include <data/location/ILocation.hpp>

#include <filesystem>
#include <string>

namespace sight::gui
{
namespace dialog
{
/**
 * @brief   Defines the generic file/folder selector dialog for IHM.
 */
class GUI_CLASS_API ILocationDialog : public gui::GuiBaseObject
{

public:

    fwCoreClassMacro(ILocationDialog, gui::GuiBaseObject)

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
    GUI_API static const FactoryRegistryKeyType REGISTRY_KEY;

    GUI_API static const std::string SOFTWARE_UI;
    GUI_API static const std::string DLG_DEFAULT_LOCATION;

    GUI_API virtual ~ILocationDialog();

    GUI_API ILocationDialog();

    /// set the title for the dialog
    GUI_API virtual void setTitle(const std::string& title);

    /// returns the title for the dialog
    GUI_API virtual const std::string& getTitle();

    /// set the initial location for the dialog
    GUI_API virtual void setDefaultLocation( data::location::ILocation::sptr loc);

    /// get the default location for the dialog (from preferences or specified by user)
    GUI_API virtual const std::filesystem::path getDefaultLocation();

    /// save the specified default location for the dialog in preferences (if available)
    GUI_API virtual void saveDefaultLocation(data::location::ILocation::sptr loc);

    /// set the type of location for the dialog (SINGLE_FILE, FORLDER, MULTI_FILES)
    GUI_API virtual void setType( Types type ) = 0;

    /// allow to set option to the file dialog mode=READ/WRITE , check=FILE_MUST_EXIST
    GUI_API virtual ILocationDialog& setOption( Options option) = 0;

    /**
     * @brief specify some filtering when browsing files:
     * @param[in] filterName a string that will be displayed as filter name
     * @param[in] wildcardList a string of extension (glob syntax) separated by spaces
     * example : addFilter("images","*.png *.jpg")
     */
    GUI_API virtual void addFilter(const std::string& filterName, const std::string& wildcardList ) = 0;

    /**
     * Display the dialog
     * @return the ILocation selected or null sptr if user cancel the operation
     */
    GUI_API virtual data::location::ILocation::sptr show() = 0;

    /// Gets the current extension file selection
    GUI_API virtual std::string getCurrentSelection() const = 0;

protected:

    GUI_API void readConfig();

    GUI_API void writeConfig();

    GUI_API data::Composite::sptr getPreferenceUI();

private:

    std::string m_title;
    data::location::ILocation::sptr m_defaultLocaction;
};

} //namespace dialog
} // namespace sight::gui
