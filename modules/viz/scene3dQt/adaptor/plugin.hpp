/************************************************************************
 *
 * Copyright (C) 2023 IRCAD France
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

#include "modules/viz/scene3dQt/config.hpp"

#include <core/runtime/plugin.hpp>

namespace sight::module::viz::scene3dQt::adaptor
{

class MODULE_VIZ_SCENE3DQT_CLASS_API plugin : public core::runtime::plugin
{
public:

    MODULE_VIZ_SCENE3DQT_CLASS_API ~plugin() noexcept override;
    MODULE_VIZ_SCENE3DQT_API void start() override;
    MODULE_VIZ_SCENE3DQT_API void stop() noexcept override;
};

} // namespace sight::module::viz::scene3dQt::adaptor
