/************************************************************************
 *
 * Copyright (C) 2018-2021 IRCAD France
 * Copyright (C) 2018-2021 IHU Strasbourg
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

#pragma once

#include "viz/ogre/config.hpp"

#include <OGRE/OgreMovableObject.h>

namespace sight::viz::ogre
{

namespace factory
{

class VIZ_OGRE_CLASS_API Text : public ::Ogre::MovableObjectFactory
{
public:

    /// Constructor.
    VIZ_OGRE_API Text()
    {
    }

    /// Destructor.
    VIZ_OGRE_API ~Text()
    {
    }

    /// Type of '::Ogre::MovableObject' produced by this factory. 'Text' in this case.
    VIZ_OGRE_API static const ::Ogre::String FACTORY_TYPE_NAME;

    /// Get the produced object's type. Always returns 'Text'.
    VIZ_OGRE_API virtual const ::Ogre::String& getType(void) const override;
#if OGRE_VERSION_PATCH < 9
    VIZ_OGRE_API virtual void destroyInstance( ::Ogre::MovableObject* obj) override;
#endif

protected:

    /// Create a 'Text' instance with the given name and parameters.
    VIZ_OGRE_API ::Ogre::MovableObject* createInstanceImpl(const ::Ogre::String& name,
                                                           const ::Ogre::NameValuePairList* params) override;

};

} // namespace factory

} // namespace sight::viz::ogre
