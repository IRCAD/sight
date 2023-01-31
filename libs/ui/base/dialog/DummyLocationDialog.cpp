/************************************************************************
 *
 * Copyright (C) 2022 IRCAD France
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

#include "DummyLocationDialog.hpp"

#include <core/location/MultipleFiles.hpp>
#include <core/location/SingleFile.hpp>
#include <core/location/SingleFolder.hpp>

namespace sight::ui::base::dialog
{

std::vector<std::filesystem::path> DummyLocationDialog::paths;

DummyLocationDialog::DummyLocationDialog(sight::ui::base::factory::Key /*key*/)
{
}

//------------------------------------------------------------------------------

void DummyLocationDialog::setType(Types type)
{
    m_type = type;
}

//------------------------------------------------------------------------------

sight::ui::base::dialog::ILocationDialog& DummyLocationDialog::setOption(Options /*option*/)
{
    return *this;
}

//------------------------------------------------------------------------------

void DummyLocationDialog::addFilter(const std::string& /*filterName*/, const std::string& /*wildcardList*/)
{
}

//------------------------------------------------------------------------------

std::string DummyLocationDialog::getCurrentSelection() const
{
    return "";
}

//------------------------------------------------------------------------------

void DummyLocationDialog::setPaths(const std::vector<std::filesystem::path>& files)
{
    paths = files;
}

//------------------------------------------------------------------------------

sight::core::location::ILocation::sptr DummyLocationDialog::show()
{
    if(m_type == sight::ui::base::dialog::ILocationDialog::SINGLE_FILE)
    {
        auto singleFile = std::make_shared<sight::core::location::SingleFile>();
        singleFile->setFile(paths[0]);
        return singleFile;
    }

    if(m_type == sight::ui::base::dialog::ILocationDialog::FOLDER)
    {
        auto singleFolder = std::make_shared<sight::core::location::SingleFolder>();
        singleFolder->setFolder(paths[0]);
        return singleFolder;
    }

    auto multipleFiles = std::make_shared<sight::core::location::MultipleFiles>();
    multipleFiles->setFiles(paths);
    return multipleFiles;
}

} // namespace sight::ui::base::dialog
