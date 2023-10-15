/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
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

// cspell:ignore NOLINTNEXTLINE

#include "image_multi_distances.hpp"

#include "core/runtime/path.hpp"

#include "data/fiducials_series.hpp"

#include <core/com/slots.hxx>
#include <core/tools/uuid.hpp>

#include <data/boolean.hpp>
#include <data/helper/MedicalImage.hpp>
#include <data/image.hpp>
#include <data/image_series.hpp>
#include <data/material.hpp>
#include <data/point_list.hpp>

#include <service/macros.hpp>

#include <ui/__/cursor.hpp>

#include <viz/scene3d/helper/ManualObject.hpp>
#include <viz/scene3d/ogre.hpp>
#include <viz/scene3d/Utils.hpp>

#include <libs/viz/scene3d/helper/Scene.hpp>

#include <modules/viz/scene3dQt/window_interactor.hpp>

#include <OgreEntity.h>
#include <OgreNode.h>
#include <OgreSceneNode.h>

namespace sight::module::viz::scene3dQt::adaptor
{

static const core::com::slots::key_t REMOVE_ALL_SLOT                    = "removeAll";
static const core::com::slots::key_t REMOVE_DISTANCES_SLOT              = "removeDistances";
static const core::com::slots::key_t UPDATE_VISIBILITY_FROM_FIELDS_SLOT = "updateVisibilityFromField";
static const core::com::slots::key_t ACTIVATE_DISTANCE_TOOL_SLOT        = "activateDistanceTool";
static const core::com::slots::key_t UPDATE_MODIFIED_DISTANCE_SLOT      = "updateModifiedDistance";

static constexpr std::uint8_t s_DISTANCE_RQ_GROUP_ID = sight::viz::scene3d::rq::s_SURFACE_ID;

//------------------------------------------------------------------------------

namespace
{

//------------------------------------------------------------------------------

data::point_list::sptr toPointList(const data::fiducials_series::Fiducial& fiducial)
{
    SIGHT_ASSERT(
        "Only RULER-shaped fiducials are supported",
        fiducial.shapeType == data::fiducials_series::Shape::RULER
    );
    data::point_list::sptr res;
    if(fiducial.fiducialUID.has_value())
    {
        core::tools::object::sptr o = core::tools::id::get_object(*fiducial.fiducialUID);
        if(o == nullptr)
        {
            res = std::make_shared<data::point_list>();
            res->set_id(*fiducial.fiducialUID);
        }
        else
        {
            res = std::dynamic_pointer_cast<data::point_list>(o);
            SIGHT_ASSERT(
                "The ID " << *fiducial.fiducialUID << " is already set to an object which isn't a point list.",
                res
            );
        }
    }
    else
    {
        // The fiducial doesn't have a meaningful way to uniquely identify it, ignore it.
        return nullptr;
    }

    if(!fiducial.contourData.empty())
    {
        SIGHT_ASSERT("Contour Data should have two elements", fiducial.contourData.size() == 2);
        res->clear();
        res->pushBack(
            std::make_shared<data::point>(
                fiducial.contourData[0].x,
                fiducial.contourData[0].y,
                fiducial.contourData[0].z
            )
        );
        res->pushBack(
            std::make_shared<data::point>(
                fiducial.contourData[1].x,
                fiducial.contourData[1].y,
                fiducial.contourData[1].z
            )
        );
    }
    else
    {
        // Position with Graphic Coordinates Data Sequence isn't supported
        return nullptr;
    }

    return res;
}

} // namespace

//------------------------------------------------------------------------------

Ogre::ColourValue image_multi_distances::generateColor()
{
    if(std::getenv("GUI_TESTS_ARE_RUNNING") != nullptr)
    {
        // on windows and linux, the color is not the same and prevent comparison with a reference image in GUI tests.
        // For that reason, the color is fixed in gui tests.
        return Ogre::ColourValue(236 / 255.0F, 219 / 255.0F, 84 / 255.0F);
    }

    ++m_colorIndex;
    switch(m_colorIndex % 7)
    {
        case 0:
            return Ogre::ColourValue(63 / 255.0F, 105 / 255.0F, 170 / 255.0F);

        case 1:
            return Ogre::ColourValue(249 / 255.0F, 103 / 255.0F, 20 / 255.0F);

        case 2:
            return Ogre::ColourValue(236 / 255.0F, 219 / 255.0F, 84 / 255.0F);

        case 3:
            return Ogre::ColourValue(233 / 255.0F, 75 / 255.0F, 60 / 255.0F);

        case 4:
            return Ogre::ColourValue(121 / 255.0F, 199 / 255.0F, 83 / 255.0F);

        case 5:
            return Ogre::ColourValue(149 / 255.0F, 222 / 255.0F, 227 / 255.0F);

        case 6:
        default:
            return Ogre::ColourValue(29 / 255.0F, 45 / 255.0F, 168 / 255.0F);
    }
}

//------------------------------------------------------------------------------

Ogre::Vector3 image_multi_distances::getCamDirection(const Ogre::Camera* const _cam)
{
    const Ogre::Matrix4 view = _cam->getViewMatrix();
    Ogre::Vector3 direction(view[2][0], view[2][1], view[2][2]);
    direction.normalise();
    return -direction;
}

//------------------------------------------------------------------------------

void image_multi_distances::generateDashedLine(
    Ogre::ManualObject* const _object,
    const Ogre::Vector3& _begin,
    const Ogre::Vector3& _end,
    float _thickness
)
{
    const Ogre::Vector3 dashedLineDir = (_end - _begin);
    const float len                   = dashedLineDir.length();
    Ogre::Vector3 dashedLineDirN      = (_end - _begin);
    dashedLineDirN.normalise();

    Ogre::Vector3 dashedLinePos = _begin;
    for(std::size_t i = 0 ; i <= static_cast<std::size_t>((len - _thickness * 1.5) / (_thickness * 2)) ; i++)
    {
        _object->position(dashedLinePos);
        dashedLinePos += dashedLineDirN * _thickness;
        _object->position(dashedLinePos);
        dashedLinePos += dashedLineDirN * _thickness;
    }

    _object->end();
}

//------------------------------------------------------------------------------

std::string image_multi_distances::getLength(const Ogre::Vector3& _begin, const Ogre::Vector3& _end)
{
    const int length = static_cast<int>(std::round((_end - _begin).length()));
    return std::to_string(length) + "mm";
}

//------------------------------------------------------------------------------

image_multi_distances::image_multi_distances() noexcept
{
    new_slot(REMOVE_ALL_SLOT, &image_multi_distances::removeAll, this);
    new_slot(REMOVE_DISTANCES_SLOT, &image_multi_distances::removeDistances, this);
    new_slot(UPDATE_VISIBILITY_FROM_FIELDS_SLOT, &image_multi_distances::updateVisibilityFromField, this);
    new_slot(ACTIVATE_DISTANCE_TOOL_SLOT, &image_multi_distances::activateDistanceTool, this);
    new_slot(UPDATE_MODIFIED_DISTANCE_SLOT, &image_multi_distances::updateModifiedDistance, this);

    new_signal<signals::void_signal_t>(signals::DEACTIVATE_DISTANCE_TOOL);
}

//------------------------------------------------------------------------------

service::connections_t image_multi_distances::auto_connections() const
{
    connections_t connections;
    connections.push(s_IMAGE_INOUT, data::image::DISTANCE_MODIFIED_SIG, UPDATE_MODIFIED_DISTANCE_SLOT);
    connections.push(s_IMAGE_INOUT, data::image::DISTANCE_REMOVED_SIG, REMOVE_DISTANCES_SLOT);
    connections.push(s_IMAGE_INOUT, data::image::DISTANCE_DISPLAYED_SIG, UPDATE_VISIBILITY_SLOT);
    connections.push(s_IMAGE_INOUT, data::image::MODIFIED_SIG, service::slots::UPDATE);
    return connections;
}

//------------------------------------------------------------------------------

void image_multi_distances::configuring()
{
    this->configureParams();

    const config_t config = this->get_config();

    static const std::string s_FONT_SIZE_CONFIG   = s_CONFIG + "fontSize";
    static const std::string s_RADIUS_CONFIG      = s_CONFIG + "radius";
    static const std::string s_INTERACTIVE_CONFIG = s_CONFIG + "interactive";
    static const std::string s_PRIORITY_CONFIG    = s_CONFIG + "priority";
    static const std::string s_QUERY_MASK_CONFIG  = s_CONFIG + "queryMask";
    static const std::string s_QUERY_FLAGS_CONFIG = s_CONFIG + "distanceQueryFlags";

    m_fontSize             = config.get<std::size_t>(s_FONT_SIZE_CONFIG, m_fontSize);
    m_distanceSphereRadius = config.get<float>(s_RADIUS_CONFIG, m_distanceSphereRadius);
    m_interactive          = config.get<bool>(s_INTERACTIVE_CONFIG, m_interactive);
    m_priority             = config.get<int>(s_PRIORITY_CONFIG, m_priority);

    std::string hexaMask = config.get<std::string>(s_QUERY_MASK_CONFIG, "");
    if(!hexaMask.empty())
    {
        SIGHT_ASSERT(
            "Hexadecimal values should start with '0x'"
            "Given value : " + hexaMask,
            hexaMask.length() > 2
            && hexaMask.substr(0, 2) == "0x"
        );
        m_queryMask = static_cast<std::uint32_t>(std::stoul(hexaMask, nullptr, 16));
    }

    hexaMask = config.get<std::string>(s_QUERY_FLAGS_CONFIG, "");
    if(!hexaMask.empty())
    {
        SIGHT_ASSERT(
            "Hexadecimal values should start with '0x'"
            "Given value : " + hexaMask,
            hexaMask.length() > 2
            && hexaMask.substr(0, 2) == "0x"
        );
        m_distanceQueryFlag = static_cast<std::uint32_t>(std::stoul(hexaMask, nullptr, 16));
    }
}

//------------------------------------------------------------------------------

void image_multi_distances::starting()
{
    this->initialize();

    this->getRenderService()->makeCurrent();

    const sight::viz::scene3d::Layer::sptr layer = this->getLayer();

    m_sphereMaterialName     = this->get_id() + "_sphereMaterialName";
    m_lineMaterialName       = this->get_id() + "_lineMaterialName";
    m_dashedLineMaterialName = this->get_id() + "_dashedLineMaterialName";

    // Create materials from our wrapper.
    m_sphereMaterial = std::make_unique<sight::viz::scene3d::material>(
        m_sphereMaterialName,
        sight::viz::scene3d::material::DEFAULT_MATERIAL_TEMPLATE_NAME
    );
    m_sphereMaterial->setHasVertexColor(true);
    m_sphereMaterial->updateShadingMode(data::material::PHONG, layer->getLightsNumber(), false, false);

    m_lineMaterial = std::make_unique<sight::viz::scene3d::material>(
        m_lineMaterialName,
        sight::viz::scene3d::material::DEFAULT_MATERIAL_TEMPLATE_NAME
    );
    m_lineMaterial->setHasVertexColor(true);
    m_lineMaterial->updateShadingMode(data::material::AMBIENT, layer->getLightsNumber(), false, false);

    m_dashedLineMaterial = std::make_unique<sight::viz::scene3d::material>(
        m_dashedLineMaterialName,
        sight::viz::scene3d::material::DEFAULT_MATERIAL_TEMPLATE_NAME
    );
    m_dashedLineMaterial->setHasVertexColor(true);
    m_dashedLineMaterial->updateShadingMode(data::material::AMBIENT, layer->getLightsNumber(), false, false);

    // Retrieve the ogre material to change the depth check.
    const Ogre::MaterialPtr ogreSphereMaterial = Ogre::MaterialManager::getSingleton().getByName(
        m_sphereMaterialName,
        sight::viz::scene3d::RESOURCE_GROUP
    );
    SIGHT_ASSERT("'" + m_sphereMaterialName + "' does not exist.", ogreSphereMaterial);
    const Ogre::Technique* const sphereTech = ogreSphereMaterial->getTechnique(0);
    SIGHT_ASSERT("No technique found", sphereTech);
    Ogre::Pass* const spherePass = sphereTech->getPass(0);
    SIGHT_ASSERT("No pass found", spherePass);
    spherePass->setDepthCheckEnabled(false);

    const Ogre::MaterialPtr ogreDashedLineMaterial = Ogre::MaterialManager::getSingleton().getByName(
        m_dashedLineMaterialName,
        sight::viz::scene3d::RESOURCE_GROUP
    );
    SIGHT_ASSERT("'" + m_dashedLineMaterialName + "' does not exist.", ogreDashedLineMaterial);
    const Ogre::Technique* const dashedTech = ogreDashedLineMaterial->getTechnique(0);
    SIGHT_ASSERT("No technique found", dashedTech);
    Ogre::Pass* const dashedPass = dashedTech->getPass(0);
    SIGHT_ASSERT("No pass found", dashedPass);
    dashedPass->setDepthCheckEnabled(false);

    if(m_interactive)
    {
        auto interactor = std::dynamic_pointer_cast<sight::viz::scene3d::interactor::base>(this->get_sptr());
        layer->addInteractor(interactor, m_priority);
    }
}

//------------------------------------------------------------------------------

void image_multi_distances::updating()
{
    this->getRenderService()->makeCurrent();

    const sight::viz::scene3d::Layer::csptr layer = this->getLayer();

    m_sphereMaterial->updateShadingMode(data::material::PHONG, layer->getLightsNumber(), false, false);
    m_lineMaterial->updateShadingMode(data::material::AMBIENT, layer->getLightsNumber(), false, false);
    m_dashedLineMaterial->updateShadingMode(data::material::AMBIENT, layer->getLightsNumber(), false, false);
}

//------------------------------------------------------------------------------

void image_multi_distances::stopping()
{
    this->getRenderService()->makeCurrent();

    m_sphereMaterial.reset();
    m_lineMaterial.reset();
    m_dashedLineMaterial.reset();

    if(m_interactive)
    {
        auto interactor = std::dynamic_pointer_cast<sight::viz::scene3d::interactor::base>(this->get_sptr());
        this->getLayer()->removeInteractor(interactor);
    }

    while(!m_distances.empty())
    {
        this->destroyDistance(m_distances.begin()->first);
    }

    m_eventFilter = nullptr;
}

//------------------------------------------------------------------------------

void image_multi_distances::removeAll()
{
    if(m_binButton != nullptr)
    {
        m_binButton->hide();
        delete m_binButton;
        m_binButton = nullptr;
    }

    const auto image                    = m_image.lock();
    data::vector::sptr distanceListCopy = std::make_shared<data::vector>();
    if(auto imageSeries = std::dynamic_pointer_cast<data::image_series>(image.get_shared()))
    {
        std::vector<data::fiducials_series::FiducialSet> fiducialSets = imageSeries->getFiducials()->getFiducialSets();
        for(auto itFiducialSet = fiducialSets.begin() ; itFiducialSet != fiducialSets.end() ; )
        {
            for(auto itFiducial = itFiducialSet->fiducialSequence.begin() ;
                itFiducial != itFiducialSet->fiducialSequence.end() ; )
            {
                if(itFiducial->shapeType == data::fiducials_series::Shape::RULER)
                {
                    distanceListCopy->push_back(toPointList(*itFiducial));
                    itFiducial = itFiducialSet->fiducialSequence.erase(itFiducial);
                }
                else
                {
                    ++itFiducial;
                }
            }

            if(itFiducialSet->fiducialSequence.empty())
            {
                itFiducialSet = fiducialSets.erase(itFiducialSet);
            }
            else
            {
                ++itFiducialSet;
            }
        }

        imageSeries->getFiducials()->setFiducialSets(fiducialSets);
    }
    else
    {
        data::vector::sptr distanceList = data::helper::MedicalImage::getDistances(*image);
        distanceListCopy->shallow_copy(distanceList);
        distanceList->clear();
    }

    for(const data::object::sptr& element : *distanceListCopy)
    {
        auto pl = std::dynamic_pointer_cast<data::point_list>(element);
        SIGHT_ASSERT("All elements in distance image field must be point lists.", pl);
        image->signal<data::image::DistanceRemovedSignalType>(data::image::DISTANCE_REMOVED_SIG)->async_emit(pl);
    }
}

//------------------------------------------------------------------------------

void image_multi_distances::removeDistances()
{
    this->getRenderService()->makeCurrent();

    const auto image = m_image.lock();

    data::vector::sptr distanceField;
    if(auto imageSeries = std::dynamic_pointer_cast<data::image_series>(image.get_shared()))
    {
        distanceField = std::make_shared<data::vector>();
        for(const data::fiducials_series::FiducialSet& fiducialSet : imageSeries->getFiducials()->getFiducialSets())
        {
            for(const data::fiducials_series::Fiducial& fiducial : fiducialSet.fiducialSequence)
            {
                if(fiducial.shapeType == data::fiducials_series::Shape::RULER)
                {
                    if(data::point_list::sptr pl = toPointList(fiducial))
                    {
                        distanceField->push_back(pl);
                    }
                }
            }
        }
    }
    else
    {
        distanceField = data::helper::MedicalImage::getDistances(*image);
    }

    std::vector<core::tools::id::type> foundId;
    if(distanceField)
    {
        for(const auto& object : *distanceField)
        {
            foundId.push_back(object->get_id());
        }
    }
    else
    {
        while(!m_distances.empty())
        {
            this->destroyDistance(m_distances.begin()->first);
        }
    }

    std::vector<core::tools::id::type> currentdId;
    for(const auto& [id, _] : m_distances)
    {
        currentdId.push_back(id);
    }

    for(const core::tools::id::type& id : currentdId)
    {
        if(std::find(foundId.begin(), foundId.end(), id) == foundId.end())
        {
            destroyDistance(id);
        }
    }

    this->requestRender();
}

//------------------------------------------------------------------------------

void image_multi_distances::updateVisibilityFromField()
{
    this->getRenderService()->makeCurrent();

    const auto image = m_image.lock();

    m_isVisible = data::helper::MedicalImage::getDistanceVisibility(*image);

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

void image_multi_distances::setVisible(bool _visible)
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

    m_visible = _visible;

    this->requestRender();
}

//------------------------------------------------------------------------------

std::optional<Ogre::Vector3> image_multi_distances::getNearestPickedPosition(int _x, int _y)
{
    Ogre::SceneManager* sm = this->getSceneManager();
    const auto result      = sight::viz::scene3d::Utils::pickObject(_x, _y, m_queryMask, *sm);

    if(result.has_value())
    {
        const auto* const camera = sm->getCamera(sight::viz::scene3d::Layer::s_DEFAULT_CAMERA_NAME);
        const auto* const vp     = camera->getViewport();

        // Screen to viewport space conversion.
        const float vpX = static_cast<float>(_x - vp->getActualLeft()) / static_cast<float>(vp->getActualWidth());
        const float vpY = static_cast<float>(_y - vp->getActualTop()) / static_cast<float>(vp->getActualHeight());

        const Ogre::Ray ray = camera->getCameraToViewportRay(vpX, vpY);

        Ogre::Vector3 normal = -ray.getDirection();
        normal.normalise();

        return result->second + normal * 0.01F;
    }

    return std::nullopt;
}

//------------------------------------------------------------------------------

void image_multi_distances::buttonPressEvent(MouseButton _button, Modifier /*_mods*/, int _x, int _y)
{
    if(m_binButton != nullptr)
    {
        m_binButton->hide();
        delete m_binButton;
        m_binButton = nullptr;
    }

    m_isAMouseMove = false;

    if(_button == LEFT && m_toolActivated && m_visible && !m_creationMode)
    {
        const sight::viz::scene3d::Layer::csptr layer = this->getLayer();
        Ogre::SceneManager* const sceneMgr            = layer->getSceneManager();

        const Ogre::Camera* const cam = layer->getDefaultCamera();
        const auto* const vp          = cam->getViewport();

        const float vpX = static_cast<float>(_x - vp->getActualLeft()) / static_cast<float>(vp->getActualWidth());
        const float vpY = static_cast<float>(_y - vp->getActualTop()) / static_cast<float>(vp->getActualHeight());

        const Ogre::Ray ray = cam->getCameraToViewportRay(vpX, vpY);

        bool found                               = false;
        Ogre::RaySceneQuery* const raySceneQuery = sceneMgr->createRayQuery(ray, m_distanceQueryFlag);
        raySceneQuery->setSortByDistance(true);
        if(!raySceneQuery->execute().empty())
        {
            const Ogre::RaySceneQueryResult& queryResultVect = raySceneQuery->getLastResults();

            for(std::size_t qrIdx = 0 ; qrIdx < queryResultVect.size() && !found ; qrIdx++)
            {
                const Ogre::MovableObject* const object = queryResultVect[qrIdx].movable;
                const auto objectType                   = object->getMovableType();

                if(objectType == "Entity" && object->isVisible())
                {
                    //First point
                    auto firstPoint      = std::make_shared<data::point>();
                    auto clickedPosition = this->getNearestPickedPosition(_x, _y);
                    firstPoint->setCoord({clickedPosition->x, clickedPosition->y, clickedPosition->z});
                    //Second Point
                    auto secondPoint = std::make_shared<data::point>();
                    secondPoint->setCoord({clickedPosition->x, clickedPosition->y, clickedPosition->z});
                    m_points.push_back(firstPoint);
                    m_points.push_back(secondPoint);

                    //createDistance equal to 0, firstPoint = secondPoint
                    auto pointList = std::make_shared<data::point_list>();
                    pointList->setPoints(m_points);
                    this->createDistance(pointList);
                    this->updateImageDistanceField(pointList);
                    auto& distanceData = m_distances[pointList->get_id()];
                    m_pickedData = {&distanceData, false};

                    //remember that this is a creation.
                    m_creationMode = true;

                    setCursor(Qt::ClosedHandCursor);

                    break;
                }

                if(objectType == "ManualObject" && object->isVisible())
                {
                    const Ogre::Real scale = 1.15F;
                    for(auto& distance : m_distances)
                    {
                        DistanceData& distanceData = distance.second;
                        if(distanceData.m_sphere1 == object)
                        {
                            distanceData.m_node1->setScale(scale, scale, scale);
                            m_pickedData = {&distanceData, true};
                            found        = true;

                            setCursor(Qt::ClosedHandCursor);
                            break;
                        }

                        if(distanceData.m_sphere2 == object)
                        {
                            distanceData.m_node2->setScale(scale, scale, scale);
                            m_pickedData = {&distanceData, false};
                            found        = true;

                            setCursor(Qt::ClosedHandCursor);
                            break;
                        }
                    }
                }
            }

            delete raySceneQuery;
        }
    }
}

//------------------------------------------------------------------------------

void image_multi_distances::mouseMoveEvent(
    MouseButton /*_button*/,
    Modifier /*_mods*/,
    int _x,
    int _y,
    int /*_dx*/,
    int /*_dy*/
)
{
    m_isAMouseMove = true;

    if(m_toolActivated && m_visible)
    {
        this->getRenderService()->makeCurrent();

        if(m_pickedData.m_data != nullptr)
        {
            // Discard the current distance to launch the ray over the scene without picking this one.
            m_pickedData.m_data->m_sphere1->setQueryFlags(0x0);
            m_pickedData.m_data->m_sphere2->setQueryFlags(0x0);

            const auto layer              = this->getLayer();
            const Ogre::Camera* const cam = layer->getDefaultCamera();
            SIGHT_ASSERT("No camera found", cam);

            bool moveInCameraPlane = true;

            Ogre::Vector3 newPos;
            if(cam->getProjectionType() == Ogre::ProjectionType::PT_PERSPECTIVE)
            {
                // If something is picked, we will snap the landmark to it
                std::optional<Ogre::Vector3> pickedPos = this->getNearestPickedPosition(_x, _y);
                if(pickedPos.has_value())
                {
                    newPos            = pickedPos.value();
                    moveInCameraPlane = false;
                }
            }

            // Else we move the distance along a plane.
            if(moveInCameraPlane)
            {
                const auto* const vp = cam->getViewport();

                const float vpX = static_cast<float>(_x - vp->getActualLeft())
                                  / static_cast<float>(vp->getActualWidth());
                const float vpY = static_cast<float>(_y - vp->getActualTop())
                                  / static_cast<float>(vp->getActualHeight());

                const Ogre::Ray ray           = cam->getCameraToViewportRay(vpX, vpY);
                const Ogre::Vector3 direction = this->getCamDirection(cam);

                Ogre::Vector3 position;
                if(m_pickedData.m_first)
                {
                    position = m_pickedData.m_data->m_node1->getPosition();
                }
                else
                {
                    position = m_pickedData.m_data->m_node2->getPosition();
                }

                const Ogre::Plane plane(direction, position);

                const std::pair<bool, Ogre::Real> hit = Ogre::Math::intersects(ray, plane);

                if(!hit.first)
                {
                    SIGHT_ERROR("The ray must intersect the plane")
                    return;
                }

                newPos = ray.getPoint(hit.second);
            }

            // Reset the query flag.
            m_pickedData.m_data->m_sphere1->setQueryFlags(m_distanceQueryFlag);
            m_pickedData.m_data->m_sphere2->setQueryFlags(m_distanceQueryFlag);

            if(m_pickedData.m_first)
            {
                const Ogre::Vector3 secondPos = m_pickedData.m_data->m_node2->getPosition();
                this->updateDistance(m_pickedData.m_data, newPos, secondPos);
            }
            else
            {
                const Ogre::Vector3 firstPos = m_pickedData.m_data->m_node1->getPosition();
                this->updateDistance(m_pickedData.m_data, firstPos, newPos);
            }

            this->requestRender();

            const auto image = m_image.lock();
            const auto sig   = image->signal<data::image::DistanceModifiedSignalType>(
                data::image::DISTANCE_MODIFIED_SIG
            );
            sig->async_emit(m_pickedData.m_data->m_pointList);
        }
        else
        {
            const sight::viz::scene3d::Layer::csptr layer = this->getLayer();
            Ogre::SceneManager* const sceneMgr            = layer->getSceneManager();

            const Ogre::Camera* const cam = layer->getDefaultCamera();
            const auto* const vp          = cam->getViewport();

            const float vpX = static_cast<float>(_x - vp->getActualLeft()) / static_cast<float>(vp->getActualWidth());
            const float vpY = static_cast<float>(_y - vp->getActualTop()) / static_cast<float>(vp->getActualHeight());

            const Ogre::Ray ray = cam->getCameraToViewportRay(vpX, vpY);

            bool found                               = false;
            Ogre::RaySceneQuery* const raySceneQuery = sceneMgr->createRayQuery(ray, m_distanceQueryFlag);
            raySceneQuery->setSortByDistance(true);
            if(!raySceneQuery->execute().empty())
            {
                const Ogre::RaySceneQueryResult& queryResultVect = raySceneQuery->getLastResults();

                for(std::size_t qrIdx = 0 ; qrIdx < queryResultVect.size() && !found ; qrIdx++)
                {
                    const Ogre::MovableObject* const object = queryResultVect[qrIdx].movable;
                    const auto objectType                   = object->getMovableType();

                    if(objectType == "ManualObject" && object->isVisible())
                    {
                        for(auto& distance : m_distances)
                        {
                            DistanceData& distanceData = distance.second;
                            if(distanceData.m_sphere1 == object || distanceData.m_sphere2 == object)
                            {
                                setCursor(Qt::OpenHandCursor);
                                m_isOverDistance = true;
                                found            = true;
                                break;
                            }
                        }
                    }
                }

                if(m_isOverDistance && !found)
                {
                    m_isOverDistance = false;
                    setCursor(Qt::CrossCursor);
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

void image_multi_distances::buttonReleaseEvent(MouseButton _button, Modifier /*_mods*/, int /*_x*/, int /*_y*/)
{
    if(_button == LEFT && m_pickedData.m_data != nullptr)
    {
        const Ogre::Real scale = 1.F;
        m_pickedData.m_data->m_node1->setScale(scale, scale, scale);
        m_pickedData.m_data->m_node2->setScale(scale, scale, scale);

        const auto pl                     = m_pickedData.m_data->m_pointList;
        const std::array<double, 3> front = pl->getPoints().front()->getCoord();
        const std::array<double, 3> back  = pl->getPoints().back()->getCoord();

        const Ogre::Vector3 begin(static_cast<float>(front[0]),
                                  static_cast<float>(front[1]),
                                  static_cast<float>(front[2]));
        const Ogre::Vector3 end(static_cast<float>(back[0]),
                                static_cast<float>(back[1]),
                                static_cast<float>(back[2]));

        const int length = static_cast<int>(std::round((end - begin).length()));

        // if it is in creation mode, and a distance is calculated, it is the second point of the distance.
        // Finish the distance creation
        if(m_creationMode && length != 0)
        {
            m_creationMode = false;
            setCursor(Qt::OpenHandCursor);
            m_pickedData = {nullptr, true};
            m_points.clear();
        }
        //If it is not a creationMode, a distance null means that the distance should be removed
        else if(!m_creationMode)
        {
            if(length == 0)
            {
                destroyDistance(pl->get_id());
                removeDistance(pl);
                setCursor(Qt::CrossCursor);
                m_pickedData = {nullptr, true};
                m_points.clear();
            }
            else
            {
                if(!m_isAMouseMove)
                {
                    auto interactor    = getLayer()->getRenderService()->getInteractorManager();
                    auto qtInteractor  = std::dynamic_pointer_cast<window_interactor>(interactor);
                    auto* parentWidget = qtInteractor->getQtWidget();
                    static const QIcon trashBinIcon(QString::fromStdString(
                                                        (core::runtime::get_module_resource_path(
                                                             "sight::module::ui::flaticons"
                                                         ) / "RedTrashBin.svg").string()
                    ));
                    m_binButton = new QPushButton(trashBinIcon, "", parentWidget);
                    const std::string serviceID = get_id().substr(get_id().find_last_of('_') + 1);
                    m_binButton->setObjectName(QString::fromStdString(serviceID) + "/binButton");
                    m_binButton->setCursor(Qt::ArrowCursor);
                    m_binButton->adjustSize();
                    if(m_binButton->width() < 40)
                    {
                        m_binButton->setMinimumWidth(40);
                    }

                    if(m_binButton->height() < 40)
                    {
                        m_binButton->setMinimumHeight(40);
                    }

                    m_binButton->adjustSize();
                    m_binButton->setIconSize(m_binButton->size());
                    m_binButton->raise();
                    Ogre::SceneNode* node =
                        m_pickedData.m_first ? m_pickedData.m_data->m_node1 : m_pickedData.m_data->m_node2;
                    std::pair<Ogre::Vector2,
                              Ogre::Vector2> screenPos = sight::viz::scene3d::helper::Scene::computeBoundingRect(
                        *getLayer()->getDefaultCamera(),
                        node
                              );
                    double ratio = m_binButton->devicePixelRatioF();
                    const int x  = std::clamp(
                        int(((screenPos.first.x + screenPos.second.x) / 2) / ratio),
                        0,
                        parentWidget->width() - m_binButton->width()
                    );
                    int y = int((screenPos.first.y / ratio) - m_binButton->height());
                    if(y < 0)
                    {
                        // If there isn't enough place upward the landmark, place the menu downward.
                        y = int(screenPos.second.y / ratio);
                    }

                    m_binButton->move(x, y);
                    m_binButton->show();
                    QObject::connect(
                        m_binButton,
                        &QPushButton::clicked,
                        [this, pl = m_pickedData.m_data->m_pointList]
                        {
                            m_binButton->hide();
                            destroyDistance(pl->get_id());
                            removeDistance(pl);
                            setCursor(Qt::CrossCursor);
                            m_pickedData = {nullptr, true};
                            m_points.clear();

                            delete m_binButton;
                            m_binButton = nullptr;
                        });
                }

                setCursor(Qt::OpenHandCursor);
                m_pickedData = {nullptr, true};
                m_points.clear();
            }
        }

        this->getLayer()->requestRender();
    }
    else if(_button == RIGHT)
    {
        // right button in creation mode destroys the created distance
        if(m_creationMode)
        {
            m_creationMode = false;
            const auto pl = m_pickedData.m_data->m_pointList;
            destroyDistance(pl->get_id());
            removeDistance(pl);
            setCursor(Qt::CrossCursor);
            m_pickedData = {nullptr, true};
            m_points.clear();
        }
        //right button other than in creation mode goes out of the add distance mode
        else
        {
            activateDistanceTool(false);
            this->signal<signals::void_signal_t>(signals::DEACTIVATE_DISTANCE_TOOL)->async_emit();
        }
    }
}

//------------------------------------------------------------------------------

void image_multi_distances::keyPressEvent(int key, Modifier /*_mods*/, int /*_mouseX*/, int /*_mouseY*/)
{
    if(m_toolActivated && key == Qt::Key_Escape)
    {
        activateDistanceTool(false);
        this->signal<signals::void_signal_t>(signals::DEACTIVATE_DISTANCE_TOOL)->async_emit();
    }
}

//------------------------------------------------------------------------------

void image_multi_distances::wheelEvent(Modifier /*_mods*/, double /*_angleDelta*/, int /*_x*/, int /*_y*/)
{
    if(m_binButton != nullptr)
    {
        m_binButton->hide();
        delete m_binButton;
        m_binButton = nullptr;
    }
}

//------------------------------------------------------------------------------

void image_multi_distances::createDistance(data::point_list::sptr& _pl)
{
    const core::tools::id::type id = _pl->get_id();
    SIGHT_ASSERT("The distance already exist", m_distances.find(id) == m_distances.end());

    Ogre::SceneManager* const sceneMgr = this->getSceneManager();
    Ogre::SceneNode* const rootNode    = sceneMgr->getRootSceneNode();

    // Retrieve data used to create Ogre resources.
    const Ogre::ColourValue colour = image_multi_distances::generateColor();

    const std::array<double, 3> front = _pl->getPoints().front()->getCoord();
    const std::array<double, 3> back  = _pl->getPoints().back()->getCoord();

    const Ogre::Vector3 begin(static_cast<float>(front[0]),
                              static_cast<float>(front[1]),
                              static_cast<float>(front[2]));
    const Ogre::Vector3 end(static_cast<float>(back[0]),
                            static_cast<float>(back[1]),
                            static_cast<float>(back[2]));

    // First sphere.
    Ogre::ManualObject* const sphere1 = sceneMgr->createManualObject(this->get_id() + "_sphere1_" + id);
    sight::viz::scene3d::helper::ManualObject::createSphere(
        sphere1,
        m_sphereMaterialName,
        colour,
        m_distanceSphereRadius
    );
    sphere1->setQueryFlags(m_distanceQueryFlag);
    // Render this sphere over all others objects.
    sphere1->setRenderQueueGroup(s_DISTANCE_RQ_GROUP_ID);
    SIGHT_ASSERT("Can't create the first entity", sphere1);
    Ogre::SceneNode* const node1 = rootNode->createChildSceneNode(this->get_id() + "_node1_" + id, begin);
    SIGHT_ASSERT("Can't create the first node", node1);
    node1->attachObject(sphere1);

    // Second sphere.
    Ogre::ManualObject* const sphere2 = sceneMgr->createManualObject(this->get_id() + "_sphere2_" + id);
    sight::viz::scene3d::helper::ManualObject::createSphere(
        sphere2,
        m_sphereMaterialName,
        colour,
        m_distanceSphereRadius
    );
    sphere2->setQueryFlags(m_distanceQueryFlag);
    // Render this sphere over all others objects.
    sphere2->setRenderQueueGroup(s_DISTANCE_RQ_GROUP_ID);
    SIGHT_ASSERT("Can't create the second entity", sphere2);
    Ogre::SceneNode* const node2 = rootNode->createChildSceneNode(this->get_id() + "_node2_" + id, end);
    SIGHT_ASSERT("Can't create the second node", node2);
    node2->attachObject(sphere2);

    // Line.
    Ogre::ManualObject* const line = sceneMgr->createManualObject(this->get_id() + "_line_" + id);
    SIGHT_ASSERT("Can't create the line", line);
    line->begin(m_lineMaterialName, Ogre::RenderOperation::OT_LINE_LIST, sight::viz::scene3d::RESOURCE_GROUP);
    line->colour(colour);
    line->position(begin);
    line->position(end);
    line->end();
    line->setQueryFlags(0x0);
    rootNode->attachObject(line);

    // Dashed line.
    Ogre::ManualObject* const dashedLine = sceneMgr->createManualObject(this->get_id() + "_dashedLine_" + id);
    SIGHT_ASSERT("Can't create the dashed line", dashedLine);
    dashedLine->begin(
        m_dashedLineMaterialName,
        Ogre::RenderOperation::OT_LINE_LIST,
        sight::viz::scene3d::RESOURCE_GROUP
    );
    dashedLine->colour(colour);
    image_multi_distances::generateDashedLine(
        dashedLine,
        begin,
        end,
        m_distanceSphereRadius
    );
    dashedLine->setQueryFlags(0x0);
    // Render this line over all others objects.
    dashedLine->setRenderQueueGroup(s_DISTANCE_RQ_GROUP_ID);
    rootNode->attachObject(dashedLine);

    // Label.
    const sight::viz::scene3d::Layer::sptr layer = this->getLayer();

    sight::viz::scene3d::IText::sptr label = sight::viz::scene3d::IText::make(layer);

    // NOLINTNEXTLINE(readability-suspicious-call-argument)
    const std::string length = image_multi_distances::getLength(begin, end);
    label->setText(length);
    label->setTextColor(colour);
    label->setFontSize(m_fontSize);
    Ogre::SceneNode* const labelNode = rootNode->createChildSceneNode(this->get_id() + "_labelNode_" + id, end);
    SIGHT_ASSERT("Can't create the label node", labelNode);
    label->attachToNode(labelNode, this->getLayer()->getDefaultCamera());

    // Set the visibility.
    sphere1->setVisible(m_isVisible);
    sphere2->setVisible(m_isVisible);
    line->setVisible(m_isVisible);
    dashedLine->setVisible(m_isVisible);
    label->setVisible(m_isVisible);

    // Store data in the map.
    const DistanceData data {_pl, node1, sphere1, node2, sphere2, line, dashedLine, labelNode, label};
    m_distances[id] = data;
}

//------------------------------------------------------------------------------

void image_multi_distances::updateImageDistanceField(data::point_list::sptr _pl)
{
    const auto image = m_image.lock();
    if(data::helper::MedicalImage::checkImageValidity(image.get_shared()))
    {
        if(auto imageSeries = std::dynamic_pointer_cast<data::image_series>(image.get_shared()))
        {
            data::fiducials_series::FiducialSet fiducialSet;
            std::string frameOfReferenceUID = imageSeries->getStringValue(
                data::dicom::attribute::Keyword::FrameOfReferenceUID
            );
            if(frameOfReferenceUID.empty())
            {
                // Generate a frame of reference UID if the image doesn't have one. It is supposed to be mandatory
                // according
                // to the DICOM standard anyway.
                frameOfReferenceUID = core::tools::UUID::generate();
                imageSeries->setStringValue(data::dicom::attribute::Keyword::FrameOfReferenceUID, frameOfReferenceUID);
            }

            fiducialSet.frameOfReferenceUID = frameOfReferenceUID;
            data::fiducials_series::Fiducial fiducial;
            fiducial.shapeType           = data::fiducials_series::Shape::RULER;
            fiducial.fiducialIdentifier  = _pl->get_id();
            fiducial.fiducialDescription = "Distance";
            fiducial.fiducialUID         = _pl->get_id();
            std::array<double, 3> firstPoint  = _pl->getPoints().front()->getCoord();
            std::array<double, 3> secondPoint = _pl->getPoints().back()->getCoord();
            fiducial.contourData = {
                {.x = firstPoint[0], .y = firstPoint[1], .z = firstPoint[2]},
                {.x = secondPoint[0], .y = firstPoint[1], .z = firstPoint[2]}
            };
            // If both ContourData and GraphicCoordinatesDataSequence are set, they must be synchronized, but I'm too
            // lazy to do that, so I simply get rid of GraphicCoordinatesDataSequence.
            fiducial.graphicCoordinatesDataSequence = std::nullopt;
            fiducialSet.fiducialSequence.push_back(fiducial);
            imageSeries->getFiducials()->appendFiducialSet(fiducialSet);
        }
        else
        {
            data::vector::sptr distancesField = data::helper::MedicalImage::getDistances(*image);

            if(!distancesField)
            {
                distancesField = std::make_shared<data::vector>();
                distancesField->push_back(_pl);
                data::helper::MedicalImage::setDistances(*image, distancesField);
            }
            else
            {
                distancesField->push_back(_pl);
                data::helper::MedicalImage::setDistances(*image, distancesField);
            }
        }
    }
}

//------------------------------------------------------------------------------

void image_multi_distances::updateDistance(
    const DistanceData* const _data,
    Ogre::Vector3 _begin,
    Ogre::Vector3 _end
)
{
    SIGHT_ASSERT("Distance can't be null", _data);

    // Update spheres position.
    _data->m_node1->setPosition(_begin);
    _data->m_node2->setPosition(_end);

    // Update the line.
    Ogre::ManualObject* const line = _data->m_line;
    line->beginUpdate(0);
    line->position(_begin);
    line->position(_end);
    line->end();

    // Update the label.
    // NOLINTNEXTLINE(readability-suspicious-call-argument)
    const std::string length = image_multi_distances::getLength(_begin, _end);
    _data->m_label->setText(length);
    _data->m_labelNode->setPosition(_end);

    // Update the dashed line
    Ogre::ManualObject* const dashedLine = _data->m_dashedLine;
    dashedLine->beginUpdate(0);
    image_multi_distances::generateDashedLine(dashedLine, _begin, _end, m_distanceSphereRadius);

    // Update the field data.
    const data::mt::locked_ptr lock(_data->m_pointList);
    _data->m_pointList->getPoints().front()->setCoord({_begin[0], _begin[1], _begin[2]});
    _data->m_pointList->getPoints().back()->setCoord({_end[0], _end[1], _end[2]});

    {
        const auto image = m_image.lock();
        if(auto imageSeries = std::dynamic_pointer_cast<data::image_series>(image.get_shared()))
        {
            // With fields, the modified point list is a shared pointer to the distance field, so there is nothing more
            // to do; however this isn't enough for fiducials, which must be updated manually.
            std::vector<data::fiducials_series::FiducialSet> fiducialSets =
                imageSeries->getFiducials()->getFiducialSets();
            for(data::fiducials_series::FiducialSet& fiducialSet : fiducialSets)
            {
                for(data::fiducials_series::Fiducial& fiducial : fiducialSet.fiducialSequence)
                {
                    if(fiducial.shapeType == data::fiducials_series::Shape::RULER
                       && fiducial.fiducialUID == _data->m_pointList->get_id())
                    {
                        fiducial.contourData.clear();
                        fiducial.contourData.push_back({.x = _begin.x, .y = _begin.y, .z = _begin.z});
                        fiducial.contourData.push_back({.x = _end.x, .y = _end.y, .z = _end.z});
                    }
                }
            }

            imageSeries->getFiducials()->setFiducialSets(fiducialSets);
        }
    }

    const auto& sigModified = _data->m_pointList->signal<data::point_list::ModifiedSignalType>(
        data::point_list::MODIFIED_SIG
    );

    core::com::connection::blocker blocker(sigModified->get_connection(slot(service::slots::UPDATE)));
    sigModified->async_emit();

    this->requestRender();
}

//------------------------------------------------------------------------------

void image_multi_distances::destroyDistance(core::tools::id::type _id)
{
    const auto it = m_distances.find(_id);
    SIGHT_ASSERT("The distance is not found", it != m_distances.end());

    // Destroy Ogre resource.
    DistanceData distanceData          = it->second;
    Ogre::SceneManager* const sceneMgr = this->getSceneManager();

    distanceData.m_label->detachFromNode();
    distanceData.m_label->setVisible(false);
    distanceData.m_label.reset();
    sceneMgr->destroySceneNode(distanceData.m_node1);
    sceneMgr->destroyManualObject(distanceData.m_sphere1);
    sceneMgr->destroySceneNode(distanceData.m_node2);
    sceneMgr->destroyManualObject(distanceData.m_sphere2);
    sceneMgr->destroyManualObject(distanceData.m_line);
    sceneMgr->destroyManualObject(distanceData.m_dashedLine);
    sceneMgr->destroySceneNode(distanceData.m_labelNode);

    // Remove it from the map.
    m_distances.erase(it);
}

//------------------------------------------------------------------------------

void image_multi_distances::activateDistanceTool(bool _activate)
{
    if(_activate)
    {
        m_toolActivated = true;
        setCursor(Qt::CrossCursor);
        auto interactor    = getLayer()->getRenderService()->getInteractorManager();
        auto qtInteractor  = std::dynamic_pointer_cast<window_interactor>(interactor);
        auto* parentWidget = qtInteractor->getQtWidget();
        m_eventFilter = std::make_unique<DeleteBinButtonWhenFocusOut>(this);
        parentWidget->installEventFilter(m_eventFilter.get());
    }
    else
    {
        m_toolActivated  = false;
        m_isOverDistance = false;
        m_creationMode   = false;
        m_pickedData     = {nullptr, true};
        m_points.clear();

        auto interactor    = getLayer()->getRenderService()->getInteractorManager();
        auto qtInteractor  = std::dynamic_pointer_cast<window_interactor>(interactor);
        auto* parentWidget = qtInteractor->getQtWidget();
        parentWidget->unsetCursor();

        if(m_binButton != nullptr)
        {
            m_binButton->hide();
            delete m_binButton;
            m_binButton = nullptr;
        }
    }
}

//------------------------------------------------------------------------------

void image_multi_distances::updateModifiedDistance(data::point_list::sptr _pl)
{
    if(m_distances.find(_pl->get_id()) == m_distances.end())
    {
        // create Distance if it doesn't exist
        this->createDistance(_pl);
    }
    else
    {
        // if it already exists, update distance with the new position
        auto distanceToUpdate = m_distances[_pl->get_id()];

        const std::array<double, 3> front = _pl->getPoints().front()->getCoord();
        const std::array<double, 3> back  = _pl->getPoints().back()->getCoord();

        const Ogre::Vector3 begin(static_cast<float>(front[0]),
                                  static_cast<float>(front[1]),
                                  static_cast<float>(front[2]));
        const Ogre::Vector3 end(static_cast<float>(back[0]),
                                static_cast<float>(back[1]),
                                static_cast<float>(back[2]));
        this->updateDistance(&distanceToUpdate, begin, end);
    }

    this->requestRender();
}

//------------------------------------------------------------------------------

void image_multi_distances::setCursor(QCursor cursor)
{
    auto interactor    = getLayer()->getRenderService()->getInteractorManager();
    auto qtInteractor  = std::dynamic_pointer_cast<window_interactor>(interactor);
    auto* parentWidget = qtInteractor->getQtWidget();
    parentWidget->setCursor(cursor);
}

//------------------------------------------------------------------------------

void image_multi_distances::removeDistance(data::point_list::sptr _pl)
{
    const auto image = m_image.lock();
    if(auto imageSeries = std::dynamic_pointer_cast<data::image_series>(image.get_shared()))
    {
        std::vector<data::fiducials_series::FiducialSet> fiducialSets = imageSeries->getFiducials()->getFiducialSets();
        for(auto itFiducialSet = fiducialSets.begin() ; itFiducialSet != fiducialSets.end() ; )
        {
            for(auto itFiducial = itFiducialSet->fiducialSequence.begin() ;
                itFiducial != itFiducialSet->fiducialSequence.end() ; )
            {
                if(itFiducial->shapeType == data::fiducials_series::Shape::RULER
                   && itFiducial->fiducialUID == _pl->get_id())
                {
                    itFiducial = itFiducialSet->fiducialSequence.erase(itFiducial);
                }
                else
                {
                    ++itFiducial;
                }
            }

            if(itFiducialSet->fiducialSequence.empty())
            {
                itFiducialSet = fiducialSets.erase(itFiducialSet);
            }
            else
            {
                ++itFiducialSet;
            }
        }

        imageSeries->getFiducials()->setFiducialSets(fiducialSets);
    }
    else
    {
        const auto vectDistance = data::helper::MedicalImage::getDistances(*image);
        const auto newVec       = std::remove(vectDistance->begin(), vectDistance->end(), _pl);
        vectDistance->erase(newVec, vectDistance->end());
    }

    const auto sig = image->signal<data::image::DistanceRemovedSignalType>(
        data::image::DISTANCE_REMOVED_SIG
    );
    sig->async_emit(_pl);
}

image_multi_distances::DeleteBinButtonWhenFocusOut::DeleteBinButtonWhenFocusOut(
    image_multi_distances* sImageMultiDistances
) :
    m_sImageMultiDistances(sImageMultiDistances)
{
}

//------------------------------------------------------------------------------

bool image_multi_distances::DeleteBinButtonWhenFocusOut::eventFilter(QObject* /*o*/, QEvent* e)
{
    if(m_sImageMultiDistances->m_binButton != nullptr && !m_sImageMultiDistances->m_binButton->hasFocus()
       && (e->type() == QEvent::FocusOut || e->type() == QEvent::Resize))
    {
        m_sImageMultiDistances->m_binButton->hide();
        delete m_sImageMultiDistances->m_binButton;
        m_sImageMultiDistances->m_binButton = nullptr;
        return true;
    }

    return false;
}

//------------------------------------------------------------------------------

} // namespace sight::module::viz::scene3dQt::adaptor
