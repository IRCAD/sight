/************************************************************************
 *
 * Copyright (C) 2025 IRCAD France
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

#include "concat.hpp"

#include <boost/format.hpp>

namespace sight::module::data
{

concat::concat() :
    service::filter(m_signals)
{
}

//------------------------------------------------------------------------------

service::connections_t concat::auto_connections() const
{
    return {
        {m_fragments, sight::data::object::MODIFIED_SIG, slots::UPDATE}
    };
}

//-----------------------------------------------------------------------------

void concat::configuring(const config_t& /*_config*/)
{
}

//-----------------------------------------------------------------------------

void concat::starting()
{
}

//-----------------------------------------------------------------------------

void concat::updating()
{
    std::string result;

    if(const std::string format = *m_format; !format.empty())
    {
        // Convert to a string vector first
        std::vector<std::string> fragment_strings;
        fragment_strings.reserve(m_fragments.size());

        std::ranges::transform(
            m_fragments,
            std::back_inserter(fragment_strings),
            [](const auto& _fragment)
            {
                return _fragment.second->const_lock()->to_string();
            });

        result = [&format, &fragment_strings]() -> std::string
                 {
                     boost::format fmt(format);
                     for(const auto& s : fragment_strings)
                     {
                         fmt % s;
                     }

                     return fmt.str();
                 }();
    }
    else
    {
        // Simple concatenation
        for(const auto& fragment : m_fragments)
        {
            result += fragment.second->const_lock()->to_string();
        }
    }

    {
        // Update the target
        auto target = m_target.lock();
        target->from_string(result);

        target->async_emit(this, sight::data::object::MODIFIED_SIG);
    }
}

//-----------------------------------------------------------------------------

void concat::stopping()
{
}

//-----------------------------------------------------------------------------

} // namespace sight::module::data.
