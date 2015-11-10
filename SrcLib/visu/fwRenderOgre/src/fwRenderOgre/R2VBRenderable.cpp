/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRenderOgre/R2VBRenderable.hpp"

#include <OGRE/OgreRenderQueue.h>

namespace fwRenderOgre
{
//-----------------------------------------------------------------------------

const ::Ogre::String& R2VBRenderable::getMovableType(void) const
{
    return R2VBRenderableFactory::FACTORY_TYPE_NAME;
}

//-----------------------------------------------------------------------------

void R2VBRenderable::_updateRenderQueue(::Ogre::RenderQueue* queue)
{
    if(m_srcObject->getParent()->isVisible())
    {
        mR2vbObject->update(mParentSceneManager);
        queue->addRenderable(this);
    }
}

//-----------------------------------------------------------------------------

void R2VBRenderable::getRenderOperation(::Ogre::RenderOperation& op)
{
    mR2vbObject->getRenderOperation(op);
}

//-----------------------------------------------------------------------------

void R2VBRenderable::setSourceObject(::Ogre::SubEntity *_sourceObject)
{
    m_srcObject         = _sourceObject;
    mParentSceneManager = _sourceObject->getParent()->_getManager();
    if (!mR2vbObject.isNull())
    {
        mR2vbObject->setSourceRenderable(_sourceObject);
    }
}

//-----------------------------------------------------------------------------

::Ogre::String R2VBRenderableFactory::FACTORY_TYPE_NAME = "R2VBRenderable";

//-----------------------------------------------------------------------------

const ::Ogre::String& R2VBRenderableFactory::getType(void) const
{
    return FACTORY_TYPE_NAME;
}

//-----------------------------------------------------------------------------

::Ogre::MovableObject* R2VBRenderableFactory::createInstanceImpl(
    const ::Ogre::String& name, const ::Ogre::NameValuePairList* params)
{
    return new R2VBRenderable();
}

//-----------------------------------------------------------------------------

void R2VBRenderableFactory::destroyInstance( ::Ogre::MovableObject* obj)
{
    delete obj;
}

//-----------------------------------------------------------------------------

} // namespace fwRenderOgre
