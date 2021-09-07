/************************************************************************
 *
 * Copyright (C) 2016-2021 IRCAD France
 * Copyright (C) 2016-2021 IHU Strasbourg
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

#include "viz/scene3d/registry/macros.hpp"
#include "viz/scene3d/vr/IVolumeRenderer.hpp"

#include <OGRE/Ogre.h>
#include <OGRE/OgreCamera.h>
#include <OGRE/OgreSceneNode.h>

namespace sight::viz::scene3d::widget
{

/**
 * @brief Holds the clipping widgets, used to clip/move/scale the volume.
 */
class ClippingBox
{
public:

    using ClippingUpdateCallbackType = std::function<void (void)>;
    using sptr                       = std::shared_ptr<ClippingBox>;
    using wptr                       = std::weak_ptr<ClippingBox>;

    /**
     * @brief Constructor.
     *
     * @param id                     parent service id.
     * @param parentSceneNode        holds the volume object.
     * @param camera                 camera viewing this scene.
     * @param sceneManager           scene manager handling this object.
     * @param clippingMatrix         the initial clipping transform in world space
     * @param clippingUpdateCallback function called when the widget is modified through interaction.
     * @param boxMtlName             material used to display the box edges and faces.
     * @param handleMtlName          material used to display the widget handles.
     */
    VIZ_SCENE3D_API ClippingBox(
        const std::string& id,
        ::Ogre::SceneNode* parentSceneNode,
        ::Ogre::Camera* camera,
        ::Ogre::SceneManager* sceneManager,
        const ::Ogre::Matrix4& clippingMatrix,
        const ClippingUpdateCallbackType& clippingUpdateCallback,
        const std::string& boxMtlName    = "BasicAmbient",
        const std::string& handleMtlName = "BasicPhong"
    );

    /// Destructor.
    VIZ_SCENE3D_API virtual ~ClippingBox();

    /// Find out if a movable object belongs to this widget.
    bool belongsToWidget(const ::Ogre::MovableObject* const _object) const;

    /**
     * @brief Drags a widget toward a screen position.
     *
     * @pre the _pickedWidget must belong to this widget.
     *
     * @param _pickedWidget picked widget.
     * @param _screenX cursor's horizontal position.
     * @param _screenY cursor's vertical position.
     */
    VIZ_SCENE3D_API void widgetPicked(::Ogre::MovableObject* _pickedWidget, int _screenX, int _screenY);

    /// Drops the currently selected widget.
    VIZ_SCENE3D_API void widgetReleased();

    /**
     * @brief Translates the clipping box along the screen's axes.
     *
     * @param x  cursor current horizontal position.
     * @param y  cursor current vertical position.
     * @param dx displacement along the horizontal axis.
     * @param dy displacement along the vertical axis.
     *
     * @return whether the box was selected and moved.
     */
    VIZ_SCENE3D_API bool moveClippingBox(int x, int y, int dx, int dy);

    /**
     * @brief Scales the clipping box.
     *
     * @param x  cursor current horizontal position.
     * @param y  cursor current vertical position.
     * @param dy displacement along the vertical axis, used to compute scale factor.
     *
     * @return whether the box was selected and scaled.
     */
    VIZ_SCENE3D_API bool scaleClippingBox(int x, int y, int dy);

    /// Returns whether the widgets are visible or hidden.
    VIZ_SCENE3D_API bool getVisibility() const;

    /// Hides or shows the widget.
    VIZ_SCENE3D_API void setVisibility(bool visibility);

    /// Returns the axis aligned coordinates of the clipping widget in volume image space.
    VIZ_SCENE3D_API ::Ogre::AxisAlignedBox getClippingBox() const;

    /// Returns the clipping box transform in world space.
    VIZ_SCENE3D_API ::Ogre::Matrix4 getClippingTransform() const;

    /// Sets the clipping cube from the input transform.
    VIZ_SCENE3D_API void updateFromTransform(const ::Ogre::Matrix4& _clippingMx);

private:

    /// The current selection mode.
    enum
    {
        NONE,
        BOX,
        CAMERA
    } m_selectionMode {NONE};

    /// Get the face's image positions.
    std::array< ::Ogre::Vector3, 4> getFacePositions(vr::IVolumeRenderer::CubeFace _faceName) const;

    /// Get the center of a clipping box face.
    ::Ogre::Vector3 getFaceCenter(vr::IVolumeRenderer::CubeFace _faceName) const;

    /// Returns the clipping box's image space positions.
    std::array< ::Ogre::Vector3, 8> getClippingBoxPositions() const;

    /// Creates the widget objects and scene nodes.
    void initWidgets();

    /// Updates the widget's positions based on the clipping box.
    void updateWidgets();

    /// Highlight a clipping box face.
    void selectFace(vr::IVolumeRenderer::CubeFace _faceName);

    /// Unhighlight face.
    void deselectFace();

    /// Computes the axis aligned clipping box positions from the input transform.
    void applyTransform(const ::Ogre::Matrix4& _clippingMx);

    /// ID of the service using this widget.
    const std::string m_id;

    /// This object's scene manager.
    ::Ogre::SceneManager* m_sceneManager {nullptr};

    /// Camera too which the volume is rendered.
    ::Ogre::Camera* m_camera {nullptr};

    /// Parent node containing the volume.
    ::Ogre::SceneNode* m_volumeSceneNode {nullptr};

    /// Node holding widget objects.
    ::Ogre::SceneNode* m_widgetSceneNode {nullptr};

    /// Maps widget objects to their scene node and to a cube face.
    std::map<const ::Ogre::MovableObject*,
             std::pair<vr::IVolumeRenderer::CubeFace, ::Ogre::SceneNode*> > m_widgets;

    /// Axis aligned clipping cube.
    std::array< ::Ogre::Vector3, 2> m_clippingCube {{::Ogre::Vector3::ZERO, ::Ogre::Vector3::UNIT_SCALE}};

    /// Object used to display the clipping box.
    ::Ogre::ManualObject* m_boundingBox {nullptr};

    /// Oject holding the highlighted face geometry.
    ::Ogre::ManualObject* m_selectedFace {nullptr};

    /// Widget currently being dragged.
    ::Ogre::Entity* m_selectedWidget {nullptr};

    /// Picked box point.
    ::Ogre::Vector3 m_pickedBoxPoint;

    /// Material applied to the pickable spheres.
    ::Ogre::MaterialPtr m_handleMtl;

    /// Material applied to a sphere when it is selected.
    ::Ogre::MaterialPtr m_handleHightlightMtl;

    /// Material displaying the edges of the box.
    ::Ogre::MaterialPtr m_boxMtl;

    /// Material displaying the edges when user is interacting with the widget.
    ::Ogre::MaterialPtr m_boxHighlightMtl;

    /// Material displaying the currently selected face.
    ::Ogre::MaterialPtr m_boxFaceMtl;

    /// Called when the clipping box is modified through interaction.
    ClippingUpdateCallbackType m_clippingUpdateCallback;
};

} // namespace sight::viz::scene3d::widget
