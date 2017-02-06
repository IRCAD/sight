/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWRENDEROGRE_ITRANSFORMABLE_HPP__
#define __FWRENDEROGRE_ITRANSFORMABLE_HPP__

#include "fwRenderOgre/config.hpp"
#include <fwRenderOgre/IAdaptor.hpp>
#include <fwRenderOgre/SRender.hpp>

#include <OgreMatrix4.h>

namespace fwRenderOgre
{

/**
 * @brief Must be inherited by all class containing a transformation matrix
 */
class FWRENDEROGRE_CLASS_API ITransformable
{
public:
    /**
     * @brief Get the Ogre identifier of the transform in the scene manager.
     * @return This transform service Id
     */
    FWRENDEROGRE_API ::fwRenderOgre::SRender::OgreObjectIdType getTransformId() const;

    /**
     * @brief Set the Ogre identifier of the transform in the scene manager.
     * @param newUID The new transform service Id
     */
    FWRENDEROGRE_API virtual void setTransformId(::fwRenderOgre::SRender::OgreObjectIdType newId);

    /**
     * @brief getParentTransformUID
     * @return This parent transform service UID
     */
    FWRENDEROGRE_API ::fwRenderOgre::SRender::OgreObjectIdType getParentTransformId() const;

    /**
     * @brief setParentTransformId
     * @param newUID This new parent transform service UID
     */
    FWRENDEROGRE_API virtual void setParentTransformId(::fwRenderOgre::SRender::OgreObjectIdType newId);

protected:
    /// Ogre transformation of this service
    ::Ogre::Matrix4 m_ogreTransform;
    /// Attached transform service
    ::fwRenderOgre::IAdaptor::wptr m_transformService;

    /// This transform identifier
    ::fwRenderOgre::SRender::OgreObjectIdType m_transformId;
    /// This parent transform identifier
    ::fwRenderOgre::SRender::OgreObjectIdType m_parentTransformId;
};

} //namespace fwRenderOgre

#endif // __FWRENDEROGRE_ITRANSFORMABLE_HPP__
