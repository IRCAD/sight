/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2016-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include <fwRenderOgre/registry/macros.hpp>

#include <OGRE/OgreCamera.h>
#include <OGRE/OgreSceneNode.h>
#include <OGRE/OgreVector3.h>

namespace fwRenderOgre
{

namespace ui
{

/**
 * @brief Holds the clipping widgets, used to clip/move/scale the volume.
 */
class VRWidget
{
public:

    typedef std::function< void (void) > ClippingUpdateCallbackType;

    /**
     * @brief Constructor.
     *
     * @param id                     parent service id.
     * @param parentSceneNode        holds the volume object.
     * @param camera                 camera viewing this scene.
     * @param sceneManager           scene manager handling this object.
     * @param clippingMatrix         the initial clipping transform in world space
     * @param clippingUpdateCallback function called when the widget is modified through interaction.
     */
    FWRENDEROGRE_API VRWidget(const std::string& id,
                              ::Ogre::SceneNode* parentSceneNode,
                              ::Ogre::Camera* camera,
                              ::Ogre::SceneManager* sceneManager,
                              const ::Ogre::Matrix4& clippingMatrix,
                              const ClippingUpdateCallbackType& clippingUpdateCallback);

    /// Destructor.
    FWRENDEROGRE_API virtual ~VRWidget();

    /// Find out if a movable object belongs to this widget.
    bool belongsToWidget(const Ogre::MovableObject* const _object) const;

    /**
     * @brief Drags a widget toward a screen position.
     *
     * @pre the _pickedWidget must belong to this widget.
     *
     * @param _pickedWidget picked widget.
     * @param _screenX cursor's horizontal position.
     * @param _screenY cursor's vertical position.
     */
    FWRENDEROGRE_API void widgetPicked(::Ogre::MovableObject* _pickedWidget, int _screenX, int _screenY);

    /// Drops the currently selected widget.
    FWRENDEROGRE_API void widgetReleased();

    /**
     * @brief Translates the clipping box along the screen's axes.
     *
     * @param x  cursor current horizontal position.
     * @param y  cursor current vertical position.
     * @param dx displacement along the horizontal axis.
     * @param dy displacement along the vertical axis.
     */
    FWRENDEROGRE_API void moveClippingBox(int x, int y, int dx, int dy);

    /**
     * @brief Scales the clipping box.
     *
     * @param x  cursor current horizontal position.
     * @param y  cursor current vertical position.
     * @param dy displacement along the vertical axis, used to compute scale factor.
     */
    FWRENDEROGRE_API void scaleClippingBox(int x, int y, int dy);

    /// Returns whether the widgets are visible or hidden.
    FWRENDEROGRE_API bool getVisibility() const;

    /// Hides or shows the widget.
    FWRENDEROGRE_API void setVisibility(bool visibility);

    /// Returns the axis aligned coordinates of the clipping widget in volume image space.
    FWRENDEROGRE_API ::Ogre::AxisAlignedBox getClippingBox() const;

    /// Returns the clipping box transform in world space.
    FWRENDEROGRE_API ::Ogre::Matrix4 getClippingTransform() const;

    /// Sets the clipping cube from the input transform.
    FWRENDEROGRE_API void updateFromTransform(const ::Ogre::Matrix4& _clippingMx);

private:

    /// The current selection mode.
    enum
    {
        NONE,
        BOX,
        CAMERA
    } m_selectionMode { NONE };

    /// Get the face's image positions.
    std::array< ::Ogre::Vector3, 4 > getFacePositions(::fwRenderOgre::vr::IVolumeRenderer::CubeFace _faceName) const;

    /// Get the center of a clipping box face.
    ::Ogre::Vector3 getFaceCenter(::fwRenderOgre::vr::IVolumeRenderer::CubeFace _faceName) const;

    /// Returns the clipping box's image space positions.
    std::array< ::Ogre::Vector3, 8 > getClippingBoxPositions() const;

    /// Creates the widget objects and scene nodes.
    void initWidgets();

    /// Updates the widget's positions based on the clipping box.
    void updateWidgets();

    /// Highlight a clipping box face.
    void selectFace(::fwRenderOgre::vr::IVolumeRenderer::CubeFace _faceName);

    /// Unhighlight face.
    void deselectFace();

    /// Computes the axis aligned clipping box positions from the input transform.
    void applyTransform(const ::Ogre::Matrix4& _clippingMx);

    /// ID of the service using this widget.
    const std::string m_id;

    /// This object's scene manager.
    ::Ogre::SceneManager* m_sceneManager { nullptr };

    /// Camera too which the volume is rendered.
    ::Ogre::Camera* m_camera { nullptr };

    /// Parent node containing the volume.
    ::Ogre::SceneNode* m_volumeSceneNode { nullptr };

    /// Node holding widget objects.
    ::Ogre::SceneNode* m_widgetSceneNode { nullptr };

    /// Maps widget objects to their scene node and to a cube face.
    std::map< const ::Ogre::MovableObject*,
              std::pair< ::fwRenderOgre::vr::IVolumeRenderer::CubeFace, ::Ogre::SceneNode* > >  m_widgets;

    /// Axis aligned clipping cube.
    std::array< ::Ogre::Vector3, 2> m_clippingCube { ::Ogre::Vector3::ZERO, ::Ogre::Vector3::UNIT_SCALE};

    /// Object used to display the clipping box.
    ::Ogre::ManualObject* m_boundingBox { nullptr };

    /// Oject holding the highlighted face geometry.
    ::Ogre::ManualObject* m_selectedFace { nullptr };

    /// Widget currently being dragged.
    ::Ogre::Entity* m_selectedWidget { nullptr };

    /// Picked box point.
    ::Ogre::Vector3 m_pickedBoxPoint;

    /// Materials
    ::Ogre::MaterialPtr m_sphereHighlightMtl, m_frameMtl, m_frameHighlightMtl, m_faceMtl;

    /// Called when the clipping box is modified through interaction.
    ClippingUpdateCallbackType m_clippingUpdateCallback;
};

} // namespace ui
} // namespace fwRenderOgre
