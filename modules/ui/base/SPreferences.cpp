/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
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

#include "SPreferences.hpp"

#include <atoms/conversion/convert.hpp>

#include <core/runtime/ConfigurationElement.hpp>

#include <data/Composite.hpp>
#include <data/reflection/visitor/RecursiveLock.hpp>

#include <io/atoms/Reader.hpp>
#include <io/atoms/types.hpp>
#include <io/atoms/Writer.hpp>
#include <io/zip/ReadDirArchive.hpp>
#include <io/zip/WriteDirArchive.hpp>

#include <service/macros.hpp>

#include <ui/base/preferences/helper.hpp>

#include <filesystem>

namespace sight::module::ui::base
{

//-----------------------------------------------------------------------------

void SPreferences::starting()
{
    this->load();
}

//-----------------------------------------------------------------------------

void SPreferences::stopping()
{
    this->save();
}

//-----------------------------------------------------------------------------

void SPreferences::updating()
{
    this->save();
}

//-----------------------------------------------------------------------------

void SPreferences::configuring()
{
    std::vector<core::runtime::ConfigurationElement::sptr> vectConfig = m_configuration->find("filename");
    SIGHT_ASSERT("The <filename> element must exist.", !vectConfig.empty());
    m_prefFile = vectConfig.at(0)->getValue();
}

//-----------------------------------------------------------------------------

void SPreferences::load()
{
    if(std::filesystem::is_regular_file(m_prefFile))
    {
        const std::filesystem::path folderPath = m_prefFile.parent_path();
        const std::filesystem::path filename   = m_prefFile.filename();

        auto data = this->getLockedInOut<data::Object>(sight::ui::base::preferences::s_PREFERENCES_KEY);

        // Read atom
        io::zip::IReadArchive::sptr readArchive = io::zip::ReadDirArchive::New(folderPath.string());

        io::atoms::Reader reader;
        try
        {
            auto atom = atoms::Object::dynamicCast(reader.read(readArchive, filename));

            data::Object::sptr newData = atoms::conversion::convert(
                atom,
                atoms::conversion::AtomVisitor::ChangePolicy()
            );
            data->shallowCopy(newData);
        }
        catch(...)
        {
            SIGHT_ERROR("Failed to load preference file '" + m_prefFile.string() + "'.");
        }
    }
}

//-----------------------------------------------------------------------------

void SPreferences::save()
{
    const std::filesystem::path folderPath = m_prefFile.parent_path();
    const std::filesystem::path filename   = m_prefFile.filename();

    data::Object::sptr obj;
    {
        obj = this->getLockedInOut<data::Object>(sight::ui::base::preferences::s_PREFERENCES_KEY).get_shared();
    }

    // Lock recursively all objects referenced in the root object
    data::reflection::visitor::RecursiveLock recursiveLock(obj);

    // Convert data to atom
    atoms::Object::sptr atom = atoms::conversion::convert(obj);
    // Write atom
    io::zip::IWriteArchive::sptr writeArchive = io::zip::WriteDirArchive::New(folderPath.string());
    io::atoms::FormatType format              = io::atoms::JSON;

    namespace fs = std::filesystem;
    if(fs::exists(m_prefFile) && fs::is_regular_file(m_prefFile)
       && (fs::status(m_prefFile).permissions() & fs::perms::owner_write) == fs::perms::none)
    {
        SIGHT_ERROR(
            "SPreference need write access to the file '" + m_prefFile.string() + "'."
                                                                                  "Please, change file permission."
        );
    }
    else
    {
        io::atoms::Writer(atom).write(writeArchive, filename, format);
    }
}

//-----------------------------------------------------------------------------

} //namespace sight::module::ui::base
