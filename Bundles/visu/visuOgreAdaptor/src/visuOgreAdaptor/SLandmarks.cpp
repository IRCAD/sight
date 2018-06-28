/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuOgreAdaptor/SLandmarks.hpp"

#include "visuOgreAdaptor/STransform.hpp"

#include <fwData/Landmarks.hpp>
#include <fwData/mt/ObjectReadLock.hpp>

#include <fwRenderOgre/helper/Font.hpp>
#include <fwRenderOgre/helper/ManualObject.hpp>
#include <fwRenderOgre/helper/Scene.hpp>

#include <fwServices/macros.hpp>

namespace visuOgreAdaptor
{

fwServicesRegisterMacro(::fwRenderOgre::IAdaptor, ::visuOgreAdaptor::SLandmarks);

static const std::string s_LANDMARKS_INPUT = "landmarks";

//-----------------------------------------------------------------------------

SLandmarks::SLandmarks() noexcept
{
}

//-----------------------------------------------------------------------------

SLandmarks::~SLandmarks() noexcept
{
}

//-----------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SLandmarks::getAutoConnections() const
{
    ::fwServices::IService::KeyConnectionsMap connections;

    connections.push(s_CONFIG_TRANSFORM, ::fwData::TransformationMatrix3D::s_MODIFIED_SIG, s_UPDATE_SLOT);

    connections.push(s_LANDMARKS_INPUT, ::fwData::Landmarks::s_MODIFIED_SIG, s_UPDATE_SLOT );
    connections.push(s_LANDMARKS_INPUT, ::fwData::Landmarks::s_GROUP_ADDED_SIG, s_UPDATE_SLOT );
    connections.push(s_LANDMARKS_INPUT, ::fwData::Landmarks::s_GROUP_REMOVED_SIG, s_UPDATE_SLOT );
    connections.push(s_LANDMARKS_INPUT, ::fwData::Landmarks::s_POINT_ADDED_SIG, s_UPDATE_SLOT );
    connections.push(s_LANDMARKS_INPUT, ::fwData::Landmarks::s_POINT_REMOVED_SIG, s_UPDATE_SLOT );
    connections.push(s_LANDMARKS_INPUT, ::fwData::Landmarks::s_POINT_INSERTED_SIG, s_UPDATE_SLOT );
    connections.push(s_LANDMARKS_INPUT, ::fwData::Landmarks::s_POINT_MODIFIED_SIG, s_UPDATE_SLOT );
    connections.push(s_LANDMARKS_INPUT, ::fwData::Landmarks::s_GROUP_MODIFIED_SIG, s_UPDATE_SLOT );
    connections.push(s_LANDMARKS_INPUT, ::fwData::Landmarks::s_GROUP_RENAMED_SIG, s_UPDATE_SLOT );

    return connections;
}

//-----------------------------------------------------------------------------

void SLandmarks::configuring()
{
    this->configureParams();

    const ConfigType configType = this->getConfigTree();
    const ConfigType config     = configType.get_child("config.<xmlattr>");

    this->setTransformId(config.get<std::string>( ::fwRenderOgre::ITransformable::s_CONFIG_TRANSFORM,
                                                  this->getID() + "_transform"));
}

//-----------------------------------------------------------------------------

void SLandmarks::starting()
{
    this->initialize();

    this->getRenderService()->makeCurrent();

    ::Ogre::SceneNode* rootSceneNode = this->getSceneManager()->getRootSceneNode();
    m_transNode                      = this->getTransformNode(rootSceneNode);

    m_material = ::fwData::Material::New();

    m_materialAdaptor = this->registerService< ::visuOgreAdaptor::SMaterial >("::visuOgreAdaptor::SMaterial");
    m_materialAdaptor->registerInOut(m_material, ::visuOgreAdaptor::SMaterial::s_MATERIAL_INOUT, true);
    m_materialAdaptor->setID(this->getID() + m_materialAdaptor->getID());
    m_materialAdaptor->setMaterialName(this->getID() + m_materialAdaptor->getID());
    m_materialAdaptor->setRenderService( this->getRenderService() );
    m_materialAdaptor->setLayerID(m_layerID);
    m_materialAdaptor->setMaterialTemplateName(::fwRenderOgre::Material::DEFAULT_MATERIAL_TEMPLATE_NAME);
    m_materialAdaptor->start();

    m_materialAdaptor->getMaterialFw()->setHasVertexColor(true);
    m_materialAdaptor->update();

    m_text = this->getRenderService()->getOverlayTextPanel();
    m_font = ::fwRenderOgre::helper::Font::getFont("DejaVuSans.ttf", 32);

    this->updating();
}

//-----------------------------------------------------------------------------

void SLandmarks::updating()
{
    this->getRenderService()->makeCurrent();

    this->clearData();

    ::Ogre::Camera* cam            = this->getLayer()->getDefaultCamera();
    ::Ogre::SceneManager* sceneMgr = this->getSceneManager();

    const ::fwData::Landmarks::csptr landmarks = this->getInput< ::fwData::Landmarks >(s_LANDMARKS_INPUT);
    SLM_ASSERT("Input '" + s_LANDMARKS_INPUT + "' is missing", landmarks);

    const ::fwData::mt::ObjectReadLock lockLandmark(landmarks);
    {
        const ::fwData::Landmarks::GroupNameContainer groupNames = landmarks->getGroupNames();
        for(const std::string& groupName : groupNames)
        {
            const ::fwData::Landmarks::LandmarksGroup& group = landmarks->getGroup(groupName);
            const size_t ptNumber                            = landmarks->getNumberOfPoints(groupName);
            for(size_t index = 0; index < ptNumber; ++index)
            {
                const std::string baseName = this->getID() + "_" + groupName + "_" + std::to_string(index);

                const std::string objectName = baseName + "_object";
                ::Ogre::ManualObject* object = sceneMgr->createManualObject(objectName);
                m_manualObjects.push_back(object);

                const std::string textName = baseName + "_text";
                ::fwRenderOgre::Text* text
                    = fwRenderOgre::Text::New(textName, sceneMgr, m_text, m_font, cam);
                m_labels.push_back(text);

                const ::Ogre::ColourValue color
                    = ::Ogre::ColourValue(group.m_color[0], group.m_color[1], group.m_color[2], group.m_color[3]);

                text->setText(groupName + "_" + std::to_string(index));
                text->setCharHeight(0.025f);
                text->setTextColor(color);

                switch(group.m_shape)
                {
                    case ::fwData::Landmarks::Shape::SPHERE:
                        ::fwRenderOgre::helper::ManualObject::createSphere(object,
                                                                           m_materialAdaptor->getMaterialName(), color,
                                                                           group.m_size/2.0f);
                        object->setBoundingBox(::Ogre::AxisAlignedBox(::Ogre::Vector3(-group.m_size/4.0f,
                                                                                      -group.m_size/4.0f,
                                                                                      -group.m_size/4.0f),
                                                                      ::Ogre::Vector3(group.m_size/4.0f,
                                                                                      group.m_size/4.0f,
                                                                                      group.m_size/4.0f)));
                        break;
                    case ::fwData::Landmarks::Shape::CUBE:
                        ::fwRenderOgre::helper::ManualObject::createCube(object,
                                                                         m_materialAdaptor->getMaterialName(), color,
                                                                         group.m_size);
                        object->setBoundingBox(::Ogre::AxisAlignedBox(::Ogre::Vector3(-group.m_size/2.0f,
                                                                                      -group.m_size/2.0f,
                                                                                      -group.m_size/2.0f),
                                                                      ::Ogre::Vector3(group.m_size/2.0f,
                                                                                      group.m_size/2.0f,
                                                                                      group.m_size/2.0f)));
                        break;
                }

                ::Ogre::SceneNode* node = m_transNode->createChildSceneNode(baseName + "_node");

                node->attachObject(object);
                node->attachObject(text);

                const ::fwData::Landmarks::PointType& point = landmarks->getPoint(groupName, index);
                node->setPosition(::Ogre::Real(point[0]), ::Ogre::Real(point[1]), ::Ogre::Real(point[2]));
            }
        }
    }

    this->requestRender();
}

//-----------------------------------------------------------------------------

void SLandmarks::stopping()
{
    this->getRenderService()->makeCurrent();

    this->clearData();

    this->unregisterServices();
    m_materialAdaptor.reset();
    m_material.reset();
    m_font.reset();
}

//-----------------------------------------------------------------------------

void SLandmarks::clearData()
{
    ::Ogre::SceneManager* sceneMgr = this->getSceneManager();

    for(::Ogre::ManualObject* object : m_manualObjects)
    {
        sceneMgr->destroyManualObject(object);
    }
    m_manualObjects.clear();

    for(::fwRenderOgre::Text* label : m_labels)
    {
        sceneMgr->destroyMovableObject(label);
    }
    m_labels.clear();

    if(m_transNode != nullptr)
    {
        m_transNode->removeAndDestroyAllChildren();
    }
}

} //visuOgreAdaptor
