/************************************************************************
 *
 * Copyright (C) 2014-2017 IRCAD France
 * Copyright (C) 2014-2017 IHU Strasbourg
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
