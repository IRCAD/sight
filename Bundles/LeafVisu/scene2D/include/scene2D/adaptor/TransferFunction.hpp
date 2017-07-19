/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __SCENE2D_ADAPTOR_TRANSFERFUNCTION_HPP__
#define __SCENE2D_ADAPTOR_TRANSFERFUNCTION_HPP__

#include "scene2D/adaptor/IAdaptor.hpp"

#include <fwData/TransferFunction.hpp>

#include <fwDataTools/helper/MedicalImageAdaptor.hpp>


namespace scene2D
{
namespace adaptor
{


class SCENE2D_CLASS_API TransferFunction : public ::fwDataTools::helper::MedicalImageAdaptor,
                                           public ::scene2D::adaptor::IAdaptor
{

public:
    fwCoreServiceClassDefinitionsMacro ( (TransferFunction)(::scene2D::adaptor::IAdaptor) );

    /// Enumeration of authorized line types
    enum LineType
    {
        PLAIN,
        DOTTED
    };

    /// Constructor, add handle events TRANSFERFUNCTION and WINDOWING.
    SCENE2D_API TransferFunction() noexcept;

    /// Basic destructor, do nothing.
    SCENE2D_API virtual ~TransferFunction() noexcept;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Image::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     * Connect Image::s_BUFFER_MODIFIED_SIG to this::s_UPDATE_SLOT
     */
    SCENE2D_API virtual KeyConnectionsType getObjSrvConnections() const;

protected:
    /**
     * @brief Configuring the TransferFunction adaptor.
     *
     * Example of configuration
     * @code{.xml}
       <adaptor id="tf2" class="::scene2D::adaptor::TransferFunction" objectId="myImage">
        <config lineColor="lightGray" circleColor="lightGray" xAxis="xAxis" yAxis="yAxis" zValue="4"/>
       </adaptor>
       @endcode
     * - \<config lineColor="lightGray" circleColor="lightGray" xAxis="xAxis" yAxis="yAxis" zValue="4"/\> : Set the config.
     *
     * \b lineColor : no mandatory (default value : black) : Set the color of the lines between the TF points.
     *
     * \b circleColor : no mandatory (default value : black) : Set the outline color of the circles representing the TF points.
     *
     * \b xAxis : no mandatory (default value : ::scene2D::data::Axis::New() : m_origin (0), m_scale (1), m_scaleType (LINEAR)) : Set the x Axis of the TF layer.
     *
     * \b yAxis : no mandatory (default value : ::scene2D::data::Axis::New() : m_origin (0), m_scale (1), m_scaleType (LINEAR)) : Set the y Axis of the TF layer.
     *
     * \b zValue : no mandatory (default value : 0) : Set the zValue of the TF layer (the higher the zValue, the higher the layer is).
     */
    SCENE2D_API void configuring();

    /// Initialize the layer m_layer (QGraphicsGroupItem), m_circleWidth and m_circleHeight from the viewport
    ///  dimensions, and call DoUpdate().
    SCENE2D_API void doStart();

    /// Call buildTFPoints(), buildCircles(), buildLinesAndPolygons() and buildLayer() to build the tf points map,
    ///  the circles vector, the lines and polygons vector, and to add'em all to the layer and add it to the scene.
    SCENE2D_API void doUpdate();

    /// @todo
    SCENE2D_API void doSwap();

    /// Clear the m_circles and m_linesAndPolygons vectors and remove the layer (and therefore all it's related
    ///  items) from the scene.
    SCENE2D_API void doStop();

    /// Iterate m_circles vector (and in parallel m_TFPoints map) and, as the case, call the function associated
    ///  to a specific event.
    SCENE2D_API void processInteraction( SPTR(::scene2D::data::Event) _event );


    /// Called when transfer function points are modified.
    SCENE2D_API virtual void updatingTFPoints();

    /// Called when transfer function windowing is modified.
    SCENE2D_API virtual void updatingTFWindowing(double window, double level);

private:

    /// Convert the view coordinates to item coordinates.
    SCENE2D_API ::scene2D::data::Coord coordViewToCoordItem( const ::scene2D::data::Coord& _coord );

    /// Get the selected tf of the image, calculate the window and the level, clear the m_TFPoints map and fill
    ///  it with the tf points of the selected tf.
    SCENE2D_API void buildTFPoints();

    /// Remove all circle items from the scene, clear the m_circles vector and push it back circles generated
    ///  from m_TFPoints.
    SCENE2D_API void buildCircles();

    /// From an iterator on the m_TFPoints map, create a QGraphicsEllipseItem, give it the appropriated color
    ///  and pen, and return it.
    SCENE2D_API QGraphicsEllipseItem* buildCircle(::fwData::TransferFunction::TFValueType value,
                                                  ::fwData::TransferFunction::TFColor color);

    /// Remove all line and polygon items from the scene, clear the m_linesAndPolygons vector, and push it back
    ///  lines and gradient polygons generated from m_circles.
    SCENE2D_API void buildLinesAndPolygons();

    /// Create lines and gradient polygons generated from m_circles
    SCENE2D_API void buildLinearLinesAndPolygons();

    /// Create lines and polygons generated from m_circles
    SCENE2D_API void buildNearestLinesAndPolygons();

    /// Build lines on TF bounds with color of first/last point (use when TF is not clamped)
    SCENE2D_API void buildBounds();

    /// Add the items from m_circles and m_linesAndPolygons to m_layer, set its position and its zValue and add
    /// it to the scene.
    SCENE2D_API void buildLayer();

    /// Clear the selected tf, rebuilt it from m_TFPoints (building TransferFunctionPoints and inserting'em),
    ///  update image min and max, and notify the image with a TRANSFERFUNCTION message.
    SCENE2D_API void updateImageTF();

    /// Open a color dialog and change the selected tf point color
    SCENE2D_API void doubleClickEvent(QGraphicsEllipseItem* circle, ::fwData::TransferFunction::TFColor& tfColor);

    /// Store the circle selected and its coordinates, and set its outline yellow
    SCENE2D_API void leftButtonEvent(QGraphicsEllipseItem* circle, ::scene2D::data::Event::sptr _event);

    /// Check if the mouse is out of bounds, as the case, move the circle on x and y, x or y, destroy the related
    /// point in the tf points map, create a new one with the new coord as key and alpha, rescale the tf map
    /// to 0-1 and update the image tf.
    SCENE2D_API void mouseMoveEvent(QGraphicsEllipseItem* circle,
                                    ::fwData::TransferFunction::TFValueType tfPoint,
                                    ::scene2D::data::Event::sptr _event);

    /// Reset the circle pen to the selected circle
    SCENE2D_API void mouseButtonReleaseEvent(QGraphicsEllipseItem* circle, ::scene2D::data::Event::sptr _event);

    /// Erase the selected point
    SCENE2D_API void rightButtonEvent(::fwData::TransferFunction::TFValueType tfPoint,
                                      ::scene2D::data::Event::sptr _event);

    /// Create a new point without modifying the TF (placed between the 2 encompassing points with linear
    /// interpolation)
    SCENE2D_API void doubleClickEvent( ::scene2D::data::Event::sptr _event);

    /// Return the x coordinate of the center of the circle in a 0-1 scale (for storage in m_TFPoints).
    SCENE2D_API double pointValue(QGraphicsEllipseItem* circle);

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
    ::scene2D::data::Coord m_oldCoord;

    /// Is a point captured by a mouse click?
    bool m_pointIsCaptured;

    /// The captured circle.
    QGraphicsEllipseItem* m_capturedCircle;

    /// fWID of the viewport
    std::string m_viewportID;

    /// Connection to the viewport
    ::fwCom::Connection m_connection;

    ::scene2D::data::Viewport::sptr m_viewport;

    float m_pointSize;
};



} // namespace adaptor
} // namespace scene2D


#endif // __SCENE2D_ADAPTOR_TRANSFERFUNCTION_HPP__

