/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _SCENE2D_ADAPTOR_CURVEDHISTOGRAM_HPP_
#define _SCENE2D_ADAPTOR_CURVEDHISTOGRAM_HPP_

#include <scene2D/adaptor/IAdaptor.hpp>

#include <fwData/Histogram.hpp>
#include <fwData/Point.hpp>

namespace scene2D
{

namespace adaptor
{

/**
 * @brief   IAdaptor implementation for histogram data.
 *
 * Configuration example:
 *
   @verbatim
   <adaptor id="histogram" class="::scene2D::adaptor::CurvedHistogram" objectId="myCurvedHistogram">
       <config xAxis="xAxis" yAxis="axeCurvedHistogramY" color="gray" opacity="0.25" zValue="5"/>
   </adaptor>
   @endverbatim
 *
 * \b color (mandatory)     : the background color of the histogram
 *
 * \b opacity (mandatory)   : the opacity of the histogram (from 0.0 to 1.0)
 *
 * \b xAxis                 : see ::scene2D::adaptor::IAdaptor
 *
 * \b yAxis                 : see ::scene2D::adaptor::IAdaptor
 *
 * \b zValue                : see ::scene1D::adaptor::IAdaptor
 */
class SCENE2D_CLASS_API CurvedHistogram : public ::scene2D::adaptor::IAdaptor
{
    public:
        fwCoreServiceClassDefinitionsMacro( (CurvedHistogram)( ::scene2D::adaptor::IAdaptor) );

        typedef std::pair< double, double > Point;
        typedef std::vector< Point > Points;

        SCENE2D_API CurvedHistogram() throw();
        SCENE2D_API virtual ~CurvedHistogram() throw();

    protected:
        SCENE2D_API void configuring()  throw ( ::fwTools::Failed );
        SCENE2D_API void doStart()      throw ( ::fwTools::Failed );
        SCENE2D_API void doUpdate()     throw ( ::fwTools::Failed );
        SCENE2D_API void doUpdate( fwServices::ObjectMsg::csptr _msg ) throw ( ::fwTools::Failed );
        SCENE2D_API void doSwap()       throw ( ::fwTools::Failed );
        SCENE2D_API void doStop()       throw ( ::fwTools::Failed );

        SCENE2D_API void processInteraction( SPTR(::scene2D::data::Event) _event );

        /// Ratio used for vertical scaling (default value: 1.1)
        static const float SCALE;

        /// The number of points between to points of the final Bezier curve to compute
        static const float NB_POINTS_BEZIER;

    private:

        Points getControlPoints( ::fwData::Histogram::sptr _histogram );

        Points getBSplinePoints( Points & _controlPoints );

        Points getResampledBSplinePoints( Points & _bSplinePoints );

        void buildBSplineFromPoints( Points & _bSplinePoints );

        void computePointToPathLengthMapFromBSplinePoints( Points & _bSplinePoints );

        /// Update the value of m_ordinateValueUID according to the value pointed by mouse cursor.
        void updateCurrentPoint( ::scene2D::data::Event::sptr _event );

        /// Append the vector of points to the path
        void appendPointsToPath( QPainterPath & _path, Points & _points );

        /// Build and add a part of histogram's border, according to the given path.
        void addBorderItem( const QPainterPath & _path );

        /// Build and add a part of the histogram, according to the given path.
        void addInnerItem( const QPainterPath & _path );


        Points linearInterpolation( const Point _p1, const Point _p2 );

        Points cosinusInterpolation( const Point _p0, const Point _p1 );

        Points quadraticInterpolation( const Point _p0, const Point _p1, const Point _p2 );

        Points cubicInterpolation(
                const Point _p0, const Point _p1, const Point _p2, const Point _p3 );


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
        QPainterPath m_painterPath;

        /// Width of histram's border
        float m_borderWidth;

        /// Current vertical scaling ratio
        float m_scale;

        // Graphis items contained into m_items are also added to this group item,
        // which is then added to the scene.
        // (This is the only graphic item which has to be added into the scene).
        QGraphicsItemGroup* m_layer;

        // Curve point at the current index of the histogram pointed by the mouse. This adaptor looks for 
        // mouse move events: when the mouse cursor is onto the histogram, the corresponding point of the 
        // histogram is informed into the object this UID is all about.
        std::string m_histogramPointUID;
};


}   // namespace adaptor

}   // namespace scene2D

#endif  // _SCENE2D_ADAPTOR_CURVEDHISTOGRAM_HPP_

