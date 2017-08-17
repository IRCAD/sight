/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "scene2D/adaptor/STransferFunction.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>

#include <fwData/Composite.hpp>
#include <fwData/Image.hpp>
#include <fwData/String.hpp>

#include <fwDataTools/fieldHelper/Image.hpp>
#include <fwDataTools/fieldHelper/MedicalImageHelpers.hpp>
#include <fwDataTools/helper/Image.hpp>

#include <fwRenderQt/data/InitQtPen.hpp>
#include <fwRenderQt/data/Viewport.hpp>
#include <fwRenderQt/Scene2DGraphicsView.hpp>

#include <fwServices/macros.hpp>

#include <QColorDialog>
#include <QGraphicsItemGroup>
#include <QPoint>

fwServicesRegisterMacro( ::fwRenderQt::IAdaptor, ::scene2D::adaptor::STransferFunction);

namespace scene2D
{
namespace adaptor
{

static const ::fwServices::IService::KeyType s_IMAGE_INOUT        = "image";
static const ::fwServices::IService::KeyType s_TF_SELECTION_INOUT = "tfSelection";
static const ::fwServices::IService::KeyType s_VIEWPORT_INPUT     = "viewport";

STransferFunction::STransferFunction() noexcept :
    m_layer(nullptr),
    m_circleWidth(0.),
    m_circleHeight(0.),
    m_pointIsCaptured(false),
    m_capturedCircle(nullptr),
    m_pointSize(10.f)
{
    this->installTFSlots(this);
}

//-----------------------------------------------------------------------------

STransferFunction::~STransferFunction() noexcept
{
}

//-----------------------------------------------------------------------------

void STransferFunction::configuring()
{
    this->configureParams();

    const ConfigType config = this->getConfigTree().get_child("service.config.<xmlattr>");

    // Set the line and circle pens color if the respective attribute are present in the config
    if (config.count("lineColor"))
    {
        ::fwRenderQt::data::InitQtPen::setPenColor(m_linePen, config.get<std::string>("lineColor"));
    }

    if (config.count("circleColor"))
    {
        ::fwRenderQt::data::InitQtPen::setPenColor(m_circlePen, config.get<std::string>("circleColor"));
    }

    if (config.count("pointSize"))
    {
        m_pointSize = config.get<float>("pointSize");
    }

    SLM_ASSERT("'selectedTFKey' attribute is missing.", config.count("selectedTFKey"));
    this->setSelectedTFKey(config.get<std::string>("selectedTFKey"));
}

//-----------------------------------------------------------------------------

void STransferFunction::buildTFPoints()
{
    // Get the selected tf of the image
    ::fwData::TransferFunction::sptr selectedTF = this->getTransferFunction();

    // Clear the tf points map
    m_TFPoints.clear();

    // Iterate on the selected tf and fill the tf points map with key = value and T = Color(RGBA)
    ::fwData::TransferFunction::TFValuePairType minMax = selectedTF->getMinMaxTFValues();
    ::fwData::TransferFunction::TFValueType wlMin      = selectedTF->getWLMinMax().first;
    ::fwData::TransferFunction::TFValueType window     = selectedTF->getWindow();
    ::fwData::TransferFunction::TFValueType width      = minMax.second - minMax.first;

    for(const ::fwData::TransferFunction::TFDataType::value_type& elt : selectedTF->getTFData())
    {
        ::fwData::TransferFunction::TFValueType val;
        val             = (elt.first - minMax.first) / width;
        val             = val * window + wlMin;
        m_TFPoints[val] = elt.second;
    }
}

//-----------------------------------------------------------------------------

void STransferFunction::buildCircles()
{
    ::fwRenderQt::data::Viewport::sptr viewport = this->getScene2DRender()->getViewport();

    this->initializeViewSize();
    this->initializeViewportSize();

    const double viewportHeight = viewport->getHeight();
    const double viewportWidth  = viewport->getWidth();

    // Total ratio
    const Scene2DRatio ratio = this->getRatio();

    const double viewportWidthRatio = this->getViewportSizeRatio().first;

    // Initialize the width of the circle
    m_circleWidth = m_pointSize;

    // Initialize the height of the circles
    m_circleHeight  = m_pointSize * viewportHeight / viewportWidth;
    m_circleHeight /= viewportWidthRatio;
    m_circleHeight *= (m_viewInitialSize.first / m_viewInitialSize.second);

    // Apply the ratio of the scene 2D in order to keep the same size for the circles if viewport's size or
    // view's size change:
    m_circleWidth  *= ratio.first;
    m_circleHeight *= ratio.second;

    // Remove the circles items from the scene and clear the circles vector
    for(QGraphicsEllipseItem* circle : m_circles)
    {
        this->getScene2DRender()->getScene()->removeItem(circle);
        delete circle;
    }

    m_circles.clear();

    // Iterate on the tf points map to add circle items to the circles vector
    for(const ::fwData::TransferFunction::TFDataType::value_type& elt : m_TFPoints)
    {
        // Build circle corresponding to the tf point and push it back into the circles vector
        m_circles.push_back(this->buildCircle(elt.first, elt.second));
    }
}

//-----------------------------------------------------------------------------

QGraphicsEllipseItem* STransferFunction::buildCircle(::fwData::TransferFunction::TFValueType value,
                                                     ::fwData::TransferFunction::TFColor color)
{
    ::fwData::TransferFunction::sptr selectedTF = this->getTransferFunction();
    Point2DType valColor(value, color.a );
    Point2DType coord = this->mapAdaptorToScene(valColor, m_xAxis, m_yAxis);

    // Build a circle item, set its color, pen and zValue
    QGraphicsEllipseItem* circle = new QGraphicsEllipseItem(
        coord.first - m_circleWidth / 2,
        coord.second - m_circleHeight / 2,
        m_circleWidth,
        m_circleHeight
        );
    circle->setBrush(QBrush(QColor(color.r*255, color.g*255, color.b*255)));
    circle->setCacheMode( QGraphicsItem::DeviceCoordinateCache );
    circle->setPen(m_circlePen);
    circle->setZValue( 3 );

    return circle;
}

//-----------------------------------------------------------------------------

void STransferFunction::buildLinesAndPolygons()
{
    ::fwData::TransferFunction::sptr selectedTF = this->getTransferFunction();

    // Remove line and polygon items from the scene and clear the lines and polygons vector
    for( QGraphicsItem* linesAndPolygons : m_linesAndPolygons)
    {
        this->getScene2DRender()->getScene()->removeItem(linesAndPolygons);
        delete linesAndPolygons;
    }

    m_linesAndPolygons.clear();

    if (selectedTF->getInterpolationMode() == ::fwData::TransferFunction::LINEAR)
    {
        this->buildLinearLinesAndPolygons();
    }
    else
    {
        this->buildNearestLinesAndPolygons();
        if (!selectedTF->getIsClamped())
        {
            this->buildBounds();
        }
    }
}

//-----------------------------------------------------------------------------

void STransferFunction::buildBounds()
{
    ::fwRenderQt::data::Viewport::sptr viewport = this->getScene2DRender()->getViewport();

    const QGraphicsEllipseItem* beginCircle = m_circles.front();
    const QGraphicsEllipseItem* endCircle   = m_circles.back();

    double x1 = viewport->getX() - 10;
    double x2 = beginCircle->rect().x() + beginCircle->pos().x() + m_circleWidth /2;
    double y  = beginCircle->rect().y() + beginCircle->pos().y() + m_circleHeight / 2;
    // Build the line between the actual and the next TF Point and push it back into the lines and polygons vector
    QGraphicsLineItem* line = new QGraphicsLineItem(x1, y, x2, y);
    line->setPen(m_linePen);
    line->setCacheMode( QGraphicsItem::DeviceCoordinateCache );
    line->setZValue( 2 );

    m_linesAndPolygons.push_back(line);

    // Build the points vector defining the polygon and build the polygon
    QVector<QPointF> vect;
    vect.append(QPointF(line->line().x1(), 0));
    vect.append(QPointF(line->line().x1(), line->line().y1()));
    vect.append(QPointF(line->line().x2(), line->line().y2()));
    vect.append(QPointF(line->line().x2(), 0));

    QGraphicsPolygonItem* poly = new QGraphicsPolygonItem( QPolygonF( vect ) );

    // Set gradient, opacity and pen to the polygon
    poly->setBrush( beginCircle->brush() );
    poly->setOpacity(0.8);
    poly->setPen(QPen(Qt::NoPen));
    poly->setCacheMode( QGraphicsItem::DeviceCoordinateCache );
    poly->setZValue( 1 );

    // Push the polygon back into the lines and polygons vector
    m_linesAndPolygons.push_back(poly);

    x1 = endCircle->rect().x() + endCircle->pos().x() + m_circleWidth /2;
    x2 = viewport->getX() + viewport->getWidth() + 10;
    y  = endCircle->rect().y() + endCircle->pos().y() + m_circleHeight / 2;
    // Build the line between the actual and the next TF Point and push it back into the lines and polygons vector
    QGraphicsLineItem* line2 = new QGraphicsLineItem(x1, y, x2, y);
    line2->setPen(m_linePen);
    line2->setCacheMode( QGraphicsItem::DeviceCoordinateCache );
    line2->setZValue( 2 );

    m_linesAndPolygons.push_back(line2);

    // Build the points vector defining the polygon and build the polygon
    QVector<QPointF> vect2;
    vect2.append(QPointF(line2->line().x1(), 0));
    vect2.append(QPointF(line2->line().x1(), line2->line().y1()));
    vect2.append(QPointF(line2->line().x2(), line2->line().y2()));
    vect2.append(QPointF(line2->line().x2(), 0));

    QGraphicsPolygonItem* poly2 = new QGraphicsPolygonItem( QPolygonF( vect2 ) );

    // Set gradient, opacity and pen to the polygon
    poly2->setBrush( endCircle->brush() );
    poly2->setOpacity(0.8);
    poly2->setPen(QPen(Qt::NoPen));
    poly2->setCacheMode( QGraphicsItem::DeviceCoordinateCache );
    poly2->setZValue( 1 );

    // Push the polygon back into the lines and polygons vector
    m_linesAndPolygons.push_back(poly2);
}

//-----------------------------------------------------------------------------

void STransferFunction::buildLinearLinesAndPolygons()
{
    SLM_ASSERT("Circles must not be empty", !m_circles.empty());

    ::fwData::TransferFunction::sptr selectedTF = this->getTransferFunction();
    ::fwRenderQt::data::Viewport::sptr viewport = this->getScene2DRender()->getViewport();

    QVector<QPointF> vect;
    QLinearGradient grad;

    const QGraphicsEllipseItem* firtsCircle = m_circles.front();
    const QGraphicsEllipseItem* lastCircle  = m_circles.back();

    double xBegin;
    double xEnd;
    xBegin = firtsCircle->rect().x() + firtsCircle->pos().x() + m_circleWidth / 2;
    xEnd   = lastCircle->rect().x() + lastCircle->pos().x() + m_circleWidth / 2;
    if (selectedTF->getIsClamped())
    {
        vect.append(QPointF(xBegin, 0));
    }
    else
    {
        if (xBegin > viewport->getX())
        {
            xBegin = viewport->getX()-10;
            vect.append(QPointF(xBegin, 0));
            vect.append(QPointF(xBegin, firtsCircle->rect().y() + firtsCircle->pos().y() + m_circleHeight / 2));
        }
        else
        {
            vect.append(QPointF(xBegin, 0));
        }
        if (xEnd < viewport->getX() + viewport->getWidth())
        {
            xEnd = viewport->getX() + viewport->getWidth() +10;
        }
    }

    grad.setColorAt(0,  firtsCircle->brush().color());

    grad.setStart( xBegin, 0);
    grad.setFinalStop( xEnd, 0 );

    double distanceMax = xEnd - xBegin;

    // Iterate on the circles vector to add line and polygon items to the lines and polygons vector
    for ( auto circleIt = m_circles.cbegin(); circleIt != m_circles.cend()-1; ++circleIt)
    {
        QPointF p1((*circleIt)->rect().x() + (*circleIt)->pos().x() + m_circleWidth / 2,
                   (*circleIt)->rect().y() + (*circleIt)->pos().y() + m_circleHeight / 2);
        QPointF p2((*(circleIt + 1))->rect().x() + (*(circleIt + 1))->pos().x() + m_circleWidth / 2,
                   (*(circleIt + 1))->rect().y() + (*(circleIt + 1))->pos().y() + m_circleHeight / 2);

        vect.append(p1);
        vect.append(p2);

        // Build the gradient
        grad.setColorAt((p1.x() - xBegin)/distanceMax,  (*circleIt)->brush().color());
    }

    if (!selectedTF->getIsClamped())
    {
        if (xEnd == viewport->getX() + viewport->getWidth() + 10)
        {
            vect.append(QPointF(xEnd, lastCircle->rect().y() + lastCircle->pos().y() + m_circleHeight / 2));
        }
        double lastCircleX = lastCircle->rect().x() + lastCircle->pos().x() + m_circleWidth / 2;
        grad.setColorAt((lastCircleX-xBegin)/distanceMax, lastCircle->brush().color());
    }
    vect.append(QPointF(xEnd, 0));
    grad.setColorAt(1, lastCircle->brush().color());

    QGraphicsPolygonItem* poly = new QGraphicsPolygonItem( QPolygonF( vect ) );
    // Set gradient, opacity and pen to the polygon
    poly->setBrush( QBrush( grad ) );
    poly->setOpacity(0.8);
    poly->setPen(m_linePen);
    poly->setCacheMode( QGraphicsItem::DeviceCoordinateCache );
    poly->setZValue( 1 );

    // Push the polygon back into the lines and polygons vector
    m_linesAndPolygons.push_back(poly);
}

//-----------------------------------------------------------------------------

void STransferFunction::buildNearestLinesAndPolygons()
{
    // Iterate on the circles vector to add line and polygon items to the lines and polygons vector
    for ( auto circleIt = m_circles.cbegin(); circleIt != m_circles.cend(); ++circleIt)
    {
        QGraphicsEllipseItem* circle = *circleIt;
        QGraphicsEllipseItem* previousCircle;
        if (circleIt == m_circles.cbegin())
        {
            previousCircle = circle;
        }
        else
        {
            previousCircle = *(circleIt-1);
        }
        QGraphicsEllipseItem* nextCircle;
        if (circleIt == m_circles.cend()-1)
        {
            nextCircle = circle;
        }
        else
        {
            nextCircle = *(circleIt+1);
        }

        double x1 = previousCircle->rect().x() + previousCircle->pos().x() + (circle->pos().x() + circle->rect().x()
                                                                              - (previousCircle->pos().x() +
                                                                                 previousCircle->rect().x()))/2 +
                    m_circleWidth /2;
        double x2 = circle->rect().x() + circle->pos().x() + (nextCircle->pos().x() + nextCircle->rect().x()
                                                              - (circle->pos().x() + circle->rect().x()))/2 +
                    m_circleWidth /2;
        double y = circle->rect().y() + circle->pos().y() + m_circleHeight / 2;
        // Build the line between the actual and the next TF Point and push it back into the lines and polygons vector
        QGraphicsLineItem* line = new QGraphicsLineItem(x1, y, x2, y);
        line->setPen(m_linePen);
        line->setCacheMode( QGraphicsItem::DeviceCoordinateCache );
        line->setZValue( 2 );

        m_linesAndPolygons.push_back(line);

        // Build the points vector defining the polygon and build the polygon
        QVector<QPointF> vect;
        vect.append(QPointF(line->line().x1(), 0));
        vect.append(QPointF(line->line().x1(), line->line().y1()));
        vect.append(QPointF(line->line().x2(), line->line().y2()));
        vect.append(QPointF(line->line().x2(), 0));

        QGraphicsPolygonItem* poly = new QGraphicsPolygonItem( QPolygonF( vect ) );

        // Set gradient, opacity and pen to the polygon
        poly->setBrush( circle->brush() );
        poly->setOpacity(0.8);
        poly->setPen(QPen(Qt::NoPen));
        poly->setCacheMode( QGraphicsItem::DeviceCoordinateCache );
        poly->setZValue( 1 );

        // Push the polygon back into the lines and polygons vector
        m_linesAndPolygons.push_back(poly);
    }
}

//-----------------------------------------------------------------------------

void STransferFunction::buildLayer()
{
    // Add graphics items vectors to the layer
    for ( unsigned int i = 0; i < m_linesAndPolygons.size(); i++)
    {
        m_layer->addToGroup(m_linesAndPolygons.at(i));
    }

    for ( unsigned int i = 0; i < m_circles.size(); i++)
    {
        m_layer->addToGroup(m_circles.at(i));
    }

    // Adjust the layer's position and zValue depending on the associated axis
    m_layer->setPos(m_xAxis->getOrigin(), m_yAxis->getOrigin());
    m_layer->setZValue(m_zValue);

    // Add the layer item to the scene
    this->getScene2DRender()->getScene()->addItem(m_layer);
}

//-----------------------------------------------------------------------------

void STransferFunction::updateImageTF()
{
    // Get the selected tf of the image
    ::fwData::TransferFunction::sptr selectedTF        = this->getTransferFunction();
    ::fwData::TransferFunction::TFValuePairType minMax = selectedTF->getMinMaxTFValues();
    ::fwData::TransferFunction::TFValueType window     = selectedTF->getWindow();
    ::fwData::TransferFunction::TFValueType wlMin      = selectedTF->getWLMinMax().first;
    ::fwData::TransferFunction::TFValueType val;
    selectedTF->clear();

    // Rebuild the selected tf from the tf points map
    double width = minMax.second - minMax.first;
    double min   = m_TFPoints.begin()->first;
    double max   = m_TFPoints.rbegin()->first;
    for(const ::fwData::TransferFunction::TFDataType::value_type& elt :  m_TFPoints)
    {
        val = (elt.first - wlMin) / window;
        val = val * width + minMax.first;
        selectedTF->addTFColor(val, elt.second);
    }

    if (window >= 0)
    {
        selectedTF->setWLMinMax(::fwData::TransferFunction::TFValuePairType(min, max));
    }
    else
    {
        selectedTF->setWLMinMax(::fwData::TransferFunction::TFValuePairType(max, min));
    }

    auto sig = selectedTF->signal< ::fwData::TransferFunction::PointsModifiedSignalType >(
        ::fwData::TransferFunction::s_POINTS_MODIFIED_SIG);
    {
        ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdateTFPoints));
        sig->asyncEmit();
    }
}

