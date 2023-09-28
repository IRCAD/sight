/************************************************************************
 *
 * Copyright (C) 2023 IRCAD France
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

#include "location_dummy.hpp"

#include <core/location/multiple_files.hpp>
#include <core/location/single_file.hpp>
#include <core/location/single_folder.hpp>

namespace sight::ui::dialog
{

std::queue<std::vector<std::filesystem::path> > location_dummy::pathsList;

//------------------------------------------------------------------------------

void location_dummy::setType(Types type)
{
    m_type = type;
}

//------------------------------------------------------------------------------

void location_dummy::setOption(Options /*option*/)
{
}

//------------------------------------------------------------------------------

void location_dummy::addFilter(const std::string& /*filterName*/, const std::string& /*wildcardList*/)
{
}

//------------------------------------------------------------------------------

std::string location_dummy::getCurrentSelection() const
{
    return "";
}

//------------------------------------------------------------------------------

void location_dummy::setPaths(const std::vector<std::filesystem::path>& files)
{
    pushPaths(files);
}

//------------------------------------------------------------------------------

void location_dummy::pushPaths(const std::vector<std::filesystem::path>& files)
{
    pathsList.push(files);
}

//------------------------------------------------------------------------------

bool location_dummy::clear()
{
    if(pathsList.empty())
    {
        return true;
    }

    while(!pathsList.empty())
    {
        pathsList.pop();
    }

    return false;
}

//------------------------------------------------------------------------------

sight::core::location::base::sptr location_dummy::show()
{
    std::vector<std::filesystem::path> paths = pathsList.front();
    pathsList.pop();
    if(m_type == SINGLE_FILE)
    {
        auto singleFile = std::make_shared<sight::core::location::single_file>();
        singleFile->set_file(paths[0]);
        return singleFile;
    }

    if(m_type == FOLDER)
    {
        auto singleFolder = std::make_shared<sight::core::location::single_folder>();
        singleFolder->set_folder(paths[0]);
        return singleFolder;
    }

    auto multipleFiles = std::make_shared<sight::core::location::multiple_files>();
    multipleFiles->set_files(paths);
    return multipleFiles;
}

} // namespace sight::ui::dialog
