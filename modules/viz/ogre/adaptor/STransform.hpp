/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
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

#include "modules/viz/ogre/config.hpp"

#include <OGRE/OgreSceneNode.h>

#include <viz/ogre/IAdaptor.hpp>
#include <viz/ogre/ITransformable.hpp>
#include <viz/ogre/SRender.hpp>

namespace sight::modules::viz::ogre::adaptor
{

/**
 * @brief This adaptor binds a data::TransformationMatrix3D to an Ogre scene node.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service type="::sight::modules::viz::ogre::adaptor::STransform">
        <inout key="transform" uid="..." />
        <config layer="default" transform="meshTFAdaptor" />
   </service>
   @endcode
 *
 * @subsection In-Out In-Out:
 * - \b transform [data::TransformationMatrix3D]: Sight transform matrix.
 *
 * @subsection Configuration Configuration:
 * - \b layer (mandatory, string): Defines the transform's layer.
 * - \b transform (mandatory, string): Name of the Ogre Transform.
 * - \b parent (optional, string, default=""): Name of the parent Ogre Transform you want to attach to.
 */
class MODULE_VIZ_OGRE_CLASS_API STransform final :
    public sight::viz::ogre::IAdaptor,
    public sight::viz::ogre::ITransformable
{

public:

    /// Generates default methods as New, dynamicCast, ...
    fwCoreServiceMacro(STransform, sight::viz::ogre::IAdaptor)

    /// Creates the service.
    MODULE_VIZ_OGRE_API STransform() noexcept;

    /// Destroys the adaptor.
    MODULE_VIZ_OGRE_API ~STransform() noexcept override;

protected:

    /// Retrieves id sets in the configurations.
    MODULE_VIZ_OGRE_API void configuring() override;

    /// Creates the ::Ogre::SceneNode corresonding to the associated transform matrix.
    MODULE_VIZ_OGRE_API void starting() override;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect data::Object::s_MODIFIED_SIG of s_TRANSFORM_INOUT to s_UPDATE_SLOT
     */
    MODULE_VIZ_OGRE_API services::IService::KeyConnectionsMap getAutoConnections() const override;

    /// Updates m_transformNode from the data::TransformationMatrix3D.
    MODULE_VIZ_OGRE_API void updating() override;

    /// Does nothing.
    MODULE_VIZ_OGRE_API void stopping() override;

private:

    /// Defines the Parent transform identifier.
    sight::viz::ogre::SRender::OgreObjectIdType m_parentTransformId;

    /// Contains the Ogre transform node.
    ::Ogre::SceneNode* m_transformNode { nullptr };

    /// Contains the Ogre parent transform sceneNode.
    ::Ogre::SceneNode* m_parentTransformNode { nullptr };

    /// Defines the Ogre transformation of this service
    ::Ogre::Affine3 m_ogreTransform;

};

} // namespace sight::modules::viz::ogre::adaptor.
