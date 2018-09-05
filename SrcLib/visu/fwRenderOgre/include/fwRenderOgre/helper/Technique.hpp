/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "fwRenderOgre/config.hpp"

#include <OGRE/OgreMaterial.h>
#include <OGRE/OgreTechnique.h>

namespace fwRenderOgre
{

namespace helper
{

class Technique
{
public:

    /**
     * @brief copies a technique to an existing material
     *
     * @param[in] _tech technique to copy
     * @param[in] _schemeName new technique's scheme
     * @param[inout] _mtl material to which the technique is added
     * @return the newly created technique
     */
    static FWRENDEROGRE_API Ogre::Technique* copyToMaterial(const ::Ogre::Technique* _tech,
                                                            const std::string& _schemeName,
                                                            Ogre::Material* _mtl);

};

} // fwRenderOgre
} // helper
