/************************************************************************
 *
 * Copyright (C) 2026 IRCAD France
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

#include <viz/scene3d/adaptor.hpp>
#include <viz/scene3d/interactor/base.hpp>
#include <viz/scene3d/material/standard.hpp>

#include <data/point_list.hpp>

#include <OgreManualObject.h>
#include <OgreSceneNode.h>

namespace sight::module::viz::scene3d::adaptor
{

/**
 * @brief This adaptor allows to draw a 2D shape with a stroke tool.
 *
 * This adaptor must be enabled with the signal `enable(bool)` to be used. Once enabled, a 2D shape can be drawn in
 * the scene by clicking or dragging the tool.
 *
 * @section Signal Signals
 * - \b disabled(): sent when interactions are finished, when validating.
 *
 * @section Slots Slots
 * - \b enable(bool): enable or disable the tool, it will be automatically disabled when interactions are finished.
 * - \b clear(): resets the point list.
 * - \b undo(): undo the last stroke piece.
 * - \b validate(): validate the stroke.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="..." type="sight::module::viz::scene3d::adaptor::stroke">
        <data point_list="${...}" />
        <config priority="2" query_mask="0x40000000" />
    </service>
   @endcode
 *
 * @subsection Input Input
 * - \b data.point_list [sight::data::point_list]: point list to create and display.
 *
 * @subsection Configuration Configuration:
 * - \b line_color (optional, hexadecimal, default=#FFFFFF): the color of the line.
 * - \b edge_color (optional, hexadecimal, default=#FFFFFF): the color of the edges.
 * - \b query_mask (optional, uint32, default=0xFFFFFFFF): mask used to filter in entities to intersect.
 * - \b priority (optional, int, default=2): interaction priority, higher priority interactions are performed first.
 */
class stroke final :
    public sight::viz::scene3d::adaptor,
    public sight::viz::scene3d::interactor::base
{
public:

    struct signals
    {
        using disabled_t = sight::core::com::signal<void ()>;
        static inline const signal_key_t DISABLED = "disabled";
    };

    struct slots
    {
        static inline const slot_key_t ENABLE   = "enable";
        static inline const slot_key_t CLEAR    = "clear";
        static inline const slot_key_t UNDO     = "undo";
        static inline const slot_key_t VALIDATE = "validate";
    };

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(stroke, sight::viz::scene3d::adaptor);

    /// Initializes the slot and the signal.
    stroke() noexcept;

    /// Destroys the adaptor.
    ~stroke() noexcept final = default;

    /**
     * @brief Cancels further interactions.
     * @pre @ref m_interactionEnableState must be true.
     */
    void wheel_event(modifier /*_mods*/, double /*_angleDelta*/, int /*_x*/, int /*_y*/) final;

    /**
     * @brief Adds a new point to the stroke.
     * @pre @ref m_toolEnableState must be true.
     * @param _button mouse modifier.
     * @param _x X screen coordinate.
     * @param _y Y screen coordinate.
     */
    void button_press_event(mouse_button _button, modifier /*_mods*/, int _x, int _y) final;

    /**
     * @brief Draws the last stroke line or add a point to the stroke in the mouse is dragged.
     * @pre @ref m_interactionEnableState must be true.
     * @param _button mouse modifier.
     * @param _x X screen coordinate.
     * @param _y Y screen coordinate.
     */
    void mouse_move_event(mouse_button _button, modifier /*_mods*/, int _x, int _y, int /*_dx*/, int /*_dy*/) final;

    /**
     * @brief Ends the drag interaction.
     * @pre @ref m_interactionEnableState and @ref m_leftButtonMoveState must be true.
     */
    void button_release_event(mouse_button /*_button*/, modifier /*_mods*/, int /*_x*/, int /*_y*/) final;

protected:

    /// Configures the service.
    void configuring() final;

    /// Creates Ogre resources and materials.
    void starting() final;

    /// Does nothing.
    void updating() final;

    /// Destroys all Ogre resources.
    void stopping() final;

private:

    enum class action : std::uint8_t
    {
        add,
        remove
    };

    /// Computes the camera direction vector.
    static Ogre::Vector3 get_cam_direction(const Ogre::Camera* _cam);

    /// Sets if the tool is enabled or not.
    void enable(bool _enable);

    /// Reset all extrusions
    void clear();

    /// Undo the last stroke piece
    void undo();

    /// validate the stroke
    void validate();

    /**
     * @brief Gets the 3d position of the intersection between the ray starting from the camera
     * and @ref m_strokeNearPlane/@ref m_strokeFarPlane
     *
     * @param _x x screen coordinate.
     * @param _y y screen coordinate.
     * @return the tool, near and far 3D intersection in the world space.
     */
    Ogre::Vector3 get_3d_position(int _x, int _y) const;

    /**
     * @brief Modify the existing stroke
     * @param _action The option to do on the stroke. Either ADD or REMOVE.
     * @param _x X screen coordinate.
     * @param _y Y screen coordinate.
     */
    void modify_stroke(action _action, int _x = -1, int _y = -1);

    /// Draws the stroke from @ref m_strokeNearPositions.
    void draw_stroke();

    /// Defines the priority of the interactor.
    int m_priority {2};

    /// Contains the material data used for the stroke tool.
    std::string m_material_name;

    /// Contains the material used to display the stroke.
    sight::viz::scene3d::material::standard::uptr m_material;

    /// Defines the color of the stroke's line.
    Ogre::ColourValue m_line_color {Ogre::ColourValue::White};

    /// Defines the color of the stroke's edge.
    Ogre::ColourValue m_edge_color {Ogre::ColourValue::White};

    /// Handles the tool activation state.
    bool m_enable {false};

    /// Defines if the interaction as begin.
    bool m_interaction_enable_state {false};

    /// Handles the left button move state.
    bool m_left_button_move_state {false};

    /// Stores all position clicked or clicked and moved that are at near to the camera.
    std::vector<Ogre::Vector3> m_stroke_tool_positions;

    /// Stores all clicked position.
    std::vector<Ogre::Vector3> m_stroke_edge_positions;

    /// Defines the radius of each point drawn at edges positions.
    const float m_stroke_edge_size {0.005F};

    /// Contains the node where all manual objects that represent the stroke are attached.
    Ogre::SceneNode* m_stroke_node {nullptr};

    /// Contains the stroke object.
    Ogre::ManualObject* m_stroke {nullptr};

    /// Contains the last stroke line, this line is drawn between the last position and the current mouse position.
    Ogre::ManualObject* m_last_stroke_line {nullptr};

    /// Picking query mask. Filters out objects with mismatching flags.
    std::uint32_t m_query_mask {0xffffffff};

    sight::data::ptr<sight::data::point_list, sight::data::access::inout> m_point_list {this, "data.point_list"};
};

} // namespace sight::module::viz::scene3d::adaptor.
