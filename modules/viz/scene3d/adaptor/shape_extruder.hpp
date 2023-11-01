/************************************************************************
 *
 * Copyright (C) 2020-2023 IRCAD France
 * Copyright (C) 2020-2021 IHU Strasbourg
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

#include "modules/viz/scene3d/adaptor/material.hpp"
#include "modules/viz/scene3d/config.hpp"

#include <core/com/signals.hpp>

#include <data/material.hpp>
#include <data/model_series.hpp>

#include <service/notifier.hpp>

#include <viz/scene3d/adaptor.hpp>
#include <viz/scene3d/interactor/base.hpp>

#include <OgreManualObject.h>
#include <OgreSceneNode.h>

namespace sight::module::viz::scene3d::adaptor
{

/**
 * @brief This adaptor allows to draw a 2D shape with a lasso tool, and create an extruded mesh from it.
 *
 * This adaptor must be enabled with the signal `enableTool(bool)` to be used. Once enabled, a 2D shape can be drawn in
 * the scene by clicking or dragging the tool. When a double click is done on a clicked point, the shape is closed and
 * then, an extruded mesh will be created and pushed in a model series.
 *
 * The adaptor works as follows: the shape is stored as a points list belonging to a 2D plane in the near of the camera,
 * and a second one is stored in the far of the camera. Once the shape is closed, a triangulation is done on the two
 * points list with a constrained Bowyer-Watson algorithm. Then, for each segment of the shape, two triangles are
 * created between the segment at the near plane and the far plane.
 *
 * @section Slots Slots
 * - \b enable_tool(bool): enable or disable the tool, it will be automatically disabled when interactions are finished.
 * - \b delete_last_mesh(): delete the last extruded mesh.
 * - \b cancel_last_click(): cancel the last point clicked during the extrusion
 * - \b reset(): delete all extruded meshes.
 * - \b validate(): validate points and generates the extrusion mesh.
 *
 * @section Signal Signals
 * - \b tool_disabled(): sent when interactions are finished.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="..." type="sight::module::viz::scene3d::adaptor::shape_extruder">
        <inout key="extrudedMeshes" uid="..." />
        <config priority="2" extrude="true" validation_on_double_click="true" />
    </service>
   @endcode
 *
 * @subsection In-Out In-Out
 * - \b extrudedMeshes [sight::data::model_series]: model series where all extruded meshes are stored.
 *
 * @subsection Configuration Configuration:
 * - \b extrude (optional, bool, true) : sets if the extrusion is done or not (3D or 2D shape).
 * - \b priority (optional, int, default=2): interaction priority, higher priority interactions are performed first.
 * - \b validation_on_double_click (optional, bool, default=true): defines if extrusion mesh can be generated by double
 * click after last points
 * otherwise the slot validate() should be called (through a button, or connected to another signal).
 */
class MODULE_VIZ_SCENE3D_CLASS_API shape_extruder final :
    public sight::viz::scene3d::adaptor,
    public sight::viz::scene3d::interactor::base,
    private service::notifier
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(shape_extruder, sight::viz::scene3d::adaptor);

    /// Initializes the slot and the signal.
    MODULE_VIZ_SCENE3D_API shape_extruder() noexcept;

    /// Destroys the adaptor.
    MODULE_VIZ_SCENE3D_API ~shape_extruder() noexcept final = default;

protected:

    /// Configures the service.
    MODULE_VIZ_SCENE3D_API void configuring() final;

    /// Creates Ogre resources and materials.
    MODULE_VIZ_SCENE3D_API void starting() final;

    /// Does nothing.
    MODULE_VIZ_SCENE3D_API void updating() final;

    /// Destroys all Ogre resources.
    MODULE_VIZ_SCENE3D_API void stopping() final;

