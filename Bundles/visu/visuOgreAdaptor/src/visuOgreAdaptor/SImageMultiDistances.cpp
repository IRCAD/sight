/************************************************************************
 *
 * Copyright (C) 2018-2019 IRCAD France
 * Copyright (C) 2018-2019 IHU Strasbourg
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

#include "visuOgreAdaptor/SImageMultiDistances.hpp"

#include <fwCom/Slots.hxx>

#include <fwData/Boolean.hpp>
#include <fwData/Image.hpp>
#include <fwData/Material.hpp>
#include <fwData/mt/ObjectReadLock.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>
#include <fwData/PointList.hpp>

#include <fwDataTools/fieldHelper/Image.hpp>
#include <fwDataTools/helper/Array.hpp>
#include <fwDataTools/helper/Mesh.hpp>
#include <fwDataTools/helper/Vector.hpp>

#include <fwRenderOgre/helper/ManualObject.hpp>

#include <fwServices/macros.hpp>

#include <OgreEntity.h>
#include <OgreNode.h>
#include <OgreSceneNode.h>

namespace visuOgreAdaptor
{

static const ::fwServices::IService::KeyType s_IMAGE_INOUT = "image";

static const ::fwCom::Signals::SignalKeyType s_ADD_DISTANCES_SLOT     = "addDistances";
static const ::fwCom::Signals::SignalKeyType s_REMOVE_DISTANCES_SLOT  = "removeDistances";
static const ::fwCom::Signals::SignalKeyType s_UPDATE_VISIBILITY_SLOT = "updateVisibility";

static const std::string s_FONT_SOURCE_CONFIG = "fontSource";
static const std::string s_FONT_SIZE_CONFIG   = "fontSize";
static const std::string s_RADIUS_CONFIG      = "radius";
static const std::string s_PRIORITY_CONFIG    = "priority";

fwServicesRegisterMacro( ::fwRenderOgre::IAdaptor, ::visuOgreAdaptor::SImageMultiDistances)

//------------------------------------------------------------------------------

::Ogre::Vector3 SImageMultiDistances::getCamDirection(const ::Ogre::Camera* const _cam)
{
    const ::Ogre::Matrix4 view = _cam->getViewMatrix();
    ::Ogre::Vector3 direction(view[2][0], view[2][1], view[2][2]);
    direction.normalise();
    return -direction;
}

//------------------------------------------------------------------------------

std::string SImageMultiDistances::getLength(const ::Ogre::Vector3& _begin, const ::Ogre::Vector3& _end)
{
    const int length = static_cast<int>(std::round(std::abs((_end-_begin).length())));
    return std::to_string(length) + "mm";
}

//------------------------------------------------------------------------------

void SImageMultiDistances::generateDashedLine(::Ogre::ManualObject* const _object, const ::Ogre::Vector3& _begin,
                                              const ::Ogre::Vector3& _end, float _thickness)
{
    ::Ogre::Vector3 dashedLineDirN = (_end-_begin);
    dashedLineDirN.normalise();
    const ::Ogre::Vector3 dashedLineBegin = _begin + dashedLineDirN*_thickness;
    const ::Ogre::Vector3 dashedLineEnd   = _end - dashedLineDirN*_thickness;
    const float len                       = (dashedLineEnd-dashedLineBegin).length();

    ::Ogre::Vector3 dashedLinePos = dashedLineBegin;
    for(float i = 0.f; i+_thickness <= len; i += _thickness*2)
    {
        _object->position(dashedLinePos);
        dashedLinePos += dashedLineDirN*_thickness;
        _object->position(dashedLinePos);
        dashedLinePos += dashedLineDirN*_thickness;
    }
    _object->end();
}

//------------------------------------------------------------------------------

::Ogre::ColourValue SImageMultiDistances::generateColor(::fwTools::fwID::IDType _id)
{
    switch(std::hash<std::string>()(_id) % 7)
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

SImageMultiDistances::SImageMultiDistances() noexcept
{
    newSlot(s_ADD_DISTANCES_SLOT, &SImageMultiDistances::addDistances, this);
    newSlot(s_REMOVE_DISTANCES_SLOT, &SImageMultiDistances::removeDistances, this);
    newSlot(s_UPDATE_VISIBILITY_SLOT, &SImageMultiDistances::updateVisibility, this);
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
    m_fontSize             = config.get<size_t>(s_FONT_SIZE_CONFIG, m_fontSize);
    m_distanceSphereRadius = config.get<float>(s_RADIUS_CONFIG, m_distanceSphereRadius);
    m_priority             = config.get<int>(s_PRIORITY_CONFIG, m_priority);
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SImageMultiDistances::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push(s_IMAGE_INOUT, ::fwData::Image::s_DISTANCE_ADDED_SIG, s_ADD_DISTANCES_SLOT);
    connections.push(s_IMAGE_INOUT, ::fwData::Image::s_DISTANCE_REMOVED_SIG, s_REMOVE_DISTANCES_SLOT);
    connections.push(s_IMAGE_INOUT, ::fwData::Image::s_DISTANCE_DISPLAYED_SIG, s_UPDATE_VISIBILITY_SLOT);
    connections.push(s_IMAGE_INOUT, ::fwData::Image::s_MODIFIED_SIG, s_UPDATE_SLOT);
    return connections;
}

//------------------------------------------------------------------------------

void SImageMultiDistances::starting()
{
    this->initialize();

    this->getRenderService()->makeCurrent();

    auto interactor = std::dynamic_pointer_cast< ::fwRenderOgre::interactor::IInteractor >(this->getSptr());
    this->getLayer()->addInteractor(interactor, m_priority);

    m_depthMaterialName   = this->getID() + "_depthMaterial";
    m_noDepthMaterialName = this->getID() + "_noDepthMaterial";

    // Create materials from our wrapper.
    m_depthMaterial = std::make_unique<::fwRenderOgre::Material>(m_depthMaterialName,
                                                                 ::fwRenderOgre::Material::DEFAULT_MATERIAL_TEMPLATE_NAME);
    m_depthMaterial->setHasVertexColor(true);
    m_depthMaterial->updateShadingMode(::fwData::Material::PHONG, this->getLayer()->getLightsNumber(), false, false);

    m_noDepthMaterial = std::make_unique<::fwRenderOgre::Material>(m_noDepthMaterialName,
                                                                   ::fwRenderOgre::Material::DEFAULT_MATERIAL_TEMPLATE_NAME);
    m_noDepthMaterial->setHasVertexColor(true);
    m_noDepthMaterial->updateShadingMode(::fwData::Material::PHONG, this->getLayer()->getLightsNumber(), false, false);

    // Retrive the ogre material to change the depth check.
    ::Ogre::MaterialPtr ogreMaterial = ::Ogre::MaterialManager::getSingleton().getByName(m_noDepthMaterialName);
    SLM_ASSERT("'" + m_noDepthMaterialName + "' does not exist.", ogreMaterial);
    ::Ogre::Technique* tech = ogreMaterial->getTechnique(0);
    SLM_ASSERT("No techique found", tech);
    ::Ogre::Pass* pass = tech->getPass(0);
    SLM_ASSERT("No pass found", pass);
    pass->setDepthCheckEnabled(false);
}

//------------------------------------------------------------------------------

void SImageMultiDistances::updating()
{
    m_depthMaterial->updateShadingMode(::fwData::Material::PHONG, this->getLayer()->getLightsNumber(), false, false);
    m_noDepthMaterial->updateShadingMode(::fwData::Material::PHONG, this->getLayer()->getLightsNumber(), false, false);
    this->removeDistances();
    this->addDistances();
}

//------------------------------------------------------------------------------

void SImageMultiDistances::stopping()
{
    this->getRenderService()->makeCurrent();

    m_depthMaterial.reset();
    m_noDepthMaterial.reset();

    auto interactor = std::dynamic_pointer_cast< ::fwRenderOgre::interactor::IInteractor >(this->getSptr());
    this->getLayer()->removeInteractor(interactor);

    while(m_distances.size() != 0)
    {
        this->destroyDistance(m_distances.begin()->first);
    }
}

//------------------------------------------------------------------------------

void SImageMultiDistances::addDistances()
{
    this->getRenderService()->makeCurrent();

    const ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("'" + s_IMAGE_INOUT + "' does not exist.", image);
    const ::fwData::mt::ObjectReadLock lock(image);

    const ::fwData::Vector::sptr distanceField = image->getField< ::fwData::Vector >(
        ::fwDataTools::fieldHelper::Image::m_imageDistancesId);
    if(distanceField)
    {
        for(const ::fwData::Object::sptr object : *distanceField)
        {
            const ::fwData::PointList::sptr pointList = ::fwData::PointList::dynamicCast(object);
            SLM_ASSERT("The distance should be a point list", pointList);
            SLM_ASSERT("The distance must contains two points", pointList->getPoints().size() == 2);

            const ::fwTools::fwID::IDType id = pointList->getID();
            if(m_distances.find(id) == m_distances.end())
            {
                this->createDistance(pointList);
            }
        }
    }
    // The signal ::fwData::Image::s_DISTANCE_ADDED_SIG is send if all distances are removed.
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

    const ::fwData::Image::csptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("'" + s_IMAGE_INOUT + "' does not exist.", image);
    const ::fwData::mt::ObjectReadLock lock(image);

    const ::fwData::Vector::csptr distanceField
        = image->getField< ::fwData::Vector >(::fwDataTools::fieldHelper::Image::m_imageDistancesId);

    std::vector< ::fwTools::fwID::IDType > foundId;
    if(distanceField)
    {
        for(::fwData::Object::sptr object : *distanceField)
        {
            foundId.push_back(object->getID());
        }
    }

    std::vector< ::fwTools::fwID::IDType > currentdId;
    for(const auto& [id, _] : m_distances)
    {
        currentdId.push_back(id);
    }

    for(const ::fwTools::fwID::IDType id : currentdId)
    {
        if(std::find(foundId.begin(), foundId.end(), id) == foundId.end())
        {
            destroyDistance(id);
        }
    }

    this->requestRender();
}

//------------------------------------------------------------------------------

void SImageMultiDistances::updateVisibility()
{
    this->getRenderService()->makeCurrent();

    const ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("'" + s_IMAGE_INOUT + "' does not exist.", image);
    ::fwData::mt::ObjectReadLock lock(image);

    const bool visibility = image->getField(::fwDataTools::fieldHelper::Image::m_distanceVisibility, ::fwData::Boolean::New(
                                                true))->value();
    m_visibility = visibility;

    lock.unlock();

    for(const auto& [_, data] : m_distances)
    {
        data.m_sphere1->setVisible(m_visibility);
        data.m_sphere2->setVisible(m_visibility);
        data.m_line->setVisible(m_visibility);
        data.m_dashedLine->setVisible(m_visibility);
        // FIXME: setVisible doesn't work, so we set the alpha to 0%.
        if(m_visibility)
        {
            data.m_label->setTextColor(SImageMultiDistances::generateColor(data.m_pointList->getID()));
        }
        else
        {
            data.m_label->setTextColor(::Ogre::ColourValue::ZERO);
        }
    }

    this->requestRender();
}

//------------------------------------------------------------------------------

void SImageMultiDistances::mouseMoveEvent(MouseButton, int _x, int _y, int, int)
{
    if(!m_pickedData.m_id.empty())
    {
        const ::Ogre::Camera* const cam = this->getLayer()->getDefaultCamera();
        const ::Ogre::Ray ray           = cam->getCameraToViewportRay(
            static_cast< ::Ogre::Real>(_x) / static_cast< ::Ogre::Real>(cam->getViewport()->getActualWidth()),
            static_cast< ::Ogre::Real>(_y) / static_cast< ::Ogre::Real>(cam->getViewport()->getActualHeight()));

        const ::Ogre::Vector3 direction = this->getCamDirection(cam);
        const ::Ogre::Plane plane(direction, cam->getParentNode()->getPosition() + direction*m_pickedData.m_distance);

        const std::pair< bool, Ogre::Real > hit = ::Ogre::Math::intersects(ray, plane);
        SLM_ASSERT("The ray must intersect the plane", hit.first);

        const ::fwTools::fwID::IDType id = m_pickedData.m_id;
        if(m_pickedData.m_first)
        {
            const ::Ogre::Vector3 secondPos = m_distances[id].m_node2->getPosition();
            this->updateDistance(id, ray.getPoint(hit.second), secondPos);
        }
        else
        {
            const ::Ogre::Vector3 firstPos = m_distances[id].m_node1->getPosition();
            this->updateDistance(id, firstPos, ray.getPoint(hit.second));
        }
        this->getLayer()->cancelFurtherInteraction();
    }
}

//------------------------------------------------------------------------------

void SImageMultiDistances::buttonReleaseEvent(MouseButton, int, int)
{
    m_pickedData = {"", true, 0.f};
}

//------------------------------------------------------------------------------

void SImageMultiDistances::buttonPressEvent(MouseButton _button, int _x, int _y)
{
    if(_button == LEFT)
    {
        const Ogre::MovableObject* const picked = this->pickObject(_x, _y);
        if (picked != nullptr)
        {
            const ::Ogre::SceneNode* node             = nullptr;
            const DistanceMap::iterator foundDistance =
                std::find_if(m_distances.begin(), m_distances.end(),
                             [&](std::pair<const ::fwTools::fwID::IDType, DistanceData>& _data) -> bool
                {
                    const DistanceData distanceData = _data.second;
                    if(distanceData.m_sphere1 == picked)
                    {
                        node = distanceData.m_node1;
                        return true;
                    }
                    else if(distanceData.m_sphere2 == picked)
                    {
                        node = distanceData.m_node2;
                        return true;
                    }
                    return false;
                });
            if(foundDistance != m_distances.end())
            {
                const ::Ogre::Camera* const cam = this->getLayer()->getDefaultCamera();

                const ::Ogre::Vector3 position  = node->getPosition();
                const ::Ogre::Vector3 direction = this->getCamDirection(cam);

                const ::Ogre::Ray ray(position, -direction);
                const ::Ogre::Plane plane(direction, cam->getParentNode()->getPosition());

                const std::pair< bool, Ogre::Real > hit = ::Ogre::Math::intersects(ray, plane);
                SLM_ASSERT("The ray must intersect the plane", hit.first);

                m_pickedData =
                {foundDistance->first, picked == foundDistance->second.m_sphere1 ? true : false, hit.second};
                this->getLayer()->cancelFurtherInteraction();
            }
        }
    }
}

//------------------------------------------------------------------------------

void SImageMultiDistances::createDistance(::fwData::PointList::sptr _pl)
{
    ::fwTools::fwID::IDType id = _pl->getID();
    SLM_ASSERT("The distance already exist", m_distances.find(id) == m_distances.end());

    ::Ogre::SceneManager* const sceneMgr = this->getSceneManager();
    ::Ogre::SceneNode* const rootNode    = sceneMgr->getRootSceneNode();

    // Retrieve data used to create Ogre resources.
    ::Ogre::ColourValue colour = SImageMultiDistances::generateColor(id);

    const std::array<double, 3> front = _pl->getPoints().front()->getCoord();
    const std::array<double, 3> back  = _pl->getPoints().back()->getCoord();

    const ::Ogre::Vector3 begin(static_cast<float>(front[0]), static_cast<float>(front[1]),
                                static_cast<float>(front[2]));
    const ::Ogre::Vector3 end(static_cast<float>(back[0]), static_cast<float>(back[1]),
                              static_cast<float>(back[2]));

    // Create the mesh.
    ::Ogre::ManualObject* const sphere = sceneMgr->createManualObject(this->getID() + "_sphere");
    ::fwRenderOgre::helper::ManualObject::createSphere(sphere, m_noDepthMaterialName, colour, m_distanceSphereRadius);
    const ::Ogre::MeshPtr sphereMesh = sphere->convertToMesh(this->getID() + "_sphereMesh_" + id);
    SLM_ASSERT("Can't create the mesh", sphereMesh);
    sceneMgr->destroyManualObject(sphere);

    // First sphere.
    ::Ogre::Entity* const sphere1 = sceneMgr->createEntity(this->getID() + "_sphere1_" + id, sphereMesh);
    sphere1->setQueryFlags(0x45893521);
    SLM_ASSERT("Can't create the first entity", sphere1);
    ::Ogre::SceneNode* const node1 = rootNode->createChildSceneNode(this->getID() + "_node1_" + id, begin);
    SLM_ASSERT("Can't create the first node", node1);
    node1->attachObject(sphere1);

    // Second sphere.
    ::Ogre::Entity* const sphere2 = sceneMgr->createEntity(this->getID() + "_sphere2_" + id, sphereMesh);
    sphere2->setQueryFlags(0x45893521);
    SLM_ASSERT("Can't create the second entity", sphere2);
    ::Ogre::SceneNode* const node2 = rootNode->createChildSceneNode(this->getID() + "_node2_" + id, end);
    SLM_ASSERT("Can't create the second node", node2);
    node2->attachObject(sphere2);

    // Line.
    ::Ogre::ManualObject* const line = sceneMgr->createManualObject(this->getID() + "_line_" + id);
    SLM_ASSERT("Can't create the line", line);
    line->begin(m_depthMaterialName, ::Ogre::RenderOperation::OT_LINE_LIST);
    line->colour(colour);
    line->position(begin);
    line->position(end);
    line->end();
    line->setQueryFlags(0x00000000);
    rootNode->attachObject(line);

    // Dashed line.
    ::Ogre::ManualObject* const dashedLine = sceneMgr->createManualObject(this->getID() + "_dashedLine_" + id);
    SLM_ASSERT("Can't create the dashed line", dashedLine);
    dashedLine->begin(m_noDepthMaterialName, ::Ogre::RenderOperation::OT_LINE_LIST);
    dashedLine->colour(colour);
    SImageMultiDistances::generateDashedLine(dashedLine, begin, end, m_distanceSphereRadius);
    dashedLine->setQueryFlags(0x00000000);
    // Render this line over all others objects.
    dashedLine->setRenderQueueGroup(::Ogre::RenderQueueGroupID::RENDER_QUEUE_9);
    rootNode->attachObject(dashedLine);

    // Label.
    ::Ogre::OverlayContainer* const textContainer = this->getLayer()->getOverlayTextPanel();
    ::Ogre::Camera* const cam                     = this->getLayer()->getDefaultCamera();
    const float dpi = this->getRenderService()->getInteractorManager()->getLogicalDotsPerInch();
    ::fwRenderOgre::Text* label = ::fwRenderOgre::Text::New(
        this->getID() + "_label_" + id, sceneMgr, textContainer, m_fontSource, m_fontSize, dpi, cam);
    const std::string length = SImageMultiDistances::getLength(end, begin);
    label->setText(length);
    label->setTextColor(colour);
    label->setQueryFlags(0x00000000);
    node2->attachObject(label);

    // Set the visibility.
    sphere1->setVisible(m_visibility);
    sphere2->setVisible(m_visibility);
    line->setVisible(m_visibility);
    dashedLine->setVisible(m_visibility);
    // FIXME: setVisible doesn't work, so we set the alpha to 0%.
    if(!m_visibility)
    {
        label->setTextColor(::Ogre::ColourValue::ZERO);
    }

    // Store data in the map.
    DistanceData data {_pl, sphereMesh, node1, sphere1, node2, sphere2, line, label, dashedLine};
    m_distances[id] = data;
}

//------------------------------------------------------------------------------

void SImageMultiDistances::updateDistance(::fwTools::fwID::IDType _id, ::Ogre::Vector3 _begin, ::Ogre::Vector3 _end)
{
    const DistanceMap::const_iterator it = m_distances.find(_id);
    SLM_ASSERT("The distance is not found", it != m_distances.end());

    // Update Ogre resources.
    const DistanceData distanceData = it->second;

    // Update spheres position.
    distanceData.m_node1->setPosition(_begin);
    distanceData.m_node2->setPosition(_end);

    // Update the line.
    ::Ogre::ManualObject* const line = distanceData.m_line;
    line->beginUpdate(0);
    line->position(_begin);
    line->position(_end);
    line->end();

    // Update the label.
    const std::string length = SImageMultiDistances::getLength(_end, _begin);
    distanceData.m_label->setText(length);

    // Update the dashed line
    ::Ogre::ManualObject* const dashedLine = distanceData.m_dashedLine;
    dashedLine->beginUpdate(0);
    SImageMultiDistances::generateDashedLine(dashedLine, _begin, _end, m_distanceSphereRadius);

    // Update the field data.
    const ::fwData::mt::ObjectWriteLock lock(distanceData.m_pointList);
    distanceData.m_pointList->getPoints().front()->setCoord({_begin[0], _begin[1], _begin[2]});
    distanceData.m_pointList->getPoints().back()->setCoord({_end[0], _end[1], _end[2]});

    const auto& sigModified = distanceData.m_pointList->signal< ::fwData::PointList::ModifiedSignalType >(
        ::fwData::PointList::s_MODIFIED_SIG);
    sigModified->asyncEmit();
}

//------------------------------------------------------------------------------

void SImageMultiDistances::destroyDistance(::fwTools::fwID::IDType _id)
{
    const DistanceMap::const_iterator it = m_distances.find(_id);
    SLM_ASSERT("The distance is not found", it != m_distances.end());

    // Destroy Ogre ressource.
    const DistanceData distanceData = it->second;
    ::Ogre::SceneManager* sceneMgr = this->getSceneManager();

    sceneMgr->destroyManualObject(distanceData.m_line);
    sceneMgr->destroyManualObject(distanceData.m_dashedLine);
    sceneMgr->destroyEntity(distanceData.m_sphere1);
    sceneMgr->destroyEntity(distanceData.m_sphere2);
    sceneMgr->destroySceneNode(distanceData.m_node1);
    sceneMgr->destroySceneNode(distanceData.m_node2);
    sceneMgr->destroyMovableObject(distanceData.m_label);

    ::Ogre::MeshManager::getSingleton().remove(distanceData.m_mesh);

    // Remove it from the map.
    m_distances.erase(it);
}

//------------------------------------------------------------------------------

::Ogre::MovableObject* SImageMultiDistances::pickObject(int _x, int _y)
{
    ::Ogre::SceneManager* const sceneMgr = this->getSceneManager();

    const ::Ogre::Camera* const cam = this->getLayer()->getDefaultCamera();

    const ::Ogre::Real width  = static_cast< ::Ogre::Real>(cam->getViewport()->getActualWidth());
    const ::Ogre::Real height = static_cast< ::Ogre::Real>(cam->getViewport()->getActualHeight());

    const ::Ogre::Ray ray = cam->getCameraToViewportRay(
        static_cast< ::Ogre::Real>(_x)/width,
        static_cast< ::Ogre::Real>(_y)/height);

    ::Ogre::RaySceneQuery* const raySceneQuery = sceneMgr->createRayQuery(ray, 0x45893521);
    raySceneQuery->setSortByDistance(false);
    if (raySceneQuery->execute().size() <= 0)
    {
        return nullptr;
    }

    ::Ogre::MovableObject* result = nullptr;
    const ::Ogre::RaySceneQueryResult& queryResult = raySceneQuery->getLastResults();
    for (size_t qrIdx = 0; qrIdx < queryResult.size(); qrIdx++)
    {
        const ::Ogre::MovableObject* const object = queryResult[qrIdx].movable;
        for(const std::pair<::fwTools::fwID::IDType, DistanceData> data : m_distances)
        {
            if(object == data.second.m_sphere1)
            {
                result = data.second.m_sphere1;
                break;
            }
            else if(object == data.second.m_sphere2)
            {
                result = data.second.m_sphere2;
                break;
            }
        }
        if(result)
        {
            break;
        }
    }

    delete raySceneQuery;

    return result;
}

//------------------------------------------------------------------------------

void SImageMultiDistances::focusInEvent()
{
}

//------------------------------------------------------------------------------

void SImageMultiDistances::focusOutEvent()
{
}

//------------------------------------------------------------------------------

void SImageMultiDistances::wheelEvent(int, int, int)
{
}

//------------------------------------------------------------------------------

void SImageMultiDistances::resizeEvent(int, int)
{
}

//------------------------------------------------------------------------------

void SImageMultiDistances::keyPressEvent(int)
{
}

//------------------------------------------------------------------------------

void SImageMultiDistances::keyReleaseEvent(int)
{
}

}
