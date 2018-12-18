/************************************************************************
 *
 * Copyright (C) 2018 IRCAD France
 * Copyright (C) 2018 IHU Strasbourg
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
     * @param _font font used to render this text.
     * @param _cam camera used when this text follows a 3D object.
     * @return A new 'Text' instance.
     */
    static FWRENDEROGRE_API Text* New(const std::string& _id, ::Ogre::SceneManager* _sm,
                                      ::Ogre::OverlayContainer* _parent,
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
    FWRENDEROGRE_API void setCharHeight(float _height);

    /// Text color, white by default.
    FWRENDEROGRE_API void setTextColor(::Ogre::ColourValue _color);

    /// Set the visibility of the text. Beware this hides ::Ogre::MovableObject::setVisible() which is not virtual !
    FWRENDEROGRE_API void setVisible(bool _visible);

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

    /// Viewport in which the parent node is displayed, used to compute its projection onto the screen.
    ::Ogre::Camera* m_camera {nullptr};

    /// Overlay element displaying the text.
    ::Ogre::TextAreaOverlayElement* m_overlayText {nullptr};

    /// Infinite bounding box returned by the 'getBoundingBox' method.
    /// This allows to be always updated, otherwise the text can get stuck against the viewport edges.
    const ::Ogre::AxisAlignedBox m_bb { ::Ogre::AxisAlignedBox::EXTENT_INFINITE };

    /// Container holding the text.
    ::Ogre::OverlayContainer* m_parentContainer {nullptr};

};

} // namespace fwRenderOgre