//-----------------------------------------------------------------------------

void STransferFunction::starting()
{
    // Initialize the layer and the circle height and width
    m_layer = new QGraphicsItemGroup();

    m_linePen.setCosmetic( true );
    m_linePen.setWidthF( 0 );

    m_circlePen.setCosmetic( true );
    m_circlePen.setWidthF( 0 );

    ::fwData::Composite::sptr tfSelection = this->getInOut< ::fwData::Composite >(s_TF_SELECTION_INOUT);
    this->setTransferFunctionSelection(tfSelection);

    this->updating();
    this->installTFConnections();
}

//-----------------------------------------------------------------------------

void STransferFunction::updating()
{
    ::fwData::Composite::sptr tfSelection = this->getInOut< ::fwData::Composite >(s_TF_SELECTION_INOUT);
    this->setTransferFunctionSelection(tfSelection);

    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    this->updateImageInfos(image);
    this->updateTransferFunction(image);

    // Build the tf map points, the circles vector, the lines and polygons vector, add the items to the layer and add it
    // to the scene
    this->buildTFPoints();
    this->buildCircles();
    this->buildLinesAndPolygons();
    this->buildLayer();
}

//------------------------------------------------------------------------------

void STransferFunction::updatingTFPoints()
{
    this->updating();
}

