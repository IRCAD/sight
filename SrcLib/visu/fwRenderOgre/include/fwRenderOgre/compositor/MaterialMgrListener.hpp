/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWRENDEROGRE_COMPOSITOR_MATERIALMGRLISTENER_HPP__
#define __FWRENDEROGRE_COMPOSITOR_MATERIALMGRLISTENER_HPP__

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
    /// Helper function to copy the given technique and return the first pass
    ::Ogre::Technique* copyTechnique(::Ogre::Technique* _tech, const ::Ogre::String &_schemeName,
                                     ::Ogre::Material *_originalMaterial);

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

#endif // __FWRENDEROGRE_COMPOSITOR_MATERIALMGRLISTENER_HPP__
