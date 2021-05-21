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

#include "ui/base/dialog/ILocationDialog.hpp"

#include <core/location/SingleFile.hpp>
#include <core/location/SingleFolder.hpp>

#include <data/String.hpp>

#include <service/IService.hpp>
#include <service/registry/ObjectService.hpp>

#include <ui/base/preferences/helper.hpp>

namespace sight::ui::base
{

namespace dialog
{

const ILocationDialog::FactoryRegistryKeyType ILocationDialog::REGISTRY_KEY = "::ui::base::dialog::LocationDialog";

const std::string ILocationDialog::SOFTWARE_UI           = "SOFTWARE_UI";
const std::string ILocationDialog::DLG_DEFAULT_FILE      = "DLG_DEFAULT_FILE";
const std::string ILocationDialog::DLG_DEFAULT_DIRECTORY = "DLG_DEFAULT_DIRECTORY";

//-----------------------------------------------------------------------------

ILocationDialog::ILocationDialog()
{
}

//-----------------------------------------------------------------------------

ILocationDialog::~ILocationDialog()
{
}

//------------------------------------------------------------------------------

void ILocationDialog::setTitle(const std::string& title)
{
    m_title = title;
}

//------------------------------------------------------------------------------

const std::string& ILocationDialog::getTitle()
{
    return m_title;
}

//------------------------------------------------------------------------------

void ILocationDialog::setDefaultLocation(core::location::ILocation::sptr loc)
{
    m_defaultLocaction = loc;
}

//------------------------------------------------------------------------------

const core::location::ILocation::sptr ILocationDialog::getDefaultLocation()
{
    data::Composite::sptr prefUI = this->getPreferenceUI();
    core::location::ILocation::sptr location;

    if(prefUI)
    {
        // This code is temporary
        // @TODO: find a better way to serialize in preferences
        if(prefUI->find(ILocationDialog::DLG_DEFAULT_FILE) != prefUI->end())
        {
            const auto stringLocation =
                data::String::dynamicCast((*prefUI)[ILocationDialog::DLG_DEFAULT_FILE]);
            const auto& singleFile = std::make_shared<core::location::SingleFile>();
            singleFile->setFile(stringLocation->getValue());
            location = singleFile;
        }
        else if(prefUI->find(ILocationDialog::DLG_DEFAULT_DIRECTORY) != prefUI->end())
        {
            const auto stringLocation =
                data::String::dynamicCast((*prefUI)[ILocationDialog::DLG_DEFAULT_DIRECTORY]);
            const auto& singleDirectory = std::make_shared<core::location::SingleFolder>();
            singleDirectory->setFolder(stringLocation->getValue());
            location = singleDirectory;
        }
    }

    if(!location)
    {
        location = m_defaultLocaction;
    }

    return location;
}

//-----------------------------------------------------------------------------

void ILocationDialog::saveDefaultLocation(core::location::ILocation::sptr loc)
{
    data::Composite::sptr prefUI = this->getPreferenceUI();
    if(prefUI && loc)
    {
        // This code is temporary
        // @TODO: find a better way to serialize in preferences
        auto stringLocation = data::String::New();

        if(auto singleFile = core::location::SingleFile::dynamicCast(loc))
        {
            stringLocation->setValue(singleFile->getFile().string());
            (*prefUI)[ILocationDialog::DLG_DEFAULT_FILE] = stringLocation;
        }
        else if(auto singleDirectory = core::location::SingleFolder::dynamicCast(loc))
        {
            stringLocation->setValue(singleDirectory->getFolder().string());
            (*prefUI)[ILocationDialog::DLG_DEFAULT_DIRECTORY] = stringLocation;
        }
    }
}

//-----------------------------------------------------------------------------

data::Composite::sptr ILocationDialog::getPreferenceUI()
{
    data::Composite::sptr prefUI;

    // Get preferences
    data::Composite::sptr prefs = ui::base::preferences::getPreferences();
    if(prefs)
    {
        data::Composite::sptr framesUI;

        // Retrieves software UI pref
        if(prefs->find(ILocationDialog::SOFTWARE_UI) != prefs->end())
        {
            framesUI = data::Composite::dynamicCast((*prefs)[ILocationDialog::SOFTWARE_UI]);
        }
        else
        {
            framesUI                               = data::Composite::New();
            (*prefs)[ILocationDialog::SOFTWARE_UI] = framesUI;
        }

        // Retrieves associated dialog UI pref
        if(framesUI->find(this->getTitle()) != framesUI->end())
        {
            prefUI = data::Composite::dynamicCast((*framesUI)[this->getTitle()]);
        }
        else
        {
            prefUI                        = data::Composite::New();
            (*framesUI)[this->getTitle()] = prefUI;
        }
    }

    return prefUI;
}

//-----------------------------------------------------------------------------

} //namespace dialog

} // namespace sight::ui::base