//------------------------------------------------------------------------------

void STransferFunction::updatingTFWindowing(double window, double level)
{
    this->updating();
}

//-----------------------------------------------------------------------------

void STransferFunction::stopping()
{
    this->removeTFConnections();

    // Clear the items vectors and remove the layer (and all its children) from the scene
    for (auto circleIt = m_circles.begin(); circleIt != m_circles.end(); ++circleIt )
    {
        delete *circleIt;
    }
    m_circles.clear();

    for( auto linesPolyIt = m_linesAndPolygons.begin(); linesPolyIt != m_linesAndPolygons.end(); ++linesPolyIt)
    {
        delete *linesPolyIt;
    }
    m_linesAndPolygons.clear();

    this->getScene2DRender()->getScene()->removeItem(m_layer);
    delete m_layer;
    m_layer = nullptr;
}

//-----------------------------------------------------------------------------

void STransferFunction::processInteraction( ::fwRenderQt::data::Event& _event )
{
    SLM_ASSERT("Sizes of circles vector and tf points map are different", m_TFPoints.size() == m_circles.size());

    QPoint scenePos = QPoint(_event.getCoord().getX(), _event.getCoord().getY());

    QList<QGraphicsItem*> items = this->getScene2DRender()->getView()->items(scenePos);

    // Iterate parallely on the circles vector and the tf points map
    ::fwData::TransferFunction::TFDataType::iterator TFPointIt = m_TFPoints.begin();
    for(QGraphicsEllipseItem* circle : m_circles)
    {
        if ( items.indexOf(circle) >= 0)
        {
            // If there is a double click
            if ( _event.getType() == ::fwRenderQt::data::Event::MouseButtonDoubleClick )
            {
                this->doubleClickEvent(circle, TFPointIt->second);
                return;
            }
            // If left button is pressed
            else if ( _event.getType() == ::fwRenderQt::data::Event::MouseButtonPress
                      && _event.getButton() == ::fwRenderQt::data::Event::LeftButton )
            {

                this->leftButtonEvent(circle, _event);
                return;
            }
            else if ( _event.getType() == ::fwRenderQt::data::Event::MouseButtonPress
                      && _event.getButton() == ::fwRenderQt::data::Event::RightButton
                      && m_circles.size() > 2  )
            {
                this->rightButtonEvent(TFPointIt->first, _event);
                return;
            }
        }
        // If a point is captured, this is the good one, and the mouse move
        if ( m_pointIsCaptured && m_capturedCircle == circle
             && _event.getType() == ::fwRenderQt::data::Event::MouseMove )
        {
            this->mouseMoveEvent(circle, TFPointIt->first, _event);
            return;
        }
        // If a point is captured, this is the good one, and the button is released
        else if ( m_pointIsCaptured && m_capturedCircle == circle
                  && _event.getType() == ::fwRenderQt::data::Event::MouseButtonRelease )
        {
            this->mouseButtonReleaseEvent(circle, _event);
            return;
        }
        ++TFPointIt;
    }

    // After iteration, due to return instruction in it, the events caught aren't on circles
    if ( _event.getType() == ::fwRenderQt::data::Event::MouseButtonDoubleClick &&
         _event.getButton() == ::fwRenderQt::data::Event::LeftButton )
    {
        // If no other event has been catched, a left click means a point creation
        this->doubleClickEvent(_event);
    }

    if( _event.getType() == ::fwRenderQt::data::Event::Resize)
    {
        ::fwRenderQt::data::Size oldSize = _event.getOldSize();
        ::fwRenderQt::data::Size newSize = _event.getSize();

        if( oldSize.getWidth() == -1.0f )
        {
            m_viewInitialSize.first = newSize.getWidth();
        }

        if( oldSize.getHeight() == -1.0f )
        {
            m_viewInitialSize.second = newSize.getHeight();
        }

        this->updating();
    }
}

