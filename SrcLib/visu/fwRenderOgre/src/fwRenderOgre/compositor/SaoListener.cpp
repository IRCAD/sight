/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRenderOgre/compositor/SaoListener.hpp"

#include "OGRE/OgreCamera.h"

#include <OGRE/OgreCompositorChain.h>
#include <OGRE/OgreCompositorManager.h>
#include <OGRE/OgreHardwarePixelBuffer.h>
#include <OGRE/OgreMatrix4.h>
#include <OGRE/OgrePass.h>
#include <OGRE/OgreTechnique.h>
#include <OGRE/OgreTexture.h>

namespace fwRenderOgre
{

namespace compositor
{

//-----------------------------------------------------------------------------

void SaoListener::notifyMaterialRender(::Ogre::uint32 pass_id, ::Ogre::MaterialPtr& mat)
{
    // change the sao arguments
    auto fragmentParams = mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();

    // try to go here only when the AO_mat is called
    if (pass_id == 1)
    {
        ::Ogre::CompositorChain* compChain =
            ::Ogre::CompositorManager::getSingletonPtr()->getCompositorChain(m_viewport);

        ::Ogre::CompositorInstance* saoCompositor = compChain->getCompositor("SAO");
        ::Ogre::TexturePtr mip0, mip1, mip2, mip3, mip4, mip5, mip6, mip7, mip8, rt0;

        mip0 = saoCompositor->getTextureInstance("mip0", 0);
        mip1 = saoCompositor->getTextureInstance("mip1", 0);
        mip2 = saoCompositor->getTextureInstance("mip2", 0);
        mip3 = saoCompositor->getTextureInstance("mip3", 0);
        mip4 = saoCompositor->getTextureInstance("mip4", 0);
        mip5 = saoCompositor->getTextureInstance("mip5", 0);
        mip6 = saoCompositor->getTextureInstance("mip6", 0);
        mip7 = saoCompositor->getTextureInstance("mip7", 0);
        mip8 = saoCompositor->getTextureInstance("mip8", 0);
        rt0  = saoCompositor->getTextureInstance("rt0", 0);

        // ---------------------------------------------------
        //  Copy the content of mip0,mip1... in rt0
        // ---------------------------------------------------
        rt0->freeInternalResources();

        rt0->changeGroupOwnership(mip0.get()->getGroup());
        rt0->setWidth(mip0.get()->getWidth());
        rt0->setHeight(mip0.get()->getHeight());
        rt0->setNumMipmaps(8);
        rt0->setFormat(::Ogre::PixelFormat::PF_FLOAT32_R);
        rt0->setUsage(::Ogre::TU_DYNAMIC_WRITE_ONLY_DISCARDABLE);
        rt0->setTextureType(::Ogre::TextureType::TEX_TYPE_2D);

        rt0->createInternalResources();

        // copy the content of the mip textures in the mipmap (rt0)
        // TODO: try to automate with a loop
        rt0->getBuffer(0, 0)->blit(mip0.get()->getBuffer());
        rt0->getBuffer(0, 1)->blit(mip1.get()->getBuffer());
        rt0->getBuffer(0, 2)->blit(mip2.get()->getBuffer());
        rt0->getBuffer(0, 3)->blit(mip3.get()->getBuffer());
        rt0->getBuffer(0, 4)->blit(mip4.get()->getBuffer());
        rt0->getBuffer(0, 5)->blit(mip5.get()->getBuffer());
        rt0->getBuffer(0, 6)->blit(mip6.get()->getBuffer());
        rt0->getBuffer(0, 7)->blit(mip7.get()->getBuffer());
        rt0->getBuffer(0, 8)->blit(mip8.get()->getBuffer());

        fragmentParams->setNamedConstant("u_radius", static_cast<float>(0.85));

        fragmentParams->setNamedConstant("u_numSamples", 11);

        const ::Ogre::Matrix4& proj = m_viewport->getCamera()->getProjectionMatrix();

        const ::Ogre::Vector4 projInfo( -2.f / (static_cast<float>(mip0.get()->getWidth()) * proj[0][0]),
                                        -2.f / (static_cast<float>(mip0.get()->getHeight()) * proj[1][1]),
                                        (1.f - proj[0][2]) / proj[0][0],
                                        (1.f + proj[1][2]) / proj[1][1]);
        fragmentParams->setNamedConstant("eu_projInfo", projInfo);
    }

    if (pass_id >= 41)
    {
        ::Ogre::CompositorChain* compChain =
            ::Ogre::CompositorManager::getSingletonPtr()->getCompositorChain(m_viewport);

        ::Ogre::CompositorInstance* Sao_compositor = compChain->getCompositor("SAO");

        ::Ogre::TexturePtr prevMip = Sao_compositor->getTextureInstance("mip" + std::to_string(pass_id - 41), 0);

        fragmentParams->setNamedConstant("eu_vpWidth", static_cast<float>(prevMip.get()->getWidth()));
        fragmentParams->setNamedConstant("eu_vpHeight", static_cast<float>(prevMip.get()->getHeight()));
    }

    if (pass_id == 4)
    {
        // Change the Blend State

        // setNamedConstant doesn't work with bool value
        int state = 1;

        // change the blend state
        fragmentParams->setNamedConstant("u_blend", state);

        // Change the AO Intensity value
        fragmentParams->setNamedConstant("aoIntensity", static_cast<float>(1.0));
    }
}

//-----------------------------------------------------------------------------

} // namespace compositor

}// namespace fwRenderOgre
