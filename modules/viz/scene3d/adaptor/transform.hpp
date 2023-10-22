/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
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

#include "modules/viz/scene3d/config.hpp"

#include <viz/scene3d/adaptor.hpp>
#include <viz/scene3d/render.hpp>
#include <viz/scene3d/transformable.hpp>

#include <OGRE/OgreSceneNode.h>

namespace sight::module::viz::scene3d::adaptor
{

/**
 * @brief This adaptor binds a data::matrix4 to an Ogre scene node.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service type="sight::module::viz::scene3d::adaptor::transform">
        <inout key="transform" uid="..." />
        <config transform="meshTFAdaptor" />
   </service>
   @endcode
 *
 * @subsection In-Out In-Out:
 * - \b transform [sight::data::matrix4]: Sight transform matrix.
 *
 * @subsection Configuration Configuration:
 * - \b transform (mandatory, string): Name of the Ogre Transform.
 * - \b parent (optional, string, default=""): Name of the parent Ogre Transform you want to attach to.
 */
class MODULE_VIZ_SCENE3D_CLASS_API transform final :
    public sight::viz::scene3d::adaptor,
    public sight::viz::scene3d::transformable
{
public:

    static constexpr std::string_view s_TRANSFORM_INOUT = "transform";

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(transform, sight::viz::scene3d::adaptor);

    /// Creates the service.
    MODULE_VIZ_SCENE3D_API transform() noexcept = default;

    /// Destroys the adaptor.
    MODULE_VIZ_SCENE3D_API ~transform() noexcept final = default;

protected:

    /// Retrieves id sets in the configurations.
    MODULE_VIZ_SCENE3D_API void configuring() final;

    /// Creates the Ogre::SceneNode corresponding to the associated transform matrix.
    MODULE_VIZ_SCENE3D_API void starting() final;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect data::object::MODIFIED_SIG of s_TRANSFORM_INOUT to service::slots::UPDATE
     */
    MODULE_VIZ_SCENE3D_API service::connections_t auto_connections() const final;

    /// Updates m_transformNode from the data::matrix4.
    MODULE_VIZ_SCENE3D_API void updating() final;

    /// Does nothing.
    MODULE_VIZ_SCENE3D_API void stopping() final;

private:

    /// Defines the Parent transform identifier.
    sight::viz::scene3d::render::ogre_object_id_t m_parentTransformId;

    /// Contains the Ogre transform node.
    Ogre::SceneNode* m_transformNode {nullptr};

    /// Contains the Ogre parent transform sceneNode.
    Ogre::SceneNode* m_parentTransformNode {nullptr};

    /// Defines the Ogre transformation of this service
    Ogre::Affine3 m_ogreTransform;

    data::ptr<data::matrix4, data::Access::inout> m_matrix {this, s_TRANSFORM_INOUT, true};
};

} // namespace sight::module::viz::scene3d::adaptor.