//-----------------------------------------------------------------------------

void STransferFunction::doubleClickEvent(QGraphicsEllipseItem* circle, ::fwData::TransferFunction::TFColor& tfColor)
{
    // Open a QColorDialog with the selected circle color and the tf point alpha as default rgba color
    const QColor shapeColor = ((QAbstractGraphicsShapeItem*)circle)->brush().color();
    const QColor initialColor( shapeColor.red(), shapeColor.green(), shapeColor.blue(), tfColor.a*255);

    QColor circleColor = QColorDialog::getColor(initialColor,
                                                this->getScene2DRender()->getView(),
                                                QString("Choose the point color"),
                                                QColorDialog::ShowAlphaChannel);

    if (circleColor.isValid())
    {
        // If the color is valid, modify the selected tf point color
        tfColor.r = circleColor.redF();
        tfColor.g = circleColor.greenF();
        tfColor.b = circleColor.blueF();
        tfColor.a = circleColor.alphaF();

        // Build circles, lines and polygons cause they're modified by the new tf point color
        this->buildCircles();
        this->buildLinesAndPolygons();
        this->buildLayer();

        // Update the selected tf (no need to rescale cause the selected circle position has not changed)
        this->updateImageTF();
    }
}

//-----------------------------------------------------------------------------

void STransferFunction::leftButtonEvent(QGraphicsEllipseItem* circle, ::fwRenderQt::data::Event& _event)
{
    m_pointIsCaptured = true;

    // Store the captured circle in case it's moved
    m_capturedCircle = circle;

    //Store the circle coordinates in case it's moved
    m_oldCoord = this->coordViewToCoordItem( _event.getCoord() );

    // Set the selected circle pen to yellow to get a visual feedback that the selected circle is selected
    QPen circlePen(Qt::yellow);
    circlePen.setCosmetic(true);
    circle->setPen( circlePen );

    _event.setAccepted(true);
}

