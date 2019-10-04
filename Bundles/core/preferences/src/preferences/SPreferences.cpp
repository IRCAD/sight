/************************************************************************
 *
 * Copyright (C) 2014-2019 IRCAD France
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

#include "preferences/SPreferences.hpp"

#include <fwAtomConversion/convert.hpp>

#include <fwAtomsBoostIO/Reader.hpp>
#include <fwAtomsBoostIO/types.hpp>
#include <fwAtomsBoostIO/Writer.hpp>

#include <fwData/Composite.hpp>

#include <fwDataCamp/visitor/RecursiveLock.hpp>

#include <fwPreferences/helper.hpp>

#include <fwRuntime/ConfigurationElement.hpp>

#include <fwServices/macros.hpp>

#include <fwZip/ReadDirArchive.hpp>
#include <fwZip/WriteDirArchive.hpp>

#include <filesystem>

namespace preferences
{

fwServicesRegisterMacro( ::fwPreferences::IPreferences, ::preferences::SPreferences, ::fwData::Composite );

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
    std::vector < ::fwRuntime::ConfigurationElement::sptr > vectConfig = m_configuration->find("filename");
    SLM_ASSERT("The <filename> element must exist.", !vectConfig.empty() );
    m_prefFile = vectConfig.at(0)->getValue();
}

//-----------------------------------------------------------------------------

void SPreferences::load()
{
    if (std::filesystem::is_regular_file(m_prefFile))
    {
        const std::filesystem::path folderPath = m_prefFile.parent_path();
        const std::filesystem::path filename   = m_prefFile.filename();

        ::fwData::Object::sptr data = this->getInOut< ::fwData::Object >(::fwPreferences::s_PREFERENCES_KEY);
        SLM_ASSERT("The inout key '" + ::fwPreferences::s_PREFERENCES_KEY + "' is not correctly set.", data);

        // Read atom
        ::fwZip::IReadArchive::sptr readArchive = ::fwZip::ReadDirArchive::New(folderPath.string());
        ::fwAtomsBoostIO::Reader reader;
        try
        {
            ::fwAtoms::Object::sptr atom = ::fwAtoms::Object::dynamicCast( reader.read( readArchive, filename ) );

            ::fwData::Object::sptr newData = ::fwAtomConversion::convert(atom,
                                                                         ::fwAtomConversion::AtomVisitor::ChangePolicy());
            data->shallowCopy(newData);
        }
        catch(...)
        {
            SLM_ERROR("Failed to load preference file '"+m_prefFile.string()+"'.");
        }
    }
}

//-----------------------------------------------------------------------------

void SPreferences::save()
{
    const std::filesystem::path folderPath = m_prefFile.parent_path();
    const std::filesystem::path filename   = m_prefFile.filename();

    ::fwData::Object::sptr obj = this->getInOut< ::fwData::Object >(::fwPreferences::s_PREFERENCES_KEY);
    SLM_ASSERT("The inout key '" + ::fwPreferences::s_PREFERENCES_KEY + "' is not correctly set.", obj);

    // Mutex data lock
    ::fwDataCamp::visitor::RecursiveLock recursiveLock(obj);

    // Convert data to atom
    ::fwAtoms::Object::sptr atom = ::fwAtomConversion::convert(obj);
    // Write atom
    ::fwZip::IWriteArchive::sptr writeArchive = ::fwZip::WriteDirArchive::New(folderPath.string());
    ::fwAtomsBoostIO::FormatType format       = ::fwAtomsBoostIO::JSON;

    namespace fs = std::filesystem;
    if( fs::exists(m_prefFile) && fs::is_regular_file(m_prefFile) &&
        (fs::status(m_prefFile).permissions() & fs::perms::owner_write) != fs::perms::none)
    {
        SLM_ERROR("SPreference need write access to the file '"+m_prefFile.string()+"'."
                  "Please, change file permission.");
    }
    else
    {
        ::fwAtomsBoostIO::Writer(atom).write( writeArchive, filename, format );
    }
}

//-----------------------------------------------------------------------------

} //namespace preferences
