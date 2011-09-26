/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwServices/Base.hpp>
#include <fwServices/IEditionService.hpp>

#include <fwData/Image.hpp>
#include <fwData/Float.hpp>
#include <fwData/Composite.hpp>

#include <fwComEd/ImageMsg.hpp>

#include <QGraphicsRectItem>
#include <QGraphicsView>

#include "scene2D/bspline.h"    // test Alex

#include "scene2D/adaptor/CurvedHistogram.hpp"
#include "scene2D/data/InitQtPen.hpp"
#include "scene2D/Scene2DGraphicsView.hpp"

REGISTER_SERVICE( ::scene2D::adaptor::IAdaptor, ::scene2D::adaptor::CurvedHistogram, ::fwData::Histogram);


#define PI 3.14159265


namespace scene2D
{

namespace adaptor
{

const float CurvedHistogram::SCALE = 1.1f; // vertical scaling factor applied at each mouse scroll
const float CurvedHistogram::NB_POINTS_BEZIER = 100.0f;

//-----------------------------------------------------------------------------------------------------------------

CurvedHistogram::CurvedHistogram() throw() : m_borderWidth(1.75f)
{
}

//-----------------------------------------------------------------------------------------------------------------

CurvedHistogram::~CurvedHistogram() throw()
{
}

//-----------------------------------------------------------------------------------------------------------------

void CurvedHistogram::configuring() throw( ::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    SLM_ASSERT("\"config\" tag is missing", m_configuration->getName() == "config");

    this->IAdaptor::configuring();  // Looks for 'xAxis', 'yAxis', 'opacity' and 'zValue'

    m_scale = 1.0f;//m_yAxis->getScale();
    m_innerColor = QPen( Qt::transparent );
    m_borderColor = QPen( Qt::transparent );
    m_brush = QBrush( Qt::NoBrush );

    if (!m_configuration->getAttributeValue("borderColor").empty())
    {
        ::scene2D::data::InitQtPen::setPenColor(
                m_borderColor, m_configuration->getAttributeValue("borderColor"), m_opacity );
    }

    if (!m_configuration->getAttributeValue("innerColor").empty())
    {
        ::scene2D::data::InitQtPen::setPenColor(
                m_innerColor, m_configuration->getAttributeValue("innerColor"), m_opacity );
    }

    if (!m_configuration->getAttributeValue("borderWidth").empty())
    {
        m_borderWidth = ::boost::lexical_cast< float >( m_configuration->getAttributeValue("borderWidth") );
    }

    m_histogramPointUID = m_configuration->getAttributeValue("histogramPointUID");

    OSLM_WARN_IF("If an histogram cursor is used with this histogram, m_histogramPointUID must be set in order to "
            << "inform about the position that the cursor should use.", m_histogramPointUID.empty());
}

//----------------------------------------------------------------------------------------------------------

void CurvedHistogram::doStart() throw( ::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    // Init border style
    m_borderColor.setCosmetic( true );
    m_borderColor.setWidthF( m_borderWidth );
    m_borderColor.setStyle( Qt::SolidLine );
    m_borderColor.setJoinStyle( Qt::RoundJoin );
    m_borderColor.setCapStyle( Qt::RoundCap );

    m_layer = new QGraphicsItemGroup();
    m_brush = QBrush( m_innerColor.color() );

    doUpdate();
}

//----------------------------------------------------------------------------------------------------------

CurvedHistogram::Points CurvedHistogram::getControlPoints( ::fwData::Histogram::sptr _histogram)
{
    ::fwData::Histogram::fwHistogramValues histogramValues = _histogram->getValues();
    const float binsWidth = _histogram->getBinsWidth();
    const float histogramMin = _histogram->getMinValue();
    const float histogramMinValue = _histogram->getMinValue();

    Point p;
    Points controlPoints;
    const unsigned int nbValues = (int) histogramValues.size();

    // WARNING: we shouldn't add all the points of the histogram into the vector of controlPoints
    // (testing...)
    for(int i = 0; i < nbValues; ++i)
    {
        p.first = (double)(histogramMin + i * binsWidth);
        p.second = histogramValues[i];

        controlPoints.push_back( p );
    }

    return controlPoints;
}

//----------------------------------------------------------------------------------------------------------

CurvedHistogram::Points CurvedHistogram::getBSplinePoints( Points & _points )
{
    Points bSplinePoints;
    Points::iterator it;
    point_list list;        // see bspline.h

    // Add again the first point with a higher value in order to prevent B-Spline algorithm from removing
    // the first value.
    list.add_point(
            new point(
                (float) _points[0].first,
                (float) _points[0].second * 2) );

    // Add all the points
    for( it = _points.begin(); it != _points.end(); ++it )
    {
        list.add_point(
                new point(
                    (float)((*it).first),
                    (float)((*it).second)) );
    }

    // Add again the last point
    list.add_point(
            new point(
                (float) _points.back().first,
                (float) _points.back().second / 2 ) );


    // Commpute the points of the B-Spline with external code from AHO (to be integrated here later).
    cat_curve curve( list );
    curve.precision = _points.size() * 5;
    curve.compute();

    for(unsigned int i = 0; i < curve.precision; ++i)
    {
        bSplinePoints.push_back( Point( curve.curve_point[i].x, curve.curve_point[i].y ) );
    }

    return bSplinePoints;
}

//----------------------------------------------------------------------------------------------------------

CurvedHistogram::Points CurvedHistogram::getResampledBSplinePoints( Points & _bSplinePoints )
{
    SLM_TRACE_FUNC();

    Points points;
    Points::iterator it;

    Point point = _bSplinePoints.front();

    double dx, dy;
    const double maxLength = 2000;
    double segmentLength = 0;

    points.push_back( point );

    for(it = _bSplinePoints.begin() + 1; it != _bSplinePoints.end(); ++it)
    {
        dx = abs((*it).first - point.first);    // theoretically positive
        dy = abs((*it).second - point.second);

        segmentLength += sqrt( dx * dx + dy * dy );

        if(segmentLength > maxLength)
        {
            points.push_back( *it );
            segmentLength = 0;
        }

        point = *it;
    }

    points.push_back( _bSplinePoints.back() );

    OSLM_TRACE("B-Spline points size moved from " << _bSplinePoints.size() << " to " << points.size());

    return points;
}

//----------------------------------------------------------------------------------------------------------

void CurvedHistogram::computePointToPathLengthMapFromBSplinePoints( Points & _bSplinePoints )
{
    Point p;
    Points::iterator it;

    for(it = _bSplinePoints.begin(); it != _bSplinePoints.end(); ++it)
    {
        p = this->mapAdaptorToScene( Point((*it).first, (*it).second), m_xAxis, m_yAxis );

        m_painterPath.lineTo( p.first, p.second );
        m_positionsToPathLength[ (int) p.first ] = m_painterPath.length();
    }
}

//----------------------------------------------------------------------------------------------------------

void CurvedHistogram::doUpdate() throw( ::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    ::fwData::Histogram::sptr histogram = this->getObject< ::fwData::Histogram>();

    Points controlPoints = this->getControlPoints( histogram );
    Points bSplinePoints = this->getBSplinePoints( controlPoints );

    computePointToPathLengthMapFromBSplinePoints( bSplinePoints );

    // Try to remove unnecessary points of the B-Spline points
    Points resampledBSplinePoints = this->getResampledBSplinePoints( bSplinePoints );
    bSplinePoints = resampledBSplinePoints;

    this->buildBSplineFromPoints( bSplinePoints );

    // Adjust the layer's position and zValue depending on the associated axis
    m_layer->setPos(m_xAxis->getOrigin(), m_yAxis->getOrigin());
    m_layer->setZValue(m_zValue);

    // Add to the scene the unique item which gather the whole set of rectangle graphic items:
    this->getScene2DRender()->getScene()->addItem( m_layer );
}

//----------------------------------------------------------------------------------------------------------

void CurvedHistogram::buildBSplineFromPoints( Points & _bSplinePoints )
{
    ::fwData::Histogram::sptr histogram = this->getObject< ::fwData::Histogram>();

    const double viewportY = this->getScene2DRender()->getViewport()->getY();

    const bool useBorderColor = (m_borderColor.color() != Qt::transparent);
    const bool useInnerColor = (m_innerColor.color() != Qt::transparent);

    Point currentPoint = this->mapAdaptorToScene( Point(histogram->getMinValue(), _bSplinePoints[0].second), m_xAxis, m_yAxis );
    Point previousPoint = currentPoint;
    Points::iterator it;

    const QPointF startPoint( currentPoint.first, currentPoint.second / 10 );   // divide by 10 to cut meaningless values
    QPainterPath path( QPointF(startPoint.x(), viewportY) );
    path.lineTo( startPoint );

    const int maxLinesPerPath = 10;
    int lineCount = 0;

    previousPoint.first = startPoint.x();
    previousPoint.second = startPoint.y();

    // Build the path with the B-Spline points
    for(it = _bSplinePoints.begin() + 1; it != _bSplinePoints.end(); ++it)
    {
        currentPoint = this->mapAdaptorToScene(
                std::pair<double, double>((*it).first, (*it).second), m_xAxis, m_yAxis );

        path.lineTo( currentPoint.first, currentPoint.second );

        if(lineCount++ == maxLinesPerPath)
        {
            if( useBorderColor )
            {
                addBorderItem( path );
            }

            if( useInnerColor )
            {
                path.lineTo( currentPoint.first, viewportY );
                path.lineTo( previousPoint.first, viewportY );
                path.lineTo( previousPoint.first, previousPoint.second );
                addInnerItem( path );
            }

            previousPoint = currentPoint;
            path = QPainterPath( QPointF( currentPoint.first, currentPoint.second) );
            lineCount = 0;
        }

    }

    // Close the path:
    m_painterPath.lineTo( histogram->getMaxValue(), _bSplinePoints.back().second);

    if( useBorderColor )
    {
        path.lineTo( currentPoint.first, viewportY );
        addBorderItem( path );
    }

    if( useInnerColor )
    {
        path.lineTo( previousPoint.first, viewportY );
        path.lineTo( previousPoint.first, previousPoint.second );
        addInnerItem( path );
    }
}

//----------------------------------------------------------------------------------------------------------

void CurvedHistogram::addInnerItem( const QPainterPath & _path )
{
    QGraphicsPathItem* item = new QGraphicsPathItem( _path );
    item->setPen( Qt::NoPen );
    item->setBrush( m_brush );
    item->setPath( _path );
    item->setCacheMode( QGraphicsItem::DeviceCoordinateCache );
    item->setZValue(1);

    m_layer->addToGroup( item );
}

//----------------------------------------------------------------------------------------------------------

void CurvedHistogram::addBorderItem( const QPainterPath & _path )
{
    QGraphicsPathItem* item = new QGraphicsPathItem( _path );
    item->setPen( m_borderColor );
    item->setBrush( Qt::NoBrush );
    item->setPath( _path );
    item->setCacheMode( QGraphicsItem::DeviceCoordinateCache );
    item->setZValue(2);

    m_layer->addToGroup( item );
}

//----------------------------------------------------------------------------------------------------------

void CurvedHistogram::appendPointsToPath( QPainterPath & _path, Points & _points )
{
    Points::iterator pointsIt;

    for(pointsIt = _points.begin(); pointsIt != _points.end(); ++pointsIt)
    {
        _path.lineTo( (*pointsIt).first, (*pointsIt).second );
    }
}

//----------------------------------------------------------------------------------------------------------

CurvedHistogram::Points CurvedHistogram::quadraticInterpolation(
        const Point _p0, const Point _p1, const Point _p2 )
{
    Points points;
    Point p;

    const double d2 =
        2 * ((_p2.second - _p1.second) / (_p2.first - _p1.first) - (_p1.second - _p0.second)
                / (_p1.first - _p0.first))
            / (_p2.first - _p0.first);

    const double d1 = (_p2.second - _p1.second) / (_p2.first - _p1.first) - 0.5 * d2 * (_p2.first - _p1.first);
    const double d0 = _p1.second;

    points.push_back( _p0 );

    for(double x = _p0.first; x < _p2.first; x += 0.5)
    {
        p.first = x;
        p.second = 0.5 * d2 * pow(x - _p1.first, 2) + d1 * (x - _p1.first) + d0;

        points.push_back( p );
    }

    return points;
}

//----------------------------------------------------------------------------------------------------------

CurvedHistogram::Points CurvedHistogram::cosinusInterpolation(const Point _p0, const Point _p1)
{
    Points points;
    Point p;
    double mu2;
    double deltaX = _p1.first - _p0.first;

    for(double mu = 0.0; mu < 1.0; mu += 0.05)
    {
        mu2 = (1 - std::cos(mu * PI)) / 2;

        p.first = _p0.first + mu * deltaX;
        p.second = _p0.second * (1 - mu2) + _p1.second * mu2;

        points.push_back( p );
    }

    return points;
}

//----------------------------------------------------------------------------------------------------------

CurvedHistogram::Points CurvedHistogram::cubicInterpolation(
        const Point _p0, const Point _p1, const Point _p2, const Point _p3 )
{
    Points points;
    Point p;
    double a0, a1, a2, a3, mu2;
    const double deltaX = _p2.first - _p1.first;
    for(double mu = 0.0; mu < 1.0; mu += 0.01)
    {
        mu2 = mu * mu;

        /*
        a0 = _p3.second - _p2.second - _p0.second + _p1.second;
        a1 = _p0.second - _p1.second - a0;
        a2 = _p2.second - _p0.second;
        a3 = _p1.second;
        */

        // Smoother curves (Catmull-Rom s_plines)
        a0 = -0.5*_p0.second + 1.5*_p1.second - 1.5*_p2.second + 0.5*_p3.second;
        a1 = _p0.second - 2.5*_p1.second + 2*_p2.second - 0.5*_p3.second;
        a2 = -0.5*_p0.second + 0.5*_p2.second;
        a3 = _p1.second;

        p.first = _p1.first + mu * deltaX;
        p.second = a0 * mu * mu2 +a1 * mu2 + a2 * mu + a3;

        points.push_back( p );
    }

    return points;
}

//----------------------------------------------------------------------------------------------------------

void CurvedHistogram::updateCurrentPoint( ::scene2D::data::Event::sptr _event )
{
    SLM_TRACE_FUNC();

    SLM_ASSERT("m_histogramPointUID must be defined in order to update the related ::fwData::Point data.",
            !m_histogramPointUID.empty());
 
    ::fwData::Histogram::sptr histogram = this->getObject< ::fwData::Histogram>();
    ::fwData::Histogram::fwHistogramValues values = histogram->getValues();
    const float histogramMinValue = histogram->getMinValue();
    const float histogramBinsWidth = histogram->getBinsWidth();

    // Event coordinates in scene
    ::scene2D::data::Coord sceneCoord = this->getScene2DRender()->mapToScene( _event->getCoord() );

    const int histIndex = (int) sceneCoord.getX();
    const int index = histIndex - histogramMinValue;
    const int nbValues = (int)values.size() * histogramBinsWidth;

    if(index >= 0 && index < nbValues && m_positionsToPathLength.find( histIndex ) != m_positionsToPathLength.end())
    {
        double key = m_positionsToPathLength[ histIndex ];
        const double percent = m_painterPath.percentAtLength( key );
        QPointF qPoint = m_painterPath.pointAtPercent( percent );

        ::fwData::Point::sptr point =
            ::fwData::Point::dynamicCast( ::fwTools::fwID::getObject( m_histogramPointUID ) );

        SLM_ASSERT("m_histogramPointUID can't be null here.", point);

        point->getRefCoord()[0] = sceneCoord.getX();
        point->getRefCoord()[1] = qPoint.y() * m_scale;
    }
}

//---------------------------------------------------------------------------------------------------------

CurvedHistogram::Points CurvedHistogram::linearInterpolation( const Point _p1, const Point _p2 )
{
    Points points;
    float t = 0;
    Point p;

    for(int i = 0; i < 100; ++i)
    {
        t = i / 100;

        p.first = _p1.first + ( _p2.first - _p1.first ) * t;
        p.second = _p1.second + ( _p2.second - _p1.second ) * t;
        points.push_back( p );
    }

    return points;
}

//----------------------------------------------------------------------------------------------------------

void CurvedHistogram::doUpdate( ::fwServices::ObjectMsg::csptr _msg) throw( ::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//----------------------------------------------------------------------------------------------------------

void CurvedHistogram::doSwap() throw( ::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//----------------------------------------------------------------------------------------------------------

void CurvedHistogram::doStop() throw( ::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//----------------------------------------------------------------------------------------------------------

void CurvedHistogram::processInteraction( ::scene2D::data::Event::sptr _event)
{
    SLM_TRACE_FUNC();

    bool updatePointedPos = false;

    // Vertical scaling
    if( _event->getType() == ::scene2D::data::Event::MouseWheelUp )
    {
        m_scale *= SCALE;
        m_layer->scale(1, SCALE);

        updatePointedPos = true;
    }
    else if( _event->getType() == ::scene2D::data::Event::MouseWheelDown )
    {
        m_scale /= SCALE;
        m_layer->scale(1, 1 / SCALE);

        updatePointedPos = true;
    }
    else if( _event->getType() == ::scene2D::data::Event::MouseMove )
    {
        updatePointedPos = true;
    }

    if( !m_histogramPointUID.empty() )
    {
        updateCurrentPoint( _event );
    }
}


}   // namespace adaptor

}   // namespace scene2D

