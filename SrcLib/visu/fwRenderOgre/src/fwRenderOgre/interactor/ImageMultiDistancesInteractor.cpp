/************************************************************************
 *
 * Copyright (C) 2019 IRCAD France
 * Copyright (C) 2019 IHU Strasbourg
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

#include "fwRenderOgre/interactor/ImageMultiDistancesInteractor.hpp"

#include "fwRenderOgre/Layer.hpp"

#include <fwData/mt/ObjectWriteLock.hpp>

namespace fwRenderOgre::interactor
{

//------------------------------------------------------------------------------

::Ogre::Vector3 ImageMultiDistancesInteractor::getCamDirection(const ::Ogre::Camera* const _cam)
{
    const ::Ogre::Matrix4 view = _cam->getViewMatrix();
    ::Ogre::Vector3 direction(view[2][0], view[2][1], view[2][2]);
    direction.normalise();
    return -direction;
}

//------------------------------------------------------------------------------

void ImageMultiDistancesInteractor::generateDashedLine(::Ogre::ManualObject* const _object,
                                                      const ::Ogre::Vector3& _begin,
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

std::string ImageMultiDistancesInteractor::getLength(const ::Ogre::Vector3& _begin, const ::Ogre::Vector3& _end)
{
    const int length = static_cast<int>(std::round(std::abs((_end-_begin).length())));
    return std::to_string(length) + "mm";
}

//------------------------------------------------------------------------------

ImageMultiDistancesInteractor::ImageMultiDistancesInteractor(Layer::sptr _layer,
                                                           DistanceMap& _distances,
                                                           float _radius) noexcept :
    IInteractor(_layer),
    m_distances(_distances),
    m_distanceSphereRadius(_radius)
{
    SLM_ASSERT("This interactor must know its layer.", _layer);
}

//------------------------------------------------------------------------------

ImageMultiDistancesInteractor::~ImageMultiDistancesInteractor() noexcept
{

}

//------------------------------------------------------------------------------

void ImageMultiDistancesInteractor::mouseMoveEvent(MouseButton, int _x, int _y, int, int)
{
    if(!m_pickedData.m_id.empty())
    {
        const ::Ogre::Camera* const cam = m_layer.lock()->getDefaultCamera();
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
        this->cancelFurtherLayerInteractions();
    }
}

//------------------------------------------------------------------------------

void ImageMultiDistancesInteractor::buttonReleaseEvent(MouseButton, int, int)
{
    m_pickedData = {"", true, 0.f};
}

//------------------------------------------------------------------------------

void ImageMultiDistancesInteractor::buttonPressEvent(MouseButton _button, int _x, int _y)
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
                const ::Ogre::Camera* const cam = m_layer.lock()->getDefaultCamera();

                const ::Ogre::Vector3 position  = node->getPosition();
                const ::Ogre::Vector3 direction = this->getCamDirection(cam);

                const ::Ogre::Ray ray(position, -direction);
                const ::Ogre::Plane plane(direction, cam->getParentNode()->getPosition());

                const std::pair< bool, Ogre::Real > hit = ::Ogre::Math::intersects(ray, plane);
                SLM_ASSERT("The ray must intersect the plane", hit.first);

                m_pickedData =
                {foundDistance->first, picked == foundDistance->second.m_sphere1 ? true : false, hit.second};
                this->cancelFurtherLayerInteractions();
            }
        }
    }
}

//------------------------------------------------------------------------------

void ImageMultiDistancesInteractor::updateDistance(::fwTools::fwID::IDType _id,
                                                  ::Ogre::Vector3 _begin,
                                                  ::Ogre::Vector3 _end)
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
    const std::string length = ImageMultiDistancesInteractor::getLength(_end, _begin);
    distanceData.m_label->setText(length);

    // Update the dashed line
    ::Ogre::ManualObject* const dashedLine = distanceData.m_dashedLine;
    dashedLine->beginUpdate(0);
    ImageMultiDistancesInteractor::generateDashedLine(dashedLine, _begin, _end, m_distanceSphereRadius);

    // Update the field data.
    const ::fwData::mt::ObjectWriteLock lock(distanceData.m_pointList);
    distanceData.m_pointList->getPoints().front()->setCoord({_begin[0], _begin[1], _begin[2]});
    distanceData.m_pointList->getPoints().back()->setCoord({_end[0], _end[1], _end[2]});

    const auto& sigModified = distanceData.m_pointList->signal< ::fwData::PointList::ModifiedSignalType >(
        ::fwData::PointList::s_MODIFIED_SIG);
    sigModified->asyncEmit();
}

//------------------------------------------------------------------------------

void ImageMultiDistancesInteractor::cancelFurtherLayerInteractions()
{
    const auto layer = m_layer.lock();
    if(layer)
    {
        layer->cancelFurtherInteraction();
    }
}

//------------------------------------------------------------------------------

::Ogre::MovableObject* ImageMultiDistancesInteractor::pickObject(int _x, int _y)
{
    ::Ogre::SceneManager* const sceneMgr = m_layer.lock()->getSceneManager();

    const ::Ogre::Camera* const cam = m_layer.lock()->getDefaultCamera();

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
        if(object->isVisible())
        {
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
    }

    delete raySceneQuery;

    return result;
}

//------------------------------------------------------------------------------

void ImageMultiDistancesInteractor::focusInEvent()
{
}

//------------------------------------------------------------------------------

void ImageMultiDistancesInteractor::focusOutEvent()
{
}

//------------------------------------------------------------------------------

void ImageMultiDistancesInteractor::wheelEvent(int, int, int)
{
}

//------------------------------------------------------------------------------

void ImageMultiDistancesInteractor::resizeEvent(int, int)
{
}

//------------------------------------------------------------------------------

void ImageMultiDistancesInteractor::keyPressEvent(int)
{
}

//------------------------------------------------------------------------------

void ImageMultiDistancesInteractor::keyReleaseEvent(int)
{
}

} // namespace fwRenderOgre::interactor
