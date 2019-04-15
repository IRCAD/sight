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

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwData/Boolean.hpp>
#include <fwData/Image.hpp>
#include <fwData/Integer.hpp>
#include <fwData/Mesh.hpp>
#include <fwData/mt/ObjectReadLock.hpp>
#include <fwData/mt/ObjectReadToWriteLock.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>
#include <fwData/PointList.hpp>
#include <fwData/String.hpp>
#include <fwData/Vector.hpp>

#include <fwDataTools/fieldHelper/Image.hpp>
#include <fwDataTools/helper/Array.hpp>
#include <fwDataTools/helper/Mesh.hpp>
#include <fwDataTools/helper/Vector.hpp>

#include <fwRenderOgre/helper/Camera.hpp>
#include <fwRenderOgre/helper/Font.hpp>
#include <fwRenderOgre/helper/ManualObject.hpp>
#include <fwRenderOgre/helper/Scene.hpp>
#include <fwRenderOgre/Mesh.hpp>
#include <fwRenderOgre/R2VBRenderable.hpp>
#include <fwRenderOgre/SRender.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/op/Add.hpp>

#include <glm/glm.hpp>

#include <OgreMeshManager.h>
#include <OgreVector3.h>

namespace visuOgreAdaptor
{

fwServicesRegisterMacro( ::fwRenderOgre::IAdaptor, ::visuOgreAdaptor::SImageMultiDistances);

static const ::fwServices::IService::KeyType s_IMAGE_INOUT     = "image";
static const ::fwServices::IService::KeyType s_POINTLIST_INPUT = "pointList";

SImageMultiDistances::SImageMultiDistances() noexcept
{

}

//------------------------------------------------------------------------------

SImageMultiDistances::~SImageMultiDistances() noexcept
{

}

//------------------------------------------------------------------------------

void SImageMultiDistances::configuring()
{
    this->configureParams();
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SImageMultiDistances::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push(s_IMAGE_INOUT, ::fwData::Image::s_DISTANCE_ADDED_SIG, s_UPDATE_SLOT);
    connections.push(s_IMAGE_INOUT, ::fwData::Image::s_DISTANCE_REMOVED_SIG, s_UPDATE_SLOT);
    connections.push(s_IMAGE_INOUT, ::fwData::Image::s_DISTANCE_DISPLAYED_SIG, s_UPDATE_SLOT);
    connections.push(s_IMAGE_INOUT, ::fwData::Image::s_SLICE_INDEX_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_IMAGE_INOUT, ::fwData::Image::s_SLICE_TYPE_MODIFIED_SIG, s_UPDATE_SLOT);
    return connections;
}

//------------------------------------------------------------------------------

void SImageMultiDistances::starting()
{
    this->initialize();
    this->getRenderService()->makeCurrent();

    m_sceneMgr = this->getSceneManager();
    m_picker.setSceneManager(m_sceneMgr);

    m_rootSceneNode = m_sceneMgr->getRootSceneNode();
    m_materialAdp   = this->setMaterialAdp();

    auto layer = this->getRenderService()->getLayer(m_layerID);
    layer->setSelectInteractor(std::dynamic_pointer_cast< ::fwRenderOgre::interactor::IInteractor >(this->getSptr()));
}

//------------------------------------------------------------------------------

void SImageMultiDistances::updating()
{
    const ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("Missing image", image);

    ::fwData::Vector::sptr distanceField;
    distanceField = image->getField< ::fwData::Vector >( ::fwDataTools::fieldHelper::Image::m_imageDistancesId);

    // lock image
    const bool isShown = image->getField("ShowDistances", ::fwData::Boolean::New(true))->value();

    // removes the displayed lines if they exist
    this->deleteDistances();
    m_millimeterNodes.clear();
    m_millimeterValue.clear();
    m_labels.clear();
    m_labelNodes.clear();
    if( distanceField && isShown)
    {
        m_distanceField = distanceField;
        /// adds the origin to my distances copy
        this->removeCurrentOrigin();
        for(::fwData::Object::sptr object :  *m_distanceField)
        {
            m_millimeterNodes.push_back(nullptr);
            m_millimeterValue.push_back(nullptr);
            m_labels.push_back(nullptr);
            m_labelNodes.push_back(nullptr);
            const ::fwData::PointList::sptr distance = ::fwData::PointList::dynamicCast(object);
            SLM_ASSERT( "Empty Point List for Distance !!!!", !distance->getPoints().empty() );
            /// displays a given line
            const ::fwData::Point::cwptr pointFront = distance->getPoints().front();
            const ::fwData::Point::cwptr pointBack  = distance->getPoints().back();
            const ::fwData::Point::csptr p1         = pointFront.lock();
            const ::fwData::Point::csptr p2         = pointBack.lock();
            float ps1[3];
            float ps2[3];
            std::copy(p1->getCoord().begin(), (p1)->getCoord().end(), ps1 );
            std::copy(p2->getCoord().begin(), (p2)->getCoord().end(), ps2 );
            this->createDistance(ps1, ps2, m_distanceNb, generateColor(m_distanceNb));
            ++m_distanceNb;
        }
        /// remove the origin to my distances copy
        this->addCurrentOrigin();
    }
    this->requestRender();
}

//------------------------------------------------------------------------------

void SImageMultiDistances::stopping()
{
    this->getRenderService()->makeCurrent();
    this->unregisterServices();
    this->deleteAllRessources();
}

//------------------------------------------------------------------------------

void SImageMultiDistances::createDistance(float _begin[3], float _end[3], size_t _id,
                                          const ::Ogre::ColourValue& _color)
{
    /// Increase some points to avoid z-fighting
    switch (this->getOrientation())
    {
        /// Look in z axis, axial
        case ::fwDataTools::helper::MedicalImage::Orientation::Z_AXIS:
        {
            _begin[2] += 0.01f;
            _end[2]   += 0.01f;
            break;
        }
        /// Look in y axis, frontal
        case ::fwDataTools::helper::MedicalImage::Orientation::Y_AXIS:
        {
            _begin[1] += 0.01f;
            _end[1]   += 0.01f;
            break;
        }
        /// Look in x axis, sagittal
        case ::fwDataTools::helper::MedicalImage::Orientation::X_AXIS:
        {
            _begin[0] += 0.01f;
            _end[0]   += 0.01f;
            break;
        }
    }
    this->createLine(m_materialAdp, _begin, _end, _id, _color);
    this->createIdLabel(_begin, _id, _color);

    Ogre::Vector3 a =
        Ogre::Vector3(static_cast<Ogre::Real>(_begin[0]), static_cast<Ogre::Real>(_begin[1]),
                      static_cast<Ogre::Real>(_begin[2]));
    Ogre::Vector3 b =
        Ogre::Vector3(static_cast<Ogre::Real>(_end[0]), static_cast<Ogre::Real>(_end[1]),
                      static_cast<Ogre::Real>(_end[2]));
    this->createMillimeterLabel(_end, a.distance(b), _id, _color);

    ::Ogre::MeshPtr mesh =
        this->createSphere(m_materialAdp, this->getID() + "_sphere-" + std::to_string(_id), _color)->convertToMesh(
            this->getID() + "_sphere-" + std::to_string(_id));

    /// Create sphere 1 entity and attach the entity on the different node
    ::Ogre::Entity* const newSphere =
        m_sceneMgr->createEntity(this->getID() + "Entitypoint1-" + std::to_string(_id), mesh );
    newSphere->setVisible(true);
    m_sphere1Node =
        this->m_rootSceneNode->createChildSceneNode(this->getID() + "pointNode1" +
                                                    std::to_string(_id));
    m_sphere1Node->attachObject(newSphere);
    m_sphere1Node->translate(_begin[0], _begin[1], _begin[2]);

    /// Create sphere 2 entity and attach the entity on the different node
    ::Ogre::Entity* const newSphere2 =
        m_sceneMgr->createEntity(this->getID() + "Entitypoint2-" + std::to_string(_id), mesh );
    newSphere->setVisible(true);

    m_sphere2Node =
        this->m_rootSceneNode->createChildSceneNode(this->getID() + "pointNode2" +
                                                    std::to_string(_id));
    m_sphere2Node->attachObject(newSphere2);
    m_sphere2Node->translate(_end[0], _end[1], _end[2]);
}

//------------------------------------------------------------------------------

void SImageMultiDistances::deleteDistance(size_t _id)
{
    const ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("Missing image", image);

    /// Hide a distance
    this->deleteLine(_id);
    this->deleteIdLabel(_id);
    this->deleteMillimeterLabel(_id);

    ::Ogre::Entity* const sphere = m_sceneMgr->getEntity(
        this->getID() + "Entitypoint1-" + std::to_string(_id));
    ::Ogre::Entity* const sphere2 = m_sceneMgr->getEntity(
        this->getID() + "Entitypoint2-" + std::to_string(_id));
    ::Ogre::ManualObject* const mesh = m_sceneMgr->getManualObject(
        this->getID() + "_sphere-" + std::to_string(_id));

    m_rootSceneNode->removeAndDestroyChild(this->getID() + "pointNode1" + std::to_string(_id));
    m_rootSceneNode->removeAndDestroyChild(this->getID() + "pointNode2" + std::to_string(_id));

    m_sceneMgr->destroyEntity(sphere);
    m_sceneMgr->destroyEntity(sphere2);
    m_sceneMgr->destroyManualObject(mesh);

    ::Ogre::MeshManager& meshManager = ::Ogre::MeshManager::getSingleton();
    meshManager.remove(this->getID() + "_sphere-" + std::to_string(_id));
}

//------------------------------------------------------------------------------

void SImageMultiDistances::deleteDistances()
{
    /// Delete all distance
    for (size_t id = 0; id < m_distanceNb; ++id)
    {
        this->deleteDistance(id);
    }
    m_distanceNb = 0;
    m_millimeterValue.clear();
    m_millimeterNodes.clear();
}

//------------------------------------------------------------------------------

void SImageMultiDistances::addCurrentOrigin()
{
    const ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("Missing image", image);

    ::fwData::Vector::ContainerType& container = m_distanceField->getContainer();
    double ps1[3];
    double ps2[3];

    /// removes the origin of all points
    for (auto it = container.begin(); it != container.end(); ++it)
    {
        const ::fwData::PointList::sptr distance = ::fwData::PointList::dynamicCast(*it);
        const ::fwData::Point::cwptr pointFront  = distance->getPoints()[0];
        const ::fwData::Point::cwptr pointBack   = distance->getPoints()[1];
        const ::fwData::Point::csptr p1          = pointFront.lock();
        const ::fwData::Point::csptr p2          = pointBack.lock();
        std::copy(p1->getCoord().begin(), (p1)->getCoord().end(), ps1 );
        std::copy(p2->getCoord().begin(), (p2)->getCoord().end(), ps2 );

        double point[3];
        point[0] = (ps1[0] + image->getOrigin()[0] );
        point[1] = (ps1[1] + image->getOrigin()[1] );
        point[2] = (ps1[2] + image->getOrigin()[2] );

        double point2[3];
        point2[0] = (ps2[0] + image->getOrigin()[0] );
        point2[1] = (ps2[1] + image->getOrigin()[1] );
        point2[2] = (ps2[2] + image->getOrigin()[2] );

        distance->getPoints()[0]->setCoord({point[0], point[1], point[2]});
        distance->getPoints()[1]->setCoord({point2[0], point2[1], point2[2]});
    }
}

//------------------------------------------------------------------------------

void SImageMultiDistances::removeCurrentOrigin()
{
    const ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("Missing image", image);

    ::fwData::Vector::ContainerType& container = m_distanceField->getContainer();
    double ps1[3];
    double ps2[3];

    /// set the origin of all points
    for (auto it = container.begin(); it != container.end(); ++it)
    {
        const ::fwData::PointList::sptr distance = ::fwData::PointList::dynamicCast(*it);
        const ::fwData::Point::cwptr pointFront  = distance->getPoints()[0];
        const ::fwData::Point::cwptr pointBack   = distance->getPoints()[1];
        const ::fwData::Point::csptr p1          = pointFront.lock();
        const ::fwData::Point::csptr p2          = pointBack.lock();
        std::copy(p1->getCoord().begin(), (p1)->getCoord().end(), ps1 );
        std::copy(p2->getCoord().begin(), (p2)->getCoord().end(), ps2 );

        double point[3];
        point[0] = (ps1[0] - image->getOrigin()[0] );
        point[1] = (ps1[1] - image->getOrigin()[1] );
        point[2] = (ps1[2] - image->getOrigin()[2] );

        double point2[3];
        point2[0] = (ps2[0] - image->getOrigin()[0] );
        point2[1] = (ps2[1] - image->getOrigin()[1] );
        point2[2] = (ps2[2] - image->getOrigin()[2] );

        distance->getPoints()[0]->setCoord({point[0], point[1], point[2]});
        distance->getPoints()[1]->setCoord({point2[0], point2[1], point2[2]});
    }
}

//------------------------------------------------------------------------------

void SImageMultiDistances::createMillimeterLabel(const float point[3], const Ogre::Real distance, size_t _id,
                                                 const ::Ogre::ColourValue _color)
{
    ::Ogre::OverlayContainer* textContainer = this->getRenderService()->getOverlayTextPanel();
    const ::Ogre::FontPtr dejaVuSansFont = ::fwRenderOgre::helper::Font::getFont("DejaVuSans.ttf", 32);
    ::Ogre::Camera* cam = this->getLayer()->getDefaultCamera();
    const std::string labelNumber = std::to_string(distance) + "mm";

    m_millimeterValue[_id] = ::fwRenderOgre::Text::New(this->getID() + labelNumber + std::to_string(_id),
                                                       m_sceneMgr,
                                                       textContainer,
                                                       dejaVuSansFont, cam);
    m_millimeterValue[_id]->setText(labelNumber);
    m_millimeterValue[_id]->setCharHeight(0.03f);
    m_millimeterValue[_id]->setTextColor(_color);

    m_millimeterNodes[_id] = m_rootSceneNode->createChildSceneNode(this->getID() + "_distance" +
                                                                   std::to_string(_id));
    m_millimeterNodes[_id]->attachObject(m_millimeterValue[_id]);
    ///+ 3 so that the label is not hidden by the sphere when zooming in
    m_millimeterNodes[_id]->translate(point[0] + 3, point[1] + 3, point[2] + 3);
}

//------------------------------------------------------------------------------

void SImageMultiDistances::deleteMillimeterLabel(size_t _id)
{
    m_rootSceneNode->removeAndDestroyChild(this->getID() + "_distance" + std::to_string(_id));
    m_sceneMgr->destroyMovableObject(m_millimeterValue[_id]);

    m_millimeterNodes[_id] = nullptr;
    m_millimeterValue[_id] = nullptr;
}

//------------------------------------------------------------------------------

void SImageMultiDistances::createIdLabel(const float ps1[3], size_t _id, const ::Ogre::ColourValue _color)
{
    ::Ogre::OverlayContainer* textContainer = this->getRenderService()->getOverlayTextPanel();
    const ::Ogre::FontPtr dejaVuSansFont = ::fwRenderOgre::helper::Font::getFont("DejaVuSans.ttf", 32);
    ::Ogre::Camera* cam = this->getLayer()->getDefaultCamera();
    const std::string labelNumber = std::to_string(_id);
    ::fwRenderOgre::Text* const text = ::fwRenderOgre::Text::New(this->getID() + labelNumber, m_sceneMgr, textContainer,
                                                                 dejaVuSansFont, cam);
    m_labels[_id] = text;
    text->setText(labelNumber);
    text->setCharHeight(0.03f);
    text->setTextColor(_color);

    m_labelNodes[_id] = m_rootSceneNode->createChildSceneNode(this->getID() + "_id" + labelNumber);
    m_labelNodes[_id]->attachObject(m_labels[_id]);
    m_labelNodes[_id]->translate(ps1[0] + 3, ps1[1] + 3, ps1[2] + 3);
}

//------------------------------------------------------------------------------

void SImageMultiDistances::deleteIdLabel(size_t _id)
{
    m_rootSceneNode->removeAndDestroyChild(this->getID() + "_id" + std::to_string(_id));
    m_sceneMgr->destroyMovableObject(m_labels[_id]);

    m_labels[_id]     = nullptr;
    m_labelNodes[_id] = nullptr;
}

//------------------------------------------------------------------------------

void SImageMultiDistances::createLine(const ::visuOgreAdaptor::SMaterial::sptr materialAdp,
                                      const float ps1[3], const float ps2[3], size_t _id,
                                      const ::Ogre::ColourValue& _color) const
{
    const ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("Missing image", image);

    const ::glm::vec3 f3APos_Is(ps1[0], ps1[1], ps1[2]);
    const ::glm::vec3 f3BPos_Is(ps2[0], ps2[1], ps2[2]);

    ::glm::vec3 f3PlanePos_Is;
    ::glm::vec3 f3PlaneDir_IsN;

    /// Checks the axis orientation to add the line correctly
    switch (this->getOrientation())
    {
        case ::fwDataTools::helper::MedicalImage::Orientation::Z_AXIS:
        {
            const int axialIndex = image->getField< ::fwData::Integer >(
                ::fwDataTools::fieldHelper::Image::m_axialSliceIndexId)->value();
            const double axialPos = axialIndex * image->getSpacing()[2];
            f3PlanePos_Is  = {0, 0, axialPos};
            f3PlaneDir_IsN = {0, 0, -1};
            break;
        }
        case ::fwDataTools::helper::MedicalImage::Orientation::Y_AXIS:
        {
            const int frontalIndex = image->getField< ::fwData::Integer >(
                ::fwDataTools::fieldHelper::Image::m_frontalSliceIndexId)->value();
            const double frontalPos = frontalIndex * image->getSpacing()[1];
            f3PlanePos_Is  = {0, frontalPos, 0};
            f3PlaneDir_IsN = {0, -1, 0};
            break;
        }
        case ::fwDataTools::helper::MedicalImage::Orientation::X_AXIS:
        {
            const int sagittalIndex = image->getField< ::fwData::Integer >(
                ::fwDataTools::fieldHelper::Image::m_sagittalSliceIndexId)->value();
            const double sagittalPos = sagittalIndex * image->getSpacing()[0];
            f3PlanePos_Is  = {sagittalPos, 0, 0};
            f3PlaneDir_IsN = {-1, 0, 0};
            break;
        }
    }

    /// Compute the intersection
    const float s = ::glm::dot(f3PlaneDir_IsN, f3PlanePos_Is - f3APos_Is) / ::glm::dot(
        f3PlaneDir_IsN, f3BPos_Is - f3APos_Is);
    const glm::vec3 f3intersectPos_Is = f3APos_Is + s * (f3BPos_Is - f3APos_Is);

    /// Draw lines
    ::Ogre::ManualObject* const line =
        m_sceneMgr->createManualObject(this->getID() + "_line" + std::to_string(_id));

    ::Ogre::ManualObject* const lineDashed =
        m_sceneMgr->createManualObject(this->getID() + "_lineDashed" + std::to_string(_id));

    line->begin(materialAdp->getMaterialName(), ::Ogre::RenderOperation::OT_LINE_LIST);
    line->colour(_color);

    lineDashed->begin(materialAdp->getMaterialName(), ::Ogre::RenderOperation::OT_LINE_LIST);
    lineDashed->colour(_color);

    int compar = -1;
    switch (this->getOrientation())
    {
        case ::fwDataTools::helper::MedicalImage::Orientation::Z_AXIS:
            compar = 2;
            break;
        case ::fwDataTools::helper::MedicalImage::Orientation::Y_AXIS:
            compar = 1;
            break;
        case ::fwDataTools::helper::MedicalImage::Orientation::X_AXIS:
            compar = 0;
            break;
    }

    // Function to create a dashed line
    auto dashed = [&](::glm::vec3 _f3BeginPos_Is, const ::glm::vec3& _f3EndPos_Is)
                  {
                      const ::glm::vec3 f3LineDir_Is  = _f3EndPos_Is - _f3BeginPos_Is;
                      const float len                 = glm::distance(_f3EndPos_Is, _f3BeginPos_Is);
                      const ::glm::vec3 f3LineDir_IsN = ::glm::normalize(f3LineDir_Is);
                      const float step                = 2.5f;
                      for(float i = 0.f; i <= len; i += step*2)
                      {
                          lineDashed->position(_f3BeginPos_Is[0], _f3BeginPos_Is[1], _f3BeginPos_Is[2]);
                          _f3BeginPos_Is += f3LineDir_IsN*step;
                          lineDashed->position(_f3BeginPos_Is[0], _f3BeginPos_Is[1], _f3BeginPos_Is[2]);
                          _f3BeginPos_Is += f3LineDir_IsN*step;
                      }
                  };

    // Create line
    if(f3APos_Is[compar] >= f3intersectPos_Is[compar] && f3BPos_Is[compar] >= f3intersectPos_Is[compar])
    {
        dashed(f3APos_Is, f3BPos_Is);
    }
    else if((f3APos_Is[compar] < f3intersectPos_Is[compar] && f3intersectPos_Is[compar] < f3BPos_Is[compar]) ||
            (f3APos_Is[compar] > f3intersectPos_Is[compar] && f3intersectPos_Is[compar] > f3BPos_Is[compar]))
    {
        if(f3APos_Is[compar] > f3BPos_Is[compar])
        {
            dashed(f3APos_Is, f3intersectPos_Is);
            line->position(f3BPos_Is[0], f3BPos_Is[1], f3BPos_Is[2]);
            line->position(f3intersectPos_Is[0], f3intersectPos_Is[1], f3intersectPos_Is[2]);
        }
        else
        {
            dashed(f3BPos_Is, f3intersectPos_Is);
            line->position(f3APos_Is[0], f3APos_Is[1], f3APos_Is[2]);
            line->position(f3intersectPos_Is[0], f3intersectPos_Is[1], f3intersectPos_Is[2]);
        }
    }
    else if(f3APos_Is[compar] > f3PlanePos_Is[compar])
    {
        line->position(f3APos_Is[0], f3APos_Is[1], f3APos_Is[2]);
        line->position(f3BPos_Is[0], f3BPos_Is[1], f3BPos_Is[2]);
    }
    else
    {
        line->position(f3APos_Is[0], f3APos_Is[1], f3APos_Is[2]);
        line->position(f3BPos_Is[0], f3BPos_Is[1], f3BPos_Is[2]);
    }

    line->end();
    lineDashed->end();
    m_rootSceneNode->attachObject(line);
    m_rootSceneNode->attachObject(lineDashed);
}

//------------------------------------------------------------------------------

void SImageMultiDistances::deleteLine(size_t _id) const
{
    ::Ogre::ManualObject* const line =
        m_sceneMgr->getManualObject(this->getID() + "_line" + std::to_string(_id));
    ::Ogre::ManualObject* const lineDashed =
        m_sceneMgr->getManualObject(this->getID() + "_lineDashed" + std::to_string(_id));
    m_sceneMgr->destroyManualObject(line);
    m_sceneMgr->destroyManualObject(lineDashed);
}

//------------------------------------------------------------------------------

::Ogre::ManualObject* SImageMultiDistances::createSphere(const ::visuOgreAdaptor::SMaterial::sptr materialAdp,
                                                         const std::string name, ::Ogre::ColourValue color) const
{
    ::Ogre::ManualObject* const sphere =
        m_sceneMgr->createManualObject(name);
    ::fwRenderOgre::helper::ManualObject::createSphere(sphere,
                                                       materialAdp->getMaterialName(), color,
                                                       2.5f);
    return sphere;
}

//------------------------------------------------------------------------------

void SImageMultiDistances::deleteAllRessources()
{
    this->deleteDistances();

    m_materialAdp.reset();
    m_material.reset();
}

//------------------------------------------------------------------------------

::Ogre::ColourValue SImageMultiDistances::generateColor(size_t _id) const
{
    switch(_id % 7)
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

::visuOgreAdaptor::SMaterial::sptr SImageMultiDistances::setMaterialAdp()
{
    m_material = ::fwData::Material::New();

    ::visuOgreAdaptor::SMaterial::sptr materialAdp = this->registerService< ::visuOgreAdaptor::SMaterial >(
        "::visuOgreAdaptor::SMaterial");
    materialAdp->registerInOut(m_material, ::visuOgreAdaptor::SMaterial::s_MATERIAL_INOUT, true);
    materialAdp->setID(this->getID() + materialAdp->getID());
    materialAdp->setMaterialName(this->getID() + materialAdp->getID());
    materialAdp->setRenderService( this->getRenderService() );
    materialAdp->setLayerID(m_layerID);
    materialAdp->setShadingMode("ambient");
    materialAdp->setMaterialTemplateName(::fwRenderOgre::Material::DEFAULT_MATERIAL_TEMPLATE_NAME);
    materialAdp->start();
    materialAdp->getMaterialFw()->setHasVertexColor(true);
    materialAdp->update();
    return materialAdp;
}

//------------------------------------------------------------------------------

::fwDataTools::helper::MedicalImage::Orientation SImageMultiDistances::getOrientation() const
{
    ::Ogre::Camera* cam  = this->getLayer()->getDefaultCamera();
    ::Ogre::Matrix4 view = cam->getViewMatrix();

    /// Look in z axis, axial
    if(std::abs(view[0][0]-1) <= std::numeric_limits<float>::epsilon()*2 &&
       std::abs(view[1][1]+1) <= std::numeric_limits<float>::epsilon()*2 &&
       std::abs(view[2][2]+1) <= std::numeric_limits<float>::epsilon()*2)
    {
        return ::fwDataTools::helper::MedicalImage::Orientation::Z_AXIS;
    }
    /// Look in  y, frontal
    else if(std::abs(view[0][0]-1) <= std::numeric_limits<float>::epsilon()*2 &&
            std::abs(view[1][2]-1) <= std::numeric_limits<float>::epsilon()*2 &&
            std::abs(view[2][1]+1) <= std::numeric_limits<float>::epsilon()*2)
    {
        return ::fwDataTools::helper::MedicalImage::Orientation::Y_AXIS;
    }
    /// Look in x, sagittal
    else
    {
        return ::fwDataTools::helper::MedicalImage::Orientation::X_AXIS;
    }
}

//------------------------------------------------------------------------------

void SImageMultiDistances::mouseMoveEvent(MouseButton button, int x, int y, int, int)
{
    if (button != MouseButton::LEFT || !m_activeInteraction)
    {
        return;
    }
    this->deleteDistance(m_moveID);

    /// Set the current cursor position to ogre position
    ::fwData::Mesh::PointValueType clickedPoint[3] {static_cast<float>(x), static_cast<float>(y), -1};
    const ::Ogre::Vector3 worldspaceClikedPoint = ::fwRenderOgre::helper::Camera::convertPixelToViewSpace(
        *(this->getLayer()->getDefaultCamera()), clickedPoint);

    const ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("Missing image", image);

    float begin[3];
    float end[3];

    /// Move points and line
    if (m_isBeginMove == 0)
    {
        begin[0] = m_ps2[0];
        begin[1] = m_ps2[1];
        begin[2] = m_ps2[2];
    }
    else
    {
        begin[0] = m_ps1[0];
        begin[1] = m_ps1[1];
        begin[2] = m_ps1[2];
    }

    /// Checks the axis orientation to add the points correctly
    switch (this->getOrientation())
    {
        /// Look in z axis, axial]
        case ::fwDataTools::helper::MedicalImage::Orientation::Z_AXIS:
        {
            const int axialIndex = image->getField< ::fwData::Integer >(
                ::fwDataTools::fieldHelper::Image::m_axialSliceIndexId)->value();
            const float axialPos = static_cast<float>(axialIndex * image->getSpacing()[2]);
            end[0] = worldspaceClikedPoint[0];
            end[1] = worldspaceClikedPoint[1];
            end[2] = axialPos;
            break;
        }
        /// Look in y axis, frontal
        case ::fwDataTools::helper::MedicalImage::Orientation::Y_AXIS:
        {
            const int frontalIndex = image->getField< ::fwData::Integer >(
                ::fwDataTools::fieldHelper::Image::m_frontalSliceIndexId)->value();
            const float frontalPos = static_cast<float>(frontalIndex * image->getSpacing()[1]);
            end[0] = worldspaceClikedPoint[0];
            end[1] = frontalPos;
            end[2] = worldspaceClikedPoint[2];
            break;
        }
        /// Look in -z axis, sagittal
        case ::fwDataTools::helper::MedicalImage::Orientation::X_AXIS:
        {
            const int sagittalIndex = image->getField< ::fwData::Integer >(
                ::fwDataTools::fieldHelper::Image::m_sagittalSliceIndexId)->value();
            const float sagittalPos = static_cast<float>(sagittalIndex * image->getSpacing()[0]);
            end[0] = sagittalPos;
            end[1] = worldspaceClikedPoint[1];
            end[2] = worldspaceClikedPoint[2];
            break;
        }
    }

    if(m_isBeginMove == 0)
    {
        std::swap(begin, end);
    }

    this->createDistance(begin, end, m_moveID, ::Ogre::ColourValue::Red);
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

//------------------------------------------------------------------------------

void SImageMultiDistances::buttonReleaseEvent(MouseButton button, int x, int y)
{
    if (button != MouseButton::LEFT || !m_activeInteraction)
    {
        return;
    }
    this->removeCurrentOrigin();

    ::fwData::Vector::ContainerType& container = m_distanceField->getContainer();

    /// Set the current cursor position to ogre position
    const ::fwData::Mesh::PointValueType clickedPoint[3] {static_cast<float>(x), static_cast<float>(y), -1};
    ::Ogre::Vector3 worldspaceClikedPoint = ::fwRenderOgre::helper::Camera::convertPixelToViewSpace(
        *(this->getLayer()->getDefaultCamera()), clickedPoint);

    ::fwData::PointList::sptr pointList = ::fwData::PointList::dynamicCast(container[m_moveID]);

    const ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("Missing image", image);

    /// Checks the axis orientation to add the points correctly
    switch (this->getOrientation())
    {
        /// Look in z axis, axial]
        case ::fwDataTools::helper::MedicalImage::Orientation::Z_AXIS:
        {
            const int axialIndex = image->getField< ::fwData::Integer >(
                ::fwDataTools::fieldHelper::Image::m_axialSliceIndexId)->value();
            const double axialPos = static_cast<double>(axialIndex * image->getSpacing()[2]);
            pointList->getPoints()[m_isBeginMove]->setCoord({static_cast<double>(worldspaceClikedPoint[0]),
                                                             static_cast<double>(worldspaceClikedPoint[1]),
                                                             axialPos});
            break;
        }
        /// Look in y axis, frontal
        case ::fwDataTools::helper::MedicalImage::Orientation::Y_AXIS:
        {
            const int frontalIndex = image->getField< ::fwData::Integer >(
                ::fwDataTools::fieldHelper::Image::m_frontalSliceIndexId)->value();
            const double frontalPos = frontalIndex * image->getSpacing()[1];
            pointList->getPoints()[m_isBeginMove]->setCoord({static_cast<double>(worldspaceClikedPoint[0]),
                                                             frontalPos,
                                                             static_cast<double>(worldspaceClikedPoint[2])});
            break;
        }
        /// Look in x axis, sagittal
        case ::fwDataTools::helper::MedicalImage::Orientation::X_AXIS:
        {
            const int sagittalIndex = image->getField< ::fwData::Integer >(
                ::fwDataTools::fieldHelper::Image::m_sagittalSliceIndexId)->value();
            const double sagittalPos = sagittalIndex * image->getSpacing()[0];
            pointList->getPoints()[m_isBeginMove]->setCoord({sagittalPos,
                                                             static_cast<double>(worldspaceClikedPoint[1]),
                                                             static_cast<double>(worldspaceClikedPoint[2])});
            break;
        }
    }

    this->addCurrentOrigin();

    this->update();
}

//------------------------------------------------------------------------------

Ogre::MovableObject* SImageMultiDistances::pickObject(int x, int y)
{
    ::Ogre::Camera* cam = m_sceneMgr->getCamera(::fwRenderOgre::Layer::DEFAULT_CAMERA_NAME);

    const int height       = cam->getViewport()->getActualHeight();
    const int width        = cam->getViewport()->getActualWidth();
    const bool pickSuccess = m_picker.executeRaySceneQuery( x, y, width, height, 0 );

    return pickSuccess ? m_picker.getSelectedObject() : nullptr;
}

//------------------------------------------------------------------------------

size_t SImageMultiDistances::findIDLine(std::string str)
{
    size_t found = str.find("-");
    size_t start = found + 1;
    size_t end   = str.size();
    size_t res   = static_cast<size_t>(std::stoi(str.substr(start, end - start)));
    return res;
}

//------------------------------------------------------------------------------

bool SImageMultiDistances::clickPoint(int x, int y)
{
    Ogre::MovableObject* picked = this->pickObject(x, y);

    if (picked != nullptr)
    {
        /// Check if point 1 was picked
        std::size_t found = picked->getName().find("Entitypoint1-");
        if (found != std::string::npos)
        {
            m_moveID      = this->findIDLine(picked->getName());
            m_isBeginMove = 0;
            return true;
        }
        /// Check if point 2 was picked
        found = picked->getName().find("Entitypoint2-");
        if (found != std::string::npos)
        {
            m_moveID      = this->findIDLine(picked->getName());
            m_isBeginMove = 1;
            return true;
        }
    }
    return false;
}

//------------------------------------------------------------------------------

void SImageMultiDistances::buttonPressEvent(MouseButton button, int x, int y)
{
    const ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);

    if (button != MouseButton::LEFT || !m_distanceField)
    {
        return;
    }
    m_moveID            = 0;
    m_activeInteraction = false;

    /// Checks if an entity sphere is picked on
    if (this->clickPoint(x, y) == true)
    {
        this->removeCurrentOrigin();
        ::fwData::Vector::ContainerType& container = m_distanceField->getContainer();
        ::fwData::PointList::sptr distance         = ::fwData::PointList::dynamicCast(container[m_moveID]);
        SLM_ASSERT( "Empty Point List for Distance !!!!", !distance->getPoints().empty() );

        const ::fwData::Point::cwptr pointFront = distance->getPoints()[0];
        const ::fwData::Point::cwptr pointBack  = distance->getPoints()[1];
        const ::fwData::Point::csptr p1         = pointFront.lock();
        const ::fwData::Point::csptr p2         = pointBack.lock();

        std::copy(p1->getCoord().begin(), (p1)->getCoord().end(), m_ps1 );
        std::copy(p2->getCoord().begin(), (p2)->getCoord().end(), m_ps2 );
        m_activeInteraction = true;
        this->mouseMoveEvent(button, x, y, 0, 0);
        this->addCurrentOrigin();
    }
}

//------------------------------------------------------------------------------

void SImageMultiDistances::focusInEvent()
{
}

//------------------------------------------------------------------------------

void SImageMultiDistances::focusOutEvent()
{
}

}
