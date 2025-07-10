/************************************************************************
 *
 * Copyright (C) 2014-2024 IRCAD France
 * Copyright (C) 2014-2021 IHU Strasbourg
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

//-----------------------------------------------------------------------------

#pragma once

#include <sight/viz/scene3d/config.hpp>

#include <OGRE/OgreManualObject.h>

namespace sight::viz::scene3d::factory
{

/**
 * @brief factory used to create a r2vb_renderable.
 */
class SIGHT_VIZ_SCENE3D_CLASS_API r2vb_renderable : public Ogre::MovableObjectFactory
{
public:

    SIGHT_VIZ_SCENE3D_API r2vb_renderable()
    = default;

    SIGHT_VIZ_SCENE3D_API ~r2vb_renderable() override
    = default;

    SIGHT_VIZ_SCENE3D_API static Ogre::String s_factory_type_name;

    [[nodiscard]] SIGHT_VIZ_SCENE3D_API const Ogre::String& getType() const override;
#if OGRE_VERSION_PATCH < 9
    SIGHT_VIZ_SCENE3D_API void destroyInstance(Ogre::MovableObject* obj) override;
#endif

protected:

    SIGHT_VIZ_SCENE3D_API Ogre::MovableObject* createInstanceImpl(
        const Ogre::String& _name,
        const Ogre::NameValuePairList* _params
    ) override;
};

} // namespace sight::viz::scene3d::factory
