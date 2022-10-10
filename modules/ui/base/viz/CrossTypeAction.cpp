/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "CrossTypeAction.hpp"

#include <core/com/Signal.hxx>

#include <boost/algorithm/string.hpp>

namespace sight::module::ui::base::viz
{

static const core::com::Signals::SignalKeyType s_CROSS_TYPE_MODIFIED_SIG = "crossTypeModified";

std::map<std::string, float> CrossTypeAction::m_scaleConversion = {
    {std::string("full"), 1.0F},
    {std::string("half"), 0.5F},
    {std::string("hide"), 0.0F}
};

//------------------------------------------------------------------------------

CrossTypeAction::CrossTypeAction() noexcept :
    m_sigCrossTypeModified(newSignal<CrossTypeModifiedSignalType>(s_CROSS_TYPE_MODIFIED_SIG))
{
}

//------------------------------------------------------------------------------

void CrossTypeAction::starting()
{
    sight::ui::base::IAction::actionServiceStarting();
}

//------------------------------------------------------------------------------

void CrossTypeAction::stopping()
{
    sight::ui::base::IAction::actionServiceStopping();
}

//------------------------------------------------------------------------------

void CrossTypeAction::configuring()
{
    sight::ui::base::IAction::initialize();

    if(this->m_configuration->size() > 0)
    {
        auto vectConfig = this->m_configuration->find("crossType");
        SIGHT_ASSERT("Missing <crossType> tag!", !vectConfig.empty());
        m_crossType = vectConfig.at(0)->getValue();
        boost::algorithm::trim(m_crossType);
        boost::algorithm::to_lower(m_crossType);
        SIGHT_ASSERT("Unknown crossType", m_scaleConversion.find(m_crossType) != m_scaleConversion.end());
    }
}

//------------------------------------------------------------------------------

void CrossTypeAction::updating()
{
    m_sigCrossTypeModified->asyncEmit(m_scaleConversion[m_crossType]);
}

//------------------------------------------------------------------------------

void CrossTypeAction::info(std::ostream& /*_sstream*/)
{
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::base::viz
