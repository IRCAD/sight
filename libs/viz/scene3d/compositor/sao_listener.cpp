/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
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

#include "viz/scene3d/compositor/sao_listener.hpp"

#include <OGRE/OgreCamera.h>
#include <OGRE/OgreCompositorChain.h>
#include <OGRE/OgreCompositorManager.h>
#include <OGRE/OgreHardwarePixelBuffer.h>
#include <OGRE/OgreMatrix4.h>
#include <OGRE/OgrePass.h>
#include <OGRE/OgreTechnique.h>
#include <OGRE/OgreTexture.h>

namespace sight::viz::scene3d::compositor
{

//-----------------------------------------------------------------------------

void sao_listener::notifyMaterialRender(Ogre::uint32 _pass_id, Ogre::MaterialPtr& _mat)
{
    // change the sao arguments
    auto fragment_params = _mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();

    // try to go here only when the AO_mat is called
    if(_pass_id == 1)
    {
        Ogre::CompositorChain* comp_chain =
            Ogre::CompositorManager::getSingletonPtr()->getCompositorChain(m_viewport);

        Ogre::CompositorInstance* sao_compositor = comp_chain->getCompositor("SAO");
        Ogre::TexturePtr mip0;
        Ogre::TexturePtr mip1;
        Ogre::TexturePtr mip2;
        Ogre::TexturePtr mip3;
        Ogre::TexturePtr mip4;
        Ogre::TexturePtr mip5;
        Ogre::TexturePtr mip6;
        Ogre::TexturePtr mip7;
        Ogre::TexturePtr mip8;
        Ogre::TexturePtr rt0;

        mip0 = sao_compositor->getTextureInstance("mip0", 0);
        mip1 = sao_compositor->getTextureInstance("mip1", 0);
        mip2 = sao_compositor->getTextureInstance("mip2", 0);
        mip3 = sao_compositor->getTextureInstance("mip3", 0);
        mip4 = sao_compositor->getTextureInstance("mip4", 0);
        mip5 = sao_compositor->getTextureInstance("mip5", 0);
        mip6 = sao_compositor->getTextureInstance("mip6", 0);
        mip7 = sao_compositor->getTextureInstance("mip7", 0);
        mip8 = sao_compositor->getTextureInstance("mip8", 0);
        rt0  = sao_compositor->getTextureInstance("rt0", 0);

        // ---------------------------------------------------
        //  Copy the content of mip0,mip1... in rt0
        // ---------------------------------------------------
        rt0->unload();

        rt0->changeGroupOwnership(mip0.get()->getGroup());
        rt0->setWidth(mip0.get()->getWidth());
        rt0->setHeight(mip0.get()->getHeight());
        rt0->setNumMipmaps(8);
        rt0->setFormat(Ogre::PixelFormat::PF_FLOAT32_R);
        rt0->setUsage(Ogre::TU_DYNAMIC_WRITE_ONLY_DISCARDABLE);
        rt0->setTextureType(Ogre::TextureType::TEX_TYPE_2D);

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

        fragment_params->setNamedConstant("u_radius", static_cast<float>(0.85));

        fragment_params->setNamedConstant("u_numSamples", 11);

        const Ogre::Matrix4& proj = m_viewport->getCamera()->getProjectionMatrix();

        const Ogre::Vector4 proj_info(-2.F / (static_cast<float>(mip0.get()->getWidth()) * proj[0][0]),
                                      -2.F / (static_cast<float>(mip0.get()->getHeight()) * proj[1][1]),
                                      (1.F - proj[0][2]) / proj[0][0],
                                      (1.F + proj[1][2]) / proj[1][1]);
        fragment_params->setNamedConstant("eu_projInfo", proj_info);
    }

    if(_pass_id >= 41)
    {
        Ogre::CompositorChain* comp_chain =
            Ogre::CompositorManager::getSingletonPtr()->getCompositorChain(m_viewport);

        Ogre::CompositorInstance* sao_compositor = comp_chain->getCompositor("SAO");

        Ogre::TexturePtr prev_mip = sao_compositor->getTextureInstance("mip" + std::to_string(_pass_id - 41), 0);

        fragment_params->setNamedConstant("eu_vpWidth", static_cast<float>(prev_mip.get()->getWidth()));
        fragment_params->setNamedConstant("eu_vpHeight", static_cast<float>(prev_mip.get()->getHeight()));
    }

    if(_pass_id == 4)
    {
        // Change the Blend State

        // setNamedConstant doesn't work with bool value
        int state = 1;

        // change the blend state
        fragment_params->setNamedConstant("u_blend", state);

        // Change the AO Intensity value
        fragment_params->setNamedConstant("aoIntensity", static_cast<float>(1.0));
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::viz::scene3d::compositor
