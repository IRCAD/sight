/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
