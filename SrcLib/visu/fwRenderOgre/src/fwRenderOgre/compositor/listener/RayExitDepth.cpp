/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRenderOgre/compositor/listener/RayExitDepth.hpp"

#include <fwCore/spyLog.hpp>

#include <OGRE/OgrePass.h>
#include <OGRE/OgreTechnique.h>

namespace fwRenderOgre
{
namespace compositor
{
namespace listener
{

//-----------------------------------------------------------------------------

RayExitDepthListener::RayExitDepthListener()
{

}

//------------------------------------------------------------------------------

RayExitDepthListener::~RayExitDepthListener()
{
}

//------------------------------------------------------------------------------

::Ogre::Technique* RayExitDepthListener::handleSchemeNotFound(unsigned short /*_schemeIndex*/,
                                                              const ::Ogre::String& _schemeName,
                                                              ::Ogre::Material* _originalMaterial,
                                                              unsigned short /*_lodIndex*/,
                                                              const ::Ogre::Renderable* /*_renderable*/)
{
    ::Ogre::Technique* newTechnique = nullptr;
    const auto mtlName = _originalMaterial->getName();

    if(_schemeName == "VolumeEntries_FrontFacesMin" && !::Ogre::StringUtil::startsWith(mtlName, "RTV_Mat"))
    {
        auto entryPointsMtl   = ::Ogre::MaterialManager::getSingleton().getByName("RayEntryPoints");
        auto frontFaceMinTech = entryPointsMtl->getTechnique("FrontFacesMin");

        newTechnique = this->copyTechnique(frontFaceMinTech, _schemeName, _originalMaterial);
    }

    return newTechnique;
}

//------------------------------------------------------------------------------

Ogre::Technique* RayExitDepthListener::copyTechnique(const Ogre::Technique* _tech, const Ogre::String& _schemeName,
                                                     Ogre::Material* _originalMaterial)
{
    ::Ogre::Technique* newTech = _originalMaterial->createTechnique();
    *newTech                   = *_tech;
    newTech->setName(_schemeName);
    newTech->setSchemeName(_schemeName);

    SLM_ASSERT("Empty pass", newTech->getPass(0));

    return newTech;
}

//------------------------------------------------------------------------------

} // namespace listener

} // namespace compositor

} // namespace fwRenderOgre
