/************************************************************************
 *
 * Copyright (C) 2020-2023 IRCAD France
 * Copyright (C) 2020 IHU Strasbourg
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

#include "modules/viz/scene3d/adaptor/fragments_info.hpp"

#include <core/com/signals.hpp>
#include <core/com/slots.hxx>

#include <service/macros.hpp>

#include <viz/scene3d/helper/technique.hpp>
#include <viz/scene3d/ogre.hpp>

#include <OGRE/OgreCompositionTargetPass.h>
#include <OGRE/OgreCompositor.h>
#include <OGRE/OgreCompositorManager.h>
#include <OGRE/OgreMaterialManager.h>

#include <memory>

namespace sight::module::viz::scene3d::adaptor
{

struct fragments_info_material_listener final : public Ogre::MaterialManager::Listener
{
    ~fragments_info_material_listener() override
    = default;

    //------------------------------------------------------------------------------

    Ogre::Technique* handleSchemeNotFound(
        std::uint16_t /*schemeIndex*/,
        const Ogre::String& _scheme_name,
        Ogre::Material* _original_material,
        std::uint16_t /*lodIndex*/,
        const Ogre::Renderable*
        /*rend*/
    ) override
    {
        Ogre::Technique* new_tech = nullptr;

        if(_scheme_name == "PrimitiveID_MS")
        {
            Ogre::Technique* default_tech = _original_material->getTechnique(0);
            new_tech = sight::viz::scene3d::helper::technique::copy_to_material(
                default_tech,
                _scheme_name,
                _original_material
            );

            const Ogre::Technique::Passes& passes = new_tech->getPasses();
            for(auto* const pass : passes)
            {
                pass->setCullingMode(Ogre::CULL_NONE);
                pass->setManualCullingMode(Ogre::MANUAL_CULL_NONE);
                pass->setFragmentProgram("PrimitiveID_FP");
            }
        }

        return new_tech;
    }
};

static std::unique_ptr<fragments_info_material_listener> s_material_listener = nullptr;
static const core::com::slots::key_t RESIZE_VIEWPORT_SLOT                    = "resizeViewport";

//-----------------------------------------------------------------------------

fragments_info::fragments_info() noexcept
{
    new_slot(RESIZE_VIEWPORT_SLOT, &fragments_info::resize_viewport, this);
}

//-----------------------------------------------------------------------------

void fragments_info::configuring()
{
    // adaptor handles the layerID.
    this->configure_params();

    const config_t config = this->get_config();
    m_width      = config.get<int>(CONFIG + "width", m_width);
    m_height     = config.get<int>(CONFIG + "height", m_height);
    m_flip_image = config.get<bool>(CONFIG + "flip", m_flip_image);

    // If Both width & height are found we fix the size.
    if(m_width > 0 && m_height > 0)
    {
        m_fixed_size = true;
    }
}

//-----------------------------------------------------------------------------

void fragments_info::starting()
{
    this->initialize();

    m_compositor_name          = this->get_id() + "_Snapshot_C";
    m_target_name              = this->get_id() + "_global_RTT";
    m_target_primitive_id_name = this->get_id() + "_primitiveID_RTT";

    const sight::viz::scene3d::layer::sptr layer = this->layer();
    layer->get_render_target()->addListener(this);

    if(!s_material_listener)
    {
        s_material_listener = std::make_unique<fragments_info_material_listener>();
        Ogre::MaterialManager::getSingleton().addListener(s_material_listener.get());
    }

    // Fixed size.
    if(m_fixed_size)
    {
        this->create_compositor(m_width, m_height);
    }
    // If not listen to the resize event of the layer.
    else
    {
        Ogre::Viewport* const viewport = layer->get_viewport();

        const auto h = viewport->getActualHeight();
        const auto w = viewport->getActualWidth();

        this->create_compositor(w, h);

        m_resize_connection.connect(
            this->layer(),
            sight::viz::scene3d::layer::RESIZE_LAYER_SIG,
            this->get_sptr(),
            RESIZE_VIEWPORT_SLOT
        );
    }
}

//-----------------------------------------------------------------------------

