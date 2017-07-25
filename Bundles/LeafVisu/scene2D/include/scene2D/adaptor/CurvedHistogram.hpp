/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __SCENE2D_ADAPTOR_CURVEDHISTOGRAM_HPP__
#define __SCENE2D_ADAPTOR_CURVEDHISTOGRAM_HPP__

#include "scene2D/config.hpp"

#include <fwData/Histogram.hpp>
#include <fwData/Point.hpp>

#include <fwRenderQt/IAdaptor.hpp>

namespace scene2D
{

namespace adaptor
{

/**
 * @brief IAdaptor implementation for histogram data.
 *
 * Configuration example:
 *
   @code{.xml}
   <service uid="histogram" type="::scene2D::adaptor::CurvedHistogram">
       <in key="histogram" uid="histogramUID" />
       <inout key="point" uid="pointUID" optional="yes" />
       <config xAxis="xAxis" yAxis="axeCurvedHistogramY" borderColor="lightGray" innerColor="gray" opacity="0.25"
               zValue="6" histogramPointUID="HistogramPointID" borderWidth="2.0" />
   </service>
   @endcode
 *
 * - \b innerColor         : the background color of the histogram
 * - \b borderColor        : the color of the histogram border
 * - \b borderWidth        : the width of the histogram border
 * - \b histogramPointUID  : the fwId of the histogram point. It is used with HistogramCursor and/or HistogramValue
 *      adaptor to show information at the current histogram index pointed by the mouse.
 * - \b opacity            : the opacity of the histogram (from 0.0 to 1.0)
 * - \b xAxis              : see ::fwRenderQt::IAdaptor
 * - \b yAxis              : see ::fwRenderQt::IAdaptor
 * - \b zValue             : see ::fwRenderQt::IAdaptor
 */
class SCENE2D_CLASS_API CurvedHistogram : public ::fwRenderQt::IAdaptor
{
public:
    fwCoreServiceClassDefinitionsMacro( (CurvedHistogram)( ::fwRenderQt::IAdaptor) );

    typedef ::fwRenderQt::IAdaptor::Point2DType Point;
    typedef std::vector< Point > Points;

    SCENE2D_API CurvedHistogram() noexcept;
    SCENE2D_API virtual ~CurvedHistogram() noexcept;

protected:
    SCENE2D_API void configuring();
    SCENE2D_API void doStart();
    SCENE2D_API void doUpdate();
    SCENE2D_API void doSwap();
    SCENE2D_API void doStop();

    SCENE2D_API void processInteraction( ::fwRenderQt::data::Event& _event );

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Histogram::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     */
    SCENE2D_API ::fwServices::IService::KeyConnectionsMap getAutoConnections() const;

    /// Ratio used for vertical scaling (default value: 1.1)
    static const float SCALE;

    /// The number of points between to points of the final Bezier curve to compute
    static const float NB_POINTS_BEZIER;

private:

    Points getControlPoints(const ::fwData::Histogram::csptr& _histogram ) const;

    Points getBSplinePoints( const Points& _controlPoints ) const;

    Points getResampledBSplinePoints( const Points& _bSplinePoints ) const;

    void buildBSplineFromPoints( Points& _bSplinePoints );

    void computePointToPathLengthMapFromBSplinePoints( Points& _bSplinePoints );

    /// Update the value of m_ordinateValueUID according to the value pointed by mouse cursor.
    void updateCurrentPoint(const ::fwRenderQt::data::Event& _event, const ::fwData::Point::sptr& point );

    /// Build and add a part of histogram's border, according to the given path.
    void addBorderItem( const QPainterPath& _path );

    /// Build and add a part of the histogram, according to the given path.
    void addInnerItem( const QPainterPath& _path );

    Points linearInterpolation( const Point _p1, const Point _p2 );

    Points cosinusInterpolation( const Point _p0, const Point _p1 );

    Points quadraticInterpolation( const Point _p0, const Point _p1, const Point _p2 );

    Points cubicInterpolation(const Point _p0, const Point _p1, const Point _p2, const Point _p3 );

    // Map the absciss of the points to the corresponding length within the path.
    std::map<double, double> m_positionsToPathLength;

    /// Color used for graphic histogram's border color
    QPen m_borderColor;

    // Color used for graphic histogram's inner color. If no value is supplied in the configuration,
    // item's inner will be transparent
    QPen m_innerColor;

    /// Inner color.
    QBrush m_brush;

    // A Qt painter that is used to build a curve representing the shape of the histogram.
    // Note that the histogram won't be built thanks to this path because of a lack of performance.
    // In other words, this painter path won't be used to build a graphic item that will be added to
    // a Qt graphics scene. The graphic representation of the histogram will composed of multiple items
    // build thanks to painters like this one, because it improves significantly the rendering performance.
    // This painter path will be used to provide information about the hsitogram: for instance, it will help
    // to retrieve the coordinates of path's points.
    QPainterPath* m_painterPath;

    /// Width of histram's border
    float m_borderWidth;

    /// Current vertical scaling ratio
    float m_scale;

    // Graphis items contained into m_items are also added to this group item,
    // which is then added to the scene.
    // (This is the only graphic item which has to be added into the scene).
    QGraphicsItemGroup* m_layer;
};

}   // namespace adaptor

}   // namespace scene2D

#endif  // __SCENE2D_ADAPTOR_CURVEDHISTOGRAM_HPP__

