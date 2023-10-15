/************************************************************************
 *
 * Copyright (C) 2020-2023 IRCAD France
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

#include "modules/viz/scene2d/adaptor/transfer_function.hpp"

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>
#include <core/profiling.hpp>

#include <data/container.hxx>

#include <viz/scene2d/data/InitQtPen.hpp>
#include <viz/scene2d/Scene2DGraphicsView.hpp>

#include <glm/common.hpp>

#include <QAction>
#include <QApplication>
#include <QColorDialog>
#include <QDesktopWidget>
#include <QGraphicsRectItem>
#include <QMenu>
#include <QTimer>

namespace sight::module::viz::scene2d::adaptor
{

static const core::com::slots::key_t UPDATE_TF_SLOT = "updateTF";

//-----------------------------------------------------------------------------

transfer_function::transfer_function() noexcept :
    m_eventFilter(new QTimer())
{
    new_slot(UPDATE_TF_SLOT, &transfer_function::updateTF, this);
}

//-----------------------------------------------------------------------------

transfer_function::~transfer_function() noexcept
{
    delete m_eventFilter;
}

//-----------------------------------------------------------------------------

void transfer_function::configuring()
{
    this->configureParams();

    const config_t tree = this->get_config();
    const auto config   = tree.get_child("config.<xmlattr>");

    const std::string polygonColor = config.get("lineColor", "#FFFFFF");
    sight::viz::scene2d::data::InitQtPen::setPenColor(m_polygonsPen, polygonColor);

    const std::string pointColor = config.get("pointColor", "#FFFFFF");
    sight::viz::scene2d::data::InitQtPen::setPenColor(m_pointsPen, pointColor);

    m_secondOpacity = config.get<float>("secondOpacity", m_secondOpacity);
    m_pointSize     = config.get<float>("pointSize", m_pointSize);
    m_interactive   = config.get<bool>("interactive", m_interactive);
}

//------------------------------------------------------------------------------

void transfer_function::starting()
{
    // Sets the current TF.
    {
        // Sets the current TF used to highlight it in the editor.
        const auto tf = m_tf.const_lock();
        if(tf && !tf->pieces().empty())
        {
            m_currentTF = tf->pieces().front();
        }
        else
        {
            SIGHT_FATAL("The current TF mustn't be null");
        }
    }

    // Adds the layer item to the scene.
    m_layer = new QGraphicsItemGroup();
    this->getScene2DRender()->getScene()->addItem(m_layer);

    m_pointsPen.setCosmetic(true);
    m_pointsPen.setWidthF(0);

    m_polygonsPen.setCosmetic(true);
    m_polygonsPen.setWidthF(0);

    // Creates all entities.
    this->updating();
}

//------------------------------------------------------------------------------

service::connections_t transfer_function::auto_connections() const
{
    connections_t connections;
    connections.push(s_VIEWPORT_INPUT, sight::viz::scene2d::data::Viewport::MODIFIED_SIG, service::slots::UPDATE);
    connections.push(s_CURRENT_TF_INOUT, data::object::MODIFIED_SIG, UPDATE_TF_SLOT);
    connections.push(s_CURRENT_TF_INOUT, data::transfer_function::WINDOWING_MODIFIED_SIG, service::slots::UPDATE);
    connections.push(s_CURRENT_TF_INOUT, data::transfer_function::POINTS_MODIFIED_SIG, service::slots::UPDATE);
    return connections;
}

//-----------------------------------------------------------------------------

void transfer_function::updating()
{
    // Clears old data.
    this->destroyTFPolygons();
    this->destroyTFPoints();
    this->releaseTFData();

    // Creates all TF.
    this->createTFPoints();
    this->createTFPolygons();

    // Builds the layer.
    this->buildLayer();
}

//-----------------------------------------------------------------------------

void transfer_function::stopping()
{
    this->destroyTFPolygons();
    this->destroyTFPoints();
    this->releaseTFData();
}

//-----------------------------------------------------------------------------

void transfer_function::releaseTFData()
{
    for(PieceView* const pieceView : m_pieceView)
    {
        delete pieceView;
    }

    m_pieceView.clear();
}

//-----------------------------------------------------------------------------

void transfer_function::createTFPoints()
{
    SIGHT_ASSERT("The current TF mustn't be null", m_currentTF);

    // Iterates over each TF to create pieceView.
    const auto tf = m_tf.const_lock();

    if(!tf->pieces().empty())
    {
        int zIndex = 0;
        for(const auto& tfData : tf->pieces())
        {
            // Sets the z-index of the current TF over all others.
            const bool isCurrent = m_currentTF == tfData;
            int index            = isCurrent ? static_cast<int>(tf->pieces().size()) : zIndex;

            // Pushs the pieceView to the vector.
            m_pieceView.push_back(this->createPieceView(tfData, index));
            ++zIndex;
        }
    }
}

//-----------------------------------------------------------------------------

void transfer_function::destroyTFPoints()
{
    // Removes TF point items from the scene and clear the TF point vector of each pieceView.
    for(PieceView* const pieceView : m_pieceView)
    {
        for(std::pair<vec2d_t, QGraphicsEllipseItem*>& tfPoint : pieceView->m_TFPoints)
        {
            this->getScene2DRender()->getScene()->removeItem(tfPoint.second);
            delete tfPoint.second;
        }

        pieceView->m_TFPoints.clear();
    }
}

//-----------------------------------------------------------------------------

transfer_function::PieceView* transfer_function::createPieceView(
    const data::transfer_function_piece::sptr _tf,
    int _zIndex
)
{
    auto viewport = m_viewport.lock();

    const double sceneWidth  = this->getScene2DRender()->getView()->width();
    const double sceneHeight = this->getScene2DRender()->getView()->height();

    // Computes point size in screen space and keep the smallest size (relatively to width or height).
    double pointSize = sceneWidth * m_pointSize;
    if(pointSize > sceneHeight * m_pointSize)
    {
        pointSize = sceneHeight * m_pointSize;
    }

    const double viewportWidth  = viewport->width();
    const double viewportHeight = viewport->height();

    // Computes point size from screen space to viewport space.
    const double pointWidth  = (viewportWidth * pointSize) / sceneWidth;
    const double pointHeight = (viewportHeight * pointSize) / sceneHeight;

    // Creates the pieceView and fill basic informations.
    auto* pieceView = new PieceView();
    pieceView->m_tf     = _tf;
    pieceView->m_zIndex = _zIndex;

    // Fills pieceView point with color points.
    for(const data::transfer_function::data_t::value_type& elt : *_tf)
    {
        // Computes TF value from TF space to window/level space.
        const data::transfer_function::value_t value = _tf->mapValueToWindow(elt.first);

        // Creates the color.
        const data::transfer_function::color_t color_t = elt.second;
        const vec2d_t valColor(value, std::min(color_t.a, 1.));
        vec2d_t coord = this->mapAdaptorToScene(valColor);

        // Builds a point item, set its color, pen and zIndex.
        auto* point = new QGraphicsEllipseItem(
            coord.x - pointWidth / 2,
            coord.y - pointHeight / 2,
            pointWidth,
            pointHeight
        );
        QColor color(static_cast<int>(color_t.r * 255),
                     static_cast<int>(color_t.g * 255),
                     static_cast<int>(color_t.b * 255));
        point->setBrush(QBrush(color));
        point->setPen(m_pointsPen);
        point->setZValue(pieceView->m_zIndex * 2 + 1);

        // Pushs it back into the point vector
        if(_tf->window() > 0)
        {
            pieceView->m_TFPoints.emplace_back(coord, point);
        }
        // If the window is negative, the TF is reversed and point must be sort in reverse.
        else
        {
            pieceView->m_TFPoints.insert(pieceView->m_TFPoints.begin(), std::make_pair(coord, point));
        }
    }

    return pieceView;
}

//-----------------------------------------------------------------------------

void transfer_function::createTFPolygons()
{
    // Iterates over all pieceView to create polygons.
    for(PieceView* const pieceView : m_pieceView)
    {
        this->createTFPolygon(pieceView);
    }
}

//-----------------------------------------------------------------------------

void transfer_function::createTFPolygon(PieceView* const _pieceView)
{
    if(m_interactive)
    {
        const auto viewport = m_viewport.lock();

        QVector<QPointF> position;
        QLinearGradient grad;

        const std::pair<vec2d_t, QGraphicsEllipseItem*>& firstTFPoint = _pieceView->m_TFPoints.front();
        const std::pair<vec2d_t, QGraphicsEllipseItem*>& lastTFPoint  = _pieceView->m_TFPoints.back();

        const QGraphicsEllipseItem* const firstPoint = firstTFPoint.second;

        double xBegin = firstTFPoint.first.x;
        double xEnd   = lastTFPoint.first.x;

        if(_pieceView->m_tf->clamped())
        {
            position.append(QPointF(xBegin, 0));
        }
        else
        {
            if(xBegin > viewport->x())
            {
                xBegin = viewport->x() - 10;
                position.append(QPointF(xBegin, 0));
                position.append(QPointF(xBegin, firstTFPoint.first.y));
            }
            else
            {
                position.append(QPointF(xBegin, 0));
            }

            if(xEnd < viewport->x() + viewport->width())
            {
                xEnd = viewport->x() + viewport->width() + 10;
            }
        }

        grad.setColorAt(0, firstPoint->brush().color());

        grad.setStart(xBegin, 0);
        grad.setFinalStop(xEnd, 0);

        const double distanceMax = xEnd - xBegin;

        // Iterates on TF points vector to add line and polygon items to the polygons vector.
        if(_pieceView->m_tf->interpolationMode() == data::transfer_function::InterpolationMode::LINEAR)
        {
            sight::module::viz::scene2d::adaptor::transfer_function::buildLinearPolygons(
                _pieceView,
                position,
                grad,
                distanceMax
            );
        }
        else
        {
            sight::module::viz::scene2d::adaptor::transfer_function::buildNearestPolygons(
                _pieceView,
                position,
                grad,
                distanceMax
            );
        }

        if(!_pieceView->m_tf->clamped())
        {
            if(xEnd == viewport->x() + viewport->width() + 10)
            {
                position.append(QPointF(xEnd, lastTFPoint.first.y));
            }

            const double lastPointX = lastTFPoint.first.x;
            grad.setColorAt((lastPointX - xBegin) / distanceMax, lastTFPoint.second->brush().color());
        }

        position.append(QPointF(xEnd, 0));
        grad.setColorAt(1, lastTFPoint.second->brush().color());

        auto* const poly = new QGraphicsPolygonItem(QPolygonF(position));
        // Sets gradient, opacity and pen to the polygon
        poly->setBrush(QBrush(grad));
        poly->setPen(m_polygonsPen);
        poly->setZValue(_pieceView->m_zIndex * 2);

        // If the z-index is the highest, it's the current one.
        if(static_cast<std::size_t>(_pieceView->m_zIndex) == m_pieceView.size())
        {
            poly->setOpacity(m_opacity);
        }
        else
        {
            poly->setOpacity(m_secondOpacity);
        }

        // Pushs the polygon back into the polygons vector
        _pieceView->m_TFPolygons.push_back(poly);
    }
    else
    {
        const auto viewport = m_viewport.lock();

        auto* const view           = this->getScene2DRender()->getView();
        const double viewportWidth = viewport->width();
        const double step          = viewportWidth / view->width();
        const uint size            = static_cast<uint>(viewportWidth / step);
        double f                   = viewport->left();

        for(uint i = 0 ; i < size ; ++i)
        {
            const auto color = _pieceView->m_tf->sample(f);

            vec2d_t pt = this->mapAdaptorToScene({f, color.a});

            QColor qColor;
            qColor.setRgbF(color.r, color.g, color.b, 1.0);
            QBrush brush = QBrush(qColor);

            auto* rect = new QGraphicsRectItem(pt.x, 0.0, step, pt.y);
            rect->setBrush(brush);
            rect->setPen(Qt::NoPen);

            _pieceView->m_TFPolygons.push_back(rect);

            f += step;
        }
    }
}

//-----------------------------------------------------------------------------

void transfer_function::destroyTFPolygons()
{
    // Removes polygon items from the scene and clear the polygon vector.
    for(PieceView* const pieceView : m_pieceView)
    {
        this->destroyTFPolygon(pieceView);
    }
}

//-----------------------------------------------------------------------------

void transfer_function::destroyTFPolygon(PieceView* _pieceView)
{
    for(auto& poly : _pieceView->m_TFPolygons)
    {
        this->getScene2DRender()->getScene()->removeItem(poly);
        delete poly;
    }

    // Removes polygon items from the scene and clear the polygon vector.
    _pieceView->m_TFPolygons.clear();
}

//-----------------------------------------------------------------------------

void transfer_function::buildLinearPolygons(
    PieceView* const _pieceView,
    QVector<QPointF>& _position,
    QLinearGradient& _grad,
    double _distanceMax
)
{
    const std::vector<std::pair<vec2d_t, QGraphicsEllipseItem*> >& tfPoints = _pieceView->m_TFPoints;
    for(auto tfPointIt = tfPoints.cbegin() ; tfPointIt != tfPoints.cend() - 1 ; ++tfPointIt)
    {
        const QPointF p1(tfPointIt->first.x, tfPointIt->first.y);
        const QPointF p2((tfPointIt + 1)->first.x, (tfPointIt + 1)->first.y);

        _position.append(p1);
        _position.append(p2);

        // Builds the gradient
        _grad.setColorAt((p1.x() - _position[0].x()) / _distanceMax, (tfPointIt->second)->brush().color());
    }
}

//-----------------------------------------------------------------------------

void transfer_function::buildNearestPolygons(
    PieceView* const _pieceView,
    QVector<QPointF>& _position,
    QLinearGradient& _grad,
    double _distanceMax
)
{
    const std::vector<std::pair<vec2d_t, QGraphicsEllipseItem*> >& tfPoints = _pieceView->m_TFPoints;
    for(auto tfPointIt = tfPoints.cbegin() ; tfPointIt != tfPoints.cend() - 1 ; ++tfPointIt)
    {
        const QPointF p1(tfPointIt->first.x, tfPointIt->first.y);
        const QPointF p4((tfPointIt + 1)->first.x, (tfPointIt + 1)->first.y);

        const QPointF p2(p1.x() + (p4.x() - p1.x()) / 2., p1.y());
        const QPointF p3(p2.x(), p4.y());

        _position.append(p1);
        _position.append(p2);
        _position.append(p3);
        _position.append(p4);

        const double d1 = (p1.x() - _position[0].x()) / _distanceMax;
        const double d2 = (p2.x() - _position[0].x()) / _distanceMax;
        const double d3 = d2 + std::numeric_limits<double>::epsilon();
        const double d4 = (p4.x() - _position[0].x()) / _distanceMax;

        const QColor c1 = (tfPointIt->second)->brush().color();
        const QColor c4 = ((tfPointIt + 1)->second)->brush().color();

        _grad.setColorAt(d1, c1);
        _grad.setColorAt(d2, c1);
        _grad.setColorAt(d3, c4);
        _grad.setColorAt(d4, c4);
    }
}

//-----------------------------------------------------------------------------

void transfer_function::buildLayer()
{
    // Adds graphics items vectors to the layer.
    for(PieceView* const pieceView : m_pieceView)
    {
        for(std::pair<vec2d_t, QGraphicsEllipseItem*>& tfPoint : pieceView->m_TFPoints)
        {
            m_layer->addToGroup(tfPoint.second);
        }

        for(auto& poly : pieceView->m_TFPolygons)
        {
            m_layer->addToGroup(poly);
        }
    }

    // Adjusts the layer's position and zValue depending on the associated axis.
    m_layer->setPos(m_xAxis->getOrigin(), m_yAxis->getOrigin());
    m_layer->setZValue(m_zValue);
}

//-----------------------------------------------------------------------------

void transfer_function::setCurrentTF(PieceView* const _pieceView)
{
    // Sets the new current TF.
    SIGHT_ASSERT("The current TF mustn't be null", m_currentTF);

    // Find the old pieceView.
    PieceView* const currentPieceView = *(std::find_if(
                                              m_pieceView.begin(),
                                              m_pieceView.end(),
                                              [&](const PieceView* _pieceView)
        {
            return _pieceView->m_tf == m_currentTF;
        }));

    // Changes the current pieceView.
    m_currentTF = _pieceView->m_tf;

    // Recomputes z-index and set the z-index of the selected TF over all others.
    int zIndex = 0;
    for(PieceView* pieceView : m_pieceView)
    {
        pieceView->m_zIndex = pieceView->m_tf == m_currentTF ? static_cast<int>(m_pieceView.size()) : zIndex;
        for(std::pair<vec2d_t, QGraphicsEllipseItem*>& point : pieceView->m_TFPoints)
        {
            point.second->setZValue(pieceView->m_zIndex * 2 + 1);
        }

        ++zIndex;
    }

    // Re-draw polygons since the current TF as changed.
    this->destroyTFPolygon(currentPieceView);
    this->createTFPolygon(currentPieceView);
    this->destroyTFPolygon(_pieceView);
    this->createTFPolygon(_pieceView);
    this->buildLayer();
}

//-----------------------------------------------------------------------------

std::vector<transfer_function::PieceView*> transfer_function::getMatchingPieceView(
    const sight::viz::scene2d::data::Event& _event
) const
{
    // Finds all pieceView that match the clicked coord.
    std::vector<PieceView*> matchingPieceView;
    const QPoint scenePos = QPoint(
        static_cast<int>(_event.getCoord().x),
        static_cast<int>(_event.getCoord().y)
    );
    QList<QGraphicsItem*> items = this->getScene2DRender()->getView()->items(scenePos);

    // Fills the pieceView vector with clicked ones.
    for(PieceView* const pieceView : m_pieceView)
    {
        // Checks if a polygon is clicked.
        if(items.indexOf(pieceView->m_TFPolygons.front()) >= 0)
        {
            matchingPieceView.push_back(pieceView);
        }
    }

    return matchingPieceView;
}

//-----------------------------------------------------------------------------

void transfer_function::processInteraction(sight::viz::scene2d::data::Event& _event)
{
    if(!m_interactive)
    {
        return;
    }

    SIGHT_ASSERT("The current TF mustn't be null", m_currentTF);

    // If it's a resize event, all the scene must be recomputed.
    if(_event.getType() == sight::viz::scene2d::data::Event::Resize)
    {
        this->updating();
        _event.setAccepted(true);
        return;
    }

    // If a point as already been captured.
    if(m_capturedTFPoint != nullptr)
    {
        if(_event.getButton() == sight::viz::scene2d::data::Event::LeftButton
           && _event.getType() == sight::viz::scene2d::data::Event::MouseButtonRelease)
        {
            // Releases capture point.
            this->leftButtonReleaseEvent();
            _event.setAccepted(true);
            return;
        }
    }

    // If a pieceView as already been captured.
    if(m_capturedTF.first)
    {
        if(_event.getType() == sight::viz::scene2d::data::Event::MouseMove)
        {
            // Changes the pieceView level.
            this->mouseMoveOnPieceViewEvent(_event);
            _event.setAccepted(true);
            return;
        }

        if(_event.getButton() == sight::viz::scene2d::data::Event::MidButton
           && _event.getType() == sight::viz::scene2d::data::Event::MouseButtonRelease)
        {
            // Releases capture pieceView.
            this->midButtonReleaseEvent();
            _event.setAccepted(true);
            return;
        }
    }

    const QPoint scenePos = QPoint(
        static_cast<int>(_event.getCoord().x),
        static_cast<int>(_event.getCoord().y)
    );
    QList<QGraphicsItem*> items = this->getScene2DRender()->getView()->items(scenePos);

    // Checks if a point is clicked.
    for(PieceView* const pieceView : m_pieceView)
    {
        for(std::pair<vec2d_t, QGraphicsEllipseItem*>& tfPoint : pieceView->m_TFPoints)
        {
            // If a point has already been captured.
            if(m_capturedTFPoint == &tfPoint)
            {
                if(_event.getType() == sight::viz::scene2d::data::Event::MouseMove)
                {
                    // Moves the captured point.
                    this->mouseMoveOnPointEvent(pieceView, _event);
                    _event.setAccepted(true);
                    return;
                }
            }
            else if(items.indexOf(tfPoint.second) >= 0)
            {
                // If there is a double click on a point, open a color dialog.
                if(_event.getButton() == sight::viz::scene2d::data::Event::LeftButton
                   && _event.getType() == sight::viz::scene2d::data::Event::MouseButtonDoubleClick)
                {
                    this->leftButtonDoubleClickOnPointEvent(pieceView, tfPoint);
                    _event.setAccepted(true);
                    return;
                }

                // If left button is pressed on a point, set the TF as current.
                if(_event.getButton() == sight::viz::scene2d::data::Event::LeftButton
                   && _event.getType() == sight::viz::scene2d::data::Event::MouseButtonPress)
                {
                    this->leftButtonClickOnPointEvent(pieceView, tfPoint);
                    _event.setAccepted(true);
                    return;
                }

                // If right button is pressed on a point, remove it.
                if(_event.getButton() == sight::viz::scene2d::data::Event::RightButton
                   && _event.getType() == sight::viz::scene2d::data::Event::MouseButtonPress
                   && pieceView->m_tf == m_currentTF && pieceView->m_TFPoints.size() > 2)
                {
                    this->rightButtonClickOnPointEvent(pieceView, tfPoint);
                    _event.setAccepted(true);
                    return;
                }
            }
        }
    }

    // Adds a new TF point.
    if(_event.getButton() == sight::viz::scene2d::data::Event::LeftButton
       && _event.getType() == sight::viz::scene2d::data::Event::MouseButtonDoubleClick)
    {
        // Cancel the previous single click interaction.
        m_eventFilter->stop();
        this->leftButtonDoubleClickEvent(_event);
        _event.setAccepted(true);
        return;
    }

    // If left button is pressed, set the nearest TF as current.
    if(_event.getButton() == sight::viz::scene2d::data::Event::LeftButton
       && _event.getType() == sight::viz::scene2d::data::Event::MouseButtonPress)
    {
        // Cancel the previous event if it's needed.
        m_eventFilter->stop();
        delete m_eventFilter;

        // Validates the event in 250ms, this allow to the double click event to cancel the interaction.
        m_eventFilter = new QTimer();
        QTimer::connect(
            m_eventFilter,
            &QTimer::timeout,
            this,
            [_event, this]()
            {
                this->leftButtonClickEvent(_event);
            });
        m_eventFilter->setSingleShot(true);
        m_eventFilter->start(250);
        // _event.setAccepted(true);
        return;
    }

    // If middle button is pressed, select the current TF to adjust the window/level.
    if(_event.getButton() == sight::viz::scene2d::data::Event::MidButton
       && _event.getType() == sight::viz::scene2d::data::Event::MouseButtonPress)
    {
        this->midButtonClickEvent(_event);
        return;
    }

    // If right button is pressed, open a context menu to manage multiple actions.
    if(_event.getButton() == sight::viz::scene2d::data::Event::RightButton
       && _event.getType() == sight::viz::scene2d::data::Event::MouseButtonPress)
    {
        this->rightButtonCLickEvent(_event);
        _event.setAccepted(true);
        return;
    }

    // If the middle button wheel moves, change the whole pieceView opacity.
    if(_event.getButton() == sight::viz::scene2d::data::Event::NoButton
       && (_event.getType() == sight::viz::scene2d::data::Event::MouseWheelDown
           || _event.getType() == sight::viz::scene2d::data::Event::MouseWheelUp))
    {
        this->midButtonWheelMoveEvent(_event);
        return;
    }
}

//-----------------------------------------------------------------------------

void transfer_function::leftButtonClickEvent(const sight::viz::scene2d::data::Event& _event)
{
    SIGHT_ASSERT("Interactions disabled, this code should not reached", m_interactive);

    std::vector<PieceView*> matchingPieceView = this->getMatchingPieceView(_event);

    if(!matchingPieceView.empty())
    {
        if(matchingPieceView.size() == 1)
        {
            if(matchingPieceView[0]->m_tf != m_currentTF)
            {
                this->setCurrentTF(matchingPieceView[0]);
            }
        }
        // Finds the closest one.
        else
        {
            const auto tf = m_tf.lock();

            sight::viz::scene2d::vec2d_t clickCoord = this->getScene2DRender()->mapToScene(_event.getCoord());

            float closestDistance          = std::numeric_limits<float>::max();
            PieceView* newCurrentPieceView = nullptr;
            for(PieceView* pieceView : matchingPieceView)
            {
                // Finds nearest position of the iterate pieceView.
                float localClosestDistance          = std::numeric_limits<float>::max();
                PieceView* localNewCurrentPieceView = nullptr;
                for(std::size_t i = 0 ; i <= pieceView->m_TFPoints.size() ; ++i)
                {
                    vec2d_t tfPoint1;
                    vec2d_t tfPoint2;

                    // Creates the first fictional TF point.
                    if(i == 0)
                    {
                        tfPoint2 = pieceView->m_TFPoints[i].first;
                        if(pieceView->m_tf->clamped())
                        {
                            // The first point is the same a the real first but with a zero alpha channel.
                            tfPoint1 = vec2d_t(tfPoint2.x, 0);
                        }
                        else
                        {
                            // The first point is the same a the real but with an infinite lower value.

                            const auto viewport = m_viewport.lock();
                            tfPoint1 = vec2d_t(viewport->x(), tfPoint2.y);
                        }
                    }
                    // Creates the last fictional TF point.
                    else if(i == pieceView->m_TFPoints.size())
                    {
                        tfPoint1 = pieceView->m_TFPoints[i - 1].first;
                        if(pieceView->m_tf->clamped())
                        {
                            // The last point is the same a the real last but with a zero alpha channel.
                            tfPoint2 = vec2d_t(tfPoint1.x, 0);
                        }
                        else
                        {
                            // The last point is the same a the real but with an infinite upper value.
                            const auto viewport = m_viewport.lock();
                            tfPoint2 = vec2d_t(viewport->x() + viewport->width(), tfPoint1.y);
                        }
                    }
                    // Retrieves two TF points.
                    else
                    {
                        tfPoint1 = pieceView->m_TFPoints[i - 1].first;
                        tfPoint2 = pieceView->m_TFPoints[i].first;
                    }

                    // Gets a line/point projection.
                    const QLineF line(tfPoint1.x, tfPoint1.y, tfPoint2.x, tfPoint2.y);

                    QLineF perpendicLine(clickCoord.x, clickCoord.y, clickCoord.x, 0);
                    perpendicLine.setAngle(90.F + line.angle());

                    QPointF intersectPoint;
                    line.intersects(perpendicLine, &intersectPoint);

                    const QVector2D origin(static_cast<float>(clickCoord.x),
                                           static_cast<float>(clickCoord.y));

                    float distance = std::numeric_limits<float>::max();

                    // Checks if the intersection belong the segment.
                    if(intersectPoint.x() >= tfPoint1.x && intersectPoint.x() <= tfPoint2.x)
                    {
                        const QVector2D intersect(intersectPoint);
                        const QVector2D projection = origin - intersect;

                        distance = projection.length();
                    }
                    // Elses the lower distance is between the point and one of the segment edge.
                    else
                    {
                        const QVector2D firstLine(static_cast<float>(clickCoord.x - tfPoint1.x),
                                                  static_cast<float>(clickCoord.y - tfPoint1.y));
                        const QVector2D secondLine(static_cast<float>(clickCoord.x - tfPoint2.x),
                                                   static_cast<float>(clickCoord.y - tfPoint2.y));

                        distance = firstLine.length();
                        if(secondLine.length() < distance)
                        {
                            distance = secondLine.length();
                        }
                    }

                    if(distance < localClosestDistance)
                    {
                        localClosestDistance     = distance;
                        localNewCurrentPieceView = pieceView;
                    }
                }

                if(localClosestDistance < closestDistance)
                {
                    closestDistance     = localClosestDistance;
                    newCurrentPieceView = localNewCurrentPieceView;
                }
            }

            SIGHT_ASSERT("newCurrentPieceView is null", newCurrentPieceView != nullptr);

            // Sets the new current TF.
            if(newCurrentPieceView->m_tf != m_currentTF)
            {
                this->setCurrentTF(newCurrentPieceView);
            }
        }
    }
}

//-----------------------------------------------------------------------------

void transfer_function::leftButtonClickOnPointEvent(
    PieceView* const _pieceView,
    std::pair<vec2d_t,
              QGraphicsEllipseItem*>& TFPoint
)
{
    SIGHT_ASSERT("Interactions disabled, this code should not reached", m_interactive);

    // Stores the captured TF point in case it's moved.
    m_capturedTFPoint = &TFPoint;

    // Sets the selected point pen to lighter to get a visual feedback that the selected point is selected.
    const QColor color = TFPoint.second->brush().color();
    QPen tfPointPen(color);
    tfPointPen.setCosmetic(true);
    TFPoint.second->setPen(tfPointPen);

    // Sets the new current TF.
    if(_pieceView->m_tf != m_currentTF)
    {
        this->setCurrentTF(_pieceView);
    }
}

//-----------------------------------------------------------------------------

void transfer_function::mouseMoveOnPointEvent(
    PieceView* const _pieceView,
    const sight::viz::scene2d::data::Event& _event
)
{
    SIGHT_ASSERT("Interactions disabled, this code should not reached", m_interactive);

    const auto tf = m_tf.lock();

    // m_capturedTFPoint must be previously sets by
    // leftButtonClickOnPointEvent(PieceView* const, std::pair< vec2d_t, QGraphicsEllipseItem* >&)
    SIGHT_ASSERT("The captured TF point must exist", m_capturedTFPoint);

    const auto pointIt = std::find(_pieceView->m_TFPoints.begin(), _pieceView->m_TFPoints.end(), *m_capturedTFPoint);
    SIGHT_ASSERT("The captured point is not found", pointIt != _pieceView->m_TFPoints.end());

    // Gets the previous point of the TF.
    auto previousPoint = pointIt;
    if(*m_capturedTFPoint != _pieceView->m_TFPoints.front())
    {
        --previousPoint;
    }

    // Gets the next point of the TF.
    auto nextPoint = pointIt;
    if(*m_capturedTFPoint != _pieceView->m_TFPoints.back())
    {
        ++nextPoint;
    }

    // Gets position informations of the previous and the next point.
    const double previousPointXCoord = previousPoint->first.x;
    const double nextPointXCoord     = nextPoint->first.x;

    // Gets the actual mouse point coordinates.
    sight::viz::scene2d::vec2d_t newCoord = this->getScene2DRender()->mapToScene(_event.getCoord(), true);

    // Clamps new y coord between -1 and 0.
    newCoord.y = std::clamp(newCoord.y, -1., 0.);

    // Clamps new coord in the viewport.
    {
        auto viewport = m_viewport.lock();
        newCoord = glm::clamp(
            newCoord,
            glm::dvec2(viewport->left(), viewport->top()),
            glm::dvec2(viewport->right(), viewport->bottom())
        );
    }

    // Clamps new x coord between the previous and the next one.
    const double delta = 1.;
    if(*m_capturedTFPoint == _pieceView->m_TFPoints.front())
    {
        if(newCoord.x >= nextPointXCoord)
        {
            newCoord.x = nextPointXCoord - delta;
        }
    }
    else if(*m_capturedTFPoint == _pieceView->m_TFPoints.back())
    {
        if(newCoord.x <= previousPointXCoord)
        {
            newCoord.x = previousPointXCoord + delta;
        }
    }
    else
    {
        if(newCoord.x <= previousPointXCoord)
        {
            newCoord.x = previousPointXCoord + delta;
        }
        else if(newCoord.x >= nextPointXCoord)
        {
            newCoord.x = nextPointXCoord - delta;
        }
    }

    // Moves the selected TF point by the difference between the old coordinates and the new ones.
    m_capturedTFPoint->second->moveBy(
        newCoord.x - m_capturedTFPoint->first.x,
        newCoord.y - m_capturedTFPoint->first.y
    );

    // Stores new coordinates to the captured one.
    m_capturedTFPoint->first.x = newCoord.x;
    m_capturedTFPoint->first.y = newCoord.y;

    // Re-draw the current polygons.
    this->destroyTFPolygon(_pieceView);
    this->createTFPolygon(_pieceView);
    this->buildLayer();

    // Updates the TF with the new point position.
    std::size_t pointIndex = std::size_t(pointIt - _pieceView->m_TFPoints.begin());

    // If the window is negative, the TF point list is reversed compared to the TF data.
    if(_pieceView->m_tf->window() < 0)
    {
        pointIndex = _pieceView->m_TFPoints.size() - 1 - pointIndex;
    }

    const data::transfer_function_piece::sptr tfPiece = _pieceView->m_tf;

    // Retrieves the TF point.
    auto tfDataIt = tfPiece->cbegin();
    for(unsigned i = 0 ; i < pointIndex ; ++i)
    {
        tfDataIt++;
    }

    // Gets the TF point information
    data::transfer_function::value_t oldTFValue = tfDataIt->first;
    data::transfer_function::color_t color      = tfDataIt->second;

    // Gets new window/level min max value in the window/level space.
    const double min = _pieceView->m_TFPoints.begin()->first.x;
    const double max = _pieceView->m_TFPoints.rbegin()->first.x;

    // Computes TF value from window/level space to TF space.
    const data::transfer_function::value_t newTFValue = tfPiece->mapValueFromWindow(newCoord.x);

    // Removes the old TF point.
    tfPiece->erase(oldTFValue);

    // Updates the color alpha channel.
    color.a = std::abs(newCoord.y);

    // Adds the new TF point.
    tfPiece->insert({newTFValue, color});

    // Updates the window/level.
    if(tfPiece->window() > 0)
    {
        tfPiece->setWindowMinMax(data::transfer_function::min_max_t(min, max));
    }
    else
    {
        tfPiece->setWindowMinMax(data::transfer_function::min_max_t(max, min));
    }

    tf->fitWindow();

    pointsModified(*tf);
}

//-----------------------------------------------------------------------------

void transfer_function::leftButtonReleaseEvent()
{
    SIGHT_ASSERT("Interactions disabled, this code should not reached", m_interactive);

    // Removes the hightlighting of the captured point.
    m_capturedTFPoint->second->setPen(m_pointsPen);
    m_capturedTFPoint = nullptr;
}

//-----------------------------------------------------------------------------

void transfer_function::rightButtonClickOnPointEvent(
    PieceView* const _pieceView,
    std::pair<vec2d_t, QGraphicsEllipseItem*>& TFPoint
)
{
    SIGHT_ASSERT("Interactions disabled, this code should not reached", m_interactive);

    const auto tf = m_tf.lock();

    // Updates the TF.
    auto pointIt = std::find(_pieceView->m_TFPoints.begin(), _pieceView->m_TFPoints.end(), TFPoint);
    SIGHT_ASSERT("The captured point is not found", pointIt != _pieceView->m_TFPoints.end());
    std::size_t pointIndex = std::size_t(pointIt - _pieceView->m_TFPoints.begin());

    const data::transfer_function_piece::sptr tfPiece = _pieceView->m_tf;
    {
        // If the window is negative, the TF point list is reversed compared to the TF data.
        const double window = tfPiece->window();
        if(window <= 0)
        {
            pointIndex = _pieceView->m_TFPoints.size() - 1 - pointIndex;
        }

        // Retrieves the TF point.
        auto tfDataIt = tfPiece->cbegin();
        for(unsigned i = 0 ; i < pointIndex ; ++i)
        {
            tfDataIt++;
        }

        // Removes the TF point.
        const data::transfer_function::value_t tfValue = tfDataIt->first;
        tfPiece->erase(tfValue);

        // Gets new window/level min max value in the window/level space.
        double min = _pieceView->m_TFPoints.begin()->first.x;
        double max = _pieceView->m_TFPoints.rbegin()->first.x;

        // If the removed point is the last or the first, the min max is wrong and need to be updated.
        if((pointIndex == 0 && window >= 0) || (pointIndex == _pieceView->m_TFPoints.size() - 1 && window < 0))
        {
            min = (_pieceView->m_TFPoints.begin() + 1)->first.x;
        }
        else if((pointIndex == _pieceView->m_TFPoints.size() - 1 && window >= 0) || (pointIndex == 0 && window < 0))
        {
            max = (_pieceView->m_TFPoints.rbegin() + 1)->first.x;
        }

        // Updates the window/level.
        if(window > 0)
        {
            tfPiece->setWindowMinMax(data::transfer_function::min_max_t(min, max));
        }
        else
        {
            tfPiece->setWindowMinMax(data::transfer_function::min_max_t(max, min));
        }
    }

    tf->fitWindow();

    pointsModified(*tf);

    this->getScene2DRender()->getScene()->removeItem(pointIt->second);
    delete pointIt->second;
    _pieceView->m_TFPoints.erase(pointIt);

    // Re-draw the current polygons.
    this->destroyTFPolygon(_pieceView);
    this->createTFPolygon(_pieceView);
    this->buildLayer();
}

//-----------------------------------------------------------------------------

void transfer_function::leftButtonDoubleClickOnPointEvent(
    PieceView* const _pieceView,
    std::pair<vec2d_t, QGraphicsEllipseItem*>& TFPoint
)
{
    SIGHT_ASSERT("Interactions disabled, this code should not reached", m_interactive);

    const auto tf = m_tf.lock();

    // Opens a QColorDialog with the selected circle color and the tf point alpha as default rgba color.
    QColor oldColor = TFPoint.second->brush().color();
    oldColor.setAlphaF(-TFPoint.first.y);

    QColor newColor = QColorDialog::getColor(
        oldColor,
        this->getScene2DRender()->getView(),
        QString("Choose the point color"),
        QColorDialog::ShowAlphaChannel
    );

    if(newColor.isValid())
    {
        // Updates the TF.
        auto pointIt =
            std::find(_pieceView->m_TFPoints.begin(), _pieceView->m_TFPoints.end(), TFPoint);
        SIGHT_ASSERT("The captured point is not found", pointIt != _pieceView->m_TFPoints.end());
        std::size_t pointIndex = std::size_t(pointIt - _pieceView->m_TFPoints.begin());

        const auto tfPiece = _pieceView->m_tf;
        {
            // If the window is negative, the TF point list is reversed compared to the TF data.
            if(tfPiece->window() < 0)
            {
                pointIndex = _pieceView->m_TFPoints.size() - 1 - pointIndex;
            }

            // Retrieves the TF point.
            auto tfDataIt = tfPiece->cbegin();
            for(unsigned i = 0 ; i < pointIndex ; ++i)
            {
                tfDataIt++;
            }

            // Removes the TF point.
            data::transfer_function::value_t tfValue = tfDataIt->first;
            tfPiece->erase(tfValue);

            // Adds the new one with the new color.
            data::transfer_function::color_t color_t(newColor.red() / 255.,
                                                     newColor.green() / 255.,
                                                     newColor.blue() / 255.,
                                                     oldColor.alpha() / 255.);
            tfPiece->insert({tfValue, color_t});
        }

        tf->fitWindow();

        pointsModified(*tf);

        // Updates the displayed TF point.
        newColor.setAlpha(255);
        TFPoint.second->setBrush(QBrush(newColor));

        // Re-draw the current polygons.
        this->destroyTFPolygon(_pieceView);
        this->createTFPolygon(_pieceView);
        this->buildLayer();
    }
}

//-----------------------------------------------------------------------------

void transfer_function::leftButtonDoubleClickEvent(const sight::viz::scene2d::data::Event& _event)
{
    SIGHT_ASSERT("Interactions disabled, this code should not reached", m_interactive);

    sight::viz::scene2d::vec2d_t newCoord = this->getScene2DRender()->mapToScene(_event.getCoord());

    newCoord.y = std::clamp(newCoord.y, -1., 0.);
    {
        const auto tf = m_tf.lock();

        // Finds the current pieceView.
        SIGHT_ASSERT("The current TF mustn't be null", m_currentTF);
        PieceView* const currentPieceView = *(std::find_if(
                                                  m_pieceView.begin(),
                                                  m_pieceView.end(),
                                                  [&](const PieceView* _pieceView)
            {
                return _pieceView->m_tf == m_currentTF;
            }));
        const auto tfPiece = currentPieceView->m_tf;
        {
            data::transfer_function::color_t newColor;

            // The new coord becomes the new first TF point, get the current first color in the list.
            if(newCoord.x < currentPieceView->m_TFPoints.front().first.x)
            {
                const QColor firstColor = currentPieceView->m_TFPoints.front().second->brush().color();
                newColor = data::transfer_function::color_t(
                    firstColor.red() / 255.,
                    firstColor.green() / 255.,
                    firstColor.blue() / 255.,
                    -newCoord.y
                );
            }
            // The new coord becomes the new last TF point, get the current last color in the list.
            else if(newCoord.x > currentPieceView->m_TFPoints.back().first.x)
            {
                const QColor firstColor = currentPieceView->m_TFPoints.back().second->brush().color();
                newColor = data::transfer_function::color_t(
                    firstColor.red() / 255.,
                    firstColor.green() / 255.,
                    firstColor.blue() / 255.,
                    -newCoord.y
                );
            }
            // Gets an interpolate color since the new point is between two others.
            else
            {
                newColor   = tfPiece->sample(newCoord.x);
                newColor.a = -newCoord.y;
            }

            // Adds the new TF point.
            const data::transfer_function::value_t tfValue = tfPiece->mapValueFromWindow(newCoord.x);
            tfPiece->insert({tfValue, newColor});

            // Gets new window/level min max value in the window/level space.
            const double min = std::min(currentPieceView->m_TFPoints.begin()->first.x, newCoord.x);
            const double max = std::max(currentPieceView->m_TFPoints.rbegin()->first.x, newCoord.x);

            // Updates the window/level.
            if(tfPiece->window() > 0)
            {
                tfPiece->setWindowMinMax(data::transfer_function::min_max_t(min, max));
            }
            else
            {
                tfPiece->setWindowMinMax(data::transfer_function::min_max_t(max, min));
            }
        }

        tf->fitWindow();

        pointsModified(*tf);
    }

    // Re-draw all the scene.
    this->updating();
}

//-----------------------------------------------------------------------------

void transfer_function::midButtonClickEvent(sight::viz::scene2d::data::Event& _event)
{
    SIGHT_ASSERT("Interactions disabled, this code should not reached", m_interactive);

    const auto tf = m_tf.lock();

    // Finds all pieceView that match the clicked coord.
    std::vector<PieceView*> matchingPieceView = this->getMatchingPieceView(_event);

    // Checks if the current tf is in the matching list.
    const auto matchingIt = std::find_if(
        matchingPieceView.begin(),
        matchingPieceView.end(),
        [&](const PieceView* _pieceView)
        {
            return _pieceView->m_tf == m_currentTF;
        });

    // Moves the window/level only if the mouse if over the current TF.
    if(matchingIt != matchingPieceView.end())
    {
        this->getScene2DRender()->getView()->setCursor(Qt::ClosedHandCursor);
        sight::viz::scene2d::vec2d_t windowLevelCoord = this->getScene2DRender()->mapToScene(_event.getCoord());
        // Stores the level in window/level space and the window in screen space.
        m_capturedTF = std::make_pair(
            (*matchingIt)->m_tf,
            sight::viz::scene2d::vec2d_t(windowLevelCoord.x, _event.getCoord().y)
        );
        _event.setAccepted(true);
    }
}

//-----------------------------------------------------------------------------

void transfer_function::mouseMoveOnPieceViewEvent(const sight::viz::scene2d::data::Event& _event)
{
    // m_capturedTF must be previously sets by midButtonClickEvent(const sight::viz::scene2d::data::Event& _event)
    SIGHT_ASSERT("The captured pieceView must exist", m_capturedTF.first);

    sight::viz::scene2d::vec2d_t windowLevelCoord = this->getScene2DRender()->mapToScene(_event.getCoord());

    const auto minMax = m_capturedTF.first->minMax();
    const auto min    = m_capturedTF.first->mapValueToWindow(minMax.first);
    const auto max    = m_capturedTF.first->mapValueToWindow(minMax.second);

    // Prevent the whole curve to move outside the viewport.
    {
        auto viewport = m_viewport.lock();

        windowLevelCoord = glm::clamp(
            windowLevelCoord,
            glm::dvec2(viewport->left() + (m_capturedTF.second.x - min), viewport->top()),
            glm::dvec2(viewport->right() - (max - m_capturedTF.second.x), viewport->bottom())
        );
    }

    // The level delta is in window/level space.
    const double levelDelta = windowLevelCoord.x - m_capturedTF.second.x;

    // The window delta is in screen space.
    const double windowDelta = _event.getCoord().y - m_capturedTF.second.y;

    // Updates the TF.
    const data::transfer_function_piece::sptr tfPiece = m_capturedTF.first;
    {
        const auto tf = m_tf.lock();

        tfPiece->setWindow(tfPiece->window() - windowDelta);
        tfPiece->setLevel(tfPiece->level() + levelDelta);

        tf->fitWindow();

        // Sends the signal.
        const auto sig = tf->signal<data::transfer_function::WindowingModifiedSignalType>(
            data::transfer_function::WINDOWING_MODIFIED_SIG
        );
        {
            const core::com::connection::blocker block(sig->get_connection(slot(service::slots::UPDATE)));
            sig->async_emit(tf->window(), tf->level());
        }
    }

    // Stores the level in window/level space and the window in screen space.
    m_capturedTF.second = sight::viz::scene2d::vec2d_t(windowLevelCoord.x, _event.getCoord().y);

    // Re-draw all the scene.
    this->updating();
}

//-----------------------------------------------------------------------------

void transfer_function::midButtonReleaseEvent()
{
    SIGHT_ASSERT("Interactions disabled, this code should not reached", m_interactive);

    this->getScene2DRender()->getView()->setCursor(Qt::ArrowCursor);
    m_capturedTF.first = nullptr;
}

//-----------------------------------------------------------------------------

void transfer_function::rightButtonCLickEvent(const sight::viz::scene2d::data::Event& _event)
{
    SIGHT_ASSERT("Interactions disabled, this code should not reached", m_interactive);

    // Finds all pieceView that match the clicked coord.
    std::vector<PieceView*> matchingPieceView = this->getMatchingPieceView(_event);

    // Creates the menu.
    auto* const trapezeAction = new QAction("Add trapeze");
    QObject::connect(
        trapezeAction,
        &QAction::triggered,
        this,
        [_event, this]()
        {
            this->addTrapeze(_event);
        });
    auto* const leftRampAction = new QAction("Add left ramp");
    QObject::connect(
        leftRampAction,
        &QAction::triggered,
        this,
        [_event, this]()
        {
            this->addLeftRamp(_event);
        });
    auto* const rightRampAction = new QAction("Add right ramp");
    QObject::connect(
        rightRampAction,
        &QAction::triggered,
        this,
        [_event, this]()
        {
            this->addRightRamp(_event);
        });

    auto* const contextMenu = new QMenu();
    {
        // Checks if the current tf is in the matching list.
        const auto matchingIt = std::find_if(
            matchingPieceView.begin(),
            matchingPieceView.end(),
            [&](const PieceView* _pieceView)
            {
                return _pieceView->m_tf == m_currentTF;
            });

        // Adds the delete action if the current TF is clicked.
        if(matchingIt != matchingPieceView.end())
        {
            {
                const auto tf = m_tf.const_lock();
                // Adds the delete action if there is more than one TF.
                if(tf->pieces().size() > 1)
                {
                    auto* deleteAction = new QAction("Delete");
                    QObject::connect(
                        deleteAction,
                        &QAction::triggered,
                        this,
                        &transfer_function::removeCurrentTF
                    );
                    contextMenu->addAction(deleteAction);
                }
            }

            // Adds the clamp action.
            auto* const clampAction = new QAction("Clamp");
            clampAction->setCheckable(true);
            clampAction->setChecked(m_currentTF->clamped());
            QObject::connect(clampAction, &QAction::triggered, this, &transfer_function::clampCurrentTF);
            contextMenu->addAction(clampAction);

            // Adds the interpolation mode action.
            auto* const linearAction = new QAction("Linear");
            linearAction->setCheckable(true);
            linearAction->setChecked(
                m_currentTF->interpolationMode() == data::transfer_function::InterpolationMode::LINEAR
            );
            QObject::connect(linearAction, &QAction::triggered, this, &transfer_function::toggleLinearCurrentTF);
            contextMenu->addAction(linearAction);
        }
    }

    contextMenu->addAction(trapezeAction);
    contextMenu->addAction(leftRampAction);
    contextMenu->addAction(rightRampAction);

    // Opens the menu.
    contextMenu->exec(QCursor::pos());

    delete contextMenu;
}

//-----------------------------------------------------------------------------

void transfer_function::midButtonWheelMoveEvent(sight::viz::scene2d::data::Event& _event)
{
    SIGHT_ASSERT("Interactions disabled, this code should not reached", m_interactive);

    // Finds all pieceView that match the current coord.
    std::vector<PieceView*> matchingPieceView = this->getMatchingPieceView(_event);

    // Checks if the current tf is in the matching list.
    const auto matchingIt = std::find_if(
        matchingPieceView.begin(),
        matchingPieceView.end(),
        [&](const PieceView* _pieceView)
        {
            return _pieceView->m_tf == m_currentTF;
        });

    // Change the opacity only if the mouse if over the current TF.
    if(matchingIt != matchingPieceView.end())
    {
        data::transfer_function_piece::sptr tfPiece;
        {
            const auto tf      = m_tf.lock();
            const auto& pieces = tf->pieces();

            SIGHT_ASSERT(
                "inout '" + std::string(s_CURRENT_TF_INOUT) + "' must have at least on TF inside.",
                !tf->pieces().empty()
            );

            // Finds the tf
            SIGHT_ASSERT("The current TF mustn't be null", m_currentTF);
            tfPiece = *std::find_if(pieces.begin(), pieces.end(), [&](const auto& p){return p == m_currentTF;});

            // Updates the current TF.
            const double scale = _event.getType() == sight::viz::scene2d::data::Event::MouseWheelDown ? 0.9 : 1.1;

            // Scale data
            for(auto& data : *tfPiece)
            {
                data.second.a = data.second.a * scale;
            }

            tf->fitWindow();

            pointsModified(*tf);
        }

        // Re-draw all the scene.
        this->updating();

        _event.setAccepted(true);
    }
}

//-----------------------------------------------------------------------------

void transfer_function::removeCurrentTF()
{
    SIGHT_ASSERT("Interactions disabled, this code should not reached", m_interactive);
    {
        SIGHT_ASSERT("The current TF mustn't be null", m_currentTF);

        const auto tf = m_tf.lock();
        SIGHT_ASSERT("Transfer function is not set.", tf);
        SIGHT_ASSERT("Transfer function must have more than one TF piece inside.", tf->pieces().size() > 1);

        auto& pieces = tf->pieces();
        std::erase_if(pieces, [&](const auto& piece){return piece == m_currentTF;});

        // Sets the new current TF.
        m_currentTF = pieces.front();
        tf->fitWindow();
        // Block notifier
        auto sig = tf->signal<data::object::ModifiedSignalType>(data::object::MODIFIED_SIG);
        const core::com::connection::blocker block(sig->get_connection(slot(service::slots::UPDATE)));
        {
            sig->async_emit();
        }
    }

    // Re-draw all the scene here since swapping method as not been called.
    this->updating();
}

//-----------------------------------------------------------------------------

void transfer_function::clampCurrentTF(bool _clamp)
{
    SIGHT_ASSERT("Interactions disabled, this code should not reached", m_interactive);

    const auto tf = m_tf.lock();
    SIGHT_ASSERT("Transfer function is not set.", tf);

    const auto& pieces = tf->pieces();
    auto tfPiece       = *std::find_if(pieces.begin(), pieces.end(), [&](const auto& p){return p == m_currentTF;});

    tfPiece->setClamped(_clamp);
    tf->fitWindow();

    pointsModified(*tf);

    PieceView* currentPieceView = *(std::find_if(
                                        m_pieceView.begin(),
                                        m_pieceView.end(),
                                        [&](const PieceView* _pieceView)
        {
            return _pieceView->m_tf == m_currentTF;
        }));

    // Re-draw the current polygons.
    this->destroyTFPolygon(currentPieceView);
    this->createTFPolygon(currentPieceView);
    this->buildLayer();
}

//-----------------------------------------------------------------------------

void transfer_function::toggleLinearCurrentTF(bool _linear)
{
    SIGHT_ASSERT("Interactions disabled, this code should not reached", m_interactive);

    const auto tf = m_tf.lock();
    SIGHT_ASSERT("Transfer function is not set.", tf);

    const auto& pieces = tf->pieces();
    auto tfPiece       = *std::find_if(pieces.begin(), pieces.end(), [&](const auto& p){return p == m_currentTF;});

    tfPiece->setInterpolationMode(
        _linear ? data::transfer_function::InterpolationMode::LINEAR : data::transfer_function::InterpolationMode::NEAREST
    );
    tf->fitWindow();
    pointsModified(*tf);

    PieceView* currentPieceView = *(std::find_if(
                                        m_pieceView.begin(),
                                        m_pieceView.end(),
                                        [&](const PieceView* _pieceView)
        {
            return _pieceView->m_tf == m_currentTF;
        }));

    // Re-draw the current polygons.
    this->destroyTFPolygon(currentPieceView);
    this->createTFPolygon(currentPieceView);
    this->buildLayer();
}

//-----------------------------------------------------------------------------

void transfer_function::addNewTF(const data::transfer_function_piece::sptr _tf)
{
    SIGHT_ASSERT("Interactions disabled, this code should not reached", m_interactive);

    {
        const auto tf = m_tf.lock();
        SIGHT_ASSERT("inout '" + std::string(s_CURRENT_TF_INOUT) + "' does not exist.", tf);

        // Adds the new TF.
        tf->pieces().push_back(_tf);
        tf->fitWindow();
        // Block notifier
        auto sig = tf->signal<data::object::ModifiedSignalType>(data::object::MODIFIED_SIG);
        const core::com::connection::blocker block(sig->get_connection(slot(service::slots::UPDATE)));
        {
            sig->async_emit();
        }
    }

    // Creates the new PieceView.
    PieceView* newPieceView = this->createPieceView(_tf, 0);

    // Pushs the pieceView to the vector.
    m_pieceView.push_back(newPieceView);

    this->createTFPolygon(newPieceView);
    this->buildLayer();

    // Updates the current TF.
    this->setCurrentTF(newPieceView);
}

//-----------------------------------------------------------------------------

void transfer_function::addLeftRamp(const sight::viz::scene2d::data::Event& _event)
{
    SIGHT_ASSERT("Interactions disabled, this code should not reached", m_interactive);

    const auto leftRamp = std::make_shared<data::transfer_function_piece>();
    leftRamp->insert({0.0, data::transfer_function::color_t(1.0, 1.0, 1.0, 1.0)});
    leftRamp->insert({1.0, data::transfer_function::color_t()});
    leftRamp->setClamped(false);
    leftRamp->setWindow(500.);
    leftRamp->setLevel(50.);

    // Updates the window/level.
    sight::viz::scene2d::vec2d_t newCoord         = this->getScene2DRender()->mapToScene(_event.getCoord());
    const data::transfer_function::value_t window = leftRamp->window();
    data::transfer_function::value_t min          = newCoord.x - window / 2.;
    data::transfer_function::value_t max          = min + window;
    leftRamp->setWindowMinMax(data::transfer_function::min_max_t(min, max));

    this->addNewTF(leftRamp);
}

//-----------------------------------------------------------------------------

void transfer_function::addRightRamp(const sight::viz::scene2d::data::Event& _event)
{
    SIGHT_ASSERT("Interactions disabled, this code should not reached", m_interactive);

    // Creates the new TF.
    const auto rightRamp = std::make_shared<data::transfer_function_piece>();
    rightRamp->insert({0.0, data::transfer_function::color_t()});
    rightRamp->insert({1.0, data::transfer_function::color_t(1.0, 1.0, 1.0, 1.0)});
    rightRamp->setClamped(false);
    rightRamp->setWindow(500.);
    rightRamp->setLevel(50.);

    // Updates the window/level.
    sight::viz::scene2d::vec2d_t newCoord         = this->getScene2DRender()->mapToScene(_event.getCoord());
    const data::transfer_function::value_t window = rightRamp->window();
    data::transfer_function::value_t min          = newCoord.x - window / 2.;
    data::transfer_function::value_t max          = min + window;
    rightRamp->setWindowMinMax(data::transfer_function::min_max_t(min, max));

    this->addNewTF(rightRamp);
}

//-----------------------------------------------------------------------------

void transfer_function::addTrapeze(const sight::viz::scene2d::data::Event& _event)
{
    SIGHT_ASSERT("Interactions disabled, this code should not reached", m_interactive);

    // Creates the new TF.
    const auto trapeze = std::make_shared<data::transfer_function_piece>();
    trapeze->insert({0.0, data::transfer_function::color_t()});
    trapeze->insert({1. / 3., data::transfer_function::color_t(1.0, 1.0, 1.0, 1.0)});
    trapeze->insert({2. / 3., data::transfer_function::color_t(1.0, 1.0, 1.0, 1.0)});
    trapeze->insert({1.0, data::transfer_function::color_t()});
    trapeze->setClamped(true);
    trapeze->setWindow(500.);
    trapeze->setLevel(50.);

    // Updates the window/level.
    sight::viz::scene2d::vec2d_t newCoord         = this->getScene2DRender()->mapToScene(_event.getCoord());
    const data::transfer_function::value_t window = trapeze->window();
    data::transfer_function::value_t min          = newCoord.x - window / 2.;
    data::transfer_function::value_t max          = min + window;
    trapeze->setWindowMinMax(data::transfer_function::min_max_t(min, max));

    this->addNewTF(trapeze);
}

//------------------------------------------------------------------------------

void transfer_function::updateTF()
{
    // Sets the current TF.
    {
        // Sets the current TF used to highlight it in the editor.
        const auto tf = m_tf.const_lock();
        if(tf && !tf->pieces().empty())
        {
            m_currentTF = tf->pieces().front();
        }
        else
        {
            SIGHT_FATAL("The current TF mustn't be null");
        }
    }

    updating();
}

//------------------------------------------------------------------------------

void transfer_function::pointsModified(const sight::data::transfer_function& _tf) const
{
    // Sends the modification signal.
    const auto sigTf = _tf.signal<data::transfer_function::PointsModifiedSignalType>(
        data::transfer_function::POINTS_MODIFIED_SIG
    );

    const core::com::connection::blocker block1(sigTf->get_connection(slot(service::slots::UPDATE)));
    sigTf->async_emit();
}

//-----------------------------------------------------------------------------

} // namespace sight::module::viz::scene2d::adaptor