private:

    /// Represents a 2D triangle by three points, a barycenter, and a center/radius of the circumscribed circle.
    class triangle2_d
    {
    public:

        /**
         * @brief Creates a 2D triangle and computes necessary members.
         * @param _a first point of the triangle.
         * @param _b second point of the triangle.
         * @param _c third point of the triangle.
         */
        triangle2_d(const Ogre::Vector2& _a, const Ogre::Vector2& _b, const Ogre::Vector2& _c);

        /// Compare two triangles ID.
        inline bool operator<(const triangle2_d& _second) const;

        /// Defines the first point of the triangle.
        Ogre::Vector2 a;

        /// Defines the second point of the triangle.
        Ogre::Vector2 b;

        /// Defines the third point of the triangle.
        Ogre::Vector2 c;

        /// Defines the center of the circumscribed circle.
        Ogre::Vector2 center;

        /// Defines the radius of the circumscribed circle.
        float radius;

        /// Defines the barycenter.
        Ogre::Vector2 barycentre;

        /// Defines the unique ID of the triangle.
        std::size_t id;

    private:

        /// Defines the next ID to used for triangles.
        inline static std::size_t s_id = 0;
    };

    /// Represents a 3D triangle by three points.
    struct triangle3_d
    {
        /**
         * @brief Creates a 3D triangle.
         * @param _a first point of the triangle.
         * @param _b second point of the triangle.
         * @param _c third point of the triangle.
         */
        inline triangle3_d(const Ogre::Vector3& _a, const Ogre::Vector3& _b, const Ogre::Vector3& _c);

        /// Defines the first point of the triangle.
        Ogre::Vector3 a;

        /// Defines the second point of the triangle.
        Ogre::Vector3 b;

        /// Defines the third point of the triangle.
        Ogre::Vector3 c;
    };

    /// Represents an edge (a segment).
    struct edge
    {
        /**
         * @brief Creates an edge.
         * @param _a first point of the edge.
         * @param _b second point of the edge.
         */
        inline edge(const Ogre::Vector2& _a, const Ogre::Vector2& _b);

        /// Checks if the segment intersects an other one.
        [[nodiscard]] bool intersect(edge _edge) const;

        /// Checks if two segments a equals.
        inline bool operator==(const edge& _edge) const;

        /// Defines the first point of the triangle.
        Ogre::Vector2 a;

        /// Defines the second point of the triangle.
        Ogre::Vector2 b;
    };

    enum class action
    {
        add,
        remove
    };

    /// Computes the camera direction vector.
    static Ogre::Vector3 get_cam_direction(const Ogre::Camera* _cam);

    /// Sets if the tool is enabled or not.
    void enable_tool(bool _enable);

    /// Deletes the last extruded mesh.
    void delete_last_mesh();

    /// Cancel the last clicked point during the extrusion
    void cancel_last_click();

    /// Reset all extrusions
    void reset();

    /// Validate points and generates the extrusion mesh.
    void validate();

    /**
     * @brief Gets the near and far position of the intersection between the ray starting from the camera
     * and @ref m_lassoNearPlane/@ref m_lassoFarPlane
     *
     * @param _x x screen coordinate.
     * @param _y y screen coordinate.
     * @return the tool, near and far 3D intersection in the world space.
     */
    std::tuple<Ogre::Vector3, Ogre::Vector3, Ogre::Vector3> get_near_far_ray_positions(int _x, int _y) const;

    /**
     * @brief Modify the existing lasso
     * @param _action The option to do on the lasso. Either ADD or REMOVE.
     * @param _x X screen coordinate.
     * @param _y Y screen coordinate.
     */
    void modify_lasso(action _action, int _x = -1, int _y = -1);

    /**
     * @brief Cancels further interactions.
     * @pre @ref m_interactionEnableState must be true.
     */
    void wheel_event(modifier /*_mods*/, double /*_angleDelta*/, int /*_x*/, int /*_y*/) final;

    /**
     * @brief Adds a new point to the lasso.
     * @pre @ref m_toolEnableState must be true.
     * @param _button mouse modifier.
     * @param _x X screen coordinate.
     * @param _y Y screen coordinate.
     */
    void button_press_event(mouse_button _button, modifier /*_mods*/, int _x, int _y) final;

    /**
     * @brief Closes the lasso shape.
     * @pre @ref m_interactionEnableState must be true.
     * @param _button mouse modifier.
     * @param _x X screen coordinate.
     * @param _y Y screen coordinate.
     */
    void button_double_press_event(mouse_button _button, modifier /*_mods*/, int _x, int _y) final;

    /**
     * @brief Draws the last lasso line or add a point to the lasso in the mouse is dragged.
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

    /// Draws the lasso from @ref m_lassoNearPositions.
    void draw_lasso();

    /// Computes the triangulation of the two point lists @ref m_lassoNearPositions and @ref m_lassoFarPositions.
    void triangulate_points() const;

    /// Generates the mesh from a list of 3D triangles and adds it to the model series.
    void generate_extruded_mesh(const std::vector<triangle3_d>& _triangulation) const;

    /**
     * @brief Triangulates a point list.
     * @param _points the point list to triangulate.
     * @param _wordTriangulation vector where all generated triangles are pushed.
     */
    void generate_delaunay_triangulation(
        const std::vector<Ogre::Vector3>& _points,
        std::vector<triangle3_d>& _word_triangulation
    ) const;

    /// Adds a point to the triangulation with the delaunay constraints.
    static void add_delaunay_point(std::vector<triangle2_d>& _triangulation, const Ogre::Vector2& _sommet);

    /**
     * @brief Add a constrained edge to the triangulation. IT inserts a new point corresponding to the midpoint of the
     * segment
     * that does not appear in the triangulation, and use Lawson's incremental insertion algorithm to
     * maintain the Delaunay property.
     * @param _triangulation vector where generated triangles will be added.
     * @param _edge the constrained edge to add.
     * @param _depth used only by this method because it is recursive, it avoids stack overflow.
     * @return a list of new constrained generated points.
     */
    std::list<Ogre::Vector2> add_constraints(
        std::vector<triangle2_d>& _triangulation,
        const edge& _edge,
        int _depth = 0
    ) const;

    /// Defines the priority of the interactor.
    int m_priority {2};

    bool m_extrude {true};

    /// Contains the material data used for the lasso tool.
    data::material::sptr m_material {nullptr};

    /// Contains the adaptor used to create the Ogre material from the material data.
    module::viz::scene3d::adaptor::material::sptr m_material_adaptor {nullptr};

    /// Defines the color of the lasso's line.
    Ogre::ColourValue m_line_color {Ogre::ColourValue::White};

    /// Defines the color of the lasso's edge.
    Ogre::ColourValue m_edge_color {Ogre::ColourValue::White};

    /// Handles the tool activation state.
    bool m_tool_enable_state {false};

    /// Defines if the interaction as begin.
    bool m_interaction_enable_state {false};

    /// Handles the left button move state.
    bool m_left_button_move_state {false};

    /// Defines the plane where the lasso tool is drawn.
    Ogre::Plane m_lasso_tool_plane;

    /// Defines the near plane where the lasso work.
    Ogre::Plane m_lasso_near_plane;

    /// Defines the far plane where the lasso work.
    Ogre::Plane m_lasso_far_plane;

    /// Stores all position clicked or clicked and moved that are at near to the camera.
    std::vector<Ogre::Vector3> m_lasso_tool_positions;

    /// Stores all position clicked or clicked and moved that are near to the camera.
    std::vector<Ogre::Vector3> m_lasso_near_positions;

    /// Stores same position than near ones, but at the edge of the world bounding box.
    std::vector<Ogre::Vector3> m_lasso_far_positions;

    /// Stores all clicked position.
    std::vector<Ogre::Vector3> m_lasso_edge_positions;

    /// Defines the radius of each point drawn at edges positions.
    const float m_lasso_edge_size {0.005F};

    /// Contains the node where all manual objects that represent the lasso are attached.
    Ogre::SceneNode* m_lasso_node {nullptr};

    /// Contains the lasso object.
    Ogre::ManualObject* m_lasso {nullptr};

    /// Contains the last lasso line, this line is drawn between the last position and the current mouse position.
    Ogre::ManualObject* m_last_lasso_line {nullptr};

    /// Contains the signal sent when interactions are finished.
    core::com::signal<void()>::sptr m_tool_disabled_sig;

    sight::data::ptr<sight::data::model_series, sight::data::access::inout> m_extruded_meshes {this, "extrudedMeshes"};

    bool m_validation_by_double_click {true};
};

//------------------------------------------------------------------------------

bool shape_extruder::triangle2_d::operator<(const triangle2_d& _second) const
{
    return id < _second.id;
}

//------------------------------------------------------------------------------

shape_extruder::triangle3_d::triangle3_d(
    const Ogre::Vector3& _a,
    const Ogre::Vector3& _b,
    const Ogre::Vector3& _c
) :
    a(_a),
    b(_b),
    c(_c)
{
}

//------------------------------------------------------------------------------

shape_extruder::edge::edge(const Ogre::Vector2& _a, const Ogre::Vector2& _b) :
    a(_a),
    b(_b)
{
}

//------------------------------------------------------------------------------

bool shape_extruder::edge::operator==(const edge& _edge) const
{
    return (a == _edge.a && b == _edge.b) || (a == _edge.b && b == _edge.a);
}

} // namespace sight::module::viz::scene3d::adaptor.