//-----------------------------------------------------------------------------

void STransferFunction::mouseMoveEvent(QGraphicsEllipseItem* circle,
                                       ::fwData::TransferFunction::TFValueType tfPoint,
                                       ::fwRenderQt::data::Event& _event)
{
    QGraphicsEllipseItem* lastPoint = m_circles.back();

    std::vector< QGraphicsEllipseItem* >::iterator circleIt;
    circleIt = std::find(m_circles.begin(), m_circles.end(), circle);

    // Get the previous circle
    std::vector< QGraphicsEllipseItem* >::iterator previousCircle = circleIt;
    if (circle != m_circles.front())
    {
        --previousCircle;
    }

    // Get the next circle
    std::vector< QGraphicsEllipseItem* >::iterator nextCircle = circleIt;
    if (circle != lastPoint)
    {
        ++nextCircle;
    }

    // Get the actual selected circle coordinates
    ::fwRenderQt::data::Coord newCoord = this->coordViewToCoordItem( _event.getCoord() );

    // Calculate the real coordinates of the previous circle
    const double previousXCircleRealPos = (*previousCircle)->rect().x() + (*previousCircle)->pos().x();
    const double previousYRealNewPos    = (*previousCircle)->rect().y() + (*previousCircle)->pos().y();

    Point2DType previousValues;
    Point2DType previousXY(previousXCircleRealPos, previousYRealNewPos );
    previousValues = this->mapSceneToAdaptor(previousXY, m_xAxis, m_yAxis);

    // Calculate the real coordinates of the next circle
    const double nextXCircleRealPos = (*nextCircle)->rect().x() + (*nextCircle)->pos().x();
    const double nextYRealNewPos    = (*nextCircle)->rect().y() + (*nextCircle)->pos().y();

    Point2DType nextValues;
    const Point2DType nextXY(nextXCircleRealPos, nextYRealNewPos);
    nextValues = this->mapSceneToAdaptor(nextXY, m_xAxis, m_yAxis);

    // Calculate the real coordinates of the selected circle
    const double circleXRealNewPos = circle->rect().x() + circle->pos().x() + newCoord.getX() - m_oldCoord.getX();
    const double circleYRealNewPos = circle->rect().y() + circle->pos().y() + newCoord.getY() - m_oldCoord.getY();

    Point2DType realValues;
    const Point2DType circleXY( circleXRealNewPos, circleYRealNewPos );
    realValues = this->mapSceneToAdaptor(circleXY, m_xAxis, m_yAxis);

    Point2DType oldCoordPair;
    const Point2DType oldCoordXY( m_oldCoord.getX(), m_oldCoord.getY() );
    oldCoordPair = this->mapSceneToAdaptor(oldCoordXY, m_xAxis, m_yAxis);

    Point2DType newCoordPair;
    const Point2DType newCoordXY( newCoord.getX(), newCoord.getY() );
    newCoordPair = this->mapSceneToAdaptor(newCoordXY, m_xAxis, m_yAxis);

    // Check if the mouse isn't out of bounds vertically and horizontally
    if (   (circle == m_circles.front() || realValues.first > previousValues.first)
           && (circle == lastPoint || realValues.first < nextValues.first)
           && (realValues.second - m_circleHeight/2) >= 0
           && (realValues.second - m_circleHeight/2) <= 1 )
    {
        // Move the selected tf point by the difference between the old coordinates and the new ones
        circle->moveBy( newCoord.getX() - m_oldCoord.getX(), newCoord.getY() - m_oldCoord.getY() );

        // Set the actual coordinates as the old ones
        m_oldCoord = newCoord;
    }
    else
    {
        // Check if the mouse is out of bounds only horizontally
        if (  ((circle != m_circles.front() && realValues.first < previousValues.first)
               || (circle != lastPoint && realValues.first > nextValues.first))
              && (realValues.second - m_circleHeight/2) >= 0
              && (realValues.second - m_circleHeight/2) <= 1 )
        {
            // new abscissa of the moving TF point
            double x = (newCoordPair.first > oldCoordPair.first) ? (nextValues.first - 1) : (previousValues.first + 1);
            x = this->mapAdaptorToScene(Point2DType( x, 0 ), m_xAxis, m_yAxis).first;

            QRectF rect        = circle->rect();
            const double width = rect.width();
            rect.setX( x );
            rect.setWidth( width );
            circle->setRect( rect );

            circle->setPos( 0, circle->pos().y() + (newCoord.getY() - m_oldCoord.getY()) );

            m_oldCoord.setX( x );
            m_oldCoord.setY(newCoord.getY());
        }

        // Check if the mouse is out of bounds only vertically
        if (  (circle == m_circles.front() || realValues.first > previousValues.first)
              && (circle == lastPoint || realValues.first < nextValues.first)
              && ((realValues.second - m_circleHeight/2) < 0 || (realValues.second - m_circleHeight/2) > 1) )   // opacity
        {
            // If the mouse is vertically out of bounds, the TF point is moved to fit the nearest vertical bound (0 or
            // 1).

            double y = 0.;   // new ordinate of the moving TF point

            if( newCoordPair.second > oldCoordPair.second )
            {
                // Mouse's move goes toward (and outside) the top of the scene: the ordinate of the circle must be set
                // to 1,
                // which is the highest value for opacity
                y = this->mapAdaptorToScene(Point2DType( 0, 1 ), m_xAxis, m_yAxis).second;
            }

            QRectF rect         = circle->rect();
            const double height = rect.height();
            rect.setY( y - m_circleHeight / 2 );
            rect.setHeight( height );
            circle->setRect( rect );

            circle->setPos( circle->pos().x() + (newCoord.getX() - m_oldCoord.getX()), 0);  // position within item's
                                                                                            // rect

            m_oldCoord.setX(newCoord.getX());
            m_oldCoord.setY( y );
        }
    }

    this->buildLinesAndPolygons();
    this->buildLayer();

    // Erase the selected tf point cause it's key is const
    m_TFPoints.erase(tfPoint);

    const Point2DType point(this->pointValue(circle), circle->rect().y() + circle->pos().y() + m_circleHeight / 2 );
    // Create a new tf point with the right value (key) and alpha
    const Point2DType pair = this->mapSceneToAdaptor(point, m_xAxis, m_yAxis);

    m_TFPoints[pair.first] = ::fwData::TransferFunction::TFColor(
        circle->brush().color().redF(),
        circle->brush().color().greenF(),
        circle->brush().color().blueF(),
        pair.second);

    // Update the image tf
    this->updateImageTF();
}

