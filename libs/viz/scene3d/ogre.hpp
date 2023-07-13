/************************************************************************
 *
 * Copyright (C) 2021-2023 IRCAD France
 * Copyright (C) 2021 IHU Strasbourg
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

#include <OgreRenderQueue.h>

#include <string>

namespace sight::viz::scene3d
{

const static std::string RESOURCE_GROUP      = "Sight";
const static std::string DEFAULT_CAMERA_NAME = "DefaultCam";

namespace rq
{

/// Render queue group for surface rendering.
static constexpr std::uint8_t s_SURFACE_ID = Ogre::RenderQueueGroupID::RENDER_QUEUE_MAIN;

/// Render queue group for volume rendering. Leave an extra render queue between
/// surfaces and volumes for some effects.
static constexpr std::uint8_t s_VOLUME_ID = s_SURFACE_ID + 2;

static constexpr std::uint8_t s_NEGATO_WIDGET_ID = s_SURFACE_ID - 1;

}

} // namespace sight::viz::scene3d
