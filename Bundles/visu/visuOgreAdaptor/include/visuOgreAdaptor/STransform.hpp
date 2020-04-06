/************************************************************************
 *
 * Copyright (C) 2014-2020 IRCAD France
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

#include "visuOgreAdaptor/config.hpp"

#include <fwRenderOgre/IAdaptor.hpp>
#include <fwRenderOgre/ITransformable.hpp>
#include <fwRenderOgre/SRender.hpp>

#include <OGRE/OgreSceneNode.h>

namespace visuOgreAdaptor
{

/**
 * @brief This adaptor binds a ::fwData::TransformationMatrix3D to an Ogre scene node.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="::visuOgreAdaptor::STransform">
            <inout key="transform" uid="..." />
            <config layer="default" transform="meshTFAdaptor" />
       </service>
   @endcode
 *
 * @subsection In-Out In-Out:
 * - \b transform [::fwData::TransformationMatrix3D]: Sight transform matrix.
 *
 * @subsection Configuration Configuration:
 * - \b layer (mandatory, string): Defines the transform's layer.
 * - \b transform (mandatory, string): Name of the Ogre Transform.
 * - \b parent (optional, string, default="") : Name of the parent Ogre Transform you want to attach to.
 */
class VISUOGREADAPTOR_CLASS_API STransform final :
    public ::fwRenderOgre::IAdaptor,
    public ::fwRenderOgre::ITransformable
{

public:

    fwCoreServiceMacro(STransform, ::fwRenderOgre::IAdaptor)

    /// Creates the service.
    VISUOGREADAPTOR_API STransform() noexcept;

    /// Destroys the service.
    VISUOGREADAPTOR_API virtual ~STransform() noexcept;

private:

    /// Retrieves id sets in the configurations.
    virtual void configuring();

    /// Creates the ::Ogre::SceneNode corresonding to the associated transform matrix.
    virtual void starting();

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect ::fwData::Object::s_MODIFIED_SIG of s_TRANSFORM_INOUT to s_UPDATE_SLOT
     */
    virtual ::fwServices::IService::KeyConnectionsMap getAutoConnections() const;

    /// Updates m_transformNode from the ::fwData::TransformationMatrix3D.
    virtual void updating();

    /// Does nothing.
    virtual void stopping();

    /// Defines the Parent transform identifier.
    ::fwRenderOgre::SRender::OgreObjectIdType m_parentTransformId {""};

    /// Contains the Ogre transform node.
    ::Ogre::SceneNode* m_transformNode {nullptr};

    /// Contains the Ogre parent transform sceneNode.
    ::Ogre::SceneNode* m_parentTransformNode {nullptr};

    /// Defines the Ogre transformation of this service
    ::Ogre::Affine3 m_ogreTransform;

};

} // namespace visuOgreAdaptor.
