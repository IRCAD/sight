/************************************************************************
 *
 * Copyright (C) 2019 IRCAD France
 * Copyright (C) 2019 IHU Strasbourg
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

#include "fwGuiQml/dialog/StandardButton.hpp"

namespace fwGuiQml
{
namespace dialog
{

//------------------------------------------------------------------------------

StandardButton::StandardButton()
{
    m_button = ButtonList::NoButton;
}

//------------------------------------------------------------------------------

StandardButton::~StandardButton()
{

}

//------------------------------------------------------------------------------

void StandardButton::setButton(ButtonList button)
{
    m_button = button;
    Q_EMIT buttonChanged();
}

//------------------------------------------------------------------------------

const StandardButton::ButtonList& StandardButton::getButton()
{
    return m_button;
}

//------------------------------------------------------------------------------

} // namespace dialog
} // namespace fwGuiQml
