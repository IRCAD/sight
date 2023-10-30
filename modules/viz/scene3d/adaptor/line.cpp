/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
 * Copyright (C) 2017-2020 IHU Strasbourg
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

#include "modules/viz/scene3d/adaptor/line.hpp"

#include "modules/viz/scene3d/adaptor/transform.hpp"

#include <core/com/slots.hxx>

#include <data/tools/color.hpp>

#include <service/macros.hpp>

#include <viz/scene3d/helper/scene.hpp>
#include <viz/scene3d/ogre.hpp>

#include <Ogre.h>
#include <OgreAxisAlignedBox.h>

namespace sight::module::viz::scene3d::adaptor
{

static const core::com::slots::key_t UPDATE_LENGTH_SLOT = "updateLength";

//-----------------------------------------------------------------------------

line::line() noexcept
{
    new_slot(UPDATE_LENGTH_SLOT, &line::update_length, this);
}

//-----------------------------------------------------------------------------

line::~line() noexcept =
    default;

//-----------------------------------------------------------------------------

void line::configuring()
{
    this->configure_params();

    const config_t config = this->get_config();

    // parsing transform or create an "empty" one
    this->set_transform_id(
        config.get<std::string>(
            sight::viz::scene3d::transformable::TRANSFORM_CONFIG,
            this->get_id() + "_transform"
        )
    );

    static const std::string s_LENGTH_CONFIG     = CONFIG + "length";
    static const std::string s_DASHED_CONFIG     = CONFIG + "dashed";
    static const std::string s_DASHLENGTH_CONFIG = CONFIG + "dashLength";
    static const std::string s_COLOR_CONFIG      = CONFIG + "color";

    m_length = config.get<float>(s_LENGTH_CONFIG, m_length);

    const std::string color = config.get(s_COLOR_CONFIG, "#FFFFFF");
    std::array<std::uint8_t, 4> rgba {};
    data::tools::color::hexa_string_to_rgba(color, rgba);
    m_color.r = static_cast<float>(rgba[0]) / 255.F;
    m_color.g = static_cast<float>(rgba[1]) / 255.F;
    m_color.b = static_cast<float>(rgba[2]) / 255.F;
    m_color.a = static_cast<float>(rgba[3]) / 255.F;

    m_dashed      = config.get(s_DASHED_CONFIG, m_dashed);
    m_dash_length = config.get(s_DASHLENGTH_CONFIG, m_dash_length);
}

//-----------------------------------------------------------------------------

void line::starting()
{
    this->initialize();
    this->render_service()->make_current();

    Ogre::SceneManager* scene_mgr = this->get_scene_manager();

    m_line = scene_mgr->createManualObject(this->get_id() + "_line");
    // Set the line as dynamic, so we can update it later on, when the length changes
    m_line->setDynamic(true);

    // Set the material
    m_material = std::make_shared<data::material>();

    m_material_adaptor = this->register_service<module::viz::scene3d::adaptor::material>(
        "sight::module::viz::scene3d::adaptor::material"
    );
    m_material_adaptor->set_inout(m_material, module::viz::scene3d::adaptor::material::MATERIAL_INOUT, true);
    m_material_adaptor->configure(
        this->get_id() + m_material_adaptor->get_id(),
        this->get_id() + m_material_adaptor->get_id(),
        this->render_service(),
        m_layer_id,
        "ambient"
    );
    m_material_adaptor->start();

    m_material_adaptor->get_material_fw()->set_has_vertex_color(true);
    m_material_adaptor->update();

    // Draw the line
    this->draw_line(false);

    // Set the bounding box of your Manual Object
    Ogre::Vector3 bb_min(-0.1F, -0.1F, 0.F);
    Ogre::Vector3 bb_max(0.1F, 0.1F, m_length);
    Ogre::AxisAlignedBox box(bb_min, bb_max);
    m_line->setBoundingBox(box);

    this->attach_node(m_line);

    this->set_visible(m_visible);
}

//-----------------------------------------------------------------------------

void line::updating()
{
    if(m_visible)
    {
        this->render_service()->make_current();
        // Draw
        this->draw_line(true);

        // Set the bounding box of your Manual Object
        Ogre::Vector3 bb_min(-0.1F, -0.1F, 0.F);
        Ogre::Vector3 bb_max(0.1F, 0.1F, m_length);
        Ogre::AxisAlignedBox box(bb_min, bb_max);
        m_line->setBoundingBox(box);
    }

    this->request_render();
}

//-----------------------------------------------------------------------------

void line::stopping()
{
    this->render_service()->make_current();
    this->unregister_services();
    m_material = nullptr;
    if(m_line != nullptr)
    {
        m_line->detachFromParent();
        this->get_scene_manager()->destroyManualObject(m_line);
        m_line = nullptr;
    }
}

//-----------------------------------------------------------------------------

void line::attach_node(Ogre::MovableObject* _object)
{
    Ogre::SceneNode* root_scene_node = this->get_scene_manager()->getRootSceneNode();
    Ogre::SceneNode* trans_node      = this->get_or_create_transform_node(root_scene_node);
    SIGHT_ASSERT("Transform node shouldn't be null", trans_node);

    trans_node->setVisible(m_visible);
    trans_node->attachObject(_object);
}

//-----------------------------------------------------------------------------

void line::draw_line(bool _existing_line)
{
    if(!_existing_line)
    {
        m_line->begin(
            m_material_adaptor->get_material_name(),
            Ogre::RenderOperation::OT_LINE_LIST,
            sight::viz::scene3d::RESOURCE_GROUP
        );
    }
    else
    {
        m_line->beginUpdate(0);
    }

    m_line->colour(m_color);

    if(m_dashed)
    {
        float f = 0.F;
        for(std::size_t i = 0 ; i <= static_cast<std::size_t>(m_length / (m_dash_length * 2)) ; i++)
        {
            m_line->position(0, 0, f);
            m_line->position(0, 0, f + m_dash_length);

            f += m_dash_length * 2;
        }
    }
    else
    {
        m_line->position(0, 0, 0);
        m_line->position(0, 0, m_length);
    }

    m_line->end();
}

//-----------------------------------------------------------------------------

void line::set_visible(bool /*_visible*/)
{
    Ogre::SceneNode* root_scene_node = this->get_scene_manager()->getRootSceneNode();
    Ogre::SceneNode* trans_node      = this->get_or_create_transform_node(root_scene_node);
    trans_node->setVisible(m_visible);
    this->updating();
}

//-----------------------------------------------------------------------------

void line::update_length(float _length)
{
    m_length = _length;
    this->updating();
}

} // namespace sight::module::viz::scene3d::adaptor.
