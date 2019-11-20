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

#include "fwRenderOgre/interactor/ClippingBoxInteractor.hpp"

#include "fwRenderOgre/Layer.hpp"

namespace fwRenderOgre::interactor
{

//------------------------------------------------------------------------------

ClippingBoxInteractor::ClippingBoxInteractor(Layer::sptr _layer,
                                             const std::string& _id,
                                             ::Ogre::SceneNode* _parentSceneNode,
                                             const ::Ogre::Matrix4& _clippingMatrix,
                                             const ui::ClippingBox::ClippingUpdateCallbackType& _clippingUpdateCallback,
                                             const std::string& _boxMtlName,
                                             const std::string& _handleMtlName) noexcept :
    IInteractor(_layer),
    m_widget(_id, _parentSceneNode, _layer->getDefaultCamera(), _layer->getSceneManager(),
             _clippingMatrix, _clippingUpdateCallback, _boxMtlName, _handleMtlName)
{
    SLM_ASSERT("This interactor must know its layer.", _layer);

    m_picker.setSceneManager(_layer->getSceneManager());
}

//------------------------------------------------------------------------------

ClippingBoxInteractor::~ClippingBoxInteractor() noexcept
{

}

//------------------------------------------------------------------------------

Ogre::MovableObject* ClippingBoxInteractor::pickObject(int x, int y)
{
    if(auto layer = m_layer.lock())
    {
        const ::Ogre::Camera* const camera = layer->getDefaultCamera();

        const int height = camera->getViewport()->getActualHeight();
        const int width  = camera->getViewport()->getActualWidth();

        const bool pickSuccess = m_picker.executeRaySceneQuery( x, y, width, height, 0xFFFFFFFF );

        return pickSuccess ? m_picker.getSelectedObject() : nullptr;
    }

    return nullptr;
}

//------------------------------------------------------------------------------

void ClippingBoxInteractor::mouseMoveEvent(MouseButton button, int x, int y, int dx, int dy)
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

void ClippingBoxInteractor::buttonReleaseEvent(MouseButton /*button*/, int /*x*/, int /*y*/)
{
    if(m_widget.getVisibility())
    {
        m_widget.widgetReleased();
        m_pickedObject = nullptr;
    }
}

//------------------------------------------------------------------------------

void ClippingBoxInteractor::buttonPressEvent(MouseButton button, int x, int y)
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

void ClippingBoxInteractor::setBoxVisibility(bool _visibility)
{
    m_widget.setVisibility(_visibility);
}

//------------------------------------------------------------------------------

::Ogre::AxisAlignedBox ClippingBoxInteractor::getClippingBox() const
{
    return m_widget.getClippingBox();
}

//------------------------------------------------------------------------------

::Ogre::Matrix4 ClippingBoxInteractor::getClippingTransform() const
{
    return m_widget.getClippingTransform();
}

//------------------------------------------------------------------------------

void ClippingBoxInteractor::updateFromTransform(const ::Ogre::Matrix4& _clippingTrf)
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

//------------------------------------------------------------------------------

void ClippingBoxInteractor::wheelEvent(int, int, int) noexcept
{
}
//------------------------------------------------------------------------------

void ClippingBoxInteractor::resizeEvent(int, int) noexcept
{
}
//------------------------------------------------------------------------------

void ClippingBoxInteractor::keyPressEvent(int) noexcept
{
}
//------------------------------------------------------------------------------

void ClippingBoxInteractor::keyReleaseEvent(int) noexcept
{
}
//------------------------------------------------------------------------------

void ClippingBoxInteractor::focusInEvent() noexcept
{
}
//------------------------------------------------------------------------------

void ClippingBoxInteractor::focusOutEvent() noexcept
{
}

} // namespace fwRenderOgre::interactor