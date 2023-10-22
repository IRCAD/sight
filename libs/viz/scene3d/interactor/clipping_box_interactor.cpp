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

#include "viz/scene3d/interactor/clipping_box_interactor.hpp"

#include "viz/scene3d/layer.hpp"

namespace sight::viz::scene3d::interactor
{

//------------------------------------------------------------------------------

clipping_box_interactor::clipping_box_interactor(
    layer::sptr _layer,
    bool _layer_order_dependant,
    const std::string& _id,
    Ogre::SceneNode* _parent_scene_node,
    const Ogre::Matrix4& _clipping_matrix,
    const widget::clipping_box::clipping_update_callback_t& _clipping_update_cb,
    const std::string& _box_mtl_name,
    const std::string& _handle_mtl_name
) noexcept :
    base(_layer, _layer_order_dependant),
    m_widget(_id, _parent_scene_node, _layer->getDefaultCamera(), _layer->getSceneManager(),
             _clipping_matrix, _clipping_update_cb, _box_mtl_name, _handle_mtl_name)
{
    SIGHT_ASSERT("This interactor must know its layer.", _layer);
}

//------------------------------------------------------------------------------

Ogre::MovableObject* clipping_box_interactor::pickObject(int _x, int _y)
{
    if(auto layer = m_layer.lock())
    {
        const auto result = viz::scene3d::utils::pickObject(_x, _y, 0xFFFFFFFF, *layer->getSceneManager());

        return result.has_value() ? result->first : nullptr;
    }

    return nullptr;
}

//------------------------------------------------------------------------------

void clipping_box_interactor::mouseMoveEvent(MouseButton _button, Modifier /*_mods*/, int _x, int _y, int _dx, int _dy)
{
    if(m_widget.getVisibility()) // If a widget is present in the scene.
    {
        bool interacted = m_pickedObject != nullptr;

        if(_button == LEFT && interacted)
        {
            m_widget.widgetPicked(m_pickedObject, _x, _y);
        }
        else if(_button == MIDDLE)
        {
            interacted = m_widget.move_clipping_box(_x, _y, -_dx, -_dy);
        }
        else if(_button == RIGHT)
        {
            interacted = m_widget.scale_clipping_box(_x, _y, _dy);
        }

        if(interacted)
        {
            this->cancelFurtherLayerInteractions();
            m_layer.lock()->requestRender();
        }
    }
}

//------------------------------------------------------------------------------

void clipping_box_interactor::buttonReleaseEvent(MouseButton /*_button*/, Modifier /*_mods*/, int /*_x*/, int /*_y*/)
{
    if(m_widget.getVisibility())
    {
        m_widget.widgetReleased();
        m_pickedObject = nullptr;
        m_layer.lock()->requestRender();
    }
}

//------------------------------------------------------------------------------

void clipping_box_interactor::buttonPressEvent(MouseButton _button, Modifier /*_mods*/, int _x, int _y)
{
    if(m_widget.getVisibility())
    {
        bool interacted = false;
        if(_button == LEFT)
        {
            m_pickedObject = pickObject(_x, _y);

            interacted = m_widget.belongsToWidget(m_pickedObject);
            if(interacted)
            {
                m_widget.widgetPicked(m_pickedObject, _x, _y);
            }
            else
            {
                m_pickedObject = nullptr;
            }
        }
        else if(_button == MIDDLE)
        {
            interacted = m_widget.move_clipping_box(_x, _y, 0, 0);
        }
        else if(_button == RIGHT)
        {
            interacted = m_widget.scale_clipping_box(_x, _y, 0);
        }

        if(interacted)
        {
            this->cancelFurtherLayerInteractions();
            m_layer.lock()->requestRender();
        }
    }
}

//------------------------------------------------------------------------------

void clipping_box_interactor::pinchGestureEvent(double _scale_factor, int _center_x, int _center_y)
{
    if(m_widget.getVisibility())
    {
        // Convert back the scale from "delta"
        if(_scale_factor < 0)
        {
            _scale_factor = -1.0 / _scale_factor;
        }

        const auto dy = int(((_center_y * _scale_factor) - _center_y) * 0.5);

        if(m_widget.scale_clipping_box(_center_x, _center_y, dy) || m_pickedObject != nullptr)
        {
            cancelFurtherLayerInteractions();
        }
    }
}

//------------------------------------------------------------------------------

void clipping_box_interactor::panGestureMoveEvent(int _x, int _y, int _dx, int _dy)
{
    if(m_widget.getVisibility())
    {
        if(m_widget.move_clipping_box(_x, _y, -_dx, -_dy) || m_pickedObject != nullptr)
        {
            cancelFurtherLayerInteractions();
        }
    }
}

//------------------------------------------------------------------------------

void clipping_box_interactor::panGestureReleaseEvent(int /*_x*/, int /*_y*/, int /*_dx*/, int /*_dy*/)
{
    if(m_widget.getVisibility())
    {
        m_widget.widgetReleased();
        m_pickedObject = nullptr;
    }
}

//------------------------------------------------------------------------------

void clipping_box_interactor::setBoxVisibility(bool _visibility)
{
    m_widget.setVisibility(_visibility);
    m_layer.lock()->requestRender();
}

//------------------------------------------------------------------------------

Ogre::AxisAlignedBox clipping_box_interactor::get_clipping_box() const
{
    return m_widget.get_clipping_box();
}

//------------------------------------------------------------------------------

Ogre::Matrix4 clipping_box_interactor::get_clipping_transform() const
{
    return m_widget.get_clipping_transform();
}

//------------------------------------------------------------------------------

void clipping_box_interactor::updateFromTransform(const Ogre::Matrix4& _clipping_trf)
{
    m_widget.updateFromTransform(_clipping_trf);
}

//------------------------------------------------------------------------------

void clipping_box_interactor::cancelFurtherLayerInteractions()
{
    const auto layer = m_layer.lock();
    if(layer)
    {
        layer->cancelFurtherInteraction();
    }
}

} // namespace sight::viz::scene3d::interactor
