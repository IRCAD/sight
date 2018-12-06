/************************************************************************
 *
 * Copyright (C) 2014-2018 IRCAD France
 * Copyright (C) 2014-2018 IHU Strasbourg
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

#include "fwRenderOgre/config.hpp"

#include <OgreMaterialManager.h>

namespace fwRenderOgre
{

namespace compositor
{

/**
 * @brief Generates transparency techniques at runtime.
 */
class FWRENDEROGRE_CLASS_API MaterialMgrListener : public ::Ogre::MaterialManager::Listener
{
public:
    /// Destructor
    virtual ~MaterialMgrListener();

    /// Callback called each time a scheme is not found
    virtual ::Ogre::Technique* handleSchemeNotFound(unsigned short _schemeIndex, const ::Ogre::String& _schemeName,
                                                    ::Ogre::Material* _originalMaterial, unsigned short _lodIndex,
                                                    const ::Ogre::Renderable* _renderable);
private:

    /**
     * @brief Ensure that a given fragment program is created.
     *
     * @param[in] _name name of the program
     * @param[in] _algoName name of the algorithm
     * @param[in] _algoPassName new suffix to use
     * @param[in] _baseName new suffix to use
     */
    static ::Ogre::GpuProgramPtr ensureFPCreated(const std::string& _name,
                                                 const std::string& _algoName,
                                                 const std::string& _algoPassName,
                                                 const std::string& _baseName);

};

} // namespace compositor

} // namespace fwRenderOgre
