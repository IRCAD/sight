/************************************************************************
 *
 * Copyright (C) 2018-2021 IRCAD France
 * Copyright (C) 2018-2020 IHU Strasbourg
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

#include "modules/viz/scene3d/adaptor/SImageMultiDistances.hpp"

#include <core/com/Slots.hxx>

#include <data/Boolean.hpp>
#include <data/Image.hpp>
#include <data/Material.hpp>
#include <data/PointList.hpp>
#include <data/tools/fieldHelper/Image.hpp>
#include <data/tools/helper/Vector.hpp>

#include <service/macros.hpp>

#include <viz/scene3d/helper/ManualObject.hpp>
#include <viz/scene3d/ogre.hpp>
#include <viz/scene3d/picker/IPicker.hpp>

#include <OgreEntity.h>
#include <OgreNode.h>
#include <OgreSceneNode.h>

namespace sight::module::viz::scene3d::adaptor
{

static const service::IService::KeyType s_IMAGE_INOUT = "image";

static const core::com::Signals::SignalKeyType s_ADD_DISTANCES_SLOT                 = "addDistances";
static const core::com::Signals::SignalKeyType s_REMOVE_DISTANCES_SLOT              = "removeDistances";
static const core::com::Signals::SignalKeyType s_UPDATE_VISIBILITY_FROM_FIELDS_SLOT = "updateVisibilityFromField";

static const std::string s_FONT_SOURCE_CONFIG = "fontSource";
static const std::string s_FONT_SIZE_CONFIG   = "fontSize";
static const std::string s_RADIUS_CONFIG      = "radius";
static const std::string s_INTERACTIVE_CONFIG = "interactive";
static const std::string s_PRIORITY_CONFIG    = "priority";
static const std::string s_QUERY_MASK_CONFIG  = "queryMask";
static const std::string s_QUERY_FLAGS_CONFIG = "distanceQueryFlags";

static constexpr std::uint8_t s_DISTANCE_RQ_GROUP_ID = sight::viz::scene3d::compositor::Core::s_SURFACE_RQ_GROUP_ID;

//------------------------------------------------------------------------------

::Ogre::ColourValue SImageMultiDistances::generateColor(core::tools::fwID::IDType _id)
{
    switch(std::hash< std::string >()(_id) % 7)
    {
        case 0:
            return ::Ogre::ColourValue(63/255.0f, 105/255.0f, 170/255.0f);
        case 1:
            return ::Ogre::ColourValue(249/255.0f, 103/255.0f, 20/255.0f);
        case 2:
            return ::Ogre::ColourValue(236/255.0f, 219/255.0f, 84/255.0f);
        case 3:
            return ::Ogre::ColourValue(233/255.0f, 75/255.0f, 60/255.0f);
        case 4:
            return ::Ogre::ColourValue(121/255.0f, 199/255.0f, 83/255.0f);
        case 5:
            return ::Ogre::ColourValue(149/255.0f, 222/255.0f, 227/255.0f);
        default:
            return ::Ogre::ColourValue(29/255.0f, 45/255.0f, 168/255.0f);
    }
}

//------------------------------------------------------------------------------

::Ogre::Vector3 SImageMultiDistances::getCamDirection(const ::Ogre::Camera* const _cam)
{
    const ::Ogre::Matrix4 view = _cam->getViewMatrix();
    ::Ogre::Vector3 direction(view[2][0], view[2][1], view[2][2]);
    direction.normalise();
    return -direction;
}

//------------------------------------------------------------------------------

void SImageMultiDistances::generateDashedLine(::Ogre::ManualObject* const _object,
                                              const ::Ogre::Vector3& _begin,
                                              const ::Ogre::Vector3& _end, float _thickness)
{
    const ::Ogre::Vector3 dashedLineDir = (_end-_begin);
    const float len                     = dashedLineDir.length();
    ::Ogre::Vector3 dashedLineDirN = (_end-_begin);
    dashedLineDirN.normalise();

    ::Ogre::Vector3 dashedLinePos = _begin;
    for(float i = 0.f; i+_thickness*1.5 <= len; i += _thickness*2)
    {
        _object->position(dashedLinePos);
        dashedLinePos += dashedLineDirN*_thickness;
        _object->position(dashedLinePos);
        dashedLinePos += dashedLineDirN*_thickness;
    }
    _object->end();
}

//------------------------------------------------------------------------------

std::string SImageMultiDistances::getLength(const ::Ogre::Vector3& _begin, const ::Ogre::Vector3& _end)
{
    const int length = static_cast< int >(std::round((_end-_begin).length()));
    return std::to_string(length) + "mm";
}

//------------------------------------------------------------------------------

SImageMultiDistances::SImageMultiDistances() noexcept
{
    newSlot(s_ADD_DISTANCES_SLOT, &SImageMultiDistances::addDistances, this);
    newSlot(s_REMOVE_DISTANCES_SLOT, &SImageMultiDistances::removeDistances, this);
    newSlot(s_UPDATE_VISIBILITY_FROM_FIELDS_SLOT, &SImageMultiDistances::updateVisibilityFromField, this);
}

//------------------------------------------------------------------------------

SImageMultiDistances::~SImageMultiDistances() noexcept
{
}

//------------------------------------------------------------------------------

void SImageMultiDistances::configuring()
{
    this->configureParams();

    const ConfigType srvconfig = this->getConfigTree();
    const ConfigType config    = srvconfig.get_child("config.<xmlattr>");

    m_fontSource           = config.get(s_FONT_SOURCE_CONFIG, m_fontSource);
    m_fontSize             = config.get< size_t >(s_FONT_SIZE_CONFIG, m_fontSize);
    m_distanceSphereRadius = config.get< float >(s_RADIUS_CONFIG, m_distanceSphereRadius);
    m_interactive          = config.get<bool>(s_INTERACTIVE_CONFIG, m_interactive);
    m_priority             = config.get< int >(s_PRIORITY_CONFIG, m_priority);

    std::string hexaMask = config.get<std::string>(s_QUERY_MASK_CONFIG, "");
    if(!hexaMask.empty())
    {
        SLM_ASSERT(
            "Hexadecimal values should start with '0x'"
            "Given value : " + hexaMask,
            hexaMask.length() > 2 &&
            hexaMask.substr(0, 2) == "0x");
        m_queryMask = static_cast< std::uint32_t >(std::stoul(hexaMask, nullptr, 16));
    }

    hexaMask = config.get<std::string>(s_QUERY_FLAGS_CONFIG, "");
    if(!hexaMask.empty())
    {
        SLM_ASSERT(
            "Hexadecimal values should start with '0x'"
            "Given value : " + hexaMask,
            hexaMask.length() > 2 &&
            hexaMask.substr(0, 2) == "0x");
        m_distanceQueryFlag = static_cast< std::uint32_t >(std::stoul(hexaMask, nullptr, 16));
    }

}

//------------------------------------------------------------------------------

void SImageMultiDistances::starting()
{
    this->initialize();

    this->getRenderService()->makeCurrent();

    const sight::viz::scene3d::Layer::sptr layer = this->getLayer();

    m_sphereMaterialName     = this->getID() + "_sphereMaterialName";
    m_lineMaterialName       = this->getID() + "_lineMaterialNamel";
    m_dashedLineMaterialName = this->getID() + "_dashedLineMaterialName";

    // Create materials from our wrapper.
    m_sphereMaterial = std::make_unique< sight::viz::scene3d::Material >(m_sphereMaterialName,
                                                                         sight::viz::scene3d::Material::DEFAULT_MATERIAL_TEMPLATE_NAME);
    m_sphereMaterial->setHasVertexColor(true);
    m_sphereMaterial->updateShadingMode(data::Material::PHONG, layer->getLightsNumber(), false, false);

    m_lineMaterial = std::make_unique< sight::viz::scene3d::Material >(m_lineMaterialName,
                                                                       sight::viz::scene3d::Material::DEFAULT_MATERIAL_TEMPLATE_NAME);
    m_lineMaterial->setHasVertexColor(true);
    m_lineMaterial->updateShadingMode(data::Material::AMBIENT, layer->getLightsNumber(), false, false);

    m_dashedLineMaterial = std::make_unique< sight::viz::scene3d::Material >(m_dashedLineMaterialName,
                                                                             sight::viz::scene3d::Material::DEFAULT_MATERIAL_TEMPLATE_NAME);
    m_dashedLineMaterial->setHasVertexColor(true);
    m_dashedLineMaterial->updateShadingMode(data::Material::AMBIENT, layer->getLightsNumber(), false, false);

    // Retrive the ogre material to change the depth check.
    const ::Ogre::MaterialPtr ogreSphereMaterial = ::Ogre::MaterialManager::getSingleton().getByName(
        m_sphereMaterialName, sight::viz::scene3d::RESOURCE_GROUP);
    SLM_ASSERT("'" + m_sphereMaterialName + "' does not exist.", ogreSphereMaterial);
    const ::Ogre::Technique* const sphereTech = ogreSphereMaterial->getTechnique(0);
    SLM_ASSERT("No techique found", sphereTech);
    ::Ogre::Pass* const spherePass = sphereTech->getPass(0);
    SLM_ASSERT("No pass found", spherePass);
    spherePass->setDepthCheckEnabled(false);

    const ::Ogre::MaterialPtr ogreDashedLineMaterial = ::Ogre::MaterialManager::getSingleton().getByName(
        m_dashedLineMaterialName, sight::viz::scene3d::RESOURCE_GROUP);
    SLM_ASSERT("'" + m_dashedLineMaterialName + "' does not exist.", ogreDashedLineMaterial);
    const ::Ogre::Technique* const dashedTech = ogreDashedLineMaterial->getTechnique(0);
    SLM_ASSERT("No techique found", dashedTech);
    ::Ogre::Pass* const dashedPass = dashedTech->getPass(0);
    SLM_ASSERT("No pass found", dashedPass);
    dashedPass->setDepthCheckEnabled(false);

    if(m_interactive)
    {
        auto interactor = std::dynamic_pointer_cast< sight::viz::scene3d::interactor::IInteractor >(this->getSptr());
        layer->addInteractor(interactor, m_priority);
    }
}

//------------------------------------------------------------------------------

service::IService::KeyConnectionsMap SImageMultiDistances::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push(s_IMAGE_INOUT, data::Image::s_DISTANCE_ADDED_SIG, s_ADD_DISTANCES_SLOT);
    connections.push(s_IMAGE_INOUT, data::Image::s_DISTANCE_REMOVED_SIG, s_REMOVE_DISTANCES_SLOT);
    connections.push(s_IMAGE_INOUT, data::Image::s_DISTANCE_DISPLAYED_SIG, s_UPDATE_VISIBILITY_SLOT);
    connections.push(s_IMAGE_INOUT, data::Image::s_MODIFIED_SIG, s_UPDATE_SLOT);
    return connections;
}

//------------------------------------------------------------------------------

void SImageMultiDistances::updating()
{
    const sight::viz::scene3d::Layer::csptr layer = this->getLayer();

    m_sphereMaterial->updateShadingMode(data::Material::PHONG, layer->getLightsNumber(), false, false);
    m_lineMaterial->updateShadingMode(data::Material::AMBIENT, layer->getLightsNumber(), false, false);
    m_dashedLineMaterial->updateShadingMode(data::Material::AMBIENT, layer->getLightsNumber(), false, false);

    while(m_distances.size() != 0)
    {
        this->destroyDistance(m_distances.begin()->first);
    }
    this->addDistances();
}

//------------------------------------------------------------------------------

void SImageMultiDistances::stopping()
{
    this->getRenderService()->makeCurrent();

    m_sphereMaterial.reset();
    m_lineMaterial.reset();
    m_dashedLineMaterial.reset();

    if(m_interactive)
    {
        auto interactor = std::dynamic_pointer_cast< sight::viz::scene3d::interactor::IInteractor >(this->getSptr());
        this->getLayer()->removeInteractor(interactor);
    }

    while(m_distances.size() != 0)
    {
        this->destroyDistance(m_distances.begin()->first);
    }
}

//------------------------------------------------------------------------------

void SImageMultiDistances::addDistances()
{
    this->getRenderService()->makeCurrent();

    const auto image = this->getLockedInOut< data::Image >(s_IMAGE_INOUT);

    const data::Vector::sptr distanceField = image->getField< data::Vector >(
        data::tools::fieldHelper::Image::m_imageDistancesId);
    if(distanceField)
    {
        for(const auto& object : *distanceField)
        {
            const data::PointList::sptr pointList = data::PointList::dynamicCast(object);
            SLM_ASSERT("The distance should be a point list", pointList);
            SLM_ASSERT("The distance must contains two points", pointList->getPoints().size() == 2);

            const core::tools::fwID::IDType id = pointList->getID();
            if(m_distances.find(id) == m_distances.end())
            {
                this->createDistance(pointList);
                const auto& sigModified = pointList->signal< data::PointList::ModifiedSignalType >(
                    data::PointList::s_MODIFIED_SIG);
                sigModified->connect(m_slotUpdate);
            }
        }
    }
    // The signal data::Image::s_DISTANCE_ADDED_SIG is send if all distances are removed.
    // When all distances are removed, the field is removed in the image.
    else
    {
        while(m_distances.size() != 0)
        {
            this->destroyDistance(m_distances.begin()->first);
        }
    }

    this->requestRender();
}

//------------------------------------------------------------------------------

void SImageMultiDistances::removeDistances()
{
    this->getRenderService()->makeCurrent();

    const auto image = this->getLockedInOut< data::Image >(s_IMAGE_INOUT);

    const data::Vector::csptr distanceField
        = image->getField< data::Vector >(data::tools::fieldHelper::Image::m_imageDistancesId);

    std::vector< core::tools::fwID::IDType > foundId;
    if(distanceField)
    {
        for(const auto& object : *distanceField)
        {
            foundId.push_back(object->getID());
        }
    }

    std::vector< core::tools::fwID::IDType > currentdId;
    for(const auto& [id, _] : m_distances)
    {
        currentdId.push_back(id);
    }

    for(const core::tools::fwID::IDType& id : currentdId)
    {
        if(std::find(foundId.begin(), foundId.end(), id) == foundId.end())
        {
            destroyDistance(id);
        }
    }

    this->requestRender();
}

//------------------------------------------------------------------------------

void SImageMultiDistances::updateVisibilityFromField()
{
    this->getRenderService()->makeCurrent();

    const auto image = this->getLockedInOut< data::Image >(s_IMAGE_INOUT);

    const bool visibility = image->getField(data::tools::fieldHelper::Image::m_distanceVisibility, data::Boolean::New(
                                                true))->value();
    m_isVisible = visibility;

    for(const auto& distance : m_distances)
    {
        const DistanceData& data = distance.second;
        data.m_sphere1->setVisible(m_isVisible);
        data.m_sphere2->setVisible(m_isVisible);
        data.m_line->setVisible(m_isVisible);
        data.m_dashedLine->setVisible(m_isVisible);
        data.m_label->setVisible(m_isVisible);
    }

    this->requestRender();
}

//------------------------------------------------------------------------------

void SImageMultiDistances::setVisible(bool _visible)
{
    this->getRenderService()->makeCurrent();

    for(const auto& distance : m_distances)
    {
        const DistanceData& data = distance.second;
        data.m_sphere1->setVisible(_visible);
        data.m_sphere2->setVisible(_visible);
        data.m_line->setVisible(_visible);
        data.m_dashedLine->setVisible(_visible);
        data.m_label->setVisible(_visible);
    }

    this->requestRender();
}

//------------------------------------------------------------------------------

std::optional< ::Ogre::Vector3 > SImageMultiDistances::getNearestPickedPosition(int _x, int _y)
{
    sight::viz::scene3d::picker::IPicker picker;
    ::Ogre::SceneManager* sm = this->getSceneManager();
    picker.setSceneManager(sm);
    picker.executeRaySceneQuery(_x, _y, m_queryMask);

    if(picker.getSelectedObject())
    {
        const auto* const camera = sm->getCamera(sight::viz::scene3d::Layer::s_DEFAULT_CAMERA_NAME);
        const auto* const vp     = camera->getViewport();

        // Screen to viewport space conversion.
        const float vpX = static_cast<float>(_x - vp->getActualLeft()) / static_cast<float>(vp->getActualWidth());
        const float vpY = static_cast<float>(_y - vp->getActualTop())  / static_cast<float>(vp->getActualHeight());

        const ::Ogre::Ray ray = camera->getCameraToViewportRay(vpX, vpY);

        ::Ogre::Vector3 normal = -ray.getDirection();
        normal.normalise();

        return picker.getIntersectionInWorldSpace() + normal*0.01f;
    }

    return std::nullopt;
}

//------------------------------------------------------------------------------

void SImageMultiDistances::buttonPressEvent(MouseButton _button, Modifier, int _x, int _y)
{
    if(_button == LEFT)
    {
        const sight::viz::scene3d::Layer::csptr layer = this->getLayer();

        ::Ogre::SceneManager* const sceneMgr = layer->getSceneManager();

        const ::Ogre::Camera* const cam = layer->getDefaultCamera();
        const auto* const vp            = cam->getViewport();

        const float vpX = static_cast<float>(_x - vp->getActualLeft()) / static_cast<float>(vp->getActualWidth());
        const float vpY = static_cast<float>(_y - vp->getActualTop())  / static_cast<float>(vp->getActualHeight());

        const ::Ogre::Ray ray = cam->getCameraToViewportRay(vpX, vpY);

        bool found = false;
        ::Ogre::RaySceneQuery* const raySceneQuery = sceneMgr->createRayQuery(ray, m_distanceQueryFlag);
        raySceneQuery->setSortByDistance(false);
        if (raySceneQuery->execute().size() != 0)
        {
            const ::Ogre::Real scale = 1.15f;

            const ::Ogre::RaySceneQueryResult& queryResult = raySceneQuery->getLastResults();
            for (size_t qrIdx = 0; qrIdx < queryResult.size() && !found; qrIdx++)
            {
                const ::Ogre::MovableObject* const object = queryResult[qrIdx].movable;
                if(object->isVisible())
                {
                    for(auto& distance : m_distances)
                    {
                        DistanceData& distanceData = distance.second;
                        if(distanceData.m_sphere1 == object)
                        {
                            distanceData.m_node1->setScale(scale, scale, scale);
                            m_pickedData = {&distanceData, true};
                            found        = true;
                            break;
                        }
                        else if(distanceData.m_sphere2 == object)
                        {
                            distanceData.m_node2->setScale(scale, scale, scale);
                            m_pickedData = {&distanceData, false};
                            found        = true;
                            break;
                        }
                    }
                }
            }
        }
        delete raySceneQuery;

        if(found)
        {
            // Check if something is picked to update the position of the distance.
            std::optional< ::Ogre::Vector3 > pickedPos = this->getNearestPickedPosition(_x, _y);
            if(pickedPos.has_value())
            {
                if(m_pickedData.m_first)
                {
                    const ::Ogre::Vector3 secondPos = m_pickedData.m_data->m_node2->getPosition();
                    this->updateDistance(m_pickedData.m_data, pickedPos.value(), secondPos);
                }
                else
                {
                    const ::Ogre::Vector3 firstPos = m_pickedData.m_data->m_node1->getPosition();
                    this->updateDistance(m_pickedData.m_data, firstPos, pickedPos.value());
                }
            }

            this->requestRender();
            this->getLayer()->cancelFurtherInteraction();
        }
    }
}

//------------------------------------------------------------------------------

void SImageMultiDistances::mouseMoveEvent(MouseButton, Modifier, int _x, int _y, int, int)
{
    if(m_pickedData.m_data != nullptr)
    {
        ::Ogre::Vector3 newPos;

        // Discard the current distance to launch the ray over the scene without picking this one.
        m_pickedData.m_data->m_sphere1->setQueryFlags(0x0);
        m_pickedData.m_data->m_sphere2->setQueryFlags(0x0);

        // Check if something is picked.
        std::optional< ::Ogre::Vector3 > pickedPos = this->getNearestPickedPosition(_x, _y);
        if(pickedPos.has_value())
        {
            newPos = pickedPos.value();
        }
        // Else we move the distance along a plane.
        else
        {
            const sight::viz::scene3d::Layer::sptr layer = this->getLayer();

            const ::Ogre::Camera* const cam = layer->getDefaultCamera();
            const auto* const vp            = cam->getViewport();

            const float vpX = static_cast<float>(_x - vp->getActualLeft()) / static_cast<float>(vp->getActualWidth());
            const float vpY = static_cast<float>(_y - vp->getActualTop())  / static_cast<float>(vp->getActualHeight());

            const ::Ogre::Ray ray           = cam->getCameraToViewportRay(vpX, vpY);
            const ::Ogre::Vector3 direction = this->getCamDirection(cam);

            ::Ogre::Vector3 position;
            if(m_pickedData.m_first)
            {
                position = m_pickedData.m_data->m_node1->getPosition();
            }
            else
            {
                position = m_pickedData.m_data->m_node2->getPosition();
            }

            const ::Ogre::Plane plane(direction, position);

            const std::pair< bool, ::Ogre::Real > hit = ::Ogre::Math::intersects(ray, plane);

            if(!hit.first)
            {
                SLM_ERROR("The ray must intersect the plane")
                return;
            }

            newPos = ray.getPoint(hit.second);
        }

        // Reset the query flag.
        m_pickedData.m_data->m_sphere1->setQueryFlags(m_distanceQueryFlag);
        m_pickedData.m_data->m_sphere2->setQueryFlags(m_distanceQueryFlag);

        if(m_pickedData.m_first)
        {
            const ::Ogre::Vector3 secondPos = m_pickedData.m_data->m_node2->getPosition();
            this->updateDistance(m_pickedData.m_data, newPos, secondPos);
        }
        else
        {
            const ::Ogre::Vector3 firstPos = m_pickedData.m_data->m_node1->getPosition();
            this->updateDistance(m_pickedData.m_data, firstPos, newPos);
        }

        this->requestRender();
        this->getLayer()->cancelFurtherInteraction();
    }
}

//------------------------------------------------------------------------------

void SImageMultiDistances::buttonReleaseEvent(MouseButton, Modifier, int, int)
{
    if(m_pickedData.m_data != nullptr)
    {
        const ::Ogre::Real scale = 1.f;
        m_pickedData.m_data->m_node1->setScale(scale, scale, scale);
        m_pickedData.m_data->m_node2->setScale(scale, scale, scale);
        m_pickedData = {nullptr, true};

        this->getLayer()->requestRender();
    }
}

//------------------------------------------------------------------------------

void SImageMultiDistances::createDistance(data::PointList::sptr _pl)
{
    const core::tools::fwID::IDType id = _pl->getID();
    SLM_ASSERT("The distance already exist", m_distances.find(id) == m_distances.end());

    ::Ogre::SceneManager* const sceneMgr = this->getSceneManager();
    ::Ogre::SceneNode* const rootNode    = sceneMgr->getRootSceneNode();

    // Retrieve data used to create Ogre resources.
    const ::Ogre::ColourValue colour = SImageMultiDistances::generateColor(id);

    const std::array< double, 3 > front = _pl->getPoints().front()->getCoord();
    const std::array< double, 3 > back  = _pl->getPoints().back()->getCoord();

    const ::Ogre::Vector3 begin(static_cast< float >(front[0]),
                                static_cast< float >(front[1]),
                                static_cast< float >(front[2]));
    const ::Ogre::Vector3 end(static_cast< float >(back[0]),
                              static_cast< float >(back[1]),
                              static_cast< float >(back[2]));

    // First sphere.
    ::Ogre::ManualObject* const sphere1 = sceneMgr->createManualObject(this->getID() + "_sphere1_" + id);
    sight::viz::scene3d::helper::ManualObject::createSphere(sphere1, m_sphereMaterialName, colour,
                                                            m_distanceSphereRadius);
    sphere1->setQueryFlags(m_distanceQueryFlag);
    // Render this sphere over all others objects.
    sphere1->setRenderQueueGroup(s_DISTANCE_RQ_GROUP_ID);
    SLM_ASSERT("Can't create the first entity", sphere1);
    ::Ogre::SceneNode* const node1 = rootNode->createChildSceneNode(this->getID() + "_node1_" + id, begin);
    SLM_ASSERT("Can't create the first node", node1);
    node1->attachObject(sphere1);

    // Second sphere.
    ::Ogre::ManualObject* const sphere2 = sceneMgr->createManualObject(this->getID() + "_sphere2_" + id);
    sight::viz::scene3d::helper::ManualObject::createSphere(sphere2, m_sphereMaterialName, colour,
                                                            m_distanceSphereRadius);
    sphere2->setQueryFlags(m_distanceQueryFlag);
    // Render this sphere over all others objects.
    sphere2->setRenderQueueGroup(s_DISTANCE_RQ_GROUP_ID);
    SLM_ASSERT("Can't create the second entity", sphere2);
    ::Ogre::SceneNode* const node2 = rootNode->createChildSceneNode(this->getID() + "_node2_" + id, end);
    SLM_ASSERT("Can't create the second node", node2);
    node2->attachObject(sphere2);

    // Line.
    ::Ogre::ManualObject* const line = sceneMgr->createManualObject(this->getID() + "_line_" + id);
    SLM_ASSERT("Can't create the line", line);
    line->begin(m_lineMaterialName, ::Ogre::RenderOperation::OT_LINE_LIST, sight::viz::scene3d::RESOURCE_GROUP);
    line->colour(colour);
    line->position(begin);
    line->position(end);
    line->end();
    line->setQueryFlags(0x0);
    rootNode->attachObject(line);

    // Dashed line.
    ::Ogre::ManualObject* const dashedLine = sceneMgr->createManualObject(this->getID() + "_dashedLine_" + id);
    SLM_ASSERT("Can't create the dashed line", dashedLine);
    dashedLine->begin(m_dashedLineMaterialName, ::Ogre::RenderOperation::OT_LINE_LIST,
                      sight::viz::scene3d::RESOURCE_GROUP);
    dashedLine->colour(colour);
    SImageMultiDistances::generateDashedLine(dashedLine, begin, end,
                                             m_distanceSphereRadius);
    dashedLine->setQueryFlags(0x0);
    // Render this line over all others objects.
    dashedLine->setRenderQueueGroup(s_DISTANCE_RQ_GROUP_ID);
    rootNode->attachObject(dashedLine);

    // Label.
    const sight::viz::scene3d::Layer::sptr layer = this->getLayer();
    ::Ogre::OverlayContainer* const textContainer = layer->getOverlayTextPanel();
    ::Ogre::Camera* const cam                     = layer->getDefaultCamera();
    const float dpi                  = this->getRenderService()->getInteractorManager()->getLogicalDotsPerInch();
    sight::viz::scene3d::Text* label = sight::viz::scene3d::Text::New(
        this->getID() + "_label_" + id, sceneMgr, textContainer, m_fontSource, m_fontSize, dpi, cam);
    const std::string length = SImageMultiDistances::getLength(end, begin);
    label->setText(length);
    label->setTextColor(colour);
    label->setQueryFlags(0x0);
    ::Ogre::SceneNode* const labelNode =
        rootNode->createChildSceneNode(this->getID() + "_labelNode_" + id, end);
    SLM_ASSERT("Can't create the label node", labelNode);
    labelNode->attachObject(label);

    // Set the visibility.
    sphere1->setVisible(m_isVisible);
    sphere2->setVisible(m_isVisible);
    line->setVisible(m_isVisible);
    dashedLine->setVisible(m_isVisible);
    label->setVisible(m_isVisible);

    // Store data in the map.
    DistanceData data {_pl, node1, sphere1, node2, sphere2, line, dashedLine, labelNode, label};
    m_distances[id] = data;
}

//------------------------------------------------------------------------------

void SImageMultiDistances::updateDistance(const DistanceData* const _data,
                                          ::Ogre::Vector3 _begin,
                                          ::Ogre::Vector3 _end)
{
    SLM_ASSERT("Distance can't be null", _data);

    // Update spheres position.
    _data->m_node1->setPosition(_begin);
    _data->m_node2->setPosition(_end);

    // Update the line.
    ::Ogre::ManualObject* const line = _data->m_line;
    line->beginUpdate(0);
    line->position(_begin);
    line->position(_end);
    line->end();

    // Update the label.
    const std::string length = SImageMultiDistances::getLength(_end, _begin);
    _data->m_label->setText(length);
    _data->m_labelNode->setPosition(_end);

    // Update the dashed line
    ::Ogre::ManualObject* const dashedLine = _data->m_dashedLine;
    dashedLine->beginUpdate(0);
    SImageMultiDistances::generateDashedLine(dashedLine, _begin, _end, m_distanceSphereRadius);

    // Update the field data.
    const data::mt::locked_ptr lock(_data->m_pointList);
    _data->m_pointList->getPoints().front()->setCoord({_begin[0], _begin[1], _begin[2]});
    _data->m_pointList->getPoints().back()->setCoord({_end[0], _end[1], _end[2]});

    const auto& sigModified = _data->m_pointList->signal< data::PointList::ModifiedSignalType >(
        data::PointList::s_MODIFIED_SIG);

    core::com::Connection::Blocker blocker(sigModified->getConnection(m_slotUpdate));
    sigModified->asyncEmit();

    this->requestRender();
}

//------------------------------------------------------------------------------

void SImageMultiDistances::destroyDistance(core::tools::fwID::IDType _id)
{
    const DistanceMap::const_iterator it = m_distances.find(_id);
    SLM_ASSERT("The distance is not found", it != m_distances.end());

    // Destroy Ogre resource.
    const DistanceData distanceData = it->second;
    ::Ogre::SceneManager* const sceneMgr = this->getSceneManager();

    sceneMgr->destroySceneNode(distanceData.m_node1);
    sceneMgr->destroyManualObject(distanceData.m_sphere1);
    sceneMgr->destroySceneNode(distanceData.m_node2);
    sceneMgr->destroyManualObject(distanceData.m_sphere2);
    sceneMgr->destroyManualObject(distanceData.m_line);
    sceneMgr->destroyManualObject(distanceData.m_dashedLine);
    sceneMgr->destroySceneNode(distanceData.m_labelNode);
    sceneMgr->destroyMovableObject(distanceData.m_label);

    const auto& sigModified = distanceData.m_pointList->signal< data::PointList::ModifiedSignalType >(
        data::PointList::s_MODIFIED_SIG);
    sigModified->disconnect(m_slotUpdate);

    // Remove it from the map.
    m_distances.erase(it);
}

//------------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.
