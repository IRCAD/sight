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

#include "modules/viz/scene3d/adaptor/axis.hpp"

#include "modules/viz/scene3d/adaptor/transform.hpp"

#include <core/com/slots.hxx>

#include <data/color.hpp>

#include <service/macros.hpp>

#include <viz/scene3d/helper/manual_object.hpp>
#include <viz/scene3d/helper/scene.hpp>
#include <viz/scene3d/render.hpp>

#include <OgreNode.h>

namespace sight::module::viz::scene3d::adaptor
{

//-----------------------------------------------------------------------------

axis::axis() noexcept
{
    new_slot("updateOriginColor", &axis::updateOriginColor, this);
}

//-----------------------------------------------------------------------------

void axis::updateOriginColor(sight::data::color::sptr _new_color)
{
    if(m_origin == nullptr)
    {
        return;
    }

    m_originMaterial->setAmbient(_new_color);

    const auto sig = m_originMaterial->signal<core::com::signal<void()> >(data::material::MODIFIED_SIG);
    sig->async_emit();
}

//-----------------------------------------------------------------------------

void axis::configuring()
{
    this->configureParams();

    const config_t config = this->get_config();

    const std::string transform_id = config.get<std::string>(
        module::viz::scene3d::adaptor::transform::s_TRANSFORM_CONFIG,
        this->get_id() + "_transform"
    );

    this->setTransformId(transform_id);

    static const std::string s_LENGTH_CONFIG       = s_CONFIG + "length";
    static const std::string s_LABEL_CONFIG        = s_CONFIG + "label";
    static const std::string s_FONT_SIZE_CONFIG    = s_CONFIG + "fontSize";
    static const std::string s_ORIGIN_CONFIG       = s_CONFIG + "origin";
    static const std::string s_AXIS_CONFIG         = s_CONFIG + "axis";
    static const std::string s_ORIGIN_COLOR_CONFIG = s_CONFIG + "originColor";
    static const std::string s_AXIS_NAME           = s_CONFIG + "name";

    m_length           = config.get<float>(s_LENGTH_CONFIG, m_length);
    m_enableLabel      = config.get<bool>(s_LABEL_CONFIG, m_enableLabel);
    m_fontSize         = config.get<std::size_t>(s_FONT_SIZE_CONFIG, m_fontSize);
    m_originVisibility = config.get<bool>(s_ORIGIN_CONFIG, m_originVisibility);
    m_axisVisibility   = config.get<bool>(s_AXIS_CONFIG, m_axisVisibility);
    m_axisName         = config.get<std::string>(s_AXIS_NAME, m_axisName);

    m_originColor = config.get<std::string>(s_ORIGIN_COLOR_CONFIG, m_originColor);
    SIGHT_ASSERT(
        "Color string should start with '#' and followed by 6 or 8 "
        "hexadecimal digits. Given color: " << m_originColor,
        m_originColor[0] == '#'
        && (m_originColor.length() == 7 || m_originColor.length() == 9)
    );

    // Force disable label if axisVisibility is false.
    m_enableLabel = m_axisVisibility ? m_enableLabel : false;
}

//-----------------------------------------------------------------------------

void axis::starting()
{
    this->initialize();

    this->getRenderService()->makeCurrent();

    Ogre::SceneNode* const root_scene_node = this->getSceneManager()->getRootSceneNode();
    Ogre::SceneNode* const transform_node  = this->getOrCreateTransformNode(root_scene_node);
    m_sceneNode = transform_node->createChildSceneNode(this->get_id() + "_mainNode");

    Ogre::SceneManager* const scene_mgr = this->getSceneManager();

    if(m_originVisibility)
    {
        m_origin = scene_mgr->createManualObject(this->get_id() + "_origin");
    }

    if(m_axisVisibility)
    {
        m_xLine = scene_mgr->createManualObject(this->get_id() + "_xLine");
        m_yLine = scene_mgr->createManualObject(this->get_id() + "_yLine");
        m_zLine = scene_mgr->createManualObject(this->get_id() + "_zLine");

        m_xCone = scene_mgr->createManualObject(this->get_id() + "_xCone");
        m_yCone = scene_mgr->createManualObject(this->get_id() + "_yCone");
        m_zCone = scene_mgr->createManualObject(this->get_id() + "_zCone");
    }

    // set the material
    m_material       = std::make_shared<data::material>();
    m_originMaterial = std::make_shared<data::material>();

    const module::viz::scene3d::adaptor::material::sptr material_adaptor =
        this->registerService<module::viz::scene3d::adaptor::material>(
            "sight::module::viz::scene3d::adaptor::material"
        );
    material_adaptor->set_inout(m_material, module::viz::scene3d::adaptor::material::s_MATERIAL_INOUT, true);
    material_adaptor->configure(
        this->get_id() + material_adaptor->get_id(),
        this->get_id() + material_adaptor->get_id(),
        this->getRenderService(),
        m_layerID
    );
    material_adaptor->start();

    material_adaptor->getMaterialFw()->setHasVertexColor(true);
    material_adaptor->update();

    const module::viz::scene3d::adaptor::material::sptr origin_material_adaptor =
        this->registerService<module::viz::scene3d::adaptor::material>(
            "sight::module::viz::scene3d::adaptor::material",
            this->get_id() + "_originMaterialAdp"
        );
    origin_material_adaptor->set_inout(
        m_originMaterial,
        module::viz::scene3d::adaptor::material::s_MATERIAL_INOUT,
        true
    );
    origin_material_adaptor->configure(
        this->get_id() + origin_material_adaptor->get_id(),
        this->get_id() + origin_material_adaptor->get_id(),
        this->getRenderService(),
        m_layerID,
        "ambient"
    );
    origin_material_adaptor->start();
    origin_material_adaptor->update();

    // Sizes
    const float origin_radius   = m_length * 0.1F;
    const float cylinder_length = m_length * 0.85F;
    const float cylinder_radius = m_length * 0.025F;
    const float cone_length     = m_length - cylinder_length;
    const float cone_radius     = cylinder_radius * 2.F;
    const unsigned sample       = 64;

    // Draw

    // origin

    if(m_originVisibility)
    {
        const data::color::sptr origin_color = std::make_shared<data::color>();
        origin_color->setRGBA(m_originColor);
        m_originMaterial->setAmbient(origin_color);
        m_originMaterial->setDiffuse(std::make_shared<data::color>(0.F, 0.F, 0.F, 1.F));

        const auto sig = m_originMaterial->signal<core::com::signal<void()> >(data::material::MODIFIED_SIG);
        sig->async_emit();

        sight::viz::scene3d::helper::manual_object::createSphere(
            m_origin,
            origin_material_adaptor->getMaterialName(),
            Ogre::ColourValue(
                origin_color->red(),
                origin_color->green(),
                origin_color->blue(),
                origin_color->alpha()
            ),
            origin_radius,
            sample
        );

        m_sceneNode->attachObject(m_origin);
    }

    Ogre::Camera* const cam = this->getLayer()->getDefaultCamera();

    if(m_axisVisibility)
    {
        // X axis
        sight::viz::scene3d::helper::manual_object::createCylinder(
            m_xLine,
            material_adaptor->getMaterialName(),
            Ogre::ColourValue(Ogre::ColourValue::Red),
            cylinder_radius,
            cylinder_length,
            sample
        );
        Ogre::SceneNode* const x_line_node = m_sceneNode->createChildSceneNode(this->get_id() + "_xLine");
        x_line_node->attachObject(m_xLine);
        x_line_node->pitch(Ogre::Degree(90));

        // Y axis
        sight::viz::scene3d::helper::manual_object::createCylinder(
            m_yLine,
            material_adaptor->getMaterialName(),
            Ogre::ColourValue(Ogre::ColourValue::Green),
            cylinder_radius,
            cylinder_length,
            sample
        );
        Ogre::SceneNode* const y_line_node = m_sceneNode->createChildSceneNode(this->get_id() + "_yLine");
        y_line_node->attachObject(m_yLine);
        y_line_node->roll(Ogre::Degree(90));

        // Z axis
        sight::viz::scene3d::helper::manual_object::createCylinder(
            m_zLine,
            material_adaptor->getMaterialName(),
            Ogre::ColourValue(Ogre::ColourValue::Blue),
            cylinder_radius,
            cylinder_length,
            sample
        );
        Ogre::SceneNode* const z_line_node = m_sceneNode->createChildSceneNode(this->get_id() + "_zLine");
        z_line_node->attachObject(m_zLine);
        z_line_node->yaw(Ogre::Degree(-90));

        // X cone
        sight::viz::scene3d::helper::manual_object::createCone(
            m_xCone,
            material_adaptor->getMaterialName(),
            Ogre::ColourValue(Ogre::ColourValue::Red),
            cone_radius,
            cone_length,
            sample
        );
        Ogre::SceneNode* const x_cone_node = m_sceneNode->createChildSceneNode(this->get_id() + "_xCone");

        if(m_enableLabel)
        {
            m_axisLabels[0] = sight::viz::scene3d::text::make(this->getLayer());
            m_axisLabels[0]->setText("X");
            m_axisLabels[0]->setFontSize(m_fontSize);
            m_axisLabels[0]->attachToNode(x_cone_node, this->getLayer()->getDefaultCamera());
        }

        x_cone_node->attachObject(m_xCone);
        x_cone_node->translate(cylinder_length, 0.F, 0.F);

        // Y cone
        sight::viz::scene3d::helper::manual_object::createCone(
            m_yCone,
            material_adaptor->getMaterialName(),
            Ogre::ColourValue(Ogre::ColourValue::Green),
            cone_radius,
            cone_length,
            sample
        );
        Ogre::SceneNode* const y_cone_node = m_sceneNode->createChildSceneNode(this->get_id() + "_yCone");
        y_cone_node->attachObject(m_yCone);

        if(m_enableLabel)
        {
            m_axisLabels[1] = sight::viz::scene3d::text::make(this->getLayer());
            m_axisLabels[1]->setText("Y");
            m_axisLabels[1]->setFontSize(m_fontSize);
            m_axisLabels[1]->attachToNode(y_cone_node, cam);
        }

        y_cone_node->translate(0.F, cylinder_length, 0.F);
        y_cone_node->roll(Ogre::Degree(90));

        // Z cone
        sight::viz::scene3d::helper::manual_object::createCone(
            m_zCone,
            material_adaptor->getMaterialName(),
            Ogre::ColourValue(Ogre::ColourValue::Blue),
            cone_radius,
            cone_length,
            sample
        );
        Ogre::SceneNode* const z_cone_node = m_sceneNode->createChildSceneNode(this->get_id() + "_zCone");
        z_cone_node->attachObject(m_zCone);

        if(m_enableLabel)
        {
            m_axisLabels[2] = sight::viz::scene3d::text::make(this->getLayer());
            m_axisLabels[2]->setText("Z");
            m_axisLabels[2]->setFontSize(m_fontSize);
            m_axisLabels[2]->attachToNode(z_cone_node, this->getLayer()->getDefaultCamera());
        }

        z_cone_node->translate(0.F, 0.F, cylinder_length);
        z_cone_node->yaw(Ogre::Degree(-90));
    }

    // Display Name if provided.
    if(!m_axisName.empty())
    {
        m_axisNameTxt = sight::viz::scene3d::text::make(this->getLayer());
        m_axisNameTxt->setText(m_axisName);
        m_axisNameTxt->setFontSize(m_fontSize);
        m_axisNameTxt->attachToNode(m_sceneNode, cam);
        const data::color::sptr txt_color = std::make_shared<data::color>();
        txt_color->setRGBA(m_originColor);
        m_axisNameTxt->setTextColor(
            Ogre::ColourValue(
                txt_color->red(),
                txt_color->green(),
                txt_color->blue()
            )
        );
    }

    this->updateVisibility(m_isVisible);

    this->requestRender();
}

//-----------------------------------------------------------------------------

void axis::updating()
{
    this->requestRender();
}

//-----------------------------------------------------------------------------

void axis::stopping()
{
    this->getRenderService()->makeCurrent();

    Ogre::SceneManager* const scene_mgr = this->getSceneManager();

    if(m_sceneNode != nullptr)
    {
        if(m_axisVisibility)
        {
            m_sceneNode->removeAndDestroyChild(this->get_id() + "_xLine");
            m_sceneNode->removeAndDestroyChild(this->get_id() + "_yLine");
            m_sceneNode->removeAndDestroyChild(this->get_id() + "_zLine");
            m_sceneNode->removeAndDestroyChild(this->get_id() + "_xCone");
            m_sceneNode->removeAndDestroyChild(this->get_id() + "_yCone");
            m_sceneNode->removeAndDestroyChild(this->get_id() + "_zCone");
        }
    }

    if(m_enableLabel)
    {
        for(auto& label : m_axisLabels)
        {
            SIGHT_ASSERT("label should not be null", label);
            label->detachFromNode();
            label.reset();
        }
    }

    if(m_axisNameTxt != nullptr)
    {
        m_axisNameTxt->detachFromNode();
        m_axisNameTxt.reset();
    }

    if(m_originVisibility)
    {
        scene_mgr->destroyManualObject(m_origin);
    }

    if(m_axisVisibility)
    {
        scene_mgr->destroyManualObject(m_xLine);
        scene_mgr->destroyManualObject(m_yLine);
        scene_mgr->destroyManualObject(m_zLine);

        scene_mgr->destroyManualObject(m_xCone);
        scene_mgr->destroyManualObject(m_yCone);
        scene_mgr->destroyManualObject(m_zCone);
    }

    Ogre::SceneNode* const transform_node = this->getTransformNode();
    if(transform_node != nullptr)
    {
        transform_node->removeAndDestroyChild(this->get_id() + "_mainNode");
    }

    this->unregisterServices();
    m_material.reset();
}

//-----------------------------------------------------------------------------

void axis::setVisible(bool _visible)
{
    if(m_sceneNode != nullptr)
    {
        m_sceneNode->setVisible(_visible);
        if(m_enableLabel)
        {
            for(auto& label : m_axisLabels)
            {
                SIGHT_ASSERT("label should not be null", label);
                label->setVisible(_visible);
            }
        }

        if(m_axisNameTxt != nullptr)
        {
            m_axisNameTxt->setVisible(_visible);
        }
    }

    this->updating();
}

//-----------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.