void fragments_info::updating() noexcept
{
    {
        const auto image = m_image.lock();
        if(image)
        {
            const Ogre::TexturePtr text = m_compositor->getTextureInstance(m_target_name, 0);
            sight::viz::scene3d::utils::convert_from_ogre_texture(text, image.get_shared(), m_flip_image);

            const auto sig =
                image->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
            sig->async_emit();
        }
    }

    {
        const auto depth = m_depth.lock();
        if(depth)
        {
            const Ogre::TexturePtr depth_text = m_compositor->getTextureInstance(m_target_name, 1);
            sight::viz::scene3d::utils::convert_from_ogre_texture(depth_text, depth.get_shared(), m_flip_image);

            const auto depth_sig =
                depth->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
            depth_sig->async_emit();
        }
    }

    {
        const auto primitive_id = m_primitive.lock();
        if(primitive_id)
        {
            const Ogre::TexturePtr primitive_id_text = m_compositor->getTextureInstance(m_target_primitive_id_name, 0);
            sight::viz::scene3d::utils::convert_from_ogre_texture(
                primitive_id_text,
                primitive_id.get_shared(),
                m_flip_image
            );

            const auto primitive_id_sig =
                primitive_id->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
            primitive_id_sig->async_emit();
        }
    }
}

//-----------------------------------------------------------------------------

void fragments_info::stopping()
{
    m_resize_connection.disconnect();

    this->destroy_compositor();

    const sight::viz::scene3d::layer::sptr layer = this->layer();
    layer->get_render_target()->removeListener(this);
}

//-----------------------------------------------------------------------------

