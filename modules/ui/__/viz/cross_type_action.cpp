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

#include "cross_type_action.hpp"

#include <core/com/signal.hxx>

#include <boost/algorithm/string.hpp>

namespace sight::module::ui::viz
{

static const core::com::signals::key_t CROSS_TYPE_MODIFIED_SIG = "crossTypeModified";

std::map<std::string, float> cross_type_action::s_scale_conversion = {
    {std::string("full"), 1.0F},
    {std::string("half"), 0.5F},
    {std::string("hide"), 0.0F}
};

//------------------------------------------------------------------------------

cross_type_action::cross_type_action() noexcept :
    m_sig_cross_type_modified(new_signal<cross_type_modified_signal_t>(CROSS_TYPE_MODIFIED_SIG))
{
}

//------------------------------------------------------------------------------

void cross_type_action::starting()
{
    sight::ui::action::action_service_starting();
}

//------------------------------------------------------------------------------

void cross_type_action::stopping()
{
    sight::ui::action::action_service_stopping();
}

//------------------------------------------------------------------------------

void cross_type_action::configuring()
{
    sight::ui::action::initialize();

    const auto& config = this->get_config();
    if(const auto cross_type = config.get_optional<std::string>("crossType"); cross_type.has_value())
    {
        m_cross_type = cross_type.value();
        boost::algorithm::trim(m_cross_type);
        boost::algorithm::to_lower(m_cross_type);
        SIGHT_ASSERT("Unknown crossType", s_scale_conversion.find(m_cross_type) != s_scale_conversion.end());
    }
}

//------------------------------------------------------------------------------

void cross_type_action::updating()
{
    m_sig_cross_type_modified->async_emit(s_scale_conversion[m_cross_type]);
}

//------------------------------------------------------------------------------

void cross_type_action::info(std::ostream& /*_sstream*/)
{
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::viz
