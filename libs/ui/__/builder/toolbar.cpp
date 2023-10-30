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

const toolbar::registry_key_t toolbar::REGISTRY_KEY = "::ui::toolbar";

//-----------------------------------------------------------------------------

toolbar::toolbar() :
    m_tool_bitmap_size(std::make_pair(-1, -1))
{
}

//-----------------------------------------------------------------------------

void toolbar::initialize(const ui::config_t& _configuration)
{
    const auto alignment = _configuration.get_optional<std::string>("<xmlattr>.align");
    if(alignment.has_value())
    {
        if(alignment.value() == "top")
        {
            m_alignment = top;
        }
        else if(alignment.value() == "bottom")
        {
            m_alignment = bottom;
        }
        else if(alignment.value() == "right")
        {
            m_alignment = right;
        }
        else if(alignment.value() == "left")
        {
            m_alignment = left;
        }
        else
        {
            SIGHT_FATAL("Wrong value '" << alignment << "' for 'align' attribute (require top, bottom, right or left)");
        }
    }

    if(const auto hexa_color = _configuration.get<std::string>("<xmlattr>.backgroundColor", ""); !hexa_color.empty())
    {
        SIGHT_ASSERT(
            "Color string should start with '#' and followed by 6 or 8 "
            "hexadecimal digits. Given color: " << hexa_color,
            hexa_color[0] == '#'
            && (hexa_color.length() == 7 || hexa_color.length() == 9)
        );
        m_background_color = hexa_color;
    }

    m_tool_bitmap_size.first  = _configuration.get<int>("toolBitmapSize.<xmlattr>.width", m_tool_bitmap_size.first);
    m_tool_bitmap_size.second = _configuration.get<int>("toolBitmapSize.<xmlattr>.height", m_tool_bitmap_size.second);

    m_spacing = _configuration.get<int>("<xmlattr>.spacing", m_spacing);
}

//-----------------------------------------------------------------------------

ui::container::toolbar::sptr toolbar::get_tool_bar()
{
    return this->m_tool_bar;
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::builder
