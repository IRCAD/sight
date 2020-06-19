/************************************************************************
 *
 * Copyright (C) 2018 IRCAD France
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

#include "fwRenderOgre/helper/Technique.hpp"

#include <fwCore/spyLog.hpp>

namespace fwRenderOgre
{

namespace helper
{

//------------------------------------------------------------------------------

::Ogre::Technique* Technique::copyToMaterial(const Ogre::Technique* _tech, const std::string& _schemeName,
                                             Ogre::Material* _mtl)
{
    ::Ogre::Technique* newTech = _mtl->createTechnique();
    *newTech                   = *_tech;
    newTech->setName(_schemeName);
    newTech->setSchemeName(_schemeName);

    SLM_ASSERT("Empty pass", newTech->getPass(0));

    return newTech;
}

} // fwRenderOgre
} // helper
