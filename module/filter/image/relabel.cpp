/************************************************************************
 *
 * Copyright (C) 2026 IRCAD France
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

#include "relabel.hpp"

#include <filter/image/relabel.hpp>

#include <cstdint>

namespace sight::module::filter::image
{

relabel::relabel() noexcept :
    filter(has_signals::signals())
{
}

//------------------------------------------------------------------------------

void relabel::configuring()
{
}

//------------------------------------------------------------------------------

void relabel::starting()
{
}

//------------------------------------------------------------------------------

void relabel::updating()
{
    // Build the dictionary
    std::map<std::int32_t, std::int32_t> dictionary;
    std::optional<std::int16_t> star {std::nullopt};

    bool mapped = false;

    if(const auto& dictionary_in = m_dictionary_in.const_lock(); dictionary_in&& !dictionary_in->empty())
    {
        for(const auto& [key, value] : *dictionary_in)
        {
            const auto& serializable_data = std::dynamic_pointer_cast<sight::data::string_serializable>(value);
            SIGHT_ASSERT("Dictionary value is not serializable", serializable_data);

            const auto to = std::stoul(serializable_data->to_string());
            SIGHT_ASSERT("Dictionary `star` value is out of bound", to <= 0xFF);

            if(key == "*")
            {
                star   = static_cast<std::int32_t>(to);
                mapped = true;
            }
            else
            {
                const auto from = std::stoul(key);
                SIGHT_ASSERT("Dictionary `from` value is out of bound", from <= 0xFF);

                dictionary[static_cast<std::int32_t>(from)] = static_cast<std::int32_t>(to);
                mapped                                      = true;
            }
        }
    }
    else
    {
        const auto& root   = this->get_config();
        const auto& config = root.get_child_optional("config");

        if(!config)
        {
            // No dictionary and no configuration
            return;
        }

        for(const auto& map : boost::make_iterator_range(config->equal_range("map")))
        {
            if(const auto key = map.second.get<std::string>("<xmlattr>.from"); key == "*")
            {
                star   = map.second.get<std::int32_t>("<xmlattr>.to");
                mapped = true;
            }
            else
            {
                const auto from = map.second.get<std::int32_t>("<xmlattr>.from");
                const auto to   = map.second.get<std::int32_t>("<xmlattr>.to");

                dictionary[from] = to;
                mapped           = true;
            }
        }
    }

    if(!mapped)
    {
        // No dictionary
        return;
    }

    auto mask = m_image_inout.lock();
    SIGHT_ASSERT("No mask", mask);

    sight::filter::image::relabel(*mask, dictionary, star);
    mask->async_emit(this, sight::data::image::signals::BUFFER_MODIFIED);
}

//------------------------------------------------------------------------------

void relabel::stopping()
{
}

//------------------------------------------------------------------------------

sight::service::connections_t relabel::auto_connections() const
{
    return {
        {m_image_inout, sight::data::signals::MODIFIED, sight::service::base::slots::UPDATE},
        {m_image_inout, sight::data::image::signals::BUFFER_MODIFIED, sight::service::base::slots::UPDATE},
        {m_dictionary_in, sight::data::signals::MODIFIED, sight::service::base::slots::UPDATE},
        {m_dictionary_in, sight::data::map::signals::ADDED_OBJECTS, sight::service::base::slots::UPDATE},
        {m_dictionary_in, sight::data::map::signals::CHANGED_OBJECTS, sight::service::base::slots::UPDATE}
    };
}

//------------------------------------------------------------------------------

} // namespace sight::module::filter::image