//-----------------------------------------------------------------------------

void STransferFunction::mouseButtonReleaseEvent(QGraphicsEllipseItem* circle, ::fwRenderQt::data::Event& _event)
{
    // Reset the circle pen to the selected circle to get a visual feedback that the circle is no more selected
    circle->setPen(m_circlePen);
    m_pointIsCaptured = false;
    _event.setAccepted(true);
}

//-----------------------------------------------------------------------------

void STransferFunction::rightButtonEvent(::fwData::TransferFunction::TFValueType tfPoint,
                                         ::fwRenderQt::data::Event& _event)
{
    _event.setAccepted(true);

    // Erase the selected tf point
    m_TFPoints.erase(tfPoint);

    this->updateImageTF();
    this->updating();
}

//-----------------------------------------------------------------------------

void STransferFunction::doubleClickEvent( ::fwRenderQt::data::Event& _event)
{
    ::fwData::TransferFunction::sptr selectedTF = this->getTransferFunction();

    // Get the x and y position in the scene coordinates
    const double x = this->getScene2DRender()->mapToScene(_event.getCoord()).getX();
    const double y = this->getScene2DRender()->mapToScene(_event.getCoord()).getY();

    // Transform the x and y coordinates with axis scaling and type
    const Point2DType _xy(x, y);
    const Point2DType values = this->mapSceneToAdaptor(_xy, m_xAxis, m_yAxis);

    ::fwData::TransferFunction::TFDataType::iterator nextTFPointIt         = m_TFPoints.begin();
    ::fwData::TransferFunction::TFDataType::reverse_iterator lastTFPointIt = m_TFPoints.rbegin();

    if (values.first < (*nextTFPointIt).first)
    {
        ::fwData::TransferFunction::TFColor color((*nextTFPointIt).second.r, (*nextTFPointIt).second.g,
                                                  (*nextTFPointIt).second.b, values.second);
        m_TFPoints[values.first] = color;

        this->updateImageTF();
        this->updating();
    }
    else if (values.first > (*lastTFPointIt).first)
    {
        ::fwData::TransferFunction::TFColor color((*lastTFPointIt).second.r, (*lastTFPointIt).second.g,
                                                  (*lastTFPointIt).second.b, values.second);
        m_TFPoints[values.first] = color;

        this->updateImageTF();
        this->updating();
    }
    else
    {
        // Iterate on m_TFPoints to find the next and the previous point of our event position
        while ((*nextTFPointIt).first < values.first)
        {
            ++nextTFPointIt;
        }
        ::fwData::TransferFunction::TFDataType::iterator prevTFPointIt = nextTFPointIt;
        --prevTFPointIt;

        // Check if the new point is not placed on an already existing point
        if ( (values.first != (*nextTFPointIt).first) && (values.first != (*prevTFPointIt).first) )
        {
            // Calculate the relative position of the event between the 2 encompassing points
            const double coef = (values.first - (*prevTFPointIt).first) /
                                ((*nextTFPointIt).first - (*prevTFPointIt).first);

            // Calculate the new red, green, blue and alpha by linear interpolation in RGBA
            const double newRed = coef * ((*nextTFPointIt).second.r - (*prevTFPointIt).second.r) +
                                  (*prevTFPointIt).second.r;
            const double newGreen = coef * ((*nextTFPointIt).second.g - (*prevTFPointIt).second.g) +
                                    (*prevTFPointIt).second.g;
            const double newBlue = coef * ((*nextTFPointIt).second.b - (*prevTFPointIt).second.b) +
                                   (*prevTFPointIt).second.b;
            const double newAlpha = coef * ((*nextTFPointIt).second.a - (*prevTFPointIt).second.a) +
                                    (*prevTFPointIt).second.a;

            // Add a point with the right values to the tf points map
            m_TFPoints[values.first] = ::fwData::TransferFunction::TFColor(newRed, newGreen, newBlue, newAlpha);

            this->updateImageTF();
            this->updating();
        }
    }
}

//-----------------------------------------------------------------------------

double STransferFunction::pointValue(QGraphicsEllipseItem* circle)
{
    // Return the x coordinate of the center of a circle in a 0-1 scale
    return (circle->rect().x() + circle->pos().x() + m_circleWidth / 2);
}

//-----------------------------------------------------------------------------

::fwRenderQt::data::Coord STransferFunction::coordViewToCoordItem( const ::fwRenderQt::data::Coord& _coord )
{
    ::fwRenderQt::data::Coord scenePoint = this->getScene2DRender()->mapToScene( _coord );
    return scenePoint;
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap STransferFunction::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push(s_IMAGE_INOUT, ::fwData::Image::s_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_IMAGE_INOUT, ::fwData::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push( s_VIEWPORT_INPUT, ::fwRenderQt::data::Viewport::s_MODIFIED_SIG, s_UPDATE_SLOT );
    return connections;
}

//------------------------------------------------------------------------------

} // namespace adaptor
} // namespace scene2D

