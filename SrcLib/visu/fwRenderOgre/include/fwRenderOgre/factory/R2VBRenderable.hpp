/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWRENDEROGRE_FACTORY_R2VBRENDERABLE_HPP__
#define __FWRENDEROGRE_FACTORY_R2VBRENDERABLE_HPP__

//-----------------------------------------------------------------------------

#include "fwRenderOgre/config.hpp"

#include <OGRE/OgreManualObject.h>

namespace fwRenderOgre
{

namespace factory
{

/**
 * @brief Factory used to create a R2VBRenderable.
 */
class FWRENDEROGRE_CLASS_API R2VBRenderable : public ::Ogre::MovableObjectFactory
{
public:
    FWRENDEROGRE_API R2VBRenderable()
    {
    }
    FWRENDEROGRE_API ~R2VBRenderable()
    {
    }

    FWRENDEROGRE_API static ::Ogre::String FACTORY_TYPE_NAME;

    FWRENDEROGRE_API virtual const ::Ogre::String& getType(void) const override;
    FWRENDEROGRE_API virtual void destroyInstance( ::Ogre::MovableObject* obj) override;

protected:
    FWRENDEROGRE_API ::Ogre::MovableObject* createInstanceImpl(const ::Ogre::String& name,
                                                               const ::Ogre::NameValuePairList* params) override;
};

} // namespace factory

} // namespace fwRenderOgre

#endif // __FWRENDEROGRE_FACTORY_R2VBRENDERABLE_HPP__
