/************************************************************************
 *
 * Copyright (C) 2019-2023 IRCAD France
 * Copyright (C) 2019-2021 IHU Strasbourg
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

#include "viz/scene3d/interactor/ClippingBoxInteractor.hpp"

#include "viz/scene3d/Layer.hpp"

namespace sight::viz::scene3d::interactor
{

//------------------------------------------------------------------------------

ClippingBoxInteractor::ClippingBoxInteractor(
    Layer::sptr _layer,
    bool _layerOrderDependant,
    const std::string& _id,
    Ogre::SceneNode* _parentSceneNode,
    const Ogre::Matrix4& _clippingMatrix,
    const widget::ClippingBox::ClippingUpdateCallbackType& _clippingUpdateCb,
    const std::string& _boxMtlName,
    const std::string& _handleMtlName
) noexcept :
    IInteractor(_layer, _layerOrderDependant),
    m_widget(_id, _parentSceneNode, _layer->getDefaultCamera(), _layer->getSceneManager(),
             _clippingMatrix, _clippingUpdateCb, _boxMtlName, _handleMtlName)
{
    SIGHT_ASSERT("This interactor must know its layer.", _layer);
}

//------------------------------------------------------------------------------

Ogre::MovableObject* ClippingBoxInteractor::pickObject(int x, int y)
{
    if(auto layer = m_layer.lock())
    {
        const auto result = viz::scene3d::Utils::pickObject(x, y, 0xFFFFFFFF, *layer->getSceneManager());

        return result.has_value() ? result->first : nullptr;
    }

    return nullptr;
}

//------------------------------------------------------------------------------

void ClippingBoxInteractor::mouseMoveEvent(MouseButton button, Modifier /*_mods*/, int x, int y, int dx, int dy)
{
    if(m_widget.getVisibility()) // If a widget is present in the scene.
    {
        bool interacted = m_pickedObject != nullptr;

        if(button == LEFT && interacted)
        {
            m_widget.widgetPicked(m_pickedObject, x, y);
        }
        else if(button == MIDDLE)
        {
            interacted = m_widget.moveClippingBox(x, y, -dx, -dy);
        }
        else if(button == RIGHT)
        {
            interacted = m_widget.scaleClippingBox(x, y, dy);
        }

        if(interacted)
        {
            this->cancelFurtherLayerInteractions();
        }
    }
}

//------------------------------------------------------------------------------

void ClippingBoxInteractor::buttonReleaseEvent(MouseButton /*_button*/, Modifier /*_mods*/, int /*_x*/, int /*_y*/)
{
    if(m_widget.getVisibility())
    {
        m_widget.widgetReleased();
        m_pickedObject = nullptr;
    }
}

//------------------------------------------------------------------------------

void ClippingBoxInteractor::buttonPressEvent(MouseButton button, Modifier /*_mods*/, int x, int y)
{
    if(m_widget.getVisibility())
    {
        bool interacted = false;
        if(button == LEFT)
        {
            m_pickedObject = pickObject(x, y);

            interacted = m_widget.belongsToWidget(m_pickedObject);
            if(interacted)
            {
                m_widget.widgetPicked(m_pickedObject, x, y);
            }
            else
            {
                m_pickedObject = nullptr;
            }
        }
        else if(button == MIDDLE)
        {
            interacted = m_widget.moveClippingBox(x, y, 0, 0);
        }
        else if(button == RIGHT)
        {
            interacted = m_widget.scaleClippingBox(x, y, 0);
        }

        if(interacted)
        {
            this->cancelFurtherLayerInteractions();
        }
    }
}

//------------------------------------------------------------------------------

void ClippingBoxInteractor::pinchGestureEvent(double _scaleFactor, int _centerX, int _centerY)
{
    if(m_widget.getVisibility())
    {
        // Convert back the scale from "delta"
        if(_scaleFactor < 0)
        {
            _scaleFactor = -1.0 / _scaleFactor;
        }

        const auto dy = int(((_centerY * _scaleFactor) - _centerY) * 0.5);

        if(m_widget.scaleClippingBox(_centerX, _centerY, dy) || m_pickedObject != nullptr)
        {
            cancelFurtherLayerInteractions();
        }
    }
}

//------------------------------------------------------------------------------

void ClippingBoxInteractor::panGestureMoveEvent(int _x, int _y, int _dx, int _dy)
{
    if(m_widget.getVisibility())
    {
        if(m_widget.moveClippingBox(_x, _y, -_dx, -_dy) || m_pickedObject != nullptr)
        {
            cancelFurtherLayerInteractions();
        }
    }
}

//------------------------------------------------------------------------------

void ClippingBoxInteractor::panGestureReleaseEvent(int /*_x*/, int /*_y*/, int /*_dx*/, int /*_dy*/)
{
    if(m_widget.getVisibility())
    {
        m_widget.widgetReleased();
        m_pickedObject = nullptr;
    }
}

//------------------------------------------------------------------------------

void ClippingBoxInteractor::setBoxVisibility(bool _visibility)
{
    m_widget.setVisibility(_visibility);
}

//------------------------------------------------------------------------------

Ogre::AxisAlignedBox ClippingBoxInteractor::getClippingBox() const
{
    return m_widget.getClippingBox();
}

//------------------------------------------------------------------------------

Ogre::Matrix4 ClippingBoxInteractor::getClippingTransform() const
{
    return m_widget.getClippingTransform();
}

//------------------------------------------------------------------------------

void ClippingBoxInteractor::updateFromTransform(const Ogre::Matrix4& _clippingTrf)
{
    m_widget.updateFromTransform(_clippingTrf);
}

//------------------------------------------------------------------------------

void ClippingBoxInteractor::cancelFurtherLayerInteractions()
{
    const auto layer = m_layer.lock();
    if(layer)
    {
        layer->cancelFurtherInteraction();
    }
}

} // namespace sight::viz::scene3d::interactor
