/************************************************************************
 *
 * Copyright (C) 2025-2026 IRCAD France
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

#include "filter/image/relabel.hpp"

#include <core/tools/dispatcher.hpp>

namespace sight::filter::image
{

//-----------------------------------------------------------------------------

void relabel(
    data::image& _image,
    const std::map<std::int32_t, std::int32_t>& _dictionary,
    std::optional<std::uint16_t> _default_value
)
{
    auto do_remap =
        []<class PIXEL_TYPE>(
            data::image& _image,
            const std::map<std::int32_t, std::int32_t>& _dictionary,
            std::optional<std::uint16_t> _default_value
        )
        {
            for(auto& pixel : _image.range<PIXEL_TYPE>())
            {
                // Clamp pixel value to dictionary range [0, 255]
                const auto index = std::min(static_cast<std::size_t>(pixel), static_cast<std::size_t>(255));

                const auto it = _dictionary.find(static_cast<std::int32_t>(index));
                if(it != _dictionary.end())
                {
                    const auto& mapped_value = it->second;
                    pixel = static_cast<PIXEL_TYPE>(mapped_value);
                }
                else if(_default_value)
                {
                    pixel = static_cast<PIXEL_TYPE>(*_default_value);
                }
            }
        };

    const auto type = _image.type();
    using sight::core::tools::dispatcher;
    using sight::core::tools::integer_types;
    dispatcher<integer_types, decltype(do_remap)>::invoke(type, _image, _dictionary, _default_value);
}

//-----------------------------------------------------------------------------

} // namespace sight::filter::image
