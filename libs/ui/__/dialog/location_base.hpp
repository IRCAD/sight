/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "ui/__/config.hpp"
#include "ui/__/object.hpp"

#include <core/location/base.hpp>

#include <filesystem>
#include <string>

namespace sight::ui::dialog
{

/**
 * @brief   Defines the generic file/folder selector dialog for IHM.
 */
class UI_CLASS_API location_base : public ui::object
{
public:

    SIGHT_DECLARE_CLASS(location_base, ui::object);

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

    typedef std::string factory_registry_key_t;

    /// this *unique* key should  be used *for all* factory for specific location(qt,wx,...)
    UI_API static const factory_registry_key_t REGISTRY_KEY;

    UI_API static const std::string SOFTWARE_UI;
    UI_API static const std::string DLG_DEFAULT_FILE;
    UI_API static const std::string DLG_DEFAULT_DIRECTORY;

    UI_API ~location_base() override;

    UI_API location_base();

    /// set the title for the dialog
    UI_API virtual void setTitle(const std::string& _title);

    /// returns the title for the dialog
    UI_API virtual const std::string& getTitle();

    /// set the initial location for the dialog
    UI_API virtual void setDefaultLocation(core::location::base::sptr _loc);

    /// get the default location for the dialog (from preferences or specified by user)
    UI_API virtual core::location::base::sptr getDefaultLocation();

    /// save the specified default location for the dialog in preferences (if available)
    UI_API virtual void saveDefaultLocation(core::location::base::sptr _loc);

    /// set the type of location for the dialog (SINGLE_FILE, FOLDER, MULTI_FILES)
    UI_API virtual void setType(Types _type) = 0;

    /// allow to set option to the file dialog mode=READ/WRITE , check=FILE_MUST_EXIST
    UI_API virtual void setOption(Options _option) = 0;

    /**
     * @brief specify some filtering when browsing files:
     * @param[in] filterName a string that will be displayed as filter name
     * @param[in] wildcardList a string of extension (glob syntax) separated by spaces
     * example : addFilter("images","*.png *.jpg")
     */
    UI_API virtual void addFilter(const std::string& _filter_name, const std::string& _wildcard_list) = 0;

    /**
     * Display the dialog
     * @return the ILocation selected or null sptr if user cancel the operation
     */
    UI_API virtual core::location::base::sptr show() = 0;

    /// Gets the current extension file selection
    UI_API virtual std::string getCurrentSelection() const = 0;

    /// Helpers that splits the extension getCurrentSelection()
    /// @return a vector of selected extensions
    UI_API std::vector<std::string> getSelectedExtensions() const;

protected:

    UI_API void readConfig();

    UI_API void writeConfig();

private:

    std::string m_title;
    core::location::base::sptr m_defaultLocaction;
};

} // namespace sight::ui::dialog
