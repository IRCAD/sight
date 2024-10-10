/************************************************************************
 *
 * Copyright (C) 2017-2024 IRCAD France
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

//-----------------------------------------------------------------------------

service::connections_t line::auto_connections() const
{
    return {
        {m_length, data::object::MODIFIED_SIG, adaptor::slots::LAZY_UPDATE},
        {m_color, data::object::MODIFIED_SIG, adaptor::slots::LAZY_UPDATE},
        {m_dash_length, data::object::MODIFIED_SIG, adaptor::slots::LAZY_UPDATE},
        {m_dashed, data::object::MODIFIED_SIG, adaptor::slots::LAZY_UPDATE}
    };
}

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
}

//-----------------------------------------------------------------------------

void line::starting()
{
    adaptor::init();
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
    Ogre::Vector3 bb_max(0.1F, 0.1F, static_cast<float>(*m_length));
    Ogre::AxisAlignedBox box(bb_min, bb_max);
    m_line->setBoundingBox(box);

    this->attach_node(m_line);

    this->set_visible(visible());
}

//-----------------------------------------------------------------------------

void line::updating()
{
    if(visible())
    {
        this->render_service()->make_current();
        // Draw
        this->draw_line(true);

        // Set the bounding box of your Manual Object
        Ogre::Vector3 bb_min(-0.1F, -0.1F, 0.F);
        Ogre::Vector3 bb_max(0.1F, 0.1F, static_cast<float>(*m_length));
        Ogre::AxisAlignedBox box(bb_min, bb_max);
        m_line->setBoundingBox(box);
    }

    this->update_done();
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

    adaptor::deinit();
}

//-----------------------------------------------------------------------------

void line::attach_node(Ogre::MovableObject* _object)
{
    Ogre::SceneNode* root_scene_node = this->get_scene_manager()->getRootSceneNode();
    Ogre::SceneNode* trans_node      = this->get_or_create_transform_node(root_scene_node);
    SIGHT_ASSERT("Transform node shouldn't be null", trans_node);

    trans_node->setVisible(visible());
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

    const auto color = *m_color;
    Ogre::ColourValue ogre_color(color[0], color[1], color[2], color[3]);
    m_line->colour(ogre_color);

    const auto length      = static_cast<float>(*m_length);
    const auto dash_length = static_cast<float>(*m_dash_length);
    if(*m_dashed)
    {
        float f = 0.F;
        for(std::size_t i = 0 ; i <= static_cast<std::size_t>(length / (dash_length * 2)) ; i++)
        {
            m_line->position(0, 0, f);
            m_line->position(0, 0, f + static_cast<float>(dash_length));

            f += dash_length * 2;
        }
    }
    else
    {
        m_line->position(0, 0, 0);
        m_line->position(0, 0, length);
    }

    m_line->end();
}

//-----------------------------------------------------------------------------

void line::set_visible(bool /*_visible*/)
{
    Ogre::SceneNode* root_scene_node = this->get_scene_manager()->getRootSceneNode();
    Ogre::SceneNode* trans_node      = this->get_or_create_transform_node(root_scene_node);
    trans_node->setVisible(visible());
    this->updating();
}

} // namespace sight::module::viz::scene3d::adaptor.
