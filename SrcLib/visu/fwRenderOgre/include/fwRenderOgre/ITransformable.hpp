/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWRENDEROGRE_ITRANSFORMABLE_HPP__
#define __FWRENDEROGRE_ITRANSFORMABLE_HPP__

#include <fwRenderOgre/IAdaptor.hpp>
#include <fwRenderOgre/SRender.hpp>

#include <OgreMatrix4.h>

#include "fwRenderOgre/config.hpp"

namespace fwRenderOgre
{

/**
 * @brief The ITransformable class
 * Must be inherited by all class containing a transformation matrix
 */
class FWRENDEROGRE_CLASS_API ITransformable
{
public:
    /**
     * @brief getTransformUID
     * @return This transform service UID
     */
    FWRENDEROGRE_API ::fwRenderOgre::SRender::OgreObjectIdType getTransformUID() const;

    /**
     * @brief setTransformUID
     * @param newUID This new transform service UID
     */
    FWRENDEROGRE_API virtual void setTransformUID(::fwRenderOgre::SRender::OgreObjectIdType newUID);

    /**
     * @brief getParentTransformUID
     * @return This parent transform service UID
     */
    FWRENDEROGRE_API ::fwRenderOgre::SRender::OgreObjectIdType getParentTransformUID() const;

    /**
     * @brief setParentTransformUID
     * @param newUID This new parent transform service UID
     */
    FWRENDEROGRE_API virtual void setParentTransformUID(::fwRenderOgre::SRender::OgreObjectIdType newUID);

protected:
    /// Ogre transformation of this service
    ::Ogre::Matrix4 m_transform;
    /// Attached transform service
    ::fwRenderOgre::IAdaptor::wptr m_transformService;

    /// This transform UID
    ::fwRenderOgre::SRender::OgreObjectIdType m_transformUID;
    /// This parent transform UID
    ::fwRenderOgre::SRender::OgreObjectIdType m_parentTransformUID;
};

} //namespace fwRenderOgre

#endif // __FWRENDEROGRE_ITRANSFORMABLE_HPP__
