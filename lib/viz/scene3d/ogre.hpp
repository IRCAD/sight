/************************************************************************
 *
 * Copyright (C) 2021-2025 IRCAD France
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
static constexpr std::uint8_t SURFACE = Ogre::RenderQueueGroupID::RENDER_QUEUE_2;

/// Render queue group for volume rendering. Leave an extra render queue between
/// surfaces and volumes for some effects.
static constexpr std::uint8_t VOLUME = Ogre::RenderQueueGroupID::RENDER_QUEUE_3;

static constexpr std::uint8_t NEGATO_WIDGET = Ogre::RenderQueueGroupID::RENDER_QUEUE_1;

static constexpr std::uint8_t OVERLAY = Ogre::RenderQueueGroupID::RENDER_QUEUE_OVERLAY;

} // namespace rq

} // namespace sight::viz::scene3d