void fragments_info::create_compositor(int _width, int _height)
{
    /* Creates the following compositor:
       compositor 'm_compositorName'
       {
            technique
            {
                // The depth attachment only exist if the depth is needed.
                texture 'm_targetName' '_width' '_height' PF_R8G8B8 PF_FLOAT32_R global_scope

                // We use a local depth texture since Ogre doesn't allow global depth textures.
                // It will be copied by ForwardDepth_M.
                texture local_RTT target_width target_height PF_DEPTH32 local_scope

                // This target pass only exist if the depth is needed.
                target local_RTT
                {
                    input previous
                }

                target 'm_targetName'
                {
                    input previous
                    // This pass only exist if the depth is needed.
                    pass render_quad
                    {
                        material ForwardDepth_M
                        input 0 local_RTT
                    }
                }

                target_output
                {
                    input none

                    pass render_quad
                    {
                        material Forward
                        input 0 m_targetName 0
                    }
                }
            }
       }*/

    const bool retrieve_depth        = m_depth.lock().operator bool();
    const bool retrieve_primitive_id = m_primitive.lock().operator bool();

    Ogre::CompositorManager& cmp_manager = Ogre::CompositorManager::getSingleton();

    m_compositor = cmp_manager.create(
        m_compositor_name,
        sight::viz::scene3d::RESOURCE_GROUP
    );

    Ogre::CompositionTechnique* const technique = m_compositor->createTechnique();

    Ogre::CompositionTechnique::TextureDefinition* global_target = nullptr;
    global_target        = technique->createTextureDefinition(m_target_name);
    global_target->scope = Ogre::CompositionTechnique::TextureScope::TS_GLOBAL;
    global_target->formatList.push_back(Ogre::PixelFormat::PF_A8B8G8R8);
    global_target->height = static_cast<Ogre::uint32>(_height);
    global_target->width  = static_cast<Ogre::uint32>(_width);

    if(!retrieve_depth)
    {
        Ogre::CompositionTargetPass* const global_target_pass = technique->createTargetPass();
        {
            global_target_pass->setOutputName(m_target_name);
            global_target_pass->setInputMode(Ogre::CompositionTargetPass::InputMode::IM_PREVIOUS);
        }
    }
    else
    {
        global_target->formatList.push_back(Ogre::PixelFormat::PF_FLOAT32_R);

        const std::string local_name("local_RTT");
        Ogre::CompositionTechnique::TextureDefinition* local_target = nullptr;
        local_target        = technique->createTextureDefinition(local_name);
        local_target->scope = Ogre::CompositionTechnique::TextureScope::TS_LOCAL;
        local_target->formatList.push_back(Ogre::PixelFormat::PF_DEPTH32);
        local_target->height = static_cast<Ogre::uint32>(_height);
        local_target->width  = static_cast<Ogre::uint32>(_width);

        Ogre::CompositionTargetPass* const local_target_pass = technique->createTargetPass();
        {
            local_target_pass->setOutputName(local_name);
            local_target_pass->setInputMode(Ogre::CompositionTargetPass::InputMode::IM_PREVIOUS);
        }

        Ogre::CompositionTargetPass* const global_target_pass = technique->createTargetPass();
        {
            global_target_pass->setOutputName(m_target_name);
            global_target_pass->setInputMode(Ogre::CompositionTargetPass::InputMode::IM_PREVIOUS);
            Ogre::CompositionPass* const target_output_comp_pass = global_target_pass->createPass(
                Ogre::CompositionPass::PT_RENDERQUAD
            );
            target_output_comp_pass->setMaterialName("ForwardDepth_M");
            target_output_comp_pass->setInput(0, local_name);
        }
    }

    if(retrieve_primitive_id)
    {
        Ogre::CompositionTechnique::TextureDefinition* global_target_primitive_id = nullptr;
        global_target_primitive_id        = technique->createTextureDefinition(m_target_primitive_id_name);
        global_target_primitive_id->scope = Ogre::CompositionTechnique::TextureScope::TS_GLOBAL;
        global_target_primitive_id->formatList.push_back(Ogre::PixelFormat::PF_R32_SINT);
        global_target_primitive_id->height = static_cast<Ogre::uint32>(_height);
        global_target_primitive_id->width  = static_cast<Ogre::uint32>(_width);

        Ogre::CompositionTargetPass* const global_primitive_target_pass = technique->createTargetPass();
        {
            global_primitive_target_pass->setOutputName(m_target_primitive_id_name);
            global_primitive_target_pass->setInputMode(Ogre::CompositionTargetPass::InputMode::IM_NONE);
            global_primitive_target_pass->setMaterialScheme("PrimitiveID_MS");
            global_primitive_target_pass->createPass(Ogre::CompositionPass::PT_CLEAR);
            global_primitive_target_pass->createPass(Ogre::CompositionPass::PT_RENDERSCENE);
        }
    }

    Ogre::CompositionTargetPass* const target_output_pass = technique->getOutputTargetPass();
    {
        Ogre::CompositionPass* const target_output_comp_pass = target_output_pass->createPass(
            Ogre::CompositionPass::PT_RENDERQUAD
        );
        target_output_comp_pass->setMaterialName("Forward_M");
        target_output_comp_pass->setInput(0, m_target_name, 0);
    }

    const auto layer = this->layer();
    layer->add_available_compositor(m_compositor_name);
    layer->update_compositor_state(m_compositor_name, true);
}

//-----------------------------------------------------------------------------

void fragments_info::destroy_compositor()
{
    Ogre::CompositorManager& cmp_manager = Ogre::CompositorManager::getSingleton();

    const auto layer = this->layer();
    layer->update_compositor_state(m_compositor_name, false);
    cmp_manager.removeCompositor(layer->get_viewport(), m_compositor_name);
    cmp_manager.remove(m_compositor);
    m_compositor.reset();
}

//-----------------------------------------------------------------------------

void fragments_info::resize_viewport()
{
    const auto layer = this->layer();
    auto rect        = layer->get_viewport()->getActualDimensions();

    // Sometimes, the size can be null, we need to avoid resizing since a global texture needs absolute values.
    if(rect.width() != 0 && rect.height() != 0)
    {
        this->destroy_compositor();
        this->create_compositor(static_cast<int>(rect.width()), static_cast<int>(rect.height()));
        layer->get_render_target()->addListener(this);
    }
}

//-----------------------------------------------------------------------------

void fragments_info::postRenderTargetUpdate(const Ogre::RenderTargetEvent& /*evt*/)
{
    this->updating();
}

//-----------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.
