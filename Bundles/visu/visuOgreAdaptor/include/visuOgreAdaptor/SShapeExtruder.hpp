/************************************************************************
 *
 * Copyright (C) 2020 IRCAD France
 * Copyright (C) 2020 IHU Strasbourg
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

#include "visuOgreAdaptor/config.hpp"
#include "visuOgreAdaptor/SMaterial.hpp"

#include <fwCom/Signals.hpp>

#include <fwData/Material.hpp>

#include <fwMedData/ModelSeries.hpp>

#include <fwRenderOgre/IAdaptor.hpp>
#include <fwRenderOgre/interactor/IInteractor.hpp>

#include <OgreManualObject.h>
#include <OgreSceneNode.h>

namespace visuOgreAdaptor
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
 * - \b enableTool(bool): enable or disable the tool, it will be automatically disabled when interactions are finished.
 *
 * @section Signal Signals
 * - \b toolDisabled(): sent when interactions are finished.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="..." type="::visuOgreAdaptor::SShapeExtruder">
        <inout key="extrudedMeshes" uid="..." />
        <config layer="..." />
    </service>
   @endcode
 *
 * @subsection Configuration Configuration:
 * - \b layer (mandatory, string) : defines the layer.
 * - \b extrude (optional, bool, true) : sets if the extrusion is done or not (3D or 2D shape).
 *
 * @subsection In-Out In-Out
 * - \b extrudedMeshes [::fwMedData::ModelSeries]: model series where all extruded meshes are stored.
 */
