/************************************************************************
 *
 * Copyright (C) 2014-2018 IRCAD France
 * Copyright (C) 2014-2018 IHU Strasbourg
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

#include "fwRenderOgre/config.hpp"
#include "fwRenderOgre/IAdaptor.hpp"
#include "fwRenderOgre/SRender.hpp"

#include <OgreMatrix4.h>

namespace fwRenderOgre
{

/**
 * @brief Must be inherited by all classes containing a transformation matrix
 */
class FWRENDEROGRE_CLASS_API ITransformable
{
public:

    /**
     * @brief Get the Ogre identifier of the transform in the scene manager.
     * @return This transform service Id.
     */
    FWRENDEROGRE_API ::fwRenderOgre::SRender::OgreObjectIdType getTransformId() const;

    /**
     * @brief Set the Ogre identifier of the transform in the scene manager.
     * @param _id The new transform service Id.
     */
    FWRENDEROGRE_API void setTransformId(::fwRenderOgre::SRender::OgreObjectIdType _id);

    /**
     * @brief Retrieve the node matching m_transformId.
     * @param _parentNode The parent node of the created node.
     * @return The node matching m_transformId.
     */
    FWRENDEROGRE_API ::Ogre::SceneNode* getTransformNode(::Ogre::SceneNode* _parentNode);

    /**
     * @brief Retrieve the node matching the given node name.
     * @param _name The name of the node.
     * @param _parentNode The parent node of the created node.
     * @return The node matching m_transformId.
     */
    FWRENDEROGRE_API static ::Ogre::SceneNode* getTransformNode(const std::string& _name,
                                                                ::Ogre::SceneNode* _parentNode);

    /**
     * @brief s_TRANSFORM_CONFIG The key used in xml configuration.
     */
    FWRENDEROGRE_API static const std::string s_TRANSFORM_CONFIG;

private:

    /// Ogre transform node.
    ::Ogre::SceneNode* m_transformNode;

    /// This transform identifier
    ::fwRenderOgre::SRender::OgreObjectIdType m_transformId;
};

} //namespace fwRenderOgre
