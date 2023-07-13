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

#include "modules/viz/scene3dQt/config.hpp"

#include "viz/scene3d/IText.hpp"

#include <QLabel>
#include <QPointer>

namespace sight::module::viz::scene3dQt
{

class NodeListener;

/**
 * @brief Class used to display overlay text.
 *
 * This class inherits from 'Ogre::MovableObject' so it can be attached to 3D object and follow them around.
 * It can also be displayed in 2D if not attached to anything and it's position can be set and updated through
 * the 'setPosition' method.
 */
class Text : public sight::viz::scene3d::IText
{
public:

    /// Constructors, instantiates the overlay text element.
    Text(sight::viz::scene3d::IText::Key, const sight::viz::scene3d::Layer::sptr& _layer);

    /// Destructor, destroys the overlay text element.
    ~Text() override;

    /// Attach the given text to a scene node.
    void attachToNode(Ogre::SceneNode* _node, Ogre::Camera* _camera) override;

    /// Detach the text from a scene node
    void detachFromNode() override;

    /// Displayed text.
    void setText(const std::string& _text) override;

    /// Position in screen coordinates.
    void setPosition(float _x, float _y) override;

    /// Text color, white by default.
    void setTextColor(const Ogre::ColourValue& _color) override;

    /// Text color, white by default.
    void setTextColor(const std::string& _color) override;

    /// Set the visibility of the text.
    void setVisible(bool _visible) override;

    /// Aligns the text by setting the x and y coordinates' origin.
    void setTextAlignment(const std::string& _hAlign, const std::string& _vAlign) override;

    /// Sets the font to be used for rendering.
    void setFont(const std::string& font) override;

    /// Sets the font size to be used for rendering.
    void setFontSize(std::size_t _size) override;

private:

    /// Recompute the size according to the object properties
    void adjustSize();

    /// Set the style according to the object properties
    void adjustStyle();

    /// Contains the displayed stats in the overlay.
    QPointer<QLabel> m_text;

    /// Defines the text's color.
    QString m_textColor;

    /// Defines the font size in points.
    std::size_t m_fontSize {16};

    /// Defines the position of the text.
    Ogre::Vector2 m_position {0.F, 0.F};

    /// Defines the vertical alignment type (top, center or bottom).
    std::string m_verticalAlignment;

    /// Defines the horizontal alignment type (left, center or right).
    std::string m_horizontalAlignment;

    /// Defines the displayed message.
    std::string m_textString;

    /// Defines the font family.
    QString m_fontFamily;

    /// To update the text position according to a scene node
    NodeListener* m_nodeListener {};

    using ResizeLayerSlot_t = core::com::Slot<void (int, int)>;
    ResizeLayerSlot_t::sptr m_resizeSlot;

    /// Handles connection with the layer.
    core::com::Connection m_resizeConnection;
};

} // namespace sight::module::viz::scene3dQt
