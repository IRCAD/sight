/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
     * @brief Recover the node matching m_transformId.
     * @param _rootNode The root node of the scene.
     * @return The node matching m_transformId.
     */
    FWRENDEROGRE_API ::Ogre::SceneNode* getTransformNode(::Ogre::SceneNode* _rootNode) const;

    /**
     * @brief Get the parent transform Id.
     * @return This parent transform service UID.
     */
    FWRENDEROGRE_API ::fwRenderOgre::SRender::OgreObjectIdType getParentTransformId() const;

    /**
     * @brief Set the parent transform Id.
     * @param _id This new parent transform service UID
     */
    FWRENDEROGRE_API void setParentTransformId(::fwRenderOgre::SRender::OgreObjectIdType _id);

    /**
     * @brief s_CONFIG_TRANSFORM The key used in xml configuration.
     */
    FWRENDEROGRE_API static const std::string s_CONFIG_TRANSFORM;

private:

    /// Ogre transform node.
    mutable ::Ogre::SceneNode* m_transformNode;

    /// This transform identifier
    ::fwRenderOgre::SRender::OgreObjectIdType m_transformId;

protected:

    /// Attached transform service
    ::fwRenderOgre::IAdaptor::wptr m_transformService;

    /// This parent transform identifier
    ::fwRenderOgre::SRender::OgreObjectIdType m_parentTransformId;
};

} //namespace fwRenderOgre
