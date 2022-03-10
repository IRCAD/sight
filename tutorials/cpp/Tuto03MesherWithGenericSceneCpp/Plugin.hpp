/************************************************************************
 *
 * Copyright (C) 2018-2022 IRCAD France
 * Copyright (C) 2018-2020 IHU Strasbourg
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

#include "Tuto03MesherWithGenericSceneCpp/config.hpp"

#include <core/com/SlotBase.hpp>
#include <core/runtime/Plugin.hpp>

#include <service/AppManager.hpp>

namespace Tuto03MesherWithGenericSceneCpp
{

/**
 * @brief   This class is started when the module is loaded.
 */
class TUTO03MESHERWITHGENERICSCENECPP_CLASS_API Plugin : public sight::core::runtime::Plugin
{
public:

    /// Constructor.
    TUTO03MESHERWITHGENERICSCENECPP_API Plugin() noexcept;

    /// Destructor. Do nothing.
    TUTO03MESHERWITHGENERICSCENECPP_API ~Plugin() noexcept;

    /// Overrides start method. .
    TUTO03MESHERWITHGENERICSCENECPP_API void start();

    /// Overrides stop method. Do nothing
    TUTO03MESHERWITHGENERICSCENECPP_API void stop() noexcept;

private:

    std::unique_ptr<sight::service::AppManager> m_appManager;

    sight::core::com::SlotBase::sptr m_slotRecSelected;
    sight::core::com::SlotBase::sptr m_slotEmptySelection;
};

} // namespace Tuto03MesherWithGenericSceneCpp
