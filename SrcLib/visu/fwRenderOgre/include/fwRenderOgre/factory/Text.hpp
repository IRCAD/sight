/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "fwRenderOgre/config.hpp"

#include <OGRE/OgreMovableObject.h>

namespace fwRenderOgre
{

namespace factory
{

class FWRENDEROGRE_CLASS_API Text : public ::Ogre::MovableObjectFactory
{
public:

    /// Constructor.
    FWRENDEROGRE_API Text()
    {
    }

    /// Destructor.
    FWRENDEROGRE_API ~Text()
    {
    }

    /// Type of '::Ogre::MovableObject' produced by this factory. 'Text' in this case.
    FWRENDEROGRE_API static const ::Ogre::String FACTORY_TYPE_NAME;

    /// Get the produced object's type. Always returns 'Text'.
    FWRENDEROGRE_API virtual const ::Ogre::String& getType(void) const override;

    /// Destroys a 'Text' object instance.
    FWRENDEROGRE_API virtual void destroyInstance( ::Ogre::MovableObject* obj) override;

protected:

    /// Create a 'Text' instance with the given name and parameters.
    FWRENDEROGRE_API ::Ogre::MovableObject* createInstanceImpl(const ::Ogre::String& name,
                                                               const ::Ogre::NameValuePairList* params) override;

};

} // namespace factory

} // namespace fwRenderOgre
