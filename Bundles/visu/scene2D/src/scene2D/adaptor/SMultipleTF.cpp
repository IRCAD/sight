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

#include "scene2D/adaptor/SMultipleTF.hpp"

#include <fwCom/Signal.hxx>

#include <fwData/mt/ObjectReadLock.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>

#include <fwDataTools/helper/Composite.hpp>

#include <fwRenderQt/data/InitQtPen.hpp>
#include <fwRenderQt/Scene2DGraphicsView.hpp>

#include <fwServices/macros.hpp>

#include <QAction>
#include <QApplication>
#include <QColorDialog>
#include <QDesktopWidget>
#include <QMenu>
#include <QTimer>

namespace scene2D
{
namespace adaptor
{

static const ::fwServices::IService::KeyType s_VIEWPORT_INPUT   = "viewport";
static const ::fwServices::IService::KeyType s_CURRENT_TF_INPUT = "currentTF";

static const ::fwServices::IService::KeyType s_TF_POOL_INOUT = "tfPool";

static const ::fwServices::IService::KeyType s_TF_OUTPUT = "tf";

static const std::string s_POLYGON_COLOR_CONFIG  = "lineColor";
static const std::string s_POINT_COLOR_CONFIG    = "pointColor";
static const std::string s_SECOND_OPACITY_CONFIG = "secondOpacity";
static const std::string s_POINT_SIZE_CONFIG     = "pointSize";
static const std::string s_INTERACTIVE_CONFIG    = "interactive";

static int s_left_ramp_index_counter  = 0;
static int s_right_ramp_index_counter = 0;
static int s_trapeze_index_counter    = 0;

//-----------------------------------------------------------------------------

SMultipleTF::SMultipleTF() noexcept
{
    m_eventFilter = new QTimer();
}

//-----------------------------------------------------------------------------

SMultipleTF::~SMultipleTF() noexcept
{
    delete m_eventFilter;
}

//-----------------------------------------------------------------------------

void SMultipleTF::configuring()
{
    this->configureParams();

    const ConfigType tree = this->getConfigTree();
    const auto config     = tree.get_child("config.<xmlattr>");

    const std::string polygonColor = config.get(s_POLYGON_COLOR_CONFIG, "#FFFFFF");
    ::fwRenderQt::data::InitQtPen::setPenColor(m_polygonsPen, polygonColor);

    const std::string pointColor = config.get(s_POINT_COLOR_CONFIG, "#FFFFFF");
    ::fwRenderQt::data::InitQtPen::setPenColor(m_pointsPen, pointColor);

    m_secondOpacity = config.get< float >(s_SECOND_OPACITY_CONFIG, m_secondOpacity);
    m_pointSize     = config.get< float >(s_POINT_SIZE_CONFIG, m_pointSize);
    m_interactive   = config.get< bool >(s_INTERACTIVE_CONFIG, m_interactive);
}

//------------------------------------------------------------------------------

void SMultipleTF::starting()
{
    // Gets the TF pool.
    const ::fwData::Composite::csptr tfPool = this->getInOut< ::fwData::Composite >(s_TF_POOL_INOUT);
    SLM_ASSERT("inout '" + s_TF_POOL_INOUT + "' does not exist.", tfPool);

    // Sets the current TF.
    {
        const ::fwData::mt::ObjectReadLock tfPoolLock(tfPool);
        SLM_ASSERT("inout '" + s_TF_POOL_INOUT + "' must have at least on TF inside.", tfPool->size() > 0);

        // Sets the current TF used to highlight it in the editor.
        const ::fwData::TransferFunction::csptr tf = this->getInput< ::fwData::TransferFunction >(s_CURRENT_TF_INPUT);
        if(tf)
        {
            // Check if the current is in the composite.
            bool found = false;
            for(::fwData::Composite::value_type poolElt : *tfPool)
            {
                // Checks if it's a TF.
                const ::fwData::TransferFunction::sptr poolTF = ::fwData::TransferFunction::dynamicCast(poolElt.second);
                SLM_ASSERT("inout '" + s_TF_POOL_INOUT + "' must contain only TF.", poolTF);

                if(poolTF == tf)
                {
                    found = true;
                }
            }

            if(found)
            {
                m_currentTF = tf;
            }
        }

        if(m_currentTF == nullptr)
        {
            // Finds the first TF of the composite.
            const ::fwData::TransferFunction::sptr poolTF
                = ::fwData::TransferFunction::dynamicCast(tfPool->getContainer().begin()->second);
            SLM_ASSERT("inout '" + s_TF_POOL_INOUT + "' must contain only TF.", poolTF);
            m_currentTF = poolTF;
            // This action will call swapping method but m_currentTF is set by setCurrentTF, nothing will be done.
            this->setOutput(s_TF_OUTPUT, poolTF);
        }
    }

    m_layer = new QGraphicsItemGroup();

    m_pointsPen.setCosmetic(true);
    m_pointsPen.setWidthF(0);

    m_polygonsPen.setCosmetic(true);
    m_polygonsPen.setWidthF(0);

    // Creates all entities.
    this->updating();
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SMultipleTF::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push(s_VIEWPORT_INPUT, ::fwRenderQt::data::Viewport::s_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_TF_POOL_INOUT, ::fwData::Object::s_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_TF_POOL_INOUT, ::fwData::Composite::s_ADDED_OBJECTS_SIG, s_UPDATE_SLOT);
    connections.push(s_TF_POOL_INOUT, ::fwData::Composite::s_REMOVED_OBJECTS_SIG, s_UPDATE_SLOT);
    return connections;
}

//-----------------------------------------------------------------------------

void SMultipleTF::updating()
{
    m_connections.disconnect();

    // Clears old data.
    this->destroyTFPolygons();
    this->destroyTFPoints();
    this->releaseTFData();

    // Creates all TF.
    this->createTFPoints();
    this->createTFPolygons();

    // Builds the layer.
    this->buildLayer();

    // Connects all TF to the updating slot.
    for(const SubTF* const subTF : m_subTF)
    {
        m_connections.connect(subTF->m_tf, ::fwData::TransferFunction::s_MODIFIED_SIG, this->getSptr(), s_UPDATE_SLOT);
        m_connections.connect(subTF->m_tf, ::fwData::TransferFunction::s_POINTS_MODIFIED_SIG,
                              this->getSptr(), s_UPDATE_SLOT);
        m_connections.connect(subTF->m_tf, ::fwData::TransferFunction::s_WINDOWING_MODIFIED_SIG,
                              this->getSptr(), s_UPDATE_SLOT);
    }
}

//-----------------------------------------------------------------------------

void SMultipleTF::stopping()
{
    m_connections.disconnect();

    this->destroyTFPolygons();
    this->destroyTFPoints();
    this->releaseTFData();
}

//-----------------------------------------------------------------------------

void SMultipleTF::swapping(const KeyType& _key)
{
    SLM_ASSERT("The current TF mustn't be null", m_currentTF);

    const ::fwData::TransferFunction::csptr newCurrentTF
        = this->getInput< ::fwData::TransferFunction >(s_CURRENT_TF_INPUT);

    // Avoids swapping if it's the same TF.
    if(_key == s_CURRENT_TF_INPUT && newCurrentTF && newCurrentTF != m_currentTF)
    {
        m_currentTF = newCurrentTF;
        this->updating();
    }
}

//-----------------------------------------------------------------------------

void SMultipleTF::releaseTFData()
{
    for(SubTF* const subTF : m_subTF)
    {
        delete subTF;
    }
    m_subTF.clear();
}

//-----------------------------------------------------------------------------

void SMultipleTF::createTFPoints()
{
    // Iterates over each TF to create subTF.
    const ::fwData::Composite::csptr tfPool = this->getInOut< ::fwData::Composite >(s_TF_POOL_INOUT);
    SLM_ASSERT("inout '" + s_TF_POOL_INOUT + "' does not exist.", tfPool);
    const ::fwData::mt::ObjectReadLock tfPoolLock(tfPool);

    SLM_ASSERT("inout '" + s_TF_POOL_INOUT + "' must have at least one TF inside.", tfPool->size() > 0);

    int zIndex = 0;
    for(::fwData::Composite::value_type poolElt : *tfPool)
    {
        // Checks if it's a TF.
        const ::fwData::TransferFunction::sptr tf = ::fwData::TransferFunction::dynamicCast(poolElt.second);
        SLM_ASSERT("inout '" + s_TF_POOL_INOUT + "' must contain only TF.", tf);

        // Pushs the subTF to the vector.
        m_subTF.push_back(this->createSubTF(tf, zIndex));
        ++zIndex;
    }
}

//-----------------------------------------------------------------------------

void SMultipleTF::destroyTFPoints()
{
    // Removes TF point items from the scene and clear the TF point vector of each subTF.
    for(SubTF* const subTF : m_subTF)
    {
        for(std::pair< Point2DType, QGraphicsEllipseItem* >& tfPoint : subTF->m_TFPoints)
        {
            this->getScene2DRender()->getScene()->removeItem(tfPoint.second);
            delete tfPoint.second;
        }

        subTF->m_TFPoints.clear();
    }
}

//-----------------------------------------------------------------------------

SMultipleTF::SubTF* SMultipleTF::createSubTF(const ::fwData::TransferFunction::sptr _tf, int _zIndex)
{
    ::fwRenderQt::data::Viewport::sptr viewport = this->getScene2DRender()->getViewport();
    const ::fwData::mt::ObjectReadLock viewportLock(viewport);

    const double sceneWidth  = this->getScene2DRender()->getView()->width();
    const double sceneHeight = this->getScene2DRender()->getView()->height();

    // Computes point size in screen space and keep the smallest size (relativly to width or height).
    double pointSize = sceneWidth * m_pointSize;
    if(pointSize > sceneHeight * m_pointSize)
    {
        pointSize = sceneHeight * m_pointSize;
    }

    const double viewportWidth  = viewport->getWidth();
    const double viewportHeight = viewport->getHeight();

    // Computes point size from screen space to viewport space.
    const double pointWidth  = (viewportWidth * pointSize)/sceneWidth;
    const double pointHeight = (viewportHeight * pointSize)/sceneHeight;

    const ::fwData::Composite::csptr tfPool = this->getInOut< ::fwData::Composite >(s_TF_POOL_INOUT);
    SLM_ASSERT("inout '" + s_TF_POOL_INOUT + "' does not exist.", tfPool);
    const ::fwData::mt::ObjectReadLock tfPoolLock(tfPool);

    SLM_ASSERT("inout '" + s_TF_POOL_INOUT + "' must have at least on TF inside.", tfPool->size() > 0);

    // Creates the subTF and fill basic informations.
    SubTF* subTF = new SubTF();
    subTF->m_tf = _tf;

    // Sets the z-index of the current TF over all others.
    SLM_ASSERT("The current TF mustn't be null", m_currentTF);
    const bool isCurrent = m_currentTF == _tf;
    subTF->m_zIndex = isCurrent ? static_cast< int >(tfPool->size()) : _zIndex;

    const ::fwData::mt::ObjectReadLock tfLock(_tf);

    // Gets window/level informations to change TF value from TF space to window/level space.
    const ::fwData::TransferFunction::TFValuePairType minMaxValues = _tf->getMinMaxTFValues();
    const ::fwData::TransferFunction::TFValueType minWL            = _tf->getWLMinMax().first;
    const ::fwData::TransferFunction::TFValueType window           = _tf->getWindow();
    const ::fwData::TransferFunction::TFValueType width            = minMaxValues.second - minMaxValues.first;

    // Fills subTF point with color points.
    for(const ::fwData::TransferFunction::TFDataType::value_type& elt : _tf->getTFData())
    {
        // Computes TF value from TF space to window/level space.
        ::fwData::TransferFunction::TFValueType value;
        value = (elt.first - minMaxValues.first) / width;
        value = value * window + minWL;

        // Creates the color.
        const ::fwData::TransferFunction::TFColor tfColor = elt.second;
        const Point2DType valColor(value, tfColor.a);
        Point2DType coord = this->mapAdaptorToScene(valColor, m_xAxis, m_yAxis);

        // Builds a point item, set its color, pen and zIndex.
        QGraphicsEllipseItem* point = new QGraphicsEllipseItem(
            coord.first - pointWidth / 2,
            coord.second - pointHeight / 2,
            pointWidth,
            pointHeight
            );
        QColor color(static_cast< int >(tfColor.r*255),
                     static_cast< int >(tfColor.g*255),
                     static_cast< int >(tfColor.b*255));
        point->setBrush(QBrush(color));
        point->setCacheMode(QGraphicsItem::DeviceCoordinateCache);
        point->setPen(m_pointsPen);
        point->setZValue(subTF->m_zIndex*2+1);

        // Pushs it back into the point vector
        if(window > 0)
        {
            subTF->m_TFPoints.push_back(std::make_pair(coord, point));
        }
        // If the window is negative, the TF is reversed and point must be sort in reverse.
        else
        {
            subTF->m_TFPoints.insert(subTF->m_TFPoints.begin(), std::make_pair(coord, point));
        }
    }

    return subTF;
}

//-----------------------------------------------------------------------------

void SMultipleTF::createTFPolygons()
{
    // Iterates over all subTF to create polygons.
    for(SubTF* const subTF : m_subTF)
    {
        const ::fwData::mt::ObjectReadLock tfLock(subTF->m_tf);
        this->createTFPolygon(subTF);
    }
}

//-----------------------------------------------------------------------------

void SMultipleTF::createTFPolygon(SubTF* const _subTF)
{
    const ::fwRenderQt::data::Viewport::sptr viewport = this->getScene2DRender()->getViewport();
    const ::fwData::mt::ObjectReadLock viewportLock(viewport);

    QVector<QPointF> position;
    QLinearGradient grad;

    const std::pair< Point2DType, QGraphicsEllipseItem* >& firstTFPoint = _subTF->m_TFPoints.front();
    const std::pair< Point2DType, QGraphicsEllipseItem* >& lastTFPoint  = _subTF->m_TFPoints.back();

    const QGraphicsEllipseItem* const firtsPoint = firstTFPoint.second;

    double xBegin = firstTFPoint.first.first;
    double xEnd   = lastTFPoint.first.first;

    if(_subTF->m_tf->getIsClamped())
    {
        position.append(QPointF(xBegin, 0));
    }
    else
    {
        if(xBegin > viewport->getX())
        {
            xBegin = viewport->getX()-10;
            position.append(QPointF(xBegin, 0));
            position.append(QPointF(xBegin, firstTFPoint.first.second));
        }
        else
        {
            position.append(QPointF(xBegin, 0));
        }
        if(xEnd < viewport->getX() + viewport->getWidth())
        {
            xEnd = viewport->getX() + viewport->getWidth() + 10;
        }
    }

    grad.setColorAt(0, firtsPoint->brush().color());

    grad.setStart( xBegin, 0);
    grad.setFinalStop( xEnd, 0 );

    const double distanceMax = xEnd - xBegin;

    // Iterates on TF points vector to add line and polygon items to the polygons vector.
    if(_subTF->m_tf->getInterpolationMode() == ::fwData::TransferFunction::LINEAR)
    {
        this->buildLinearPolygons(_subTF, position, grad, distanceMax);
    }
    else
    {
        this->buildNearestPolygons(_subTF, position, grad, distanceMax);
    }

    if(!_subTF->m_tf->getIsClamped())
    {
        if(xEnd == viewport->getX() + viewport->getWidth() + 10)
        {
            position.append(QPointF(xEnd, lastTFPoint.first.second));
        }
        const double lastPointX = lastTFPoint.first.first;
        grad.setColorAt((lastPointX-xBegin)/distanceMax, lastTFPoint.second->brush().color());
    }

    position.append(QPointF(xEnd, 0));
    grad.setColorAt(1, lastTFPoint.second->brush().color());

    QGraphicsPolygonItem* const poly = new QGraphicsPolygonItem(QPolygonF(position));
    // Sets gradient, opacity and pen to the polygon
    poly->setBrush(QBrush(grad));
    poly->setPen(m_polygonsPen);
    poly->setCacheMode( QGraphicsItem::DeviceCoordinateCache );
    poly->setZValue(_subTF->m_zIndex*2);

    // If the z-index is the highest, it's the current one.
    if(_subTF->m_zIndex == m_subTF.size())
    {
        poly->setOpacity(m_opacity);
    }
    else
    {
        poly->setOpacity(m_secondOpacity);
    }

    // Pushs the polygon back into the polygons vector
    _subTF->m_TFPolygon = poly;
}

//-----------------------------------------------------------------------------

void SMultipleTF::destroyTFPolygons()
{
    // Removes polygon items from the scene and clear the polygon vector.
    for(SubTF* const subTF : m_subTF)
    {
        this->destroyTFPolygon(subTF);
    }
}

//-----------------------------------------------------------------------------

void SMultipleTF::destroyTFPolygon(SubTF* _subTF)
{
    // Removes polygon items from the scene and clear the polygon vector.
    this->getScene2DRender()->getScene()->removeItem(_subTF->m_TFPolygon);
    delete _subTF->m_TFPolygon;
}

//-----------------------------------------------------------------------------

void SMultipleTF::buildLinearPolygons(SubTF* const _subTF,
                                      QVector<QPointF>& _position,
                                      QLinearGradient& _grad,
                                      double _distanceMax)
{
    const std::vector< std::pair< Point2DType, QGraphicsEllipseItem* > >& tfPoints = _subTF->m_TFPoints;
    for(auto tfPointIt = tfPoints.cbegin(); tfPointIt != tfPoints.cend()-1; ++tfPointIt)
    {
        const QPointF p1(tfPointIt->first.first, tfPointIt->first.second);
        const QPointF p2((tfPointIt+1)->first.first, (tfPointIt+1)->first.second);

        _position.append(p1);
        _position.append(p2);

        // Builds the gradient
        _grad.setColorAt((p1.x() - _position[0].x())/_distanceMax, (tfPointIt->second)->brush().color());
    }
}

//-----------------------------------------------------------------------------

void SMultipleTF::buildNearestPolygons(SubTF* const _subTF,
                                       QVector<QPointF>& _position,
                                       QLinearGradient& _grad,
                                       double _distanceMax)
{
    const std::vector< std::pair< Point2DType, QGraphicsEllipseItem* > >& tfPoints = _subTF->m_TFPoints;
    for(auto tfPointIt = tfPoints.cbegin(); tfPointIt != tfPoints.cend()-1; ++tfPointIt)
    {
        const QPointF p1(tfPointIt->first.first, tfPointIt->first.second);
        const QPointF p4((tfPointIt+1)->first.first, (tfPointIt+1)->first.second);

        const QPointF p2(p1.x() + (p4.x() - p1.x())/2., p1.y());
        const QPointF p3(p2.x(), p4.y());

        _position.append(p1);
        _position.append(p2);
        _position.append(p3);
        _position.append(p4);

        const double d1 = (p1.x() - _position[0].x())/_distanceMax;
        const double d2 = (p2.x() - _position[0].x())/_distanceMax;
        const double d3 = d2 + std::numeric_limits< double >::epsilon();
        const double d4 = (p4.x() - _position[0].x())/_distanceMax;

        const QColor c1 = (tfPointIt->second)->brush().color();
        const QColor c4 = ((tfPointIt+1)->second)->brush().color();

        _grad.setColorAt(d1, c1);
        _grad.setColorAt(d2, c1);
        _grad.setColorAt(d3, c4);
        _grad.setColorAt(d4, c4);
    }
}

//-----------------------------------------------------------------------------

void SMultipleTF::buildLayer()
{
    // Adds graphics items vectors to the layer.
    for(SubTF* const subTF : m_subTF)
    {
        for(std::pair< Point2DType, QGraphicsEllipseItem* >& tfPoint : subTF->m_TFPoints)
        {
            m_layer->addToGroup(tfPoint.second);
        }

        m_layer->addToGroup(subTF->m_TFPolygon);
    }

    // Adjusts the layer's position and zValue depending on the associated axis.
    m_layer->setPos(m_xAxis->getOrigin(), m_yAxis->getOrigin());
    m_layer->setZValue(m_zValue);

    // Adds the layer item to the scene.
    this->getScene2DRender()->getScene()->addItem(m_layer);
}

//-----------------------------------------------------------------------------

void SMultipleTF::setCurrentTF(SubTF* const _subTF)
{
    // Sets the new current TF.
    SLM_ASSERT("The current TF mustn't be null", m_currentTF);

    // Find the old subTF.
    SubTF* const currentSubTF = *(std::find_if(m_subTF.begin(), m_subTF.end(), [&](const SubTF* _subTF)
            {
                return _subTF->m_tf == m_currentTF;
            }));

    // Changes the current subTF.
    m_currentTF = _subTF->m_tf;

    // Recomputes z-index and set the z-index of the selected TF over all others.
    int zIndex = 0;
    for(SubTF* subTF : m_subTF)
    {
        subTF->m_zIndex = subTF->m_tf == m_currentTF ? static_cast< int >(m_subTF.size()) : zIndex;
        for(std::pair< Point2DType, QGraphicsEllipseItem* >& point : subTF->m_TFPoints)
        {
            point.second->setZValue(subTF->m_zIndex*2+1);
        }
        ++zIndex;
    }

    // Re-draw polygons since the current TF as changed.
    this->destroyTFPolygon(currentSubTF);
    this->createTFPolygon(currentSubTF);
    this->destroyTFPolygon(_subTF);
    this->createTFPolygon(_subTF);
    this->buildLayer();
}

//-----------------------------------------------------------------------------

std::vector< SMultipleTF::SubTF* > SMultipleTF::getMatchingSubTF(const ::fwRenderQt::data::Event& _event) const
{
    // Finds all subTF that match the clicked coord.
    std::vector< SubTF* > matchingSubTF;
    const QPoint scenePos = QPoint(static_cast< int >(_event.getCoord().getX()),
                                   static_cast< int >(_event.getCoord().getY()));
    QList<QGraphicsItem*> items = this->getScene2DRender()->getView()->items(scenePos);

    // Fills the subTF vector with clicked ones.
    for(SubTF* const subTF : m_subTF)
    {
        // Checks if a polygon is clicked.
        if(items.indexOf(subTF->m_TFPolygon) >= 0)
        {
            matchingSubTF.push_back(subTF);
        }
    }

    return matchingSubTF;
}

//-----------------------------------------------------------------------------

void SMultipleTF::processInteraction(::fwRenderQt::data::Event& _event)
{
    if(!m_interactive)
    {
        return;
    }

    SLM_ASSERT("The current TF mustn't be null", m_currentTF);

    // If it's a resize event, all the scene must be recompted.
    if(_event.getType() == ::fwRenderQt::data::Event::Resize)
    {
        this->updating();
        _event.setAccepted(true);
        return;
    }

    // If a point as already been captured.
    if(m_capturedTFPoint)
    {
        if(_event.getButton() == ::fwRenderQt::data::Event::LeftButton &&
           _event.getType() == ::fwRenderQt::data::Event::MouseButtonRelease)
        {
            // Releases capture point.
            this->leftButtonReleaseEvent();
            _event.setAccepted(true);
            return;
        }
    }

    // If a subTF as already been captured.
    if(m_capturedTF.first)
    {
        if(_event.getType() == ::fwRenderQt::data::Event::MouseMove)
        {
            // Changes the subTF level.
            this->mouseMoveOnSubTFEvent(_event);
            _event.setAccepted(true);
            return;
        }
        else if(_event.getButton() == ::fwRenderQt::data::Event::MidButton &&
                _event.getType() == ::fwRenderQt::data::Event::MouseButtonRelease)
        {
            // Releases capture subTF.
            this->midButtonReleaseEvent();
            _event.setAccepted(true);
            return;
        }
    }

    const QPoint scenePos = QPoint(static_cast< int >(_event.getCoord().getX()),
                                   static_cast< int >(_event.getCoord().getY()));
    QList<QGraphicsItem*> items = this->getScene2DRender()->getView()->items(scenePos);

    // Checks if a point is clicked.
    for(SubTF* const subTF : m_subTF)
    {
        for(std::pair< Point2DType, QGraphicsEllipseItem* >& tfPoint : subTF->m_TFPoints)
        {
            // If a point has already been captured.
            if(m_capturedTFPoint == &tfPoint)
            {
                if(_event.getType() == ::fwRenderQt::data::Event::MouseMove)
                {
                    // Moves the captured point.
                    this->mouseMoveOnPointEvent(subTF, _event);
                    _event.setAccepted(true);
                    return;
                }

            }
            else if(items.indexOf(tfPoint.second) >= 0)
            {
                // If there is a double click on a point, open a color dialog.
                if(_event.getButton() == ::fwRenderQt::data::Event::LeftButton &&
                   _event.getType() == ::fwRenderQt::data::Event::MouseButtonDoubleClick)
                {
                    this->leftButtonDoubleClickOnPointEvent(subTF, tfPoint);
                    _event.setAccepted(true);
                    return;
                }
                // If left button is pressed on a point, set the TF as current.
                else if(_event.getButton() == ::fwRenderQt::data::Event::LeftButton &&
                        _event.getType() == ::fwRenderQt::data::Event::MouseButtonPress)
                {
                    this->leftButtonClickOnPointEvent(subTF, tfPoint);
                    _event.setAccepted(true);
                    return;
                }
                // If right button is pressed on a point, remove it.
                if(_event.getButton() == ::fwRenderQt::data::Event::RightButton &&
                   _event.getType() == ::fwRenderQt::data::Event::MouseButtonPress &&
                   subTF->m_tf == m_currentTF && subTF->m_TFPoints.size() > 2)
                {
                    this->rightButtonClickOnPointEvent(subTF, tfPoint);
                    _event.setAccepted(true);
                    return;
                }
            }
        }
    }

    // Adds a new TF point.
    if(_event.getButton() == ::fwRenderQt::data::Event::LeftButton &&
       _event.getType() == ::fwRenderQt::data::Event::MouseButtonDoubleClick)
    {
        // Cancel the previous single click interaction.
        m_eventFilter->stop();
        this->leftButtonDoubleClickEvent(_event);
        _event.setAccepted(true);
        return;
    }

    // If left button is pressed, set the nearest TF as current.
    if(_event.getButton() == ::fwRenderQt::data::Event::LeftButton &&
       _event.getType() == ::fwRenderQt::data::Event::MouseButtonPress)
    {
        // Cancel the previous event if it's needed.
        m_eventFilter->stop();
        delete m_eventFilter;

        // Validates the event in 250ms, this allow to the double click event to cancel the interaction.
        m_eventFilter = new QTimer();
        m_eventFilter->connect(m_eventFilter, &QTimer::timeout, this, [ = ]()
                {
                    this->leftButtonCLickEvent(_event);
                });
        m_eventFilter->setSingleShot(true);
        m_eventFilter->start(250);
        _event.setAccepted(true);
        return;
    }

    // If midlle button is pressed, select the current TF to adjust the window/level.
    if(_event.getButton() == ::fwRenderQt::data::Event::MidButton &&
       _event.getType() == ::fwRenderQt::data::Event::MouseButtonPress)
    {
        this->midButtonClickEvent(_event);
        _event.setAccepted(true);
        return;
    }

    // If right button is pressed, open a context menu to manage multiple actions.
    if(_event.getButton() == ::fwRenderQt::data::Event::RightButton &&
       _event.getType() == ::fwRenderQt::data::Event::MouseButtonPress)
    {
        this->rightButtonCLickEvent(_event);
        _event.setAccepted(true);
        return;
    }
}

//-----------------------------------------------------------------------------

void SMultipleTF::leftButtonCLickEvent(const ::fwRenderQt::data::Event& _event)
{
    std::vector< SubTF* > matchingSubTF = this->getMatchingSubTF(_event);

    if(matchingSubTF.size() > 0)
    {
        if(matchingSubTF.size() == 1)
        {
            if(matchingSubTF[0]->m_tf != m_currentTF)
            {
                this->setCurrentTF(matchingSubTF[0]);
                // This action will call swapping method but m_currentTF is set by setCurrentTF, nothing will be done.
                this->setOutput(s_TF_OUTPUT, matchingSubTF[0]->m_tf);
            }
        }
        // Finds the closets one.
        else
        {
            ::fwRenderQt::data::Coord clickCoord = this->getScene2DRender()->mapToScene(_event.getCoord());

            float closestDistance  = std::numeric_limits< float >::max();
            SubTF* newCurrentSubTF = nullptr;
            for(SubTF* subTF : matchingSubTF)
            {
                // Finds nearest position of the iterate subTF.
                float localClosestDistance  = std::numeric_limits< float >::max();
                SubTF* localNewCurrentSubTF = nullptr;
                for(int i = 0; i <= subTF->m_TFPoints.size(); ++i)
                {
                    Point2DType tfPoint1;
                    Point2DType tfPoint2;

                    // Creates the first fictional TF point.
                    if(i == 0)
                    {
                        tfPoint2 = subTF->m_TFPoints[i].first;
                        if(subTF->m_tf->getIsClamped())
                        {
                            // The first point is the same a the real first but with a zero alpha channel.
                            tfPoint1 = std::make_pair(tfPoint2.first, 0);
                        }
                        else
                        {
                            // The first point is the same a the real but with an infinite lower value.
                            const ::fwRenderQt::data::Viewport::csptr viewport =
                                this->getScene2DRender()->getViewport();
                            const ::fwData::mt::ObjectReadLock viewportLock(viewport);
                            tfPoint1 = std::make_pair(viewport->getX(), tfPoint2.second);
                        }
                    }
                    // Creates the last fictional TF point.
                    else if(i == subTF->m_TFPoints.size())
                    {
                        tfPoint1 = subTF->m_TFPoints[i-1].first;
                        if(subTF->m_tf->getIsClamped())
                        {
                            // The last point is the same a the real last but with a zero alpha channel.
                            tfPoint2 = std::make_pair(tfPoint1.first, 0);
                        }
                        else
                        {
                            // The last point is the same a the real but with an infinite upper value.
                            const ::fwRenderQt::data::Viewport::csptr viewport =
                                this->getScene2DRender()->getViewport();
                            const ::fwData::mt::ObjectReadLock viewportLock(viewport);
                            tfPoint2 = std::make_pair(viewport->getX() + viewport->getWidth(), tfPoint1.second);
                        }
                    }
                    // Retrieves two TF points.
                    else
                    {
                        tfPoint1 = subTF->m_TFPoints[i-1].first;
                        tfPoint2 = subTF->m_TFPoints[i].first;
                    }

                    // Gets a line/point projection.
                    const QLineF line(tfPoint1.first, tfPoint1.second, tfPoint2.first, tfPoint2.second);

                    QLineF perpendicLine(clickCoord.getX(), clickCoord.getY(), clickCoord.getX(), 0);
                    perpendicLine.setAngle(90.f + line.angle());

                    QPointF intersectPoint;
                    line.intersect(perpendicLine, &intersectPoint);

                    const QVector2D origin(static_cast< float >(clickCoord.getX()),
                                           static_cast< float >(clickCoord.getY()));

                    float distance = std::numeric_limits< float >::max();

                    // Checks if the intersection belong the segment.
                    if(intersectPoint.x() >= tfPoint1.first && intersectPoint.x() <= tfPoint2.first)
                    {
                        const QVector2D intersect(intersectPoint);
                        const QVector2D projection = origin - intersect;

                        distance = projection.length();
                    }
                    // Elses the lower distance is between the point and one of the segment edge.
                    else
                    {
                        const QVector2D firstLine(static_cast< float >(clickCoord.getX() - tfPoint1.first),
                                                  static_cast< float >(clickCoord.getY() - tfPoint1.second));
                        const QVector2D secondLine(static_cast< float >(clickCoord.getX() - tfPoint2.first),
                                                   static_cast< float >(clickCoord.getY() - tfPoint2.second));

                        distance = firstLine.length();
                        if(secondLine.length() < distance)
                        {
                            distance = secondLine.length();
                        }
                    }

                    if(distance < localClosestDistance)
                    {
                        localClosestDistance = distance;
                        localNewCurrentSubTF = subTF;
                    }
                }

                if(localClosestDistance < closestDistance)
                {
                    closestDistance = localClosestDistance;
                    newCurrentSubTF = localNewCurrentSubTF;
                }
            }

            // Sets the new current TF.
            if(newCurrentSubTF->m_tf != m_currentTF)
            {
                this->setCurrentTF(newCurrentSubTF);
                // This action will call swapping method but m_currentTF is set by setCurrentTF, nothing will be done.
                this->setOutput(s_TF_OUTPUT, newCurrentSubTF->m_tf);
            }
        }

    }
}

//-----------------------------------------------------------------------------

void SMultipleTF::leftButtonClickOnPointEvent(SubTF* const _subTF,
                                              std::pair< Point2DType,
                                                         QGraphicsEllipseItem* >& _TFPoint)
{
    // Stores the captured TF point in case it's moved.
    m_capturedTFPoint = &_TFPoint;

    // Sets the selected point pen to lighter to get a visual feedback that the selected point is selected.
    const QColor color = _TFPoint.second->brush().color();
    QPen tfPointPen(color);
    tfPointPen.setCosmetic(true);
    _TFPoint.second->setPen(tfPointPen);

    // Sets the new current TF.
    if(_subTF->m_tf != m_currentTF)
    {
        this->setCurrentTF(_subTF);
        // This action will call swapping method but m_currentTF is set by setCurrentTF, nothing will be done.
        this->setOutput(s_TF_OUTPUT, _subTF->m_tf);
    }
}

//-----------------------------------------------------------------------------

void SMultipleTF::mouseMoveOnPointEvent(SubTF* const _subTF, const ::fwRenderQt::data::Event& _event)
{
    // m_capturedTFPoint must be previously sets by
    // leftButtonClickOnPointEvent(SubTF* const, std::pair< Point2DType, QGraphicsEllipseItem* >&)
    SLM_ASSERT("The captured TF point must exist", m_capturedTFPoint);

    const auto pointIt = std::find(_subTF->m_TFPoints.begin(), _subTF->m_TFPoints.end(), *m_capturedTFPoint);
    SLM_ASSERT("The captured point is not found", pointIt != _subTF->m_TFPoints.end());

    // Gets the previous point of the TF.
    auto previousPoint = pointIt;
    if(*m_capturedTFPoint != _subTF->m_TFPoints.front())
    {
        --previousPoint;
    }

    // Gets the next point of the TF.
    auto nextPoint = pointIt;
    if(*m_capturedTFPoint != _subTF->m_TFPoints.back())
    {
        ++nextPoint;
    }

    // Gets position informations of the previous and the next point.
    const double previousPointXCoord = previousPoint->first.first;
    const double nextPointXCoord     = nextPoint->first.first;

    // Gets the actual mouse point coordinates.
    ::fwRenderQt::data::Coord newCoord = this->getScene2DRender()->mapToScene(_event.getCoord());

    // Clamps new y coord between -1 and 0.
    if(newCoord.getY() > 0)
    {
        newCoord.setY(0);
    }
    if(newCoord.getY() < -1)
    {
        newCoord.setY(-1);
    }

    // Clamps new x coord between the previous and the next one.
    const double delta = 1.;
    if(*m_capturedTFPoint == _subTF->m_TFPoints.front())
    {
        if(newCoord.getX() > nextPointXCoord)
        {
            newCoord.setX(nextPointXCoord - delta);
        }
    }
    else if(*m_capturedTFPoint == _subTF->m_TFPoints.back())
    {
        if(newCoord.getX() < previousPointXCoord)
        {
            newCoord.setX(previousPointXCoord + delta);
        }
    }
    else
    {
        if(newCoord.getX() < previousPointXCoord)
        {
            newCoord.setX(previousPointXCoord + delta);
        }
        else if(newCoord.getX() > nextPointXCoord)
        {
            newCoord.setX(nextPointXCoord - delta);
        }
    }

    // Moves the selected TF point by the difference between the old coordinates and the new ones.
    m_capturedTFPoint->second->moveBy(newCoord.getX() - m_capturedTFPoint->first.first,
                                      newCoord.getY() - m_capturedTFPoint->first.second);

    // Stores new coordinates to the captured one.
    m_capturedTFPoint->first.first  = newCoord.getX();
    m_capturedTFPoint->first.second = newCoord.getY();

    // Re-draw the current polygons.
    this->destroyTFPolygon(_subTF);
    this->createTFPolygon(_subTF);
    this->buildLayer();

    // Updates the TF with the new point position.
    size_t pointIndex = pointIt - _subTF->m_TFPoints.begin();

    // If the window is negative, the TF point list is reversed compared to the TF data.
    if(_subTF->m_tf->getWindow() < 0)
    {
        pointIndex = _subTF->m_TFPoints.size() - 1 - pointIndex;
    }

    const ::fwData::TransferFunction::sptr tf = _subTF->m_tf;
    const ::fwData::mt::ObjectWriteLock tfLock(tf);

    // Retrieves the TF point.
    ::fwData::TransferFunction::TFDataType tfData = tf->getTFData();
    auto tfDataIt = tfData.begin();
    for(unsigned i = 0; i < pointIndex; ++i)
    {
        tfDataIt++;
    }

    // Gets the TF point information
    ::fwData::TransferFunction::TFValueType oldTFValue = tfDataIt->first;
    ::fwData::TransferFunction::TFColor tfColor        = tfDataIt->second;

    // Gets window/level informations to change TF value from TF space to window/level space.
    const ::fwData::TransferFunction::TFValuePairType minMaxValues = tf->getMinMaxTFValues();
    const ::fwData::TransferFunction::TFValueType minWL            = tf->getWLMinMax().first;
    const ::fwData::TransferFunction::TFValueType window           = tf->getWindow();
    const ::fwData::TransferFunction::TFValueType width            = minMaxValues.second - minMaxValues.first;

    // Gets new window/level min max value in the window/level space.
    const double min = _subTF->m_TFPoints.begin()->first.first;
    const double max = _subTF->m_TFPoints.rbegin()->first.first;

    // Removes the old TF point.
    tf->eraseTFValue(oldTFValue);

    // Updates the color alpha channel.
    tfColor.a = std::abs(newCoord.getY());

    // Computes TF value from window/level space to TF space.
    ::fwData::TransferFunction::TFValueType newTFValue = newCoord.getX();
    newTFValue                                         = (newTFValue - minWL) / window;
    newTFValue                                         = (newTFValue * width) + minMaxValues.first;

    // Adds the new TF point.
    tf->addTFColor(newTFValue, tfColor);

    // Updates the window/level.
    if(window > 0)
    {
        tf->setWLMinMax(::fwData::TransferFunction::TFValuePairType(min, max));
    }
    else
    {
        tf->setWLMinMax(::fwData::TransferFunction::TFValuePairType(max, min));
    }

    // Sends the modification signal.
    const auto sig = tf->signal< ::fwData::TransferFunction::PointsModifiedSignalType >(
        ::fwData::TransferFunction::s_POINTS_MODIFIED_SIG);
    {
        const ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
        sig->asyncEmit();
    }
}

//-----------------------------------------------------------------------------

void SMultipleTF::leftButtonReleaseEvent()
{
    // Removes the hightlighting of the captured point.
    m_capturedTFPoint->second->setPen(m_pointsPen);
    m_capturedTFPoint = nullptr;
}

//-----------------------------------------------------------------------------

void SMultipleTF::rightButtonClickOnPointEvent(SubTF* const _subTF,
                                               std::pair< Point2DType, QGraphicsEllipseItem* >& _TFPoint)
{
    // Updates the TF.
    auto pointIt =
        std::find(_subTF->m_TFPoints.begin(), _subTF->m_TFPoints.end(), _TFPoint);
    SLM_ASSERT("The captured point is not found", pointIt != _subTF->m_TFPoints.end());
    size_t pointIndex = pointIt - _subTF->m_TFPoints.begin();

    const ::fwData::TransferFunction::sptr tf = _subTF->m_tf;
    {
        const ::fwData::mt::ObjectWriteLock tfLock(tf);

        // If the window is negative, the TF point list is reversed compared to the TF data.
        const double window = tf->getWindow();
        if(window <= 0)
        {
            pointIndex = _subTF->m_TFPoints.size() - 1 - pointIndex;
        }

        // Retrieves the TF point.
        ::fwData::TransferFunction::TFDataType tfData = tf->getTFData();
        auto tfDataIt = tfData.begin();
        for(unsigned i = 0; i < pointIndex; ++i)
        {
            tfDataIt++;
        }

        // Removes the TF point.
        const ::fwData::TransferFunction::TFValueType tfValue = tfDataIt->first;
        tf->eraseTFValue(tfValue);

        // Gets new window/level min max value in the window/level space.
        double min = _subTF->m_TFPoints.begin()->first.first;
        double max = _subTF->m_TFPoints.rbegin()->first.first;

        // If the removed point is the last or the first, the min max is wrong and need to be updated.
        if((pointIndex == 0 && window >= 0) || (pointIndex == _subTF->m_TFPoints.size()-1 && window < 0))
        {
            min = (_subTF->m_TFPoints.begin()+1)->first.first;
        }
        else if((pointIndex == _subTF->m_TFPoints.size()-1 && window >= 0) || (pointIndex == 0 && window < 0))
        {
            max = (_subTF->m_TFPoints.rbegin()+1)->first.first;
        }

        // Updates the window/level.
        if(window > 0)
        {
            tf->setWLMinMax(::fwData::TransferFunction::TFValuePairType(min, max));
        }
        else
        {
            tf->setWLMinMax(::fwData::TransferFunction::TFValuePairType(max, min));
        }
    }

    // Sends the modification signal.
    const auto sig = tf->signal< ::fwData::TransferFunction::PointsModifiedSignalType >(
        ::fwData::TransferFunction::s_POINTS_MODIFIED_SIG);
    {
        const ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
        sig->asyncEmit();
    }

    this->getScene2DRender()->getScene()->removeItem(pointIt->second);
    delete pointIt->second;
    _subTF->m_TFPoints.erase(pointIt);

    // Re-draw the current polygons.
    this->destroyTFPolygon(_subTF);
    this->createTFPolygon(_subTF);
    this->buildLayer();
}

//-----------------------------------------------------------------------------

void SMultipleTF::leftButtonDoubleClickOnPointEvent(SubTF* const _subTF,
                                                    std::pair< Point2DType, QGraphicsEllipseItem* >& _TFPoint)
{
    // Opens a QColorDialog with the selected circle color and the tf point alpha as default rgba color.
    QColor oldColor = _TFPoint.second->brush().color();
    oldColor.setAlphaF(-_TFPoint.first.second);

    QColor newColor = QColorDialog::getColor(oldColor,
                                             this->getScene2DRender()->getView(),
                                             QString("Choose the point color"),
                                             QColorDialog::ShowAlphaChannel);

    if(newColor.isValid())
    {
        // Updates the TF.
        auto pointIt =
            std::find(_subTF->m_TFPoints.begin(), _subTF->m_TFPoints.end(), _TFPoint);
        SLM_ASSERT("The captured point is not found", pointIt != _subTF->m_TFPoints.end());
        size_t pointIndex = pointIt - _subTF->m_TFPoints.begin();

        const ::fwData::TransferFunction::sptr tf = _subTF->m_tf;
        {
            const ::fwData::mt::ObjectWriteLock tfLock(tf);

            // If the window is negative, the TF point list is reversed compared to the TF data.
            if(tf->getWindow() < 0)
            {
                pointIndex = _subTF->m_TFPoints.size() - 1 - pointIndex;
            }

            // Retrieves the TF point.
            ::fwData::TransferFunction::TFDataType tfData = tf->getTFData();
            auto tfDataIt = tfData.begin();
            for(unsigned i = 0; i < pointIndex; ++i)
            {
                tfDataIt++;
            }

            // Removes the TF point.
            ::fwData::TransferFunction::TFValueType tfValue = tfDataIt->first;

            // Removes the old TF point.
            tf->eraseTFValue(tfValue);

            // Adds the new one with the new color.
            ::fwData::TransferFunction::TFColor tfColor(newColor.red()/255.,
                                                        newColor.green()/255.,
                                                        newColor.blue()/255.,
                                                        newColor.alpha()/255.);
            tf->addTFColor(tfValue, tfColor);
        }

        // Sends the modification signal.
        const auto sig = tf->signal< ::fwData::TransferFunction::PointsModifiedSignalType >(
            ::fwData::TransferFunction::s_POINTS_MODIFIED_SIG);
        {
            const ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
            sig->asyncEmit();
        }

        // Updates the displayed TF point.
        const double newYPos = -newColor.alpha()/255.;
        _TFPoint.second->moveBy(0.,  oldColor.alphaF() + newYPos);
        _TFPoint.first.second = newYPos;
        newColor.setAlpha(255);
        _TFPoint.second->setBrush(QBrush(newColor));

        // Re-draw the current polygons.
        this->destroyTFPolygon(_subTF);
        this->createTFPolygon(_subTF);
        this->buildLayer();
    }
}

//-----------------------------------------------------------------------------

void SMultipleTF::leftButtonDoubleClickEvent(const ::fwRenderQt::data::Event& _event)
{
    ::fwRenderQt::data::Coord newCoord = this->getScene2DRender()->mapToScene(_event.getCoord());

    // Clamps new y coord between -1 and 0.
    if(newCoord.getY() > 0)
    {
        newCoord.setY(0);
    }
    if(newCoord.getY() < -1)
    {
        newCoord.setY(-1);
    }

    // Finds the current subTF.
    SLM_ASSERT("The current TF mustn't be null", m_currentTF);
    SubTF* const currentSubTF = *(std::find_if(m_subTF.begin(), m_subTF.end(), [&](const SubTF* _subTF)
            {
                return _subTF->m_tf == m_currentTF;
            }));
    const ::fwData::TransferFunction::sptr tf = currentSubTF->m_tf;
    {
        const ::fwData::mt::ObjectWriteLock tfLock(tf);

        // Gets window/level informations to change TF value from TF space to window/level space.
        const ::fwData::TransferFunction::TFValuePairType minMaxValues = tf->getMinMaxTFValues();
        const ::fwData::TransferFunction::TFValueType minWL            = tf->getWLMinMax().first;
        const ::fwData::TransferFunction::TFValueType window           = tf->getWindow();
        const ::fwData::TransferFunction::TFValueType width            = minMaxValues.second - minMaxValues.first;

        // Computes TF value from window/level space to TF space.
        ::fwData::TransferFunction::TFValueType tfValue = newCoord.getX();
        tfValue                                         = (tfValue - minWL) / window;
        tfValue                                         = (tfValue * width) + minMaxValues.first;

        ::fwData::TransferFunction::TFColor newColor;

        // The new coord becomes the new first TF point, get the current first color in the list.
        if(newCoord.getX() < currentSubTF->m_TFPoints.front().first.first)
        {
            const QColor firstColor = currentSubTF->m_TFPoints.front().second->brush().color();
            newColor = ::fwData::TransferFunction::TFColor(firstColor.red()/255.,
                                                           firstColor.green()/255.,
                                                           firstColor.blue()/255.,
                                                           -newCoord.getY());

        }
        // The new coord becomes the new last TF point, get the current last color in the list.
        else if(newCoord.getX() > currentSubTF->m_TFPoints.back().first.first)
        {
            const QColor firstColor = currentSubTF->m_TFPoints.back().second->brush().color();
            newColor = ::fwData::TransferFunction::TFColor(firstColor.red()/255.,
                                                           firstColor.green()/255.,
                                                           firstColor.blue()/255.,
                                                           -newCoord.getY());

        }
        // Gets an interpolate color since the new point is between two ohers.
        else
        {
            newColor   = tf->getInterpolatedColor(tfValue);
            newColor.a = -newCoord.getY();
        }

        // Adds the new TF point.
        tf->addTFColor(tfValue, newColor);

        // Gets new window/level min max value in the window/level space.
        double min = currentSubTF->m_TFPoints.begin()->first.first;
        double max = currentSubTF->m_TFPoints.rbegin()->first.first;

        if(newCoord.getX() > max)
        {
            max = newCoord.getX();
        }
        else if(newCoord.getX() < min)
        {
            min = newCoord.getX();
        }

        // Updates the window/level.
        if(window > 0)
        {
            tf->setWLMinMax(::fwData::TransferFunction::TFValuePairType(min, max));
        }
        else
        {
            tf->setWLMinMax(::fwData::TransferFunction::TFValuePairType(max, min));
        }
    }

    // Sends the signal.
    const auto sig = tf->signal< ::fwData::TransferFunction::PointsModifiedSignalType >(
        ::fwData::TransferFunction::s_POINTS_MODIFIED_SIG);
    {
        const ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
        sig->asyncEmit();
    }

    // Re-draw all the scene.
    this->updating();
}

//-----------------------------------------------------------------------------

void SMultipleTF::midButtonClickEvent(const ::fwRenderQt::data::Event& _event)
{
    // Finds all subTF that match the clicked coord.
    std::vector< SubTF* > matchingSubTF = this->getMatchingSubTF(_event);

    // Checks if the current tf is in the matching list.
    const auto matchingIt = std::find_if(matchingSubTF.begin(), matchingSubTF.end(), [&](const SubTF* _subTF)
            {
                return _subTF->m_tf == m_currentTF;
            });

    // Moves the window/level only if the mouse if over the current TF.
    if(matchingIt != matchingSubTF.end())
    {
        this->getScene2DRender()->getView()->setCursor(Qt::ClosedHandCursor);
        ::fwRenderQt::data::Coord windowLevelCoord = this->getScene2DRender()->mapToScene(_event.getCoord());
        // Stores the level in window/level space and the window in screen space.
        m_capturedTF = std::make_pair(
            (*matchingIt)->m_tf,
            ::fwRenderQt::data::Coord(windowLevelCoord.getX(), _event.getCoord().getY()));
    }
}

//-----------------------------------------------------------------------------

void SMultipleTF::mouseMoveOnSubTFEvent(const ::fwRenderQt::data::Event& _event)
{
    // m_capturedTF must be previously sets by midButtonClickEvent(const ::fwRenderQt::data::Event& _event)
    SLM_ASSERT("The captured subTF must exist", m_capturedTF.first);

    const ::fwRenderQt::data::Coord windowLevelCoord = this->getScene2DRender()->mapToScene(_event.getCoord());

    // The level delta is in window/level space.
    const double levelDelta = windowLevelCoord.getX() - m_capturedTF.second.getX();

    // The window delta is in screen space.
    const double windowDelta = _event.getCoord().getY() - m_capturedTF.second.getY();

    // Updates the TF.
    const ::fwData::TransferFunction::sptr tf = m_capturedTF.first;
    {
        const ::fwData::mt::ObjectWriteLock tfLock(tf);
        tf->setWindow(tf->getWindow() - windowDelta);
        tf->setLevel(tf->getLevel() + levelDelta);

        // Sends the signal.
        const auto sig = tf->signal< ::fwData::TransferFunction::WindowingModifiedSignalType >(
            ::fwData::TransferFunction::s_WINDOWING_MODIFIED_SIG);
        {
            const ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
            sig->asyncEmit(tf->getWindow(), tf->getLevel());
        }
    }

    // Stores the level in window/level space and the window in screen space.
    m_capturedTF.second = ::fwRenderQt::data::Coord(windowLevelCoord.getX(), _event.getCoord().getY());

    // Re-draw all the scene.
    this->updating();
}

//-----------------------------------------------------------------------------

void SMultipleTF::midButtonReleaseEvent()
{
    this->getScene2DRender()->getView()->setCursor(Qt::ArrowCursor);
    m_capturedTF.first = nullptr;
}

//-----------------------------------------------------------------------------

void SMultipleTF::rightButtonCLickEvent(const ::fwRenderQt::data::Event& _event)
{
    // Finds all subTF that match the clicked coord.
    std::vector< SubTF* > matchingSubTF = this->getMatchingSubTF(_event);

    // Creates the menu.
    QAction* const trapezeAction = new QAction("Add trapeze");
    QObject::connect(trapezeAction, &QAction::triggered, this, [ = ]()
            {
                this->addTrapeze(_event);
            });
    QAction* const leftRampAction = new QAction("Add left ramp");
    QObject::connect(leftRampAction, &QAction::triggered, this, [ = ]()
            {
                this->addLeftRamp(_event);
            });
    QAction* const rightRampAction = new QAction("Add right ramp");
    QObject::connect(rightRampAction, &QAction::triggered, this, [ = ]()
            {
                this->addRightRamp(_event);
            });

    QMenu* const contextMenu = new QMenu();
    {
        // Checks if the current tf is in the matching list.
        const auto matchingIt = std::find_if(matchingSubTF.begin(), matchingSubTF.end(), [&](const SubTF* _subTF)
                {
                    return _subTF->m_tf == m_currentTF;
                });

        // Adds the delete action if the current TF is clicked.
        if(matchingIt != matchingSubTF.end())
        {
            const ::fwData::Composite::sptr tfPool = this->getInOut< ::fwData::Composite >(s_TF_POOL_INOUT);
            SLM_ASSERT("inout '" + s_TF_POOL_INOUT + "' does not exist.", tfPool);

            const ::fwData::mt::ObjectWriteLock tfPoolLock(tfPool);
            SLM_ASSERT("inout '" + s_TF_POOL_INOUT + "' must have at least on TF inside.", tfPool->size() > 0);

            // Adds the delete action if there is more than one TF.
            if(tfPool->size() > 1)
            {
                QAction* deleteAction = new QAction("Delete");
                QObject::connect(deleteAction, &QAction::triggered, this, &SMultipleTF::removeCurrenTF);
                contextMenu->addAction(deleteAction);
            }

            // Adds the clamp action.
            QAction* const clampAction = new QAction("Clamp");
            clampAction->setCheckable(true);
            clampAction->setChecked(m_currentTF->getIsClamped());
            QObject::connect(clampAction, &QAction::triggered, this, &SMultipleTF::clampCurrentTF);
            contextMenu->addAction(clampAction);

            // Adds the interpolation mode action.
            QAction* const linearAction = new QAction("Linear");
            linearAction->setCheckable(true);
            linearAction->setChecked(m_currentTF->getInterpolationMode() == ::fwData::TransferFunction::LINEAR);
            QObject::connect(linearAction, &QAction::triggered, this, &SMultipleTF::toggleLinearCurrentTF);
            contextMenu->addAction(linearAction);
        }
    }

    contextMenu->addAction(trapezeAction);
    contextMenu->addAction(leftRampAction);
    contextMenu->addAction(rightRampAction );

    // Opens the menu.
    contextMenu->exec(QCursor::pos());

    delete contextMenu;
}

//-----------------------------------------------------------------------------

void SMultipleTF::removeCurrenTF()
{
    const ::fwData::Composite::sptr tfPool = this->getInOut< ::fwData::Composite >(s_TF_POOL_INOUT);
    SLM_ASSERT("inout '" + s_TF_POOL_INOUT + "' does not exist.", tfPool);

    ::fwDataTools::helper::Composite compositeHelper(tfPool);
    ::fwData::TransferFunction::sptr tf;
    {
        const ::fwData::mt::ObjectWriteLock tfPoolLock(tfPool);
        SLM_ASSERT("inout '" + s_TF_POOL_INOUT + "' must have at least on TF inside.", tfPool->size() > 0);

        // Finds the key in the composite
        SLM_ASSERT("The current TF mustn't be null", m_currentTF);

        ::fwData::Composite::key_type key;
        for(::fwData::Composite::value_type poolElt : *tfPool)
        {
            if(poolElt.second == m_currentTF)
            {
                key = poolElt.first;
                break;
            }
        }

        // Removes the current TF.
        compositeHelper.remove(key);

        // Sets the new current TF.
        tf = ::fwData::TransferFunction::dynamicCast(tfPool->getContainer().begin()->second);
        SLM_ASSERT("inout '" + s_TF_POOL_INOUT + "' must contain only TF.", tf);
        m_currentTF = tf;
        // This action will call swapping method but m_currentTF is set, nothing will be done. The scene is draw after.
        this->setOutput(s_TF_OUTPUT, tf);
    }

    // Sends the signal.
    auto sig = tfPool->signal< ::fwData::Composite::RemovedObjectsSignalType >(
        ::fwData::Composite::s_REMOVED_OBJECTS_SIG);
    {
        const ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
        compositeHelper.notify();
    }

    // Re-draw all the scene here since swapping method as not been called.
    this->updating();
}

//-----------------------------------------------------------------------------

void SMultipleTF::clampCurrentTF(bool _clamp)
{
    const ::fwData::Composite::sptr tfPool = this->getInOut< ::fwData::Composite >(s_TF_POOL_INOUT);
    SLM_ASSERT("inout '" + s_TF_POOL_INOUT + "' does not exist.", tfPool);

    ::fwData::Composite::key_type key;
    ::fwData::TransferFunction::sptr tf;
    {
        const ::fwData::mt::ObjectReadLock tfPoolLock(tfPool);
        SLM_ASSERT("inout '" + s_TF_POOL_INOUT + "' must have at least on TF inside.", tfPool->size() > 0);

        // Finds the key in the composite
        SLM_ASSERT("The current TF mustn't be null", m_currentTF);
        for(::fwData::Composite::value_type poolElt : *tfPool)
        {
            if(poolElt.second == m_currentTF)
            {
                key = poolElt.first;
                break;
            }
        }

        // Updates the current TF.
        tf = ::fwData::TransferFunction::dynamicCast(tfPool->getContainer()[key]);
        SLM_ASSERT("inout '" + s_TF_POOL_INOUT + "' must contain only TF.", tf);
        tf->setIsClamped(_clamp);
    }

    // Sends the signal.
    const auto sig = tf->signal< ::fwData::TransferFunction::ModifiedSignalType >(
        ::fwData::TransferFunction::s_MODIFIED_SIG);
    {
        const ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
        sig->asyncEmit();
    }

    SubTF* currentSubTF = *(std::find_if(m_subTF.begin(), m_subTF.end(), [&](const SubTF* _subTF)
            {
                return _subTF->m_tf == m_currentTF;
            }));

    // Re-draw the current polygons.
    this->destroyTFPolygon(currentSubTF);
    this->createTFPolygon(currentSubTF);
    this->buildLayer();
}

//-----------------------------------------------------------------------------

void SMultipleTF::toggleLinearCurrentTF(bool _linear)
{
    const ::fwData::Composite::sptr tfPool = this->getInOut< ::fwData::Composite >(s_TF_POOL_INOUT);
    SLM_ASSERT("inout '" + s_TF_POOL_INOUT + "' does not exist.", tfPool);

    ::fwData::Composite::key_type key;
    ::fwData::TransferFunction::sptr tf;
    {
        const ::fwData::mt::ObjectReadLock tfPoolLock(tfPool);
        SLM_ASSERT("inout '" + s_TF_POOL_INOUT + "' must have at least on TF inside.", tfPool->size() > 0);

        // Finds the key in the composite
        SLM_ASSERT("The current TF mustn't be null", m_currentTF);
        for(::fwData::Composite::value_type poolElt : *tfPool)
        {
            if(poolElt.second == m_currentTF)
            {
                key = poolElt.first;
                break;
            }
        }

        // Updates the current TF.
        tf = ::fwData::TransferFunction::dynamicCast(tfPool->getContainer()[key]);
        SLM_ASSERT("inout '" + s_TF_POOL_INOUT + "' must contain only TF.", tf);
        tf->setInterpolationMode(_linear ? ::fwData::TransferFunction::LINEAR : ::fwData::TransferFunction::NEAREST);
    }

    // Sends the signal.
    const auto sig = tf->signal< ::fwData::TransferFunction::ModifiedSignalType >(
        ::fwData::TransferFunction::s_MODIFIED_SIG);
    {
        const ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
        sig->asyncEmit();
    }

    SubTF* currentSubTF = *(std::find_if(m_subTF.begin(), m_subTF.end(), [&](const SubTF* _subTF)
            {
                return _subTF->m_tf == m_currentTF;
            }));

    // Re-draw the current polygons.
    this->destroyTFPolygon(currentSubTF);
    this->createTFPolygon(currentSubTF);
    this->buildLayer();
}

//------------------------------------------------------------------------------

bool SMultipleTF::hasTFName(const std::string& _name) const
{
    const ::fwData::Composite::csptr tfPool = this->getInOut< ::fwData::Composite >(s_TF_POOL_INOUT);
    SLM_ASSERT("inout '" + s_TF_POOL_INOUT + "' does not exist.", tfPool);
    const ::fwData::mt::ObjectReadLock tfPoolsLock(tfPool);
    return tfPool->find(_name) != tfPool->end();
}

//-----------------------------------------------------------------------------

void SMultipleTF::addNewTF(const ::fwData::TransferFunction::sptr _tf)
{
    const ::fwData::Composite::sptr tfPool = this->getInOut< ::fwData::Composite >(s_TF_POOL_INOUT);
    SLM_ASSERT("inout '" + s_TF_POOL_INOUT + "' does not exist.", tfPool);
    const ::fwData::mt::ObjectReadLock tfPoolLock(tfPool);

    // Adds the new TF.
    ::fwDataTools::helper::Composite compositeHelper(tfPool);
    compositeHelper.add(_tf->getName(), _tf);

    // Sends the signal.
    auto sig = tfPool->signal< ::fwData::Composite::AddedObjectsSignalType >(
        ::fwData::Composite::s_ADDED_OBJECTS_SIG);
    {
        const ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
        compositeHelper.notify();
    }

    // Creates the new SubTF.
    SubTF* newSubTF = this->createSubTF(_tf, 0);

    // Pushs the subTF to the vector.
    m_subTF.push_back(newSubTF);

    // Updates the current TF.
    this->setCurrentTF(newSubTF);
    // This action will call swapping method but m_currentTF is set by setCurrentTF, nothing will be done.
    this->setOutput(s_TF_OUTPUT, _tf);
}

//-----------------------------------------------------------------------------

void SMultipleTF::addLeftRamp(const ::fwRenderQt::data::Event& _event)
{
    // Creates the new TF.
    ::fwData::Composite::KeyType name = "CT-LeftRamp_" + std::to_string(s_left_ramp_index_counter++);
    while(this->hasTFName(name))
    {
        name = "CT-LeftRamp_" + std::to_string(s_left_ramp_index_counter++);
    }
    const ::fwData::TransferFunction::sptr leftRamp = ::fwData::TransferFunction::New();
    leftRamp->setName(name);
    leftRamp->addTFColor(0.0, ::fwData::TransferFunction::TFColor(1.0, 1.0, 1.0, 1.0));
    leftRamp->addTFColor(1.0, ::fwData::TransferFunction::TFColor());
    leftRamp->setIsClamped(false);
    leftRamp->setWindow(500.);
    leftRamp->setLevel(50.);

    // Updates the window/level.
    ::fwRenderQt::data::Coord newCoord = this->getScene2DRender()->mapToScene(_event.getCoord());
    const ::fwData::TransferFunction::TFValueType window = leftRamp->getWindow();
    ::fwData::TransferFunction::TFValueType min = newCoord.getX() - window/2.;
    ::fwData::TransferFunction::TFValueType max = min + window;
    leftRamp->setWLMinMax(::fwData::TransferFunction::TFValuePairType(min, max));

    this->addNewTF(leftRamp);
}

//-----------------------------------------------------------------------------

void SMultipleTF::addRightRamp(const ::fwRenderQt::data::Event& _event)
{
    // Creates the new TF.
    ::fwData::Composite::KeyType name = "CT-RightRamp_" + std::to_string(s_left_ramp_index_counter++);
    while(this->hasTFName(name))
    {
        name = "CT-RightRamp_" + std::to_string(s_left_ramp_index_counter++);
    }
    const ::fwData::TransferFunction::sptr rightRamp = ::fwData::TransferFunction::New();
    rightRamp->setName(name);
    rightRamp->addTFColor(0.0, ::fwData::TransferFunction::TFColor());
    rightRamp->addTFColor(1.0, ::fwData::TransferFunction::TFColor(1.0, 1.0, 1.0, 1.0));
    rightRamp->setIsClamped(false);
    rightRamp->setWindow(500.);
    rightRamp->setLevel(50.);

    // Updates the window/level.
    ::fwRenderQt::data::Coord newCoord = this->getScene2DRender()->mapToScene(_event.getCoord());
    const ::fwData::TransferFunction::TFValueType window = rightRamp->getWindow();
    ::fwData::TransferFunction::TFValueType min = newCoord.getX() - window/2.;
    ::fwData::TransferFunction::TFValueType max = min + window;
    rightRamp->setWLMinMax(::fwData::TransferFunction::TFValuePairType(min, max));

    this->addNewTF(rightRamp);
}

//-----------------------------------------------------------------------------

void SMultipleTF::addTrapeze(const ::fwRenderQt::data::Event& _event)
{
    // Creates the new TF.
    ::fwData::Composite::KeyType name = "CT-Trapeze_" + std::to_string(s_left_ramp_index_counter++);
    while(this->hasTFName(name))
    {
        name = "CT-Trapeze_" + std::to_string(s_left_ramp_index_counter++);
    }
    const ::fwData::TransferFunction::sptr trapeze = ::fwData::TransferFunction::New();
    trapeze->setName(name);
    trapeze->addTFColor(0.0, ::fwData::TransferFunction::TFColor());
    trapeze->addTFColor(0.33, ::fwData::TransferFunction::TFColor(1.0, 1.0, 1.0, 1.0));
    trapeze->addTFColor(0.66, ::fwData::TransferFunction::TFColor(1.0, 1.0, 1.0, 1.0));
    trapeze->addTFColor(1.0, ::fwData::TransferFunction::TFColor());
    trapeze->setIsClamped(false);
    trapeze->setWindow(500.);
    trapeze->setLevel(50.);

    // Updates the window/level.
    ::fwRenderQt::data::Coord newCoord = this->getScene2DRender()->mapToScene(_event.getCoord());
    const ::fwData::TransferFunction::TFValueType window = trapeze->getWindow();
    ::fwData::TransferFunction::TFValueType min = newCoord.getX() - window/2.;
    ::fwData::TransferFunction::TFValueType max = min + window;
    trapeze->setWLMinMax(::fwData::TransferFunction::TFValuePairType(min, max));

    this->addNewTF(trapeze);
}

} // namespace adaptor
} // namespace scene2D
