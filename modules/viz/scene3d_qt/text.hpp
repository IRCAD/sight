/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
 * Copyright (C) 2018-2021 IHU Strasbourg
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

#pragma once

#include "modules/viz/scene3d_qt/config.hpp"

#include "viz/scene3d/text.hpp"

#include <QLabel>
#include <QLineEdit>
#include <QPointer>

namespace sight::module::viz::scene3d_qt
{

class NodeListener;

/**
 * @brief Class used to display overlay text.
 *
 * This class inherits from 'Ogre::MovableObject' so it can be attached to 3D object and follow them around.
 * It can also be displayed in 2D if not attached to anything and it's position can be set and updated through
 * the 'setPosition' method.
 */
class text : public sight::viz::scene3d::text
{
public:

    /// Constructors, instantiates the overlay text element.
    text(const sight::viz::scene3d::layer::sptr& _layer);

    /// Destructor, destroys the overlay text element.
    ~text() override;

    /// Attach the given text to a scene node.
    void attach_to_node(Ogre::SceneNode* _node, Ogre::Camera* _camera) override;

    /// Detach the text from a scene node
    void detach_from_node() override;

    /// Displayed text.
    void set_text(const std::string& _text) override;

    /// Position in screen coordinates.
    void set_position(float _x, float _y) override;

    /// Text color, white by default.
    void set_text_color(const Ogre::ColourValue& _color) override;

    /// Text color, white by default.
    void set_text_color(const std::string& _color) override;

    /// Set the visibility of the text.
    void set_visible(bool _visible) override;

    /// Aligns the text by setting the x and y coordinates' origin.
    void set_text_alignment(const std::string& _h_align, const std::string& _v_align) override;

    /// Sets the font to be used for rendering.
    void set_font(const std::string& _font) override;

    /// Sets the font size to be used for rendering.
    void set_font_size(std::size_t _size) override;

    /// Sets the edit mode. If true, the label can be modified, else it is read-only.
    void set_edit_mode(bool _edit_mode) override;

    void set_underlying_node_rect(std::pair<Ogre::Vector2, Ogre::Vector2> _rect);

private:

    /// Recompute the size according to the object properties
    void adjust_size();

    /// Compute the style according to the object properties
    QString compute_style();

    /// Set the style according to the object properties
    void adjust_style();

    /// Contains the displayed stats in the overlay.
    QPointer<QLineEdit> m_text;
    // QPointer<QLabel> m_text;

    /// Defines the text's color.
    QString m_text_color;

    /// Defines the font size in points.
    std::size_t m_font_size {16};

    /// Defines the position of the text.
    std::pair<Ogre::Vector2, Ogre::Vector2> m_position {};

    /// Defines the vertical alignment type (top, center or bottom).
    std::string m_vertical_alignment;

    /// Defines the horizontal alignment type (left, center or right).
    std::string m_horizontal_alignment;

    /// Defines the displayed message.
    std::string m_text_string;

    /// Defines the font family.
    QString m_font_family;

    /// To update the text position according to a scene node
    NodeListener* m_node_listener {};

    using resize_layer_slot_t = core::com::slot<void (int, int)>;
    resize_layer_slot_t::sptr m_resize_slot;

    /// Handles connection with the layer.
    core::com::connection m_resize_connection;
};

} // namespace sight::module::viz::scene3d_qt
