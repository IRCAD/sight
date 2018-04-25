/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "fwRenderOgre/config.hpp"
#include "fwRenderOgre/factory/Text.hpp"

#include <OGRE/OgreCamera.h>
#include <OGRE/OgreSimpleRenderable.h>
#include <OGRE/Overlay/OgreFont.h>
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
class FWRENDEROGRE_CLASS_API Text : public ::Ogre::SimpleRenderable
{
public:

    /**
     * @brief Text object factory.
     * @param _id id of the new 'Text' object.
     * @param _sm scene manager instantiating the object.
     * @param _font font used to render this text.
     * @param _cam camera used when this text follows a 3D object.
     * @return A new 'Text' instance.
     */
    static FWRENDEROGRE_API Text* New(const std::string& _id, ::Ogre::SceneManager* _sm, ::Ogre::FontPtr _font,
                                      ::Ogre::Camera* _cam = nullptr);

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

    /// Returns this MovableObject's type as a string.
    FWRENDEROGRE_API virtual const ::Ogre::String& getMovableType() const override;

    /// MovableObject interface. Unused here, always returns 0.
    FWRENDEROGRE_API virtual ::Ogre::Real getBoundingRadius() const override;

    /**
     * @brief function called before rendering a frame
     *
     * We use it to update the text's position from its parent node (if it has one).
     */
    FWRENDEROGRE_API virtual void _updateRenderQueue(::Ogre::RenderQueue* _queue) override;

    /// @copydoc SimpleRenderable::getRenderOperation.
    FWRENDEROGRE_API virtual void getRenderOperation(::Ogre::RenderOperation& _op) override;

    /// @copydoc Renderable::getSquaredViewDepth
    FWRENDEROGRE_API virtual ::Ogre::Real getSquaredViewDepth(const Ogre::Camera* _cam) const override;

    /// @copydoc Renderable::getWorldTransforms
    FWRENDEROGRE_API virtual void getWorldTransforms(Ogre::Matrix4* _xform) const override;

private:

    /// Recomputes the geometry used to display the characters.
    void updateTextGeometry();

    /// Viewport in which the parent node is displayed, used to compute its projection onto the screen.
    ::Ogre::Camera* m_camera {nullptr};

    /// Overlay element displaying the text.
    ::Ogre::TextAreaOverlayElement* m_overlayText {nullptr};

    /// Set to true when the text geometry needs to be updated.
    bool m_dirty;

};

} // namespace fwRenderOgre
