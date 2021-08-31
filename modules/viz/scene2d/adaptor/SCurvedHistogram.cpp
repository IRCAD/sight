/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "modules/viz/scene2d/adaptor/SCurvedHistogram.hpp"

#include <data/Float.hpp>

#include <service/macros.hpp>

#include <viz/scene2d/bspline.hpp>
#include <viz/scene2d/data/InitQtPen.hpp>
#include <viz/scene2d/Scene2DGraphicsView.hpp>

#include <QGraphicsRectItem>
#include <QGraphicsView>

#define PI 3.14159265

namespace sight::module::viz::scene2d
{

namespace adaptor
{

const float SCurvedHistogram::SCALE            = 1.1f; // vertical scaling factor applied at each mouse scroll
const float SCurvedHistogram::NB_POINTS_BEZIER = 100.0f;

//-----------------------------------------------------------------------------------------------------------------

SCurvedHistogram::SCurvedHistogram() noexcept :
    m_painterPath(nullptr),
    m_borderWidth(1.75f),
    m_scale(1.0f),
    m_layer(nullptr)
{
}

//-----------------------------------------------------------------------------------------------------------------

SCurvedHistogram::~SCurvedHistogram() noexcept
{
}

//-----------------------------------------------------------------------------------------------------------------

void SCurvedHistogram::configuring()
{
    this->configureParams(); // Looks for 'xAxis', 'yAxis', 'opacity' and 'zValue'

    const ConfigType config = this->getConfigTree().get_child("config.<xmlattr>");

    m_innerColor  = QPen(Qt::transparent);
    m_borderColor = QPen(Qt::transparent);
    m_brush       = QBrush(Qt::NoBrush);

    if(config.count("borderColor"))
    {
        sight::viz::scene2d::data::InitQtPen::setPenColor(
            m_borderColor,
            config.get<std::string>("borderColor"),
            m_opacity
        );
    }

    if(config.count("innerColor"))
    {
        sight::viz::scene2d::data::InitQtPen::setPenColor(
            m_innerColor,
            config.get<std::string>("innerColor"),
            m_opacity
        );
    }

    if(config.count("borderWidth"))
    {
        m_borderWidth = config.get<float>("borderWidth");
    }
}

//----------------------------------------------------------------------------------------------------------

void SCurvedHistogram::starting()
{
    // Init border style
    m_borderColor.setCosmetic(true);
    m_borderColor.setWidthF(static_cast<qreal>(m_borderWidth));
    m_borderColor.setStyle(Qt::SolidLine);
    m_borderColor.setJoinStyle(Qt::RoundJoin);
    m_borderColor.setCapStyle(Qt::RoundCap);

    m_brush = QBrush(m_innerColor.color());

    this->updating();
}

//----------------------------------------------------------------------------------------------------------

SCurvedHistogram::Points SCurvedHistogram::getControlPoints(const data::Histogram::csptr& _histogram) const
{
    data::Histogram::fwHistogramValues histogramValues = _histogram->getValues();
    const float binsWidth                              = _histogram->getBinsWidth();
    const float histogramMin                           = _histogram->getMinValue();

    Point p;
    Points controlPoints;
    const size_t nbValues = histogramValues.size();

    // WARNING: we shouldn't add all the points of the histogram into the vector of controlPoints
    // (testing...)
    for(size_t i = 0 ; i < nbValues ; ++i)
    {
        p.first  = static_cast<double>(histogramMin + i * binsWidth);
        p.second = histogramValues[i];

        controlPoints.push_back(p);
    }

    return controlPoints;
}

//----------------------------------------------------------------------------------------------------------

SCurvedHistogram::Points SCurvedHistogram::getBSplinePoints(const Points& _points) const
{
    Points bSplinePoints;
    point_list list; // see bspline.h

    // Add again the first point with a higher value in order to prevent B-Spline algorithm from removing
    // the first value.
    list.add_point(new point(static_cast<float>(_points[0].first), static_cast<float>(_points[0].second * 2)));

    // Add all the points
    for(const auto& pt : _points)
    {
        list.add_point(new point(static_cast<float>(pt.first), static_cast<float>(pt.second)));
    }

    // Add again the last point
    list.add_point(
        new point(
            static_cast<float>(_points.back().first),
            static_cast<float>(_points.back().second / 2)
        )
    );

    // Commpute the points of the B-Spline with external code from AHO (to be integrated here later).
    cat_curve curve(list);
    curve.m_precision = static_cast<int>(_points.size() * 5);
    curve.compute();

    for(int i = 0 ; i < curve.m_precision ; ++i)
    {
        bSplinePoints.push_back(Point(curve.m_curve_point[i].x, curve.m_curve_point[i].y));
    }

    return bSplinePoints;
}

//----------------------------------------------------------------------------------------------------------

SCurvedHistogram::Points SCurvedHistogram::getResampledBSplinePoints(const Points& _bSplinePoints) const
{
    Points points;
    Point point = _bSplinePoints.front();

    double dx, dy;
    const double maxLength = 2000;
    double segmentLength   = 0;

    points.push_back(point);

    for(Points::const_iterator it = _bSplinePoints.begin() + 1 ; it != _bSplinePoints.end() ; ++it)
    {
        dx = abs((*it).first - point.first); // theoretically positive
        dy = abs((*it).second - point.second);

        segmentLength += sqrt(dx * dx + dy * dy);

        if(segmentLength > maxLength)
        {
            points.push_back(*it);
            segmentLength = 0;
        }

        point = *it;
    }

    points.push_back(_bSplinePoints.back());

    return points;
}

//----------------------------------------------------------------------------------------------------------

void SCurvedHistogram::computePointToPathLengthMapFromBSplinePoints(Points& _bSplinePoints)
{
    Points::iterator it = _bSplinePoints.begin();

    if(it != _bSplinePoints.end())
    {
        Point p;

        p = this->mapAdaptorToScene(*it, m_xAxis, m_yAxis);
        QPointF prevPt = QPointF(p.first, p.second);
        m_painterPath->lineTo(p.first, p.second);
        qreal len = m_painterPath->length();
        ++it;

        for( ; it != _bSplinePoints.end() ; ++it)
        {
            p = this->mapAdaptorToScene(*it, m_xAxis, m_yAxis);

            m_painterPath->lineTo(p.first, p.second);

            // This is way too slow as the complexity is O(N.log(N) )
            //m_positionsToPathLength[ (int) p.first ] = m_painterPath->length();

            QPointF pt(p.first, p.second);
            len                                               += QLineF(prevPt, pt).length();
            m_positionsToPathLength[static_cast<int>(p.first)] = len;

            prevPt = pt;
        }
    }
}

//----------------------------------------------------------------------------------------------------------

void SCurvedHistogram::updating()
{
    this->stopping();

    const auto histogram = m_histogram.lock();

    m_layer = new QGraphicsItemGroup();

    m_painterPath = new QPainterPath();

    if(!histogram->getValues().empty())
    {
        Points controlPoints = this->getControlPoints(histogram.get_shared());
        Points bSplinePoints = this->getBSplinePoints(controlPoints);

        this->computePointToPathLengthMapFromBSplinePoints(bSplinePoints);

        // Try to remove unnecessary points of the B-Spline points
        Points resampledBSplinePoints = this->getResampledBSplinePoints(bSplinePoints);
        bSplinePoints = resampledBSplinePoints;

        this->buildBSplineFromPoints(bSplinePoints);

        // Adjust the layer's position and zValue depending on the associated axis
        m_layer->setPos(static_cast<qreal>(m_xAxis->getOrigin()), static_cast<qreal>(m_yAxis->getOrigin()));
        m_layer->setZValue(static_cast<qreal>(m_zValue));

        // Add to the scene the unique item which gather the whole set of rectangle graphic items:
        this->getScene2DRender()->getScene()->addItem(m_layer);
    }
}

//----------------------------------------------------------------------------------------------------------

void SCurvedHistogram::buildBSplineFromPoints(Points& _bSplinePoints)
{
    const auto histogram = m_histogram.lock();

    const bool useBorderColor = (m_borderColor.color() != Qt::transparent);
    const bool useInnerColor  = (m_innerColor.color() != Qt::transparent);

    Point currentPoint = this->mapAdaptorToScene(
        Point(histogram->getMinValue(), _bSplinePoints[0].second),
        m_xAxis,
        m_yAxis
    );
    Point previousPoint = currentPoint;
    Points::iterator it;

    const QPointF startPoint(currentPoint.first, currentPoint.second / 10); // divide by 10 to cut meaningless
                                                                            // values
    QPainterPath path(QPointF(startPoint.x(), 0.0));
    path.lineTo(startPoint);

    previousPoint.first  = startPoint.x();
    previousPoint.second = startPoint.y();

    // Build the path with the B-Spline points
    for(it = _bSplinePoints.begin() + 1 ; it != _bSplinePoints.end() ; ++it)
    {
        currentPoint = this->mapAdaptorToScene(*it, m_xAxis, m_yAxis);

        path.lineTo(currentPoint.first, currentPoint.second);
    }

    // Close the path:
    m_painterPath->lineTo(static_cast<qreal>(histogram->getMaxValue()), _bSplinePoints.back().second);

    if(useBorderColor)
    {
        path.lineTo(currentPoint.first, 0.0);
        this->addBorderItem(path);
    }

    if(useInnerColor)
    {
        path.lineTo(previousPoint.first, 0.0);
        this->addInnerItem(path);
    }
}

//----------------------------------------------------------------------------------------------------------

void SCurvedHistogram::addInnerItem(const QPainterPath& _path)
{
    QGraphicsPathItem* item = new QGraphicsPathItem(_path);
    item->setPen(Qt::NoPen);
    item->setBrush(m_brush);
    item->setPath(_path);
    item->setCacheMode(QGraphicsItem::DeviceCoordinateCache);
    item->setZValue(1);

    m_layer->addToGroup(item);
}

//----------------------------------------------------------------------------------------------------------

void SCurvedHistogram::addBorderItem(const QPainterPath& _path)
{
    QGraphicsPathItem* item = new QGraphicsPathItem(_path);
    item->setPen(m_borderColor);
    item->setBrush(Qt::NoBrush);
    item->setPath(_path);
    item->setCacheMode(QGraphicsItem::DeviceCoordinateCache);
    item->setZValue(2);

    m_layer->addToGroup(item);
}

//----------------------------------------------------------------------------------------------------------

SCurvedHistogram::Points SCurvedHistogram::quadraticInterpolation(
    const Point _p0,
    const Point _p1,
    const Point _p2
)
{
    Points points;
    Point p;

    const double d2 =
        2 * ((_p2.second - _p1.second) / (_p2.first - _p1.first) - (_p1.second - _p0.second)
             / (_p1.first - _p0.first))
        / (_p2.first - _p0.first);

    const double d1 = (_p2.second - _p1.second) / (_p2.first - _p1.first) - 0.5 * d2 * (_p2.first - _p1.first);
    const double d0 = _p1.second;

    points.push_back(_p0);

    for(double x = _p0.first ; x < _p2.first ; x += 0.5)
    {
        p.first  = x;
        p.second = 0.5 * d2 * pow(x - _p1.first, 2) + d1 * (x - _p1.first) + d0;

        points.push_back(p);
    }

    return points;
}

//----------------------------------------------------------------------------------------------------------

SCurvedHistogram::Points SCurvedHistogram::cosinusInterpolation(const Point _p0, const Point _p1)
{
    Points points;
    Point p;
    double mu2;
    const double deltaX = _p1.first - _p0.first;

    for(double mu = 0.0 ; mu < 1.0 ; mu += 0.05)
    {
        mu2 = (1 - std::cos(mu * PI)) / 2;

        p.first  = _p0.first + mu * deltaX;
        p.second = _p0.second * (1 - mu2) + _p1.second * mu2;

        points.push_back(p);
    }

    return points;
}

//----------------------------------------------------------------------------------------------------------

SCurvedHistogram::Points SCurvedHistogram::cubicInterpolation(
    const Point _p0,
    const Point _p1,
    const Point _p2,
    const Point _p3
)
{
    Points points;
    Point p;
    double a0, a1, a2, a3, mu2;
    const double deltaX = _p2.first - _p1.first;
    for(double mu = 0.0 ; mu < 1.0 ; mu += 0.01)
    {
        mu2 = mu * mu;

        /*
           a0 = _p3.second - _p2.second - _p0.second + _p1.second;
           a1 = _p0.second - _p1.second - a0;
           a2 = _p2.second - _p0.second;
           a3 = _p1.second;
         */

        // Smoother curves (Catmull-Rom s_plines)
        a0 = -0.5 * _p0.second + 1.5 * _p1.second - 1.5 * _p2.second + 0.5 * _p3.second;
        a1 = _p0.second - 2.5 * _p1.second + 2 * _p2.second - 0.5 * _p3.second;
        a2 = -0.5 * _p0.second + 0.5 * _p2.second;
        a3 = _p1.second;

        p.first  = _p1.first + mu * deltaX;
        p.second = a0 * mu * mu2 + a1 * mu2 + a2 * mu + a3;

        points.push_back(p);
    }

    return points;
}

//----------------------------------------------------------------------------------------------------------

void SCurvedHistogram::updateCurrentPoint(
    const sight::viz::scene2d::data::Event& _event,
    const data::Point::sptr& point
)
{
    const auto histogram                            = m_histogram.lock();
    const data::Histogram::fwHistogramValues values = histogram->getValues();

    const float histogramMinValue  = histogram->getMinValue();
    const float histogramBinsWidth = histogram->getBinsWidth();

    // Event coordinates in scene
    const sight::viz::scene2d::data::Coord sceneCoord = this->getScene2DRender()->mapToScene(_event.getCoord());

    const int histIndex = static_cast<int>(sceneCoord.getX());
    const int index     = static_cast<const int>(histIndex - histogramMinValue);
    const int nbValues  = static_cast<int>(values.size() * histogramBinsWidth);

    if(index >= 0 && index < nbValues && m_positionsToPathLength.find(histIndex) != m_positionsToPathLength.end())
    {
        const double key     = m_positionsToPathLength[histIndex];
        const double percent = m_painterPath->percentAtLength(key);
        const QPointF qPoint = m_painterPath->pointAtPercent(percent);

        point->getCoord()[0] = sceneCoord.getX();
        point->getCoord()[1] = qPoint.y() * static_cast<double>(m_scale);
    }
}

//---------------------------------------------------------------------------------------------------------

SCurvedHistogram::Points SCurvedHistogram::linearInterpolation(const Point _p1, const Point _p2)
{
    Points points;
    float t = 0.f;
    Point p;
    for(int i = 0 ; i < 100 ; ++i)
    {
        t = i / 100;

        p.first  = _p1.first + (_p2.first - _p1.first) * static_cast<double>(t);
        p.second = _p1.second + (_p2.second - _p1.second) * static_cast<double>(t);
        points.push_back(p);
    }

    return points;
}

//----------------------------------------------------------------------------------------------------------

void SCurvedHistogram::stopping()
{
    if(m_layer)
    {
        this->getScene2DRender()->getScene()->removeItem(m_layer);
        delete m_layer;
        m_layer = nullptr;
    }

    m_positionsToPathLength.clear();
    if(m_painterPath)
    {
        delete m_painterPath;
        m_painterPath = nullptr;
    }
}

//----------------------------------------------------------------------------------------------------------

void SCurvedHistogram::processInteraction(sight::viz::scene2d::data::Event& _event)
{
    bool updatePointedPos = false;

    // Vertical scaling
    if(_event.getType() == sight::viz::scene2d::data::Event::MouseWheelUp)
    {
        m_scale *= SCALE;
        m_layer->setTransform(QTransform::fromScale(1, static_cast<qreal>(SCALE)), true);

        updatePointedPos = true;
    }
    else if(_event.getType() == sight::viz::scene2d::data::Event::MouseWheelDown)
    {
        m_scale /= SCALE;
        m_layer->setTransform(QTransform::fromScale(1, 1 / static_cast<qreal>(SCALE)), true);

        updatePointedPos = true;
    }
    else if(_event.getType() == sight::viz::scene2d::data::Event::MouseMove)
    {
        updatePointedPos = true;
    }

    auto point = m_point.lock();

    if(point && updatePointedPos)
    {
        this->updateCurrentPoint(_event, point.get_shared());
    }
}

//----------------------------------------------------------------------------------------------------------

service::IService::KeyConnectionsMap SCurvedHistogram::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push(s_HISTOGRAM_INPUT, data::Histogram::s_MODIFIED_SIG, s_UPDATE_SLOT);
    return connections;
}

} // namespace adaptor

} // namespace sight::module::viz::scene2d
