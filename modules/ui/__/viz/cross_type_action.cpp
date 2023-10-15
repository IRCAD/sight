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

std::map<std::string, float> cross_type_action::m_scaleConversion = {
    {std::string("full"), 1.0F},
    {std::string("half"), 0.5F},
    {std::string("hide"), 0.0F}
};

//------------------------------------------------------------------------------

cross_type_action::cross_type_action() noexcept :
    m_sigCrossTypeModified(new_signal<CrossTypeModifiedSignalType>(CROSS_TYPE_MODIFIED_SIG))
{
}

//------------------------------------------------------------------------------

void cross_type_action::starting()
{
    sight::ui::action::actionServiceStarting();
}

//------------------------------------------------------------------------------

void cross_type_action::stopping()
{
    sight::ui::action::actionServiceStopping();
}

//------------------------------------------------------------------------------

void cross_type_action::configuring()
{
    sight::ui::action::initialize();

    const auto& config = this->get_config();
    if(const auto crossType = config.get_optional<std::string>("crossType"); crossType.has_value())
    {
        m_crossType = crossType.value();
        boost::algorithm::trim(m_crossType);
        boost::algorithm::to_lower(m_crossType);
        SIGHT_ASSERT("Unknown crossType", m_scaleConversion.find(m_crossType) != m_scaleConversion.end());
    }
}

//------------------------------------------------------------------------------

void cross_type_action::updating()
{
    m_sigCrossTypeModified->async_emit(m_scaleConversion[m_crossType]);
}

//------------------------------------------------------------------------------

void cross_type_action::info(std::ostream& /*_sstream*/)
{
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::viz
