/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWRENDEROGRE_R2VBRENDERABLE_HPP__
#define __FWRENDEROGRE_R2VBRENDERABLE_HPP__

#include "fwRenderOgre/config.hpp"

#include <OGRE/OgreEntity.h>
#include <OGRE/OgreManualObject.h>
#include <OGRE/OgreSimpleRenderable.h>
#include <OGRE/OgreSubEntity.h>
#include <OGRE/OgreRenderToVertexBuffer.h>


namespace fwRenderOgre
{

/**
 * @class R2VBRenderable
 * @brief This object is used to implement a render-to-vertex-buffer (r2vb) process (GL_TRANSFORM_FEEDBACK).
 *
 * This objects holds a reference to the object used as input for the render-to-vertex-buffer process.
 * It also contains the output vertex buffer, that is used to be displayed like a "regular" object.
 */
class FWRENDEROGRE_CLASS_API R2VBRenderable : public ::Ogre::SimpleRenderable
{
public:
    FWRENDEROGRE_API R2VBRenderable(const ::Ogre::String& name);
    virtual FWRENDEROGRE_API ~R2VBRenderable()
    {
        m_srcObject = nullptr;
    }

    void setBuffer(::Ogre::RenderToVertexBufferSharedPtr _r2vbObject);
    ::Ogre::RenderToVertexBufferSharedPtr getBuffer();

    FWRENDEROGRE_API void setSourceObject(::Ogre::SubEntity* _sourceObject);
    ::Ogre::SubEntity* getSourceObject() const;

    /** @copydoc SimpleRenderable::_updateRenderQueue. */
    FWRENDEROGRE_API void _updateRenderQueue(::Ogre::RenderQueue* _queue);

    /** @copydoc SimpleRenderable::getMovableType. */
    FWRENDEROGRE_API const ::Ogre::String& getMovableType(void) const;

    /** @copydoc SimpleRenderable::getRenderOperation. */
    FWRENDEROGRE_API void getRenderOperation(::Ogre::RenderOperation& _op);

    /// Delegate to the subentity.
    ::Ogre::Real getBoundingRadius(void) const;

    /// @copydoc Renderable::getSquaredViewDepth
    ::Ogre::Real getSquaredViewDepth(const Ogre::Camera* _cam) const;
    /// Mark the output verex buffer as dirty, the r2vb process will be run on next update
    void setDirty();

protected:
    /// Source object of the r2vb process
    ::Ogre::SubEntity* m_srcObject;

    /// Buffer used as output
    ::Ogre::RenderToVertexBufferSharedPtr m_r2vbBuffer;

    /// Tells if the r2vb must be run on next update - typically we want this to be done only once per frame.
    /// Thus we use this flag, depending on the technique to enable the r2vb only on the first rendering pass.
    bool m_dirty;
};

//-----------------------------------------------------------------------------
// Inline functions

//-----------------------------------------------------------------------------

inline void R2VBRenderable::setDirty()
{
    m_dirty = true;
}

//-----------------------------------------------------------------------------

inline void R2VBRenderable::setBuffer(Ogre::RenderToVertexBufferSharedPtr _r2vbObject)
{
    m_r2vbBuffer = _r2vbObject;
}

//-----------------------------------------------------------------------------

inline ::Ogre::RenderToVertexBufferSharedPtr R2VBRenderable::getBuffer()
{
    return m_r2vbBuffer;
}

//-----------------------------------------------------------------------------

inline ::Ogre::SubEntity *R2VBRenderable::getSourceObject() const
{
    return m_srcObject;
}

//-----------------------------------------------------------------------------

inline Ogre::Real R2VBRenderable::getBoundingRadius() const
{
    return m_srcObject->getParent()->getBoundingRadius();
}

//-----------------------------------------------------------------------------

inline Ogre::Real R2VBRenderable::getSquaredViewDepth(const Ogre::Camera *_cam) const
{
    return m_srcObject->getSquaredViewDepth(_cam);
}

//-----------------------------------------------------------------------------


} // namespace fwRenderOgre

#endif // __FWRENDEROGRE_R2VBRENDERABLE_HPP__
