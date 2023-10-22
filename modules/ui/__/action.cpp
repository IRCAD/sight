/************************************************************************
 *
 * Copyright (C) 2022-2023 IRCAD France
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

#include "action.hpp"

#include <core/com/signal.hxx>
#include <core/runtime/helper.hpp>
namespace sight::module::ui
{

//------------------------------------------------------------------------------

void action::configuring()
{
    this->initialize();

    const auto config = this->get_config();
    m_sync = core::runtime::get_ptree_value(config, "sync", m_sync);

    if(const auto& parameter = config.get_child_optional("parameter"); parameter)
    {
        // Convert to std::optional and check that the key is present
        const auto& key = parameter->get_optional<std::string>("<xmlattr>.key");
        SIGHT_THROW_IF("Parameter key is mandatory if a action must emit a parameterChanged signal.", !key);
        m_key = std::make_optional(*key);

        // Convert to std::optional.
        const auto& clicked = parameter->get_optional<std::string>("<xmlattr>.clicked");
        m_clicked = clicked ? std::make_optional(*clicked) : std::nullopt;

        const auto& checked = parameter->get_optional<std::string>("<xmlattr>.checked");
        m_checked = checked ? std::make_optional(*checked) : std::nullopt;

        const auto& unchecked = parameter->get_optional<std::string>("<xmlattr>.unchecked");
        m_unchecked = unchecked ? std::make_optional(*unchecked) : std::nullopt;
    }
}

//-----------------------------------------------------------------------------

void action::starting()
{
    this->actionServiceStarting();
}

//-----------------------------------------------------------------------------

void action::stopping()
{
    this->actionServiceStopping();
}

//-----------------------------------------------------------------------------

void action::updating()
{
    if(this->confirmAction())
    {
        if(m_sync)
        {
            M_CLICKED_SIG->emit();
        }
        else
        {
            M_CLICKED_SIG->async_emit();
        }

        if(m_key)
        {
            const bool is_checked = this->checked();
            if(m_sync)
            {
                M_PARAMETER_CHANGED_SIG->emit(
                    m_clicked
                    ? *m_clicked
                    : is_checked && m_checked
                    ? *m_checked
                    : !is_checked && m_unchecked
                    ? *m_unchecked
                    : sight::ui::parameter_t(is_checked),
                    *m_key
                );
            }
            else
            {
                M_PARAMETER_CHANGED_SIG->async_emit(
                    m_clicked
                    ? *m_clicked
                    : is_checked && m_checked
                    ? *m_checked
                    : !is_checked && m_unchecked
                    ? *m_unchecked
                    : sight::ui::parameter_t(is_checked),
                    *m_key
                );
            }
        }
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::module::ui
