/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

/**
 * @file ui/__/builder/toolbar.cpp
 * @brief This file defines the implementation of the interface class  for the toolbar builder.
 */

#include "ui/__/builder/toolbar.hpp"

#include <boost/lexical_cast.hpp>

namespace sight::ui::builder
{

const toolbar::RegistryKeyType toolbar::REGISTRY_KEY = "::ui::toolbar";

//-----------------------------------------------------------------------------

toolbar::toolbar() :
    m_toolBitmapSize(std::make_pair(-1, -1))
{
}

//-----------------------------------------------------------------------------

void toolbar::initialize(const ui::config_t& configuration)
{
    const auto alignment = configuration.get_optional<std::string>("<xmlattr>.align");
    if(alignment.has_value())
    {
        if(alignment.value() == "top")
        {
            m_alignment = TOP;
        }
        else if(alignment.value() == "bottom")
        {
            m_alignment = BOTTOM;
        }
        else if(alignment.value() == "right")
        {
            m_alignment = RIGHT;
        }
        else if(alignment.value() == "left")
        {
            m_alignment = LEFT;
        }
        else
        {
            SIGHT_FATAL("Wrong value '" << alignment << "' for 'align' attribute (require top, bottom, right or left)");
        }
    }

    if(const auto hexaColor = configuration.get<std::string>("<xmlattr>.backgroundColor", ""); !hexaColor.empty())
    {
        SIGHT_ASSERT(
            "Color string should start with '#' and followed by 6 or 8 "
            "hexadecimal digits. Given color: " << hexaColor,
            hexaColor[0] == '#'
            && (hexaColor.length() == 7 || hexaColor.length() == 9)
        );
        m_backgroundColor = hexaColor;
    }

    m_toolBitmapSize.first  = configuration.get<int>("toolBitmapSize.<xmlattr>.width", m_toolBitmapSize.first);
    m_toolBitmapSize.second = configuration.get<int>("toolBitmapSize.<xmlattr>.height", m_toolBitmapSize.second);

    m_spacing = configuration.get<int>("<xmlattr>.spacing", m_spacing);
}

//-----------------------------------------------------------------------------

ui::container::toolbar::sptr toolbar::getToolBar()
{
    return this->m_toolBar;
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::builder
