/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRenderOgre/R2VBRenderable.hpp"
#include "fwRenderOgre/factory/R2VBRenderable.hpp"

#include <OGRE/OgreRenderQueue.h>

namespace fwRenderOgre
{

//-----------------------------------------------------------------------------

fwRenderOgre::R2VBRenderable::R2VBRenderable(const ::Ogre::String& _name) :
    SimpleRenderable(_name),
    m_dirty(false)
{
}

//-----------------------------------------------------------------------------

const ::Ogre::String& R2VBRenderable::getMovableType(void) const
{
    return factory::R2VBRenderable::FACTORY_TYPE_NAME;
}

//-----------------------------------------------------------------------------

void R2VBRenderable::_updateRenderQueue(::Ogre::RenderQueue* _queue)
{
    // Don't do anything if the object is not visible
    if(m_srcObject->getParent()->isVisible())
    {
        // Update the ouput vertex buffer only if the dirty flag is set
        if(m_dirty)
        {
            m_r2vbBuffer->update(mParentSceneManager);
            m_dirty = false;
        }
        // Add the output vertex buffer in the render queue
        _queue->addRenderable(this);
    }
}

//-----------------------------------------------------------------------------

void R2VBRenderable::getRenderOperation(::Ogre::RenderOperation& _op)
{
    m_r2vbBuffer->getRenderOperation(_op);
}

//-----------------------------------------------------------------------------

void R2VBRenderable::setSourceObject(::Ogre::SubEntity *_sourceObject)
{
    m_srcObject         = _sourceObject;
    mParentSceneManager = _sourceObject->getParent()->_getManager();
    if (!m_r2vbBuffer.isNull())
    {
        m_r2vbBuffer->setSourceRenderable(_sourceObject);
    }
}

//-----------------------------------------------------------------------------

} // namespace fwRenderOgre
