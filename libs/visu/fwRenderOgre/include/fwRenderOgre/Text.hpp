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

#pragma once

#include "fwRenderOgre/config.hpp"
#include "fwRenderOgre/factory/Text.hpp"

#include <OGRE/OgreCamera.h>
#include <OGRE/OgreMovableObject.h>
#include <OGRE/Overlay/OgreFont.h>
#include <OGRE/Overlay/OgreOverlayContainer.h>
#include <OGRE/Overlay/OgreTextAreaOverlayElement.h>

namespace fwRenderOgre
{

class ResizeListener;

/**
 * @brief Class used to display overlay text.
 *
 * This class inherits from '::Ogre::MovableObject' so it can be attached to 3D object and follow them around.
 * It can also be displayed in 2D if not attached to anything and it's position can be set and updated through
 * the 'setPosition' method.
 */
class FWRENDEROGRE_CLASS_API Text : public ::Ogre::MovableObject
{
public:

    /**
     * @brief Text object factory.
     * @param _id id of the new 'Text' object.
     * @param _sm scene manager instantiating the object.
     * @param _parent overlay panel in which this text is displayed.
     * @param _fontSource TrueType font (*.ttf) source file used to render the text.
     * @param _fontSize text font size in points.
     * @param _fontResolution font map resolution, should match the screen's dpi to get the best rendering quality.
     * @param _cam camera on the viewport rendering this text.
     * @return A new 'Text' instance.
     */
    static FWRENDEROGRE_API Text* New(const std::string& _id, ::Ogre::SceneManager* _sm,
                                      ::Ogre::OverlayContainer* _parent,
                                      const std::string& _fontSource, size_t _fontSize,
                                      float _fontResolution, ::Ogre::Camera* _cam);

    /**
     * @brief Text object factory.
     * @param _id id of the new 'Text' object.
     * @param _sm scene manager instantiating the object.
     * @param _parent overlay panel in which this text is displayed.
     * @param _font font used to render this text.
     * @param _cam camera used when this text follows a 3D object.
     * @return A new 'Text' instance.
     */
    [[deprecated("Text objects manage their own font now.")]] static FWRENDEROGRE_API
    Text* New(const std::string& _id, ::Ogre::SceneManager* _sm, ::Ogre::OverlayContainer* _parent,
              ::Ogre::FontPtr _font, ::Ogre::Camera* _cam = nullptr);

    /// Constructors, instantiates the overlay text element.
    FWRENDEROGRE_API Text(const std::string& _id);

    /// Destructor, destroys the overlay text element.
    FWRENDEROGRE_API virtual ~Text();

    /// Displayed text.
    FWRENDEROGRE_API void setText(const std::string& _text);

    /// Position in screen coordinates.
    FWRENDEROGRE_API void setPosition(float _x, float _y);

    /// Sets the character height relatively to the screen height.
    /// It would be better to set this from the font size but we let the user set it for now.
    [[deprecated("Text objects are now rendered using the font size")]]
    FWRENDEROGRE_API void setCharHeight(float _height);

    /// Gets the text height relatively to that of the screen.
    FWRENDEROGRE_API float getTextHeight() const;

    /// Sets the screen's dpi. Recomputes the text geometry and sets a font map with the new resolution.
    /// Internal method, called by the layer on resize events.
    void setDotsPerInch(float _dpi);

    /// Text color, white by default.
    FWRENDEROGRE_API void setTextColor(const ::Ogre::ColourValue& _color);

    /// Set the visibility of the text. Beware this hides ::Ogre::MovableObject::setVisible() which is not virtual !
    FWRENDEROGRE_API void setVisible(bool _visible);

    /// Aligns the text by setting the x and y coordinates' origin.
    FWRENDEROGRE_API void setTextAlignment(const ::Ogre::TextAreaOverlayElement::Alignment _hAlignement,
                                           const ::Ogre::GuiVerticalAlignment _vAlignment = ::Ogre::GVA_TOP);

    /// Returns this MovableObject's type as a string.
    FWRENDEROGRE_API virtual const ::Ogre::String& getMovableType() const override;

    /// MovableObject interface. Unused here, always returns an empty bounding box.
    FWRENDEROGRE_API virtual const ::Ogre::AxisAlignedBox& getBoundingBox() const override;

    /// MovableObject interface. Unused here, always returns 0.
    FWRENDEROGRE_API virtual ::Ogre::Real getBoundingRadius() const override;

    /**
     * @brief function called before rendering a frame
     *
     * We use it to update the text's position from its parent node (if it has one).
     */
    FWRENDEROGRE_API virtual void _updateRenderQueue(::Ogre::RenderQueue* _queue) override;

    /// MovableObject interface. Not used here.
    FWRENDEROGRE_API virtual void visitRenderables(::Ogre::Renderable::Visitor* _visitor,
                                                   bool _debugRenderables) override;

private:

    friend class ResizeListener;

    /// Sets the font to be used for rendering. Updates the text's material.
    void setFont(::Ogre::FontPtr _font);

    /// Recomputes the text height and position when the viewport is resized.
    void resize();

    /// Viewport in which the parent node is displayed, used to compute its projection onto the screen.
    ::Ogre::Camera* m_camera {nullptr};

    /// Overlay element displaying the text.
    ::Ogre::TextAreaOverlayElement* m_overlayText {nullptr};

    /// Infinite bounding box returned by the 'getBoundingBox' method.
    /// This allows to be always updated, otherwise the text can get stuck against the viewport edges.
    const ::Ogre::AxisAlignedBox m_bb { ::Ogre::AxisAlignedBox::EXTENT_INFINITE };

    /// Container holding the text.
    ::Ogre::OverlayContainer* m_parentContainer {nullptr};

    /// Font used to display the text.
    ::Ogre::FontPtr m_font;

    /// The text's height in pixels. Computed using the font size and screen dpi.
    float m_heightInPixels { 0.f };

    /// Current position not taking alignemnt into account.
    ::Ogre::Vector2 m_position { 0.f, 0.f };

    /// Listens to when the viewport dimension are changed. Resizes the text.
    ResizeListener* m_listener { nullptr };

    /// Color of the displayed text.
    ::Ogre::ColourValue m_textColor { 1.f, 1.f, 1.f, 1.f };

};

} // namespace fwRenderOgre
