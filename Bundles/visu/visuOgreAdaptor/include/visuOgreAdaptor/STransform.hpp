/************************************************************************
 *
 * Copyright (C) 2014-2019 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
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

#include <fwData/TransformationMatrix3D.hpp>

#include <fwRenderOgre/IAdaptor.hpp>
#include <fwRenderOgre/ITransformable.hpp>
#include <fwRenderOgre/SRender.hpp>

#include <OGRE/OgreMatrix4.h>
#include <OGRE/OgreMovableObject.h>
#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreSceneNode.h>

#include <vector>

namespace visuOgreAdaptor
{

/**
 * @brief   This adaptor binds a ::fwData::TransformationMatrix3D to an Ogre scene node.

 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="::visuOgreAdaptor::STransform">
            <inout key="transform" uid="..." />
            <config layer="default" transform="meshTFAdaptor" />
       </service>
   @endcode
 * @subsection In-Out In-Out:
 * - \b transform [::fwData::TransformationMatrix3D]: Sight transform matrix. The matrix will be updated as well if the
 * scene node is modified from Ogre.
 * @subsection Configuration Configuration:
 * - \b layer (mandatory): defines the transform's layer
 * - \b transform: name of the Ogre Transform.
 * - \b parent: name of the parent Ogre Transform you want to attach to.
 */
class VISUOGREADAPTOR_CLASS_API STransform : public ::fwRenderOgre::IAdaptor,
                                             public ::fwRenderOgre::ITransformable
{

public:

    fwCoreServiceMacro(STransform, ::fwRenderOgre::IAdaptor);

    /// Constructor,
    VISUOGREADAPTOR_API STransform() noexcept;

    /// Destructor, does nothing
    VISUOGREADAPTOR_API virtual ~STransform() noexcept;

    /// Connect ::fwData::Object::s_MODIFIED_SIG of the transform data to s_UPDATE_SLOT
    VISUOGREADAPTOR_API ::fwServices::IService::KeyConnectionsMap getAutoConnections() const override;

    /// Ogre transform sceneNode getter.
    VISUOGREADAPTOR_API ::Ogre::SceneNode* getSceneNode() const;

    /// Copies the transformation matrix from Ogre to Sight
    VISUOGREADAPTOR_API void updateFromOgre();

protected:

    /// Takes the attribute "parent" from m_config, and then puts it in m_parentTransformUID
    VISUOGREADAPTOR_API void configuring() override;

    /// Creates the ::Ogre::SceneNode corresonding to the associated transform matrix.
    VISUOGREADAPTOR_API void starting() override;

    /// Unregisters the service
    VISUOGREADAPTOR_API void stopping() override;

    /// Updates m_transform and m_ogreTransformNode from ::fwData::TransformationMatrix3D
    VISUOGREADAPTOR_API void updating() override;

    /// The Parent transform identifier
    ::fwRenderOgre::SRender::OgreObjectIdType m_parentTransformId {""};

    /// Ogre transform node.
    ::Ogre::SceneNode* m_transformNode {nullptr};

    /// Ogre parent Transform sceneNode
    ::Ogre::SceneNode* m_parentTransformNode {nullptr};

    /// Ogre transformation of this service
    ::Ogre::Affine3 m_ogreTransform;

};

} //namespace visuOgreAdaptor
