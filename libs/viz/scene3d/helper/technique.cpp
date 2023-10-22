/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
 * Copyright (C) 2018 IHU Strasbourg
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

#include "viz/scene3d/helper/technique.hpp"

#include <core/spy_log.hpp>

namespace sight::viz::scene3d::helper
{

//------------------------------------------------------------------------------

Ogre::Technique* technique::copyToMaterial(
    const Ogre::Technique* _tech,
    const std::string& _scheme_name,
    Ogre::Material* _mtl
)
{
    Ogre::Technique* new_tech = _mtl->createTechnique();
    *new_tech = *_tech;
    new_tech->setName(_scheme_name);
    new_tech->setSchemeName(_scheme_name);

    SIGHT_ASSERT("Empty pass", new_tech->getPass(0));

    return new_tech;
}

} // namespace sight::viz::scene3d::helper
