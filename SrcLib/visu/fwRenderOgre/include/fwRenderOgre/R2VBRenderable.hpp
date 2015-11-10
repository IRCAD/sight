/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWRENDEROGRE_R2VBRENDERABLE_HPP__
#define __FWRENDEROGRE_R2VBRENDERABLE_HPP__

#include <OGRE/OgreEntity.h>
#include <OGRE/OgreManualObject.h>
#include <OGRE/OgreSimpleRenderable.h>
#include <OGRE/OgreSubEntity.h>
#include <OGRE/OgreRenderToVertexBuffer.h>

#include "fwRenderOgre/config.hpp"

namespace fwRenderOgre
{

class FWRENDEROGRE_CLASS_API R2VBRenderable : public ::Ogre::SimpleRenderable
{
public:
    FWRENDEROGRE_API R2VBRenderable()
    {
    }
    virtual FWRENDEROGRE_API ~R2VBRenderable()
    {
    }

    FWRENDEROGRE_API void setBuffer(::Ogre::RenderToVertexBufferSharedPtr r2vbObject)
    {
        mR2vbObject = r2vbObject;
    }
    FWRENDEROGRE_API ::Ogre::RenderToVertexBufferSharedPtr getBuffer()
    {
        return mR2vbObject;
    }

    void setSourceObject(::Ogre::SubEntity* _sourceObject);
    ::Ogre::SubEntity* getSourceObject() const { return m_srcObject; }

    /** @copydoc SimpleRenderable::_updateRenderQueue. */
    FWRENDEROGRE_API void _updateRenderQueue(::Ogre::RenderQueue* queue);
    /** @copydoc SimpleRenderable::getMovableType. */
    FWRENDEROGRE_API const ::Ogre::String& getMovableType(void) const;
    /** @copydoc SimpleRenderable::getRenderOperation. */
    FWRENDEROGRE_API void getRenderOperation(::Ogre::RenderOperation& op);

    /// Delegate to the subentity.
    ::Ogre::Real getBoundingRadius(void) const
    {
        return m_srcObject->getParent()->getBoundingRadius();
    }
    ::Ogre::Real getSquaredViewDepth(const Ogre::Camera* cam) const
    {
        return m_srcObject->getSquaredViewDepth(cam);
    }

protected:
    ::Ogre::SubEntity* m_srcObject;
    ::Ogre::RenderToVertexBufferSharedPtr mR2vbObject;
};

class FWRENDEROGRE_CLASS_API R2VBRenderableFactory : public ::Ogre::MovableObjectFactory
{
public:
    FWRENDEROGRE_API R2VBRenderableFactory()
    {
    }
    FWRENDEROGRE_API ~R2VBRenderableFactory()
    {
    }

    FWRENDEROGRE_API static ::Ogre::String FACTORY_TYPE_NAME;

    FWRENDEROGRE_API const ::Ogre::String& getType(void) const;
    FWRENDEROGRE_API void destroyInstance( ::Ogre::MovableObject* obj);

protected:
    FWRENDEROGRE_API ::Ogre::MovableObject* createInstanceImpl(const ::Ogre::String& name,
                                                               const ::Ogre::NameValuePairList* params);
};

} // namespace fwRenderOgre

#endif // __FWRENDEROGRE_R2VBRENDERABLE_HPP__
