/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "scene2D/config.hpp"

#include <fwData/TransferFunction.hpp>

#include <fwDataTools/helper/MedicalImage.hpp>

#include <fwRenderQt/IAdaptor.hpp>

namespace scene2D
{
namespace adaptor
{

/**
 * @brief IAdaptor implementation to display a transfer function.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service uid="tf2" type="::scene2D::adaptor::STransferFunction" autoConnect="yes">
    <inout key="tf" uid="..." />
    <inout key="viewport" uid="..." />
    <config lineColor="lightGray" circleColor="gray" xAxis="xAxis" yAxis="yAxis" zValue="4" />
   </service>
   @endcode
 *
 * @subsection In-Out In-Out
 * - \b tf [::fwData::TransferFunction]: the current TransferFunction.
 * - \b viewport [::fwRenderQt::data::Viewport]: object listened to update the adaptor.
 *
 * @subsection Configuration Configuration:
 * - \b config (mandatory): contains the adaptor configuration
 *    - \b xAxis (optional): x axis associated to the adaptor
 *    - \b yAxis (optional): y axis associated to the adaptor
 *    - \b zValue (optional, default=0): z value of the layer
 *    - \b lineColor (optional, default black): Set the color of the lines between the TF points.
 *    - \b circleColor (optional, default black): Set the outline color of the circles representing the TF points.
 *    - \b pointSize (optional, default 10): specify point size.
 */
class SCENE2D_CLASS_API STransferFunction : public ::fwRenderQt::IAdaptor
{

public:
    fwCoreServiceClassDefinitionsMacro( (STransferFunction)(::fwRenderQt::IAdaptor) )

    /// Constructor, add handle events TRANSFERFUNCTION and WINDOWING.
    SCENE2D_API STransferFunction() noexcept;

    /// Basic destructor, do nothing.
    SCENE2D_API virtual ~STransferFunction() noexcept;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Image::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     * Connect Image::s_BUFFER_MODIFIED_SIG to this::s_UPDATE_SLOT
     */
    SCENE2D_API KeyConnectionsMap getAutoConnections() const override;

protected:

    SCENE2D_API void configuring() override;

    /// Initialize the layer m_layer (QGraphicsGroupItem), m_circleWidth and m_circleHeight from the viewport
    ///  dimensions, and call DoUpdate().
    SCENE2D_API void starting() override;

    /// Call buildTFPoints(), buildCircles(), buildLinesAndPolygons() and buildLayer() to build the tf points map,
    ///  the circles vector, the lines and polygons vector, and to add'em all to the layer and add it to the scene.
    SCENE2D_API void updating() override;

    /// Clear the m_circles and m_linesAndPolygons vectors and remove the layer (and therefore all it's related
    ///  items) from the scene.
    SCENE2D_API void stopping() override;

    /// Iterate m_circles vector (and in parallel m_TFPoints map) and, as the case, call the function associated
    ///  to a specific event.
    SCENE2D_API void processInteraction( ::fwRenderQt::data::Event& _event ) override;

private:

    /// Convert the view coordinates to item coordinates.
    ::fwRenderQt::data::Coord coordViewToCoordItem( const ::fwRenderQt::data::Coord& _coord );

    /// Get the selected tf of the image, calculate the window and the level, clear the m_TFPoints map and fill
    ///  it with the tf points of the selected tf.
    void buildTFPoints();

    /// Remove all circle items from the scene, clear the m_circles vector and push it back circles generated
    ///  from m_TFPoints.
    void buildCircles();

    /// From an iterator on the m_TFPoints map, create a QGraphicsEllipseItem, give it the appropriated color
    ///  and pen, and return it.
    QGraphicsEllipseItem* buildCircle(::fwData::TransferFunction::TFValueType value,
                                      ::fwData::TransferFunction::TFColor color);

    /// Remove all line and polygon items from the scene, clear the m_linesAndPolygons vector, and push it back
    ///  lines and gradient polygons generated from m_circles.
    void buildLinesAndPolygons();

    /// Create lines and gradient polygons generated from m_circles
    void buildLinearLinesAndPolygons();

    /// Create lines and polygons generated from m_circles
    void buildNearestLinesAndPolygons();

    /// Build lines on TF bounds with color of first/last point (use when TF is not clamped)
    void buildBounds();

    /// Add the items from m_circles and m_linesAndPolygons to m_layer, set its position and its zValue and add
    /// it to the scene.
    void buildLayer();

    /// Clear the selected tf, rebuilt it from m_TFPoints (building TransferFunctionPoints and inserting'em),
    ///  update image min and max, and notify the image with a TRANSFERFUNCTION message.
    void updateImageTF();

    /// Open a color dialog and change the selected tf point color
    void doubleClickEvent(QGraphicsEllipseItem* circle, ::fwData::TransferFunction::TFColor& tfColor);

    /// Store the circle selected and its coordinates, and set its outline yellow
    void leftButtonEvent(QGraphicsEllipseItem* circle, ::fwRenderQt::data::Event& _event);

    /// Check if the mouse is out of bounds, as the case, move the circle on x and y, x or y, destroy the related
    /// point in the tf points map, create a new one with the new coord as key and alpha, rescale the tf map
    /// to 0-1 and update the image tf.
    void mouseMoveEvent(QGraphicsEllipseItem* circle,
                        ::fwData::TransferFunction::TFValueType tfPoint,
                        ::fwRenderQt::data::Event& _event);

    /// Reset the circle pen to the selected circle
    void mouseButtonReleaseEvent(QGraphicsEllipseItem* circle, ::fwRenderQt::data::Event& _event);

    /// Erase the selected point
    void rightButtonEvent(::fwData::TransferFunction::TFValueType tfPoint,
                          ::fwRenderQt::data::Event& _event);

    /// Create a new point without modifying the TF (placed between the 2 encompassing points with linear
    /// interpolation)
    void doubleClickEvent( ::fwRenderQt::data::Event& _event);

    /// Return the x coordinate of the center of the circle in a 0-1 scale (for storage in m_TFPoints).
    double pointValue(QGraphicsEllipseItem* circle);

    /// The line pen (see "lineColor" config attribute) and circle pen (see "circleColor" config attribute).
    QPen m_linePen, m_circlePen;

    /// A vector containing QGraphicsItems representing the lines between tf points and the filling gradient
    /// polygons.
    std::vector< QGraphicsItem* > m_linesAndPolygons;

    /// A vector containing QGraphicsEllipseItems representing the circles representing the tf points.
    std::vector< QGraphicsEllipseItem* > m_circles;

    /// The layer.
    QGraphicsItemGroup* m_layer;

    /// The selected tf level and window, and width/height of the circles.
    double m_circleWidth, m_circleHeight;

    /// The map associating a key representing the value of a tf point to the ::fwData::Color associated.
    ::fwData::TransferFunction::TFDataType m_TFPoints;

    /// Coordinates saved to calculate circles moves.
    ::fwRenderQt::data::Coord m_oldCoord;

    /// Is a point captured by a mouse click?
    bool m_pointIsCaptured;

    /// The captured circle.
    QGraphicsEllipseItem* m_capturedCircle;

    float m_pointSize;
};

} // namespace adaptor
} // namespace scene2D
