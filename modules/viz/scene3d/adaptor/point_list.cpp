/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
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

#include "modules/viz/scene3d/adaptor/point_list.hpp"

#include "modules/viz/scene3d/adaptor/material.hpp"

#include "viz/scene3d/ogre.hpp"

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>

#include <data/string.hpp>

#include <service/macros.hpp>
#include <service/op.hpp>

#include <viz/scene3d/helper/scene.hpp>
#include <viz/scene3d/r2vb_renderable.hpp>
#include <viz/scene3d/render.hpp>

#include <OGRE/OgreAxisAlignedBox.h>
#include <OGRE/OgreTechnique.h>
#include <OGRE/OgreTextureManager.h>

#include <cstdint>

namespace sight::module::viz::scene3d::adaptor
{

//-----------------------------------------------------------------------------

point_list::point_list() noexcept
{
    m_material = std::make_shared<data::material>();
}

//-----------------------------------------------------------------------------

point_list::~point_list() noexcept
{
    if(m_entity != nullptr)
    {
        Ogre::SceneManager* scene_mgr = this->getSceneManager();
        scene_mgr->destroyEntity(m_entity);
    }
}

//-----------------------------------------------------------------------------

void point_list::setVisible(bool _visible)
{
    if(m_entity != nullptr)
    {
        this->getRenderService()->makeCurrent();

        m_entity->setVisible(_visible);

        m_meshGeometry->setVisible(_visible);

        this->requestRender();
    }
}

//-----------------------------------------------------------------------------

void point_list::configuring()
{
    this->configureParams();

    const config_t config = this->get_config();

    static const std::string s_COLOR_CONFIG             = s_CONFIG + "color";
    static const std::string s_VISIBLE_CONFIG           = s_CONFIG + "visible";
    static const std::string s_AUTORESET_CAMERA_CONFIG  = s_CONFIG + "autoresetcamera";
    static const std::string s_MATERIAL_TEMPLATE_CONFIG = s_CONFIG + "materialTemplate";
    static const std::string s_FIXED_SIZE_CONFIG        = s_CONFIG + "fixedSize";
    static const std::string s_TEXTURE_NAME_CONFIG      = s_CONFIG + "textureName";
    static const std::string s_QUERY_CONFIG             = s_CONFIG + "queryFlags";
    static const std::string s_RADIUS_CONFIG            = s_CONFIG + "radius";
    static const std::string s_DISPLAY_LABEL_CONFIG     = s_CONFIG + "displayLabel";
    static const std::string s_LABEL_COLOR_CONFIG       = s_CONFIG + "labelColor";
    static const std::string s_FONT_SOURCE_CONFIG       = s_CONFIG + "fontSource";
    static const std::string s_FONT_SIZE_CONFIG         = s_CONFIG + "fontSize";

    const std::string color = config.get<std::string>(s_COLOR_CONFIG, "");

    const bool visible = config.get<bool>(s_VISIBLE_CONFIG, m_isVisible);
    this->updateVisibility(visible);

    SIGHT_ASSERT("Material not found", m_material);
    m_material->diffuse()->setRGBA(color.empty() ? "#FFFFFFFF" : color);

    m_autoResetCamera = config.get<bool>(s_AUTORESET_CAMERA_CONFIG, true);

    if(config.count(s_MATERIAL_TEMPLATE_CONFIG) != 0U)
    {
        // An existing Ogre material will be used for this mesh
        m_customMaterial       = true;
        m_materialTemplateName = config.get<std::string>(s_MATERIAL_TEMPLATE_CONFIG);
    }
    else if(config.get(s_FIXED_SIZE_CONFIG, false))
    {
        m_materialTemplateName = "Billboard_FixedSize";
    }

    // The mesh adaptor will pass the texture name to the created material adaptor
    m_textureName = config.get(s_TEXTURE_NAME_CONFIG, m_textureName);

    this->setTransformId(
        config.get<std::string>(
            sight::viz::scene3d::transformable::s_TRANSFORM_CONFIG,
            this->get_id() + "_transform"
        )
    );

    const std::string hexa_mask = config.get<std::string>(s_QUERY_CONFIG, "");
    if(!hexa_mask.empty())
    {
        SIGHT_ASSERT(
            "Hexadecimal values should start with '0x'"
            "Given value : " + hexa_mask,
            hexa_mask.length() > 2
            && hexa_mask.substr(0, 2) == "0x"
        );
        m_queryFlags = static_cast<std::uint32_t>(std::stoul(hexa_mask, nullptr, 16));
    }

    m_fontSource = config.get(s_FONT_SOURCE_CONFIG, m_fontSource);
    m_fontSize   = config.get<std::size_t>(s_FONT_SIZE_CONFIG, m_fontSize);

    m_radius       = config.get(s_RADIUS_CONFIG, m_radius);
    m_displayLabel = config.get(s_DISPLAY_LABEL_CONFIG, m_displayLabel);

    const std::string label_color = config.get(s_LABEL_COLOR_CONFIG, "#FFFFFF");
    m_labelColor = std::make_shared<data::color>();
    m_labelColor->setRGBA(label_color);
}

//-----------------------------------------------------------------------------

void point_list::starting()
{
    this->initialize();

    this->getRenderService()->makeCurrent();

    m_meshGeometry = std::make_shared<sight::viz::scene3d::mesh>(this->get_id());
    m_meshGeometry->setDynamic(true);
    Ogre::SceneNode* root_scene_node = this->getSceneManager()->getRootSceneNode();
    m_sceneNode = this->getOrCreateTransformNode(root_scene_node);

    const auto point_list = m_pointList.lock();
    if(point_list)
    {
        this->updateMesh(point_list.get_shared());
    }
    else
    {
        const auto mesh = m_mesh.lock();
        if(mesh)
        {
            if(!m_customMaterial && mesh->has<data::mesh::Attributes::POINT_COLORS>())
            {
                m_materialTemplateName += "_PerPointColor";
            }

            this->updateMesh(mesh.get_shared());
        }
        else
        {
            SIGHT_ERROR("No '" << s_POINTLIST_INPUT << "' or '" << s_MESH_INPUT << "' specified.")
        }
    }
}

//-----------------------------------------------------------------------------

service::connections_t point_list::auto_connections() const
{
    service::connections_t connections;
    connections.push(s_POINTLIST_INPUT, data::point_list::POINT_ADDED_SIG, service::slots::UPDATE);
    connections.push(s_POINTLIST_INPUT, data::point_list::POINT_REMOVED_SIG, service::slots::UPDATE);
    connections.push(s_POINTLIST_INPUT, data::point_list::MODIFIED_SIG, service::slots::UPDATE);

    connections.push(s_MESH_INPUT, data::mesh::VERTEX_MODIFIED_SIG, service::slots::UPDATE);
    connections.push(s_MESH_INPUT, data::mesh::MODIFIED_SIG, service::slots::UPDATE);

    return connections;
}

//-----------------------------------------------------------------------------

void point_list::stopping()
{
    this->getRenderService()->makeCurrent();

    this->unregisterServices();

    Ogre::SceneManager* scene_mgr = this->getSceneManager();
    SIGHT_ASSERT("Ogre::SceneManager is null", scene_mgr);
    m_meshGeometry->clearMesh(*scene_mgr);

    if(m_entity != nullptr)
    {
        scene_mgr->destroyEntity(m_entity);
        m_entity = nullptr;
    }

    m_meshGeometry.reset();
}

//-----------------------------------------------------------------------------

void point_list::updating()
{
    this->getRenderService()->makeCurrent();

    if((!getVisibility() || !this->getRenderService()->isShownOnScreen()))
    {
        return;
    }

    this->destroyLabel();

    const auto point_list = m_pointList.lock();
    if(point_list)
    {
        this->updateMesh(point_list.get_shared());
    }
    else
    {
        const auto mesh = m_mesh.lock();
        if(mesh)
        {
            this->updateMesh(mesh.get_shared());
        }
        else
        {
            SIGHT_ERROR("No '" << s_POINTLIST_INPUT << "' or '" << s_MESH_INPUT << "' specified.")
        }
    }

    this->requestRender();
}

//------------------------------------------------------------------------------

void point_list::createLabel(const data::point_list::csptr& _point_list)
{
    auto render_srv          = this->getRenderService();
    std::size_t i            = 0;
    std::string label_number = std::to_string(i);
    for(const auto& point : _point_list->getPoints())
    {
        const auto label = point->getLabel();
        if(!label.empty())
        {
            label_number = label;
        }
        else
        {
            label_number = std::to_string(i);
        }

        m_labels.push_back(sight::viz::scene3d::text::make(this->getLayer()));
        m_labels[i]->setFontSize(m_fontSize);
        m_labels[i]->setText(label_number);
        m_labels[i]->setTextColor(
            Ogre::ColourValue(
                m_labelColor->red(),
                m_labelColor->green(),
                m_labelColor->blue()
            )
        );
        m_nodes.push_back(m_sceneNode->createChildSceneNode(this->get_id() + label_number));
        m_labels[i]->attachToNode(m_nodes[i], this->getLayer()->getDefaultCamera());
        data::point::point_coord_array_t coord = point->getCoord();
        m_nodes[i]->translate(static_cast<float>(coord[0]), static_cast<float>(coord[1]), static_cast<float>(coord[2]));
        i++;
    }
}

//------------------------------------------------------------------------------

void point_list::destroyLabel()
{
    std::ranges::for_each(m_nodes, [this](auto& _node){m_sceneNode->removeAndDestroyChild(_node);});
    m_nodes.clear();

    std::ranges::for_each(m_labels, [](auto& _label){_label->detachFromNode();});
    m_labels.clear();
}

//-----------------------------------------------------------------------------

void point_list::updateMesh(const data::point_list::csptr& _point_list)
{
    Ogre::SceneManager* scene_mgr = this->getSceneManager();
    SIGHT_ASSERT("Ogre::SceneManager is null", scene_mgr);

    detachAndDestroyEntity();

    const std::size_t ui_num_vertices = _point_list->getPoints().size();
    if(ui_num_vertices == 0)
    {
        SIGHT_DEBUG("Empty mesh");
        m_meshGeometry->clearMesh(*scene_mgr);

        return;
    }

    if(m_displayLabel)
    {
        this->createLabel(_point_list);
    }

    this->getRenderService()->makeCurrent();

    m_meshGeometry->updateMesh(_point_list);

    //------------------------------------------
    // Create entity and attach it in the scene graph
    //------------------------------------------

    if(m_entity == nullptr)
    {
        m_entity = m_meshGeometry->createEntity(*scene_mgr);
        m_entity->setVisible(m_isVisible);
        m_entity->setQueryFlags(m_queryFlags);
    }

    //------------------------------------------
    // Update vertex layers
    //------------------------------------------

    m_meshGeometry->updateVertices(_point_list);

    //------------------------------------------
    // Create sub-services
    //------------------------------------------
    this->updateMaterialAdaptor(_point_list->get_id());

    this->attachNode(m_entity);

    m_meshGeometry->setVisible(m_isVisible);

    if(m_autoResetCamera)
    {
        this->getRenderService()->resetCameraCoordinates(m_layerID);
    }
}

//------------------------------------------------------------------------------

void point_list::updateMesh(const data::mesh::csptr& _mesh)
{
    Ogre::SceneManager* scene_mgr = this->getSceneManager();
    SIGHT_ASSERT("Ogre::SceneManager is null", scene_mgr);

    detachAndDestroyEntity();

    const std::size_t ui_num_vertices = _mesh->numPoints();
    if(ui_num_vertices == 0)
    {
        SIGHT_DEBUG("Empty mesh");

        m_meshGeometry->clearMesh(*scene_mgr);
        return;
    }

    this->getRenderService()->makeCurrent();

    m_meshGeometry->updateMesh(std::const_pointer_cast<data::mesh>(_mesh), true);

    //------------------------------------------
    // Create entity and attach it in the scene graph
    //------------------------------------------

    if(m_entity == nullptr)
    {
        m_entity = m_meshGeometry->createEntity(*scene_mgr);
        m_entity->setVisible(m_isVisible);
        m_entity->setQueryFlags(m_queryFlags);
    }

    //------------------------------------------
    // Update vertex layers
    //------------------------------------------

    m_meshGeometry->updateVertices(_mesh);
    m_meshGeometry->updateColors(_mesh);

    //------------------------------------------
    // Create sub-services
    //------------------------------------------
    this->updateMaterialAdaptor(_mesh->get_id());

    this->attachNode(m_entity);

    m_meshGeometry->setVisible(m_isVisible);

    if(m_autoResetCamera)
    {
        this->getRenderService()->resetCameraCoordinates(m_layerID);
    }
}

//------------------------------------------------------------------------------

scene3d::adaptor::material::sptr point_list::createMaterialService(const std::string& _mesh_id)
{
    auto material_adaptor = this->registerService<module::viz::scene3d::adaptor::material>(
        "sight::module::viz::scene3d::adaptor::material"
    );
    material_adaptor->set_inout(m_material, "material", true);

    const auto tpl_name =
        !m_materialTemplateName.empty() ? m_materialTemplateName : sight::viz::scene3d::material::
        DEFAULT_MATERIAL_TEMPLATE_NAME;

    material_adaptor->configure(
        this->get_id() + "_" + material_adaptor->get_id(),
        _mesh_id + "_" + material_adaptor->get_id(),
        this->getRenderService(),
        m_layerID,
        "",
        tpl_name
    );

    return material_adaptor;
}

//------------------------------------------------------------------------------

void point_list::updateMaterialAdaptor(const std::string& _mesh_id)
{
    if(!m_materialAdaptor)
    {
        if(m_entity != nullptr)
        {
            m_materialAdaptor = this->createMaterialService(_mesh_id);
            m_materialAdaptor->start();

            auto* material_fw = m_materialAdaptor->getMaterialFw();
            m_meshGeometry->updateMaterial(material_fw, false);
            material_fw->setMeshSize(m_radius);

            m_entity->setMaterialName(m_materialAdaptor->getMaterialName());

            if(!m_textureName.empty())
            {
                const auto texture = Ogre::TextureManager::getSingleton().load(
                    m_textureName,
                    sight::viz::scene3d::RESOURCE_GROUP
                );
                Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().getByName(
                    m_materialAdaptor->getMaterialName(),
                    sight::viz::scene3d::RESOURCE_GROUP
                );

                Ogre::TextureUnitState* tex_unit_state = material->getTechnique(0)->getPass(0)->getTextureUnitState(
                    "sprite"
                );
                tex_unit_state->setTexture(texture);
            }

            m_materialAdaptor->update();
        }
    }
    else if(m_materialAdaptor->inout<data::material>(material::s_MATERIAL_INOUT).lock()
            != m_material)
    {
        auto* material_fw = m_materialAdaptor->getMaterialFw();
        m_meshGeometry->updateMaterial(material_fw, false);
        material_fw->setMeshSize(m_radius);
    }
    else
    {
        auto* material_fw = m_materialAdaptor->getMaterialFw();
        m_meshGeometry->updateMaterial(material_fw, false);
        material_fw->setMeshSize(m_radius);

        m_entity->setMaterialName(m_materialAdaptor->getMaterialName());

        m_materialAdaptor->slot(service::slots::UPDATE)->run();
    }
}

//-----------------------------------------------------------------------------

void point_list::attachNode(Ogre::MovableObject* _node)
{
    SIGHT_ASSERT("transform Node shouldn't be null", m_sceneNode);
    m_sceneNode->attachObject(_node);

    // Needed to recompute world bounding boxes of the scene node using its attached mesh bounds
    m_sceneNode->_update(true, false);
}

//-----------------------------------------------------------------------------

void point_list::detachAndDestroyEntity()
{
    if(m_entity != nullptr)
    {
        Ogre::SceneManager* const scene_mgr = this->getSceneManager();
        if(m_sceneNode != nullptr)
        {
            m_sceneNode->detachObject(m_entity);
        }

        scene_mgr->destroyEntity(m_entity);
        m_entity = nullptr;
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.
