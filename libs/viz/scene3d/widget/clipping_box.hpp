/************************************************************************
 *
 * Copyright (C) 2016-2024 IRCAD France
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
#include "viz/scene3d/vr/volume_renderer.hpp"

#include <OGRE/Ogre.h>
#include <OGRE/OgreCamera.h>
#include <OGRE/OgreSceneNode.h>

namespace sight::viz::scene3d::widget
{

/**
 * @brief Holds the clipping widgets, used to clip/move/scale the volume.
 */
class clipping_box
{
public:

    using clipping_update_callback_t = std::function<void (void)>;
    using sptr                       = std::shared_ptr<clipping_box>;
    using wptr                       = std::weak_ptr<clipping_box>;

    /**
     * @brief Constructor.
     *
     * @param _id                       parent service id.
     * @param _parent_scene_node        holds the volume object.
     * @param _camera                   camera viewing this scene.
     * @param _scene_manager            scene manager handling this object.
     * @param _clipping_matrix          the initial clipping transform in world space
     * @param _clipping_update_callback function called when the widget is modified through interaction.
     * @param _box_mtl_name             material used to display the box edges and faces.
     * @param _handle_mtl_name          material used to display the widget handles.
     */
    SIGHT_VIZ_SCENE3D_API clipping_box(
        std::string _id,
        Ogre::SceneNode* _parent_scene_node,
        Ogre::Camera* _camera,
        Ogre::SceneManager* _scene_manager,
        const Ogre::Matrix4& _clipping_matrix,
        clipping_update_callback_t _clipping_update_callback,
        const std::string& _box_mtl_name    = "BasicAmbient",
        const std::string& _handle_mtl_name = "BasicPhong"
    );

    /// Destructor.
    SIGHT_VIZ_SCENE3D_API virtual ~clipping_box();

    /// Find out if a movable object belongs to this widget.
    bool belongs_to_widget(const Ogre::MovableObject* _object) const;

    /**
     * @brief Drags a widget toward a screen position.
     *
     * @pre the _pickedWidget must belong to this widget.
     *
     * @param _picked_widget picked widget.
     * @param _screen_x cursor's horizontal position.
     * @param _screen_y cursor's vertical position.
     */
    SIGHT_VIZ_SCENE3D_API void widget_picked(Ogre::MovableObject* _picked_widget, int _screen_x, int _screen_y);

    /// Drops the currently selected widget.
    SIGHT_VIZ_SCENE3D_API void widget_released();

    /**
     * @brief Translates the clipping box along the screen's axes.
     *
     * @param _x  cursor current horizontal position.
     * @param _y  cursor current vertical position.
     * @param _dx displacement along the horizontal axis.
     * @param _dy displacement along the vertical axis.
     *
     * @return whether the box was selected and moved.
     */
    SIGHT_VIZ_SCENE3D_API bool move_clipping_box(int _x, int _y, int _dx, int _dy);

    /**
     * @brief Scales the clipping box.
     *
     * @param _x  cursor current horizontal position.
     * @param _y  cursor current vertical position.
     * @param _dy displacement along the vertical axis, used to compute scale factor.
     *
     * @return whether the box was selected and scaled.
     */
    SIGHT_VIZ_SCENE3D_API bool scale_clipping_box(int _x, int _y, int _dy);

    /// Returns whether the widgets are visible or hidden.
    [[nodiscard]] SIGHT_VIZ_SCENE3D_API bool get_visibility() const;

    /// Hides or shows the widget.
    SIGHT_VIZ_SCENE3D_API void set_visibility(bool _visibility);

    /// Returns the axis aligned coordinates of the clipping widget in volume image space.
    [[nodiscard]] SIGHT_VIZ_SCENE3D_API Ogre::AxisAlignedBox get_clipping_box() const;

    /// Returns the clipping box transform in world space.
    [[nodiscard]] SIGHT_VIZ_SCENE3D_API Ogre::Matrix4 get_clipping_transform() const;

    /// Sets the clipping cube from the input transform.
    SIGHT_VIZ_SCENE3D_API void update_from_transform(const Ogre::Matrix4& _clipping_mx);

private:

    /// The current selection mode.
    enum
    {
        none,
        box,
        camera
    } m_selection_mode {none};

    /// Get the face's image positions.
    [[nodiscard]] std::array<Ogre::Vector3, 4> get_face_positions(vr::volume_renderer::cube_face _face_name) const;

    /// Get the center of a clipping box face.
    [[nodiscard]] Ogre::Vector3 get_face_center(vr::volume_renderer::cube_face _face_name) const;

    /// Returns the clipping box's image space positions.
    [[nodiscard]] std::array<Ogre::Vector3, 8> clipping_box_positions() const;

    /// Creates the widget objects and scene nodes.
    void init_widgets();

    /// Updates the widget's positions based on the clipping box.
    void update_widgets();

    /// Highlight a clipping box face.
    void select_face(vr::volume_renderer::cube_face _face_name);

    /// Unhighlight face.
    void deselect_face();

    /// Computes the axis aligned clipping box positions from the input transform.
    void apply_transform(const Ogre::Matrix4& _clipping_mx);

    /// ID of the service using this widget.
    const std::string m_id;

    /// This object's scene manager.
    Ogre::SceneManager* m_scene_manager {nullptr};

    /// Camera too which the volume is rendered.
    Ogre::Camera* m_camera {nullptr};

    /// Parent node containing the volume.
    Ogre::SceneNode* m_volume_scene_node {nullptr};

    /// Node holding widget objects.
    Ogre::SceneNode* m_widget_scene_node {nullptr};

    /// Maps widget objects to their scene node and to a cube face.
    std::map<const Ogre::MovableObject*,
             std::pair<vr::volume_renderer::cube_face, Ogre::SceneNode*> > m_widgets;

    /// Axis aligned clipping cube.
    std::array<Ogre::Vector3, 2> m_clipping_cube {{Ogre::Vector3::ZERO, Ogre::Vector3::UNIT_SCALE}};

    /// Object used to display the clipping box.
    Ogre::ManualObject* m_bounding_box {nullptr};

    /// Object holding the highlighted face geometry.
    Ogre::ManualObject* m_selected_face {nullptr};

    /// Widget currently being dragged.
    Ogre::Entity* m_selected_widget {nullptr};

    /// Picked box point.
    Ogre::Vector3 m_picked_box_point;

    /// Material applied to the pickable spheres.
    Ogre::MaterialPtr m_handle_mtl;

    /// Material applied to a sphere when it is selected.
    Ogre::MaterialPtr m_handle_hightlight_mtl;

    /// Material displaying the edges of the box.
    Ogre::MaterialPtr m_box_mtl;

    /// Material displaying the edges when user is interacting with the widget.
    Ogre::MaterialPtr m_box_highlight_mtl;

    /// Material displaying the currently selected face.
    Ogre::MaterialPtr m_box_face_mtl;

    /// Called when the clipping box is modified through interaction.
    clipping_update_callback_t m_clipping_update_callback;
};

} // namespace sight::viz::scene3d::widget