class VISUOGREADAPTOR_CLASS_API SShapeExtruder final :
    public ::fwRenderOgre::IAdaptor,
    public ::fwRenderOgre::interactor::IInteractor
{
public:

    /// Generates default methods as New, dynamicCast, ...
    fwCoreServiceMacro(SShapeExtruder, ::fwRenderOgre::IAdaptor)

    /// Initializes the slot and the signal.
    VISUOGREADAPTOR_API SShapeExtruder() noexcept;

    /// Destroys the adaptor.
    VISUOGREADAPTOR_API virtual ~SShapeExtruder() noexcept;

private:

    /// Represents a 2D triangle by three points, a barycenter, and a center/radius of the circumscribed circle.
    class Triangle2D
    {

    public:

        /**
         * @brief Creates a 2D triangle and computes necessary members.
         * @param _a first point of the triangle.
         * @param _b second point of the triangle.
         * @param _c third point of the triangle.
         */
        Triangle2D(const ::Ogre::Vector2& _a, const ::Ogre::Vector2& _b, const ::Ogre::Vector2& _c);

        /// Compare two triangles ID.
        inline bool operator<(const Triangle2D& _second) const;

        /// Defines the first point of the triangle.
        ::Ogre::Vector2 a;

        /// Defines the second point of the triangle.
        ::Ogre::Vector2 b;

        /// Defines the third point of the triangle.
        ::Ogre::Vector2 c;

        /// Defines the center of the circumscribed circle.
        ::Ogre::Vector2 center;

        /// Defines the radius of the circumscribed circle.
        float radius;

        /// Defines the berycenter.
        ::Ogre::Vector2 barycentre;

        /// Defines the unique ID of the triangle.
        size_t id;

    private:

        /// Defines the next ID to used for triangles.
        inline static size_t s_id = 0;

    };

    /// Represents a 3D triangle by three points.
    struct Triangle3D
    {

        /**
         * @brief Creates a 3D triangle.
         * @param _a first point of the triangle.
         * @param _b second point of the triangle.
         * @param _c third point of the triangle.
         */
        inline Triangle3D(const ::Ogre::Vector3& _a, const ::Ogre::Vector3& _b, const ::Ogre::Vector3& _c);

        /// Defines the first point of the triangle.
        ::Ogre::Vector3 a;

        /// Defines the second point of the triangle.
        ::Ogre::Vector3 b;

        /// Defines the third point of the triangle.
        ::Ogre::Vector3 c;

    };

    /// Representes an edge (a segment).
    struct Edge
    {

        /**
         * @brief Creates an edge.
         * @param _a first point of the edge.
         * @param _b second point of the edge.
         */
        inline Edge(const ::Ogre::Vector2& _a, const ::Ogre::Vector2& _b);

        /// Checks if the segment intersects an other one.
        bool intersect(Edge _edge) const;

        /// Checks if two segments a equals.
        inline bool operator== (const Edge& _edge) const;

        /// Defines the first point of the triangle.
        ::Ogre::Vector2 a;

        /// Defines the second point of the triangle.
        ::Ogre::Vector2 b;

    };

    /// Computes the camera direction vector.
    static ::Ogre::Vector3 getCamDirection(const ::Ogre::Camera* const _cam);

    /// Configures the service.
    virtual void configuring() override;

    /// Creates Ogre resources and materials.
    virtual void starting() override;

    /// Does nothing.
    virtual void updating() override;

    /// Destroyes all Ogre resources.
    virtual void stopping() override;

    /// Sets if the tool is enabled or not.
    void enableTool(bool _enable);

    /**
     * @brief Gets the near and far position of the intersection between the ray starting from the camera
     * and @ref m_lassoNearPlane/@ref m_lassoFarPlane
     *
     * @param _x x screen coordinate.
     * @param _y y screen coordinate.
     * @return the tool, near and far 3D intersection in the world space.
     */
    std::tuple< ::Ogre::Vector3, ::Ogre::Vector3, ::Ogre::Vector3 > getNearFarRayPositions(int _x, int _y) const;

    /**
     * @brief Cancels further interactions.
     * @pre @ref m_interactionEnableState must be true.
     */
    virtual void wheelEvent(Modifier, int, int, int) override;

    /**
     * @brief Adds a new point to the lasso.
     * @pre @ref m_toolEnableState must be true.
     * @param _button Mousse modifier.
     * @param _x X screen coordinate.
     * @param _y Y screen coordinate.
     */
    virtual void buttonPressEvent(MouseButton _button, Modifier, int _x, int _y) override;

    /**
     * @brief Closes the lasso shape.
     * @pre @ref m_interactionEnableState must be true.
     * @param _button Mousse modifier.
     * @param _x X screen coordinate.
     * @param _y Y screen coordinate.
     */
    virtual void buttonDoublePressEvent(MouseButton _button, Modifier, int _x, int _y) override;

    /**
     * @brief Draws the last lasso line or add a point to the lasso in the mousse is dragged.
     * @pre @ref m_interactionEnableState must be true.
     * @param _button Mousse modifier.
     * @param _x X screen coordinate.
     * @param _y Y screen coordinate.
     */
    virtual void mouseMoveEvent(MouseButton _button, Modifier, int _x, int _y, int, int) override;

    /**
     * @brief Ends the drag interaction.
     * @pre @ref m_interactionEnableState and @ref m_leftButtonMoveState must be true.
     */
    virtual void buttonReleaseEvent(MouseButton, Modifier, int, int) override;

    /// Draws the lasso from @ref m_lassoNearPositions.
    void drawLasso();

    /// Computes the triangulation of the two point lists @ref m_lassoNearPositions and @ref m_lassoFarPositions.
    void triangulatePoints() const;

    /// Generates the mesh from a list of 3D triangles and adds it to the model series.
    void generateExtrudedMesh(const std::vector< Triangle3D >& _triangulation) const;

    /**
     * @brief Triangluates a point list.
     * @param _points the point list to triangulate.
     * @param _wordTriangulation vector where all generated triangles are pushed.
     */
    void generateDelaunayTriangulation(const std::vector< ::Ogre::Vector3 >& _points,
                                       std::vector< Triangle3D >& _wordTriangulation) const;

    /// Adds a point to the triangulation with the delaunay constraints.
    void addDelaunayPoint(std::vector< Triangle2D >& _triangulation, const ::Ogre::Vector2& _sommet) const;

    /**
     * @brief Add a contrained edge to the triangulation. IT inserts a new point corresponding to the midpoint of the
     * segment
     * that does not appear in the triangulation, and use Lawson's incremental insertion algorithm to
     * maintain the Delaunay property.
     * @param _triangulation vector where generated triangles will be added.
     * @param _edge the constrained edge to add.
     * @param _depth used only by this method because it is recursive, it avoids stack overflow.
     * @return a list of new constrained generated oints.
     */
    std::list< ::Ogre::Vector2 > addConstraints(std::vector< Triangle2D >& _triangulation,
                                                const Edge& _edge,
                                                int _depth = 0) const;

    /// Defines the priority of the interactor.
    int m_priority { 2 };

    bool m_extrude { true };

    /// Contains the material data used for the lasso tool.
    ::fwData::Material::sptr m_material { nullptr };

    /// Contains the adaptor used to create the Ogre material from the material data.
    ::visuOgreAdaptor::SMaterial::sptr m_materialAdaptor { nullptr };

    /// Defines the color of the lasso's line.
    ::Ogre::ColourValue m_lineColor { ::Ogre::ColourValue::White };

    /// Defines the color of the lasso's edge.
    ::Ogre::ColourValue m_edgeColor { ::Ogre::ColourValue::White };

    /// Handles the tool activation state.
    bool m_toolEnableState { false };

    /// Defines if the interaction as begin.
    bool m_interactionEnableState { false };

    /// Handles the left button move state.
    bool m_leftButtonMoveState { false };

    /// Defines the plane where the lasso tool is drawn.
    ::Ogre::Plane m_lassoToolPlane;

    /// Defines the near plane where the lasso work.
    ::Ogre::Plane m_lassoNearPlane;

    /// Defines the far plane where the lasso work.
    ::Ogre::Plane m_lassoFarPlane;

    /// Stores all position clicked or clicked and moved that are at near to the camera.
    std::vector< ::Ogre::Vector3 > m_lassoToolPositions;

    /// Stores all position clicked or clicked and moved that are near to the camera.
    std::vector< ::Ogre::Vector3 > m_lassoNearPositions;

    /// Stores same position than near ones, but at the edge of the world bounding box.
    std::vector< ::Ogre::Vector3 > m_lassoFarPositions;

    /// Stores all clicked position.
    std::vector< ::Ogre::Vector3 > m_lassoEdgePositions;

    /// Defines the radius of each point drawn at edges positions.
    const float m_lassoEdgeSize { 0.005f };

    /// Contains the node where all manual objects that represent the lasso are attached.
    ::Ogre::SceneNode* m_lassoNode { nullptr };

    /// Contains the lasso object.
    ::Ogre::ManualObject* m_lasso { nullptr };

    /// Contains the last lasso line, this line is drawn between the last position and the current mouse position.
    ::Ogre::ManualObject* m_lastLassoLine { nullptr };

    /// Contains the signal sent when interactions are finished.
    ::fwCom::Signal<void()>::sptr m_toolDisabledSig;

};

//------------------------------------------------------------------------------

bool SShapeExtruder::Triangle2D::operator<(const Triangle2D& _second) const
{
    return id < _second.id;
}

//------------------------------------------------------------------------------

SShapeExtruder::Triangle3D::Triangle3D(const ::Ogre::Vector3& _a,
                                       const ::Ogre::Vector3& _b,
                                       const ::Ogre::Vector3& _c) :
    a(_a),
    b(_b),
    c(_c)
{
}

//------------------------------------------------------------------------------

SShapeExtruder::Edge::Edge(const ::Ogre::Vector2& _a, const ::Ogre::Vector2& _b) :
    a(_a),
    b(_b)
{
}

//------------------------------------------------------------------------------

bool SShapeExtruder::Edge::operator==(const Edge& _edge) const
{
    return (a == _edge.a && b == _edge.b) || (a == _edge.b && b == _edge.a);
}

} // namespace visuOgreAdaptor.
