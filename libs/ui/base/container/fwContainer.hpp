/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#pragma once

#include "ui/base/config.hpp"
#include "ui/base/GuiBaseObject.hpp"

namespace sight::ui::base
{

namespace container
{

/**
 * @brief   Defines the generic container for IHM.
 *
 */
class UI_BASE_CLASS_API fwContainer : public ui::base::GuiBaseObject
{
public:

    SIGHT_DECLARE_CLASS(fwContainer, ui::base::GuiBaseObject);

    UI_BASE_API virtual void clean()            = 0;
    UI_BASE_API virtual void destroyContainer() = 0;
    UI_BASE_API virtual bool isShownOnScreen()  = 0;

    UI_BASE_API virtual void setVisible(bool isVisible) = 0;
    UI_BASE_API virtual void setEnabled(bool isEnabled) = 0;
};

} // namespace container

} // namespace sight::ui::base
