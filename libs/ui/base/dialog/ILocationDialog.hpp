/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "ui/base/config.hpp"
#include "ui/base/GuiBaseObject.hpp"

#include <core/location/ILocation.hpp>

#include <data/Composite.hpp>

#include <filesystem>
#include <string>

namespace sight::ui::base
{

namespace dialog
{

/**
 * @brief   Defines the generic file/folder selector dialog for IHM.
 */
class UI_BASE_CLASS_API ILocationDialog : public ui::base::GuiBaseObject
{
public:

    SIGHT_DECLARE_CLASS(ILocationDialog, ui::base::GuiBaseObject);

    typedef enum
    {
        NONE            = 0,
        READ            = 1 << 1,
        WRITE           = 1 << 2,
        FILE_MUST_EXIST = 1 << 3
    } Options;

    typedef enum
    {
        SINGLE_FILE,
        FOLDER,
        MULTI_FILES
    } Types;

    typedef std::string FactoryRegistryKeyType;

    /// this *unique* key should  be used *for all* factory for specific LocationDialog(qt,wx,...)
    UI_BASE_API static const FactoryRegistryKeyType REGISTRY_KEY;

    UI_BASE_API static const std::string SOFTWARE_UI;
    UI_BASE_API static const std::string DLG_DEFAULT_FILE;
    UI_BASE_API static const std::string DLG_DEFAULT_DIRECTORY;

    UI_BASE_API virtual ~ILocationDialog();

    UI_BASE_API ILocationDialog();

    /// set the title for the dialog
    UI_BASE_API virtual void setTitle(const std::string& title);

    /// returns the title for the dialog
    UI_BASE_API virtual const std::string& getTitle();

    /// set the initial location for the dialog
    UI_BASE_API virtual void setDefaultLocation(core::location::ILocation::sptr loc);

    /// get the default location for the dialog (from preferences or specified by user)
    UI_BASE_API virtual const core::location::ILocation::sptr getDefaultLocation();

    /// save the specified default location for the dialog in preferences (if available)
    UI_BASE_API virtual void saveDefaultLocation(core::location::ILocation::sptr loc);

    /// set the type of location for the dialog (SINGLE_FILE, FOLDER, MULTI_FILES)
    UI_BASE_API virtual void setType(Types type) = 0;

    /// allow to set option to the file dialog mode=READ/WRITE , check=FILE_MUST_EXIST
    UI_BASE_API virtual ILocationDialog& setOption(Options option) = 0;

    /**
     * @brief specify some filtering when browsing files:
     * @param[in] filterName a string that will be displayed as filter name
     * @param[in] wildcardList a string of extension (glob syntax) separated by spaces
     * example : addFilter("images","*.png *.jpg")
     */
    UI_BASE_API virtual void addFilter(const std::string& filterName, const std::string& wildcardList) = 0;

    /**
     * Display the dialog
     * @return the ILocation selected or null sptr if user cancel the operation
     */
    UI_BASE_API virtual core::location::ILocation::sptr show() = 0;

    /// Gets the current extension file selection
    UI_BASE_API virtual std::string getCurrentSelection() const = 0;

protected:

    UI_BASE_API void readConfig();

    UI_BASE_API void writeConfig();

private:

    std::string m_title;
    core::location::ILocation::sptr m_defaultLocaction;
};

} //namespace dialog

} // namespace sight::ui::base
