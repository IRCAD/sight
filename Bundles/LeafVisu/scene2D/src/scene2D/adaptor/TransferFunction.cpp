/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include <fwServices/Base.hpp>

#include <fwData/Image.hpp>
#include <fwData/Composite.hpp>
#include <fwData/String.hpp>

#include <fwComEd/ImageMsg.hpp>
#include <fwComEd/TransferFunctionMsg.hpp>
#include <fwComEd/Dictionary.hpp>
#include <fwComEd/fieldHelper/MedicalImageHelpers.hpp>
#include <fwComEd/helper/Image.hpp>

#include <fwServices/IEditionService.hpp>

#include <QGraphicsItemGroup>
#include <QPoint>
#include <QColorDialog>

#include "scene2D/data/Viewport.hpp"
#include "scene2D/adaptor/TransferFunction.hpp"
#include "scene2D/data/InitQtPen.hpp"
#include "scene2D/Scene2DGraphicsView.hpp"
#include "scene2D/data/ViewportMsg.hpp"

REGISTER_SERVICE( ::scene2D::adaptor::IAdaptor , ::scene2D::adaptor::TransferFunction  , ::fwData::Image ) ;


namespace scene2D
{
namespace adaptor
{

TransferFunction::TransferFunction() throw() : m_pointSize(10)
{
    this->installTFPoolEventHandler(this);
    this->addNewHandledEvent( ::scene2D::data::ViewportMsg::VALUE_IS_MODIFIED);
    this->addNewHandledEvent( ::fwComEd::TransferFunctionMsg::MODIFIED_POINTS );
    this->addNewHandledEvent( ::fwComEd::TransferFunctionMsg::WINDOWING );
}

//-----------------------------------------------------------------------------

TransferFunction::~TransferFunction() throw()
{}

//-----------------------------------------------------------------------------

void TransferFunction::configuring() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    SLM_ASSERT("\"config\" tag missing", m_configuration->getName() == "config");

    this->IAdaptor::configuring();

    // Set the line and circle pens color if the respective attribute are present in the config
    if (!m_configuration->getAttributeValue("lineColor").empty())
    {
        ::scene2D::data::InitQtPen::setPenColor(m_linePen, m_configuration->getAttributeValue("lineColor"));
    }

    if (!m_configuration->getAttributeValue("circleColor").empty())
    {
        ::scene2D::data::InitQtPen::setPenColor(m_circlePen, m_configuration->getAttributeValue("circleColor"));
    }

    if (!m_configuration->getAttributeValue("pointSize").empty())
    {
        m_pointSize = ::boost::lexical_cast< float >( m_configuration->getAttributeValue("pointSize") );
    }

    SLM_ASSERT("A viewport attribute must be specified with 'viewportUID'.", !m_configuration->getAttributeValue("viewportUID").empty());

    if( !m_configuration->getAttributeValue("viewportUID").empty() )
    {
        m_viewport = ::scene2D::data::Viewport::dynamicCast( ::fwTools::fwID::getObject( m_configuration->getAttributeValue("viewportUID") ) );
        m_comChannel = ::fwServices::registerCommunicationChannel( m_viewport , this->getSptr());
        m_comChannel->start();
    }
    this->parseTFConfig( m_configuration );
}

//-----------------------------------------------------------------------------

void TransferFunction::buildTFPoints()
{
    SLM_TRACE_FUNC();

    // Get the selected tf of the image
    ::fwData::TransferFunction::sptr selectedTF = this->getTransferFunction();

    // Get the window and the level from the selected tf
    m_level = selectedTF->getLevel();
    m_window = selectedTF->getWindow();

    // Clear the tf points map
    m_TFPoints.clear();

    // Iterate on the selected tf and fill the tf points map with key = value and T = Color(RGBA)
//    ::fwData::TransfertFunction::TransfertFunctionPointIterator TFPointIt;
//    for( TFPointIt = m_selectedTF->getTransfertFunctionPoints().first ; TFPointIt != m_selectedTF->getTransfertFunctionPoints().second ; ++TFPointIt )
//    {
//        m_TFPoints[((*TFPointIt)->getValue() - (double)m_selectedTF->getMinMax().first) / m_window] = m_selectedTF->getColor((*TFPointIt)->getValue());
//    }
    ::fwData::TransferFunction::TFValueType wlMax = selectedTF->getWLMinMax().first;
    BOOST_FOREACH(::fwData::TransferFunction::TFDataType::value_type elt, selectedTF->getTFData())
    {
        m_TFPoints[(elt.first - wlMax) / m_window ] = elt.second;
    }
}

//-----------------------------------------------------------------------------

void TransferFunction::buildCircles()
{
    SLM_TRACE_FUNC();

    this->initializeViewSize();
    this->initializeViewportSize();

    ::scene2D::data::Viewport::sptr viewport = this->getScene2DRender()->getViewport();
    const double viewportHeight = viewport->getHeight();
    const double viewportWidth = viewport->getWidth();

    // Total ratio
    Scene2DRatio ratio = this->getRatio();

    double viewportWidthRatio = this->getViewportSizeRatio().first;

    // Initialize the width of the circle
    m_circleWidth = m_pointSize;

    // Initialize the height of the circles
    m_circleHeight = m_pointSize * viewportHeight / viewportWidth;
    m_circleHeight /= viewportWidthRatio;
    m_circleHeight *= (m_viewInitialSize.first / m_viewInitialSize.second);

    // Apply the ratio of the scene 2D in order to keep the same size for the circles if viewport's size or
    // view's size change:
    m_circleWidth *= ratio.first;
    m_circleHeight *= ratio.second;

    // Remove the circles items from the scene and clear the circles vector
    for (std::vector< QGraphicsEllipseItem* >::iterator circleIt = m_circles.begin() ; circleIt != m_circles.end() ; ++circleIt)
    {
        this->getScene2DRender()->getScene()->removeItem(*circleIt);
        delete *circleIt;
    }

    m_circles.clear();

    // Iterate on the tf points map to add circle items to the circles vector
    BOOST_FOREACH(::fwData::TransferFunction::TFDataType::value_type elt,  m_TFPoints)
    {
        // Build circle corresponding to the tf point and push it back into the circles vector
        m_circles.push_back(this->buildCircle(elt.first, elt.second));
    }
}

//-----------------------------------------------------------------------------

QGraphicsEllipseItem* TransferFunction::buildCircle(::fwData::TransferFunction::TFValueType value, ::fwData::TransferFunction::TFColor color)
{
    SLM_TRACE_FUNC();
    ::fwData::TransferFunction::sptr selectedTF = this->getTransferFunction();
    std::pair<double, double> coord =
        this->mapAdaptorToScene(std::pair< double , double >( value, color.a ) , m_xAxis, m_yAxis);

    // Build a circle item, set its color, pen and zValue
    QGraphicsEllipseItem* circle = new QGraphicsEllipseItem(
    coord.first * m_window + selectedTF->getWLMinMax().first - m_circleWidth / 2,
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

void TransferFunction::buildLinesAndPolygons()
{
    SLM_TRACE_FUNC();

    // Remove line and polygon items from the scene and clear the lines and polygons vector
    std::vector< QGraphicsItem* >::iterator linesAndPolygonsIt;
    for( linesAndPolygonsIt = m_linesAndPolygons.begin() ; linesAndPolygonsIt != m_linesAndPolygons.end() ; ++linesAndPolygonsIt)
    {
        this->getScene2DRender()->getScene()->removeItem(*linesAndPolygonsIt);
        delete *linesAndPolygonsIt;
    }

    m_linesAndPolygons.clear();

    // Ratio between the current size of the widget  and its initial size
    ViewSizeRatio ratio = this->getViewSizeRatio();

    // Iterate on the circles vector to add line and polygon items to the lines and polygons vector
    for ( std::vector< QGraphicsEllipseItem* >::iterator circleIt = m_circles.begin() ; circleIt != m_circles.end()-1 ; ++circleIt)
    {
        // Build the line between the actual and the next TF Point and push it back into the lines and polygons vector
        QGraphicsLineItem* line = new QGraphicsLineItem(
                (*circleIt)->rect().x() + (*circleIt)->pos().x() + m_circleWidth / 2,
                (*circleIt)->rect().y() + (*circleIt)->pos().y() + m_circleHeight / 2,
                (*(circleIt + 1))->rect().x() + (*(circleIt + 1))->pos().x() + m_circleWidth / 2,
                (*(circleIt + 1))->rect().y() + (*(circleIt + 1))->pos().y() + m_circleHeight / 2 );
        line->setPen(m_linePen);
        line->setCacheMode( QGraphicsItem::DeviceCoordinateCache );
        line->setZValue( 2 );

        m_linesAndPolygons.push_back(line);

        // Build the points vector defining the polygon and buil the polygon
        QVector<QPointF> vect;
        vect.append(QPointF(line->line().x1(), 0));
        vect.append(QPointF(line->line().x1(), line->line().y1()));
        vect.append(QPointF(line->line().x2(), line->line().y2()));
        vect.append(QPointF(line->line().x2(), 0));

        QGraphicsPolygonItem* poly = new QGraphicsPolygonItem( QPolygonF( vect ) );

        QColor circle = (*circleIt)->brush().color();
        QColor circleNext = (*(circleIt + 1))->brush().color();

        // Build the gradient
        QLinearGradient grad;
        grad.setStart( line->line().x1(), 0);
        grad.setFinalStop( line->line().x2(), 0 );
        grad.setColorAt(0 , QColor(circle.redF()*255, circle.greenF()*255, circle.blueF()*255));
        grad.setColorAt(1 , QColor(circleNext.redF()*255, circleNext.greenF()*255, circleNext.blueF()*255));

        // Set gradient, opacity and pen to the polygon
        poly->setBrush( QBrush( grad ) );
        poly->setOpacity(0.8);
        poly->setPen(QPen(Qt::NoPen));
        poly->setCacheMode( QGraphicsItem::DeviceCoordinateCache );
        poly->setZValue( 1 );

        // Push the polygon back into the lines and polygons vector
        m_linesAndPolygons.push_back(poly);
    }
}

//-----------------------------------------------------------------------------

void TransferFunction::buildLayer()
{
    SLM_TRACE_FUNC();

    // Add graphics items vectors to the layer
    for ( unsigned int i = 0 ; i < m_linesAndPolygons.size() ; i++)
    {
        m_layer->addToGroup(m_linesAndPolygons.at(i));
    }

    for ( unsigned int i = 0 ; i < m_circles.size() ; i++)
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

void TransferFunction::updateImageTF()
{
    // Get the selected tf of the image
    ::fwData::TransferFunction::sptr selectedTF = this->getTransferFunction();
    ::fwData::TransferFunction::TFValueType val;
    // Rebuild the selected tf from the tf points map
    BOOST_FOREACH(::fwData::TransferFunction::TFDataType::value_type elt,  m_TFPoints)
    {
        val = elt.first * m_window + (m_level - m_window/2);
        selectedTF->addTFColor(val, elt.second);
    }

    // Update image window and level
    //::fwComEd::fieldHelper::MedicalImageHelpers::updateMinMaxFromTF( ::fwData::Image::dynamicCast(this->getObject()) );
    this->notifyTFWindowing(this->getSptr());
}

//-----------------------------------------------------------------------------

void TransferFunction::doStart() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    // Initialize the layer and the circle height and width
    m_layer = new QGraphicsItemGroup();

    m_linePen.setCosmetic( true );
    m_linePen.setWidthF( 1.2f );

    m_circlePen.setCosmetic( true );
    m_circlePen.setWidthF( 1.2f );

    this->doUpdate();
}

//-----------------------------------------------------------------------------

void TransferFunction::doUpdate() throw ( ::fwTools::Failed )
{
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    this->updateImageInfos(image);

    // Build the tf map points, the circles vector, the lines and polygons vector, add the items to the layer and add it to the scene
    this->buildTFPoints();
    this->buildCircles();
    this->buildLinesAndPolygons();
    this->buildLayer();
}

//-----------------------------------------------------------------------------

void TransferFunction::doUpdate( fwServices::ObjectMsg::csptr _msg) throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    ::fwComEd::ImageMsg::csptr imageMsg = ::fwComEd::ImageMsg::dynamicConstCast(_msg);
    ::fwComEd::TransferFunctionMsg::csptr tfMsg = ::fwComEd::TransferFunctionMsg::dynamicConstCast(_msg);

    if(tfMsg && imageMsg->hasEvent( ::fwComEd::TransferFunctionMsg::WINDOWING ) )
    {
        this->doUpdate();
    }

    if( _msg->hasEvent( ::scene2D::data::ViewportMsg::VALUE_IS_MODIFIED) )
    {
        this->doUpdate();
    }
}

//-----------------------------------------------------------------------------

void TransferFunction::doSwap() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    this->removeTFObserver();
    this->doUpdate();
    this->installTFObserver( this->getSptr() );
}

//-----------------------------------------------------------------------------

void TransferFunction::doStop() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    this->removeTFObserver();
    m_comChannel->stop();
    ::fwServices::registry::ObjectService::unregisterService( m_comChannel );

    // Clear the items vectors and remove the layer (and all its children) from the scene
    for (std::vector< QGraphicsEllipseItem* >::iterator circleIt = m_circles.begin() ; circleIt != m_circles.end() ; ++circleIt )
    {
        delete *circleIt;
    }

    for( std::vector< QGraphicsItem* >::iterator linesPolyIt = m_linesAndPolygons.begin() ;
            linesPolyIt != m_linesAndPolygons.end() ; ++linesPolyIt)
    {
        delete *linesPolyIt;
    }

    this->getScene2DRender()->getScene()->removeItem(m_layer);
    delete m_layer;
}

//-----------------------------------------------------------------------------

void TransferFunction::processInteraction( ::scene2D::data::Event::sptr _event )
{
    SLM_TRACE_FUNC();

    SLM_ASSERT("Sizes of circles vector and tf points map are different", m_TFPoints.size() == m_circles.size());

    // Iterate parallely on the circles vector and the tf points map
    ::fwData::TransferFunction::TFDataType::iterator TFPointIt = m_TFPoints.begin();
    for (std::vector< QGraphicsEllipseItem* >::iterator circleIt = m_circles.begin() ; circleIt != m_circles.end() ; ++circleIt, ++TFPointIt )
    {
        // If there is a double click
        if ( _event->getType() == ::scene2D::data::Event::MouseButtonDoubleClick )
        {
            // Check if the click position is on the circle
            if ( this->coordViewIsInItem( _event->getCoord(), (*circleIt) ) )
            {
                this->doubleClickEvent(circleIt, TFPointIt);

                return;
            }
        }
        // If left button is pressed
        else if ( _event->getType() == ::scene2D::data::Event::MouseButtonPress && _event->getButton() == ::scene2D::data::Event::LeftButton )
        {
            // Check if the click position is on the circle
            if ( this->coordViewIsInItem( _event->getCoord(), (*circleIt) ) )
            {
                this->leftButtonEvent(circleIt, _event);

                return;
            }
        }
        // If a point is captured, this is the good one, and the mouse move
        else if ( m_pointIsCaptured && m_capturedCircle == (*circleIt) && _event->getType() == ::scene2D::data::Event::MouseMove )
        {
            this->mouseMoveEvent(circleIt, TFPointIt, _event);

            return;
        }
        // If a point is captured, this is the good one, and the button is released
        else if ( m_pointIsCaptured && m_capturedCircle == (*circleIt) && _event->getType() == ::scene2D::data::Event::MouseButtonRelease )
        {
            this->mouseButtonReleaseEvent(circleIt, _event);

            return;
        }
        // If right button is pressed
        else if ( _event->getType() == ::scene2D::data::Event::MouseButtonPress && _event->getButton() == ::scene2D::data::Event::RightButton )
        {
            // Check if there is at least 2 points in the tf and if the click position is on the circle
            if ( m_circles.size() > 2 && this->coordViewIsInItem( _event->getCoord(), (*circleIt) ) )
            {
                this->rightButtonEvent(TFPointIt, _event);

                return;
            }
        }
    }
    // After iteration, due to return instruction in it, the events caught aren't on circles
    if ( _event->getType() == ::scene2D::data::Event::MouseButtonDoubleClick && _event->getButton() == ::scene2D::data::Event::LeftButton )
    {
        // If no other event has been catched, a left click means a point creation
        this->doubleClickEvent(_event);
    }

    if( _event->getType() == ::scene2D::data::Event::Resize)
    {
        ::scene2D::data::Size oldSize = _event->getOldSize();
        ::scene2D::data::Size newSize = _event->getSize();

        if( oldSize.getWidth() == -1.0f )
        {
            m_viewInitialSize.first = newSize.getWidth();
        }

        if( oldSize.getHeight() == -1.0f )
        {
            m_viewInitialSize.second = newSize.getHeight();
        }

        this->doUpdate();
    }
}

//-----------------------------------------------------------------------------

void TransferFunction::doubleClickEvent(std::vector< QGraphicsEllipseItem* >::iterator _circleIt, ::fwData::TransferFunction::TFDataType::iterator _TFPointIt)
{
    SLM_TRACE_FUNC();

    // Open a QColorDialog with the selected circle color and the tf point alpha as default rgba color
    QColor circleColor = QColorDialog::getColor(QColor(
        ((QAbstractGraphicsShapeItem*)(*_circleIt))->brush().color().red(),
        ((QAbstractGraphicsShapeItem*)(*_circleIt))->brush().color().green(),
        ((QAbstractGraphicsShapeItem*)(*_circleIt))->brush().color().blue(),
        (*_TFPointIt).second.a*255
        ), this->getScene2DRender()->getView(), QString("Choose the point color"), QColorDialog::ShowAlphaChannel);
    if (circleColor.isValid())
    {
        // If the color is valid, modify the selected tf point color
        (*_TFPointIt).second = ::fwData::TransferFunction::TFColor(circleColor.redF(), circleColor.greenF(), circleColor.blueF(), circleColor.alphaF());

        // Build circles, lines and polygons cause they're modified by the new tf point color
        this->buildCircles();
        this->buildLinesAndPolygons();
        this->buildLayer();

        // Update the selected tf (no need to rescale cause the selected circle position has not changed)
        this->updateImageTF();
    }
}

//-----------------------------------------------------------------------------

void TransferFunction::leftButtonEvent(std::vector< QGraphicsEllipseItem* >::iterator _circleIt, SPTR(::scene2D::data::Event) _event)
{
    SLM_TRACE_FUNC();

    m_pointIsCaptured = true;

    // Store the captured circle in case it's moved
    m_capturedCircle = (*_circleIt);

    //Store the circle coordinates in case it's moved
    m_oldCoord = this->coordViewToCoordItem( _event->getCoord() );

    // Set the selected circle pen to yellow to get a visual feedback that the selected circle is selected
    (*_circleIt)->setPen( QPen( Qt::GlobalColor( Qt::yellow ) ) );

    _event->setAccepted(true);
}

//-----------------------------------------------------------------------------

void TransferFunction::mouseMoveEvent(std::vector< QGraphicsEllipseItem* >::iterator _circleIt,
        ::fwData::TransferFunction::TFDataType::iterator _TFPointIt,
         ::scene2D::data::Event::sptr _event)
{
    SLM_TRACE_FUNC();

    std::vector< QGraphicsEllipseItem* >::iterator lastPoint = m_circles.end();
    --lastPoint;

    // Get the previous circle
    std::vector< QGraphicsEllipseItem* >::iterator previousCircle = _circleIt;
    if (_circleIt != m_circles.begin())
    {
        --previousCircle;
    }

    // Get the next circle
    std::vector< QGraphicsEllipseItem* >::iterator nextCircle = _circleIt;
    if (_circleIt != lastPoint)
    {
        ++nextCircle;
    }

    // Get the actual selected circle coordinates
    ::scene2D::data::Coord newCoord = this->coordViewToCoordItem( _event->getCoord() );

    // Calculate the real coordinates of the previous circle
    double previousXCircleRealPos = (*previousCircle)->rect().x() + (*previousCircle)->pos().x();
    double previousYRealNewPos = (*previousCircle)->rect().y() + (*previousCircle)->pos().y();

    std::pair< double , double > previousValues =
        this->mapSceneToAdaptor(std::pair< double , double >( previousXCircleRealPos , previousYRealNewPos ) , m_xAxis, m_yAxis);


    // Calculate the real coordinates of the next circle
    double nextXCircleRealPos = (*nextCircle)->rect().x() + (*nextCircle)->pos().x();
    double nextYRealNewPos = (*nextCircle)->rect().y() + (*nextCircle)->pos().y();

    std::pair< double , double > nextValues =
        this->mapSceneToAdaptor(std::pair< double , double >( nextXCircleRealPos , nextYRealNewPos ) , m_xAxis, m_yAxis);


    // Calculate the real coordinates of the selected circle
    double circleXRealNewPos = (*_circleIt)->rect().x() + (*_circleIt)->pos().x() + newCoord.getX() - m_oldCoord.getX();
    double circleYRealNewPos = (*_circleIt)->rect().y() + (*_circleIt)->pos().y() + newCoord.getY() - m_oldCoord.getY();

    std::pair< double , double > realValues =
        this->mapSceneToAdaptor(std::pair< double , double >( circleXRealNewPos , circleYRealNewPos ) , m_xAxis, m_yAxis);

    std::pair<double, double> oldCoordPair =
        this->mapSceneToAdaptor(std::pair<double, double>( m_oldCoord.getX(), m_oldCoord.getY() ), m_xAxis, m_yAxis);

    std::pair<double, double> newCoordPair =
        this->mapSceneToAdaptor(std::pair<double, double>( newCoord.getX(), newCoord.getY() ), m_xAxis, m_yAxis);


    // Check if the mouse isn't out of bounds vertically and horizontally
    if (   (_circleIt == m_circles.begin() || realValues.first > previousValues.first)
        && (_circleIt == lastPoint || realValues.first < nextValues.first)
        && (realValues.second - m_circleHeight/2) >= 0
        && (realValues.second - m_circleHeight/2) <= 1 )
    {
        // Move the selected tf point by the difference between the old coordinates and the new ones
        (*_circleIt)->moveBy( newCoord.getX() - m_oldCoord.getX(), newCoord.getY() - m_oldCoord.getY() );

        // Set the actual coordinates as the old ones
        m_oldCoord = newCoord;
    }
    else
    {
        // Check if the mouse is out of bounds only horizontally
        if (  ((_circleIt != m_circles.begin() && realValues.first < previousValues.first)
                    || (_circleIt != lastPoint && realValues.first > nextValues.first))
                && (realValues.second - m_circleHeight/2) >= 0
                && (realValues.second - m_circleHeight/2) <= 1 )
        {
            // new abcissa of the moving TF point
            double x = (newCoordPair.first > oldCoordPair.first) ? (nextValues.first - 1) : (previousValues.first + 1);
            x = this->mapAdaptorToScene(std::pair<double, double>( x, 0 ), m_xAxis, m_yAxis).first;

            QRectF rect = (*_circleIt)->rect();
            const double width = rect.width();
            rect.setX( x );
            rect.setWidth( width );
            (*_circleIt)->setRect( rect );

            (*_circleIt)->setPos( 0, (*_circleIt)->pos().y() + (newCoord.getY() - m_oldCoord.getY()) );

            m_oldCoord.setX( x );
            m_oldCoord.setY(newCoord.getY());
        }

        // Check if the mouse is out of bounds only vertically
        if (  (_circleIt == m_circles.begin() || realValues.first > previousValues.first)
                && (_circleIt == lastPoint || realValues.first < nextValues.first)
                && ((realValues.second - m_circleHeight/2) < 0 || (realValues.second - m_circleHeight/2) > 1) ) // opacity
        {
            // If the mouse is vertically out of bounds, the TF point is moved to fit the nearest vertical bound (0 or 1).

            double y = 0;   // new ordinate of the moving TF point
            ViewSizeRatio ratio = this->getViewSizeRatio();

            if( newCoordPair.second > oldCoordPair.second )
            {
                // Mouse's move goes toward (and outside) the top of the scene: the ordinate of the circle must be set to 1,
                // which is the highest value for opacity
                y = this->mapAdaptorToScene(std::pair<double, double>( 0, 1 ), m_xAxis, m_yAxis).second;
            }

            QRectF rect = (*_circleIt)->rect();
            const double height = rect.height();
            rect.setY( y - m_circleHeight / 2 );
            rect.setHeight( height );
            (*_circleIt)->setRect( rect );

            (*_circleIt)->setPos( (*_circleIt)->pos().x() + (newCoord.getX() - m_oldCoord.getX()), 0);  // position within item's rect

            m_oldCoord.setX(newCoord.getX());
            m_oldCoord.setY( y );
        }
    }

    this->buildLinesAndPolygons();
    this->buildLayer();

    // Erase the selected tf point cause it's key is const
    m_TFPoints.erase(_TFPointIt);

    // Create a new tf point with the right value (key) and alpha
    std::pair<double, double> pair = this->mapSceneToAdaptor(
            std::pair< double , double >(
                this->pointValue(_circleIt) , (*_circleIt)->rect().y() + (*_circleIt)->pos().y() + m_circleHeight / 2 ) , m_xAxis, m_yAxis);

    m_TFPoints[pair.first] = ::fwData::TransferFunction::TFColor(
        (*_circleIt)->brush().color().redF(),
        (*_circleIt)->brush().color().greenF(),
        (*_circleIt)->brush().color().blueF(),
        pair.second);

    // Need to rescale if the tf point moved was the first or the last one
    this->rescaleTFPoints();

    // Update the image tf
    this->updateImageTF();
}

//-----------------------------------------------------------------------------

void TransferFunction::mouseButtonReleaseEvent(std::vector< QGraphicsEllipseItem* >::iterator _circleIt, SPTR(::scene2D::data::Event) _event)
{
    // Reset the circle pen to the selected circle to get a visual feedback that the circle is no more selected
    (*_circleIt)->setPen(m_circlePen);
    m_pointIsCaptured = false;
    _event->setAccepted(true);
}

//-----------------------------------------------------------------------------

void TransferFunction::rightButtonEvent(::fwData::TransferFunction::TFDataType::iterator _TFPointIt,
                                        ::scene2D::data::Event::sptr _event)
{
    _event->setAccepted(true);

    // Erase the selected tf point
    m_TFPoints.erase(_TFPointIt);

    this->rescaleTFPoints();
    this->updateImageTF();
    this->doUpdate();
}

//-----------------------------------------------------------------------------

void TransferFunction::doubleClickEvent( ::scene2D::data::Event::sptr _event)
{
    ::fwData::TransferFunction::sptr selectedTF = this->getTransferFunction();

    // Get the x and y position in the scene coordinates
    double x = this->getScene2DRender()->mapToScene(_event->getCoord()).getX();
    double y = this->getScene2DRender()->mapToScene(_event->getCoord()).getY();

    // Transform the x and y coordinates with axis scaling and type
    std::pair< double , double > _xy((x - selectedTF->getWLMinMax().first) / m_window , y );
    std::pair< double , double > values = this->mapSceneToAdaptor(_xy , m_xAxis, m_yAxis);

    ::fwData::TransferFunction::TFDataType::iterator nextTFPointIt = m_TFPoints.begin();
    ::fwData::TransferFunction::TFDataType::reverse_iterator lastTFPointIt = m_TFPoints.rbegin();

    if (values.first < (*nextTFPointIt).first)
    {
        ::fwData::TransferFunction::TFColor color((*nextTFPointIt).second.r, (*nextTFPointIt).second.g, (*nextTFPointIt).second.b, values.second);
        m_TFPoints[values.first] = color;

        this->rescaleTFPoints();
        this->updateImageTF();
        this->doUpdate();
    }
    else if (values.first > (*lastTFPointIt).first)
    {
        ::fwData::TransferFunction::TFColor color((*lastTFPointIt).second.r, (*lastTFPointIt).second.g, (*lastTFPointIt).second.b, values.second);
        m_TFPoints[values.first] = color;

        this->rescaleTFPoints();
        this->updateImageTF();
        this->doUpdate();
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
            double coef = (values.first - (*prevTFPointIt).first) / ((*nextTFPointIt).first - (*prevTFPointIt).first);

            // Calculate the new red, green, blue and alpha by linear interpolation in RGBA
            double newRed = coef * ((*nextTFPointIt).second.r - (*prevTFPointIt).second.r) + (*prevTFPointIt).second.r;
            double newGreen = coef * ((*nextTFPointIt).second.g - (*prevTFPointIt).second.g) + (*prevTFPointIt).second.g;
            double newBlue = coef * ((*nextTFPointIt).second.b - (*prevTFPointIt).second.b) + (*prevTFPointIt).second.b;
            double newAlpha = coef * ((*nextTFPointIt).second.a - (*prevTFPointIt).second.a) + (*prevTFPointIt).second.a;

            // Add a point with the right values to the tf points map
            m_TFPoints[values.first] = ::fwData::TransferFunction::TFColor(newRed, newGreen, newBlue, newAlpha);

            this->updateImageTF();
            this->doUpdate();
        }
    }
}

//-----------------------------------------------------------------------------

double TransferFunction::pointValue(std::vector< QGraphicsEllipseItem* >::iterator _circleIt)
{
    SLM_TRACE_FUNC();
    ::fwData::TransferFunction::sptr selectedTF = this->getTransferFunction();

    // Return the x coordinate of the center of a circle in a 0-1 scale
    return (((*_circleIt)->rect().x() + (*_circleIt)->pos().x() + m_circleWidth / 2)
            - selectedTF->getWLMinMax().first) / m_window;
}

//-----------------------------------------------------------------------------

void TransferFunction::rescaleTFPoints()
{
    SLM_TRACE_FUNC();

    double min = (*(m_TFPoints.begin())).first;
    double max = (*(m_TFPoints.rbegin())).first;

    // If min and max are respectively different to 0 and 1, we need to rescale to get a 0-1 scale
    if ( !(min == 0 && max == 1))
    {
        // Calculate the real old values of the selected tf min and max points values
        double realOldMin = m_level - m_window / 2;
        double realOldMax = m_level + m_window / 2;

        // Calculate the real new values of the selected tf min and max points values
        double realNewMin = realOldMin + min * m_window;
        double realNewMax = realOldMax + (max - 1) * m_window;

        // Update the window value
        m_window *= (max - min);

        // Update the level value
        m_level = realNewMin + (realNewMax - realNewMin) / 2;

        // Create a new map to store tf points map values in a 0-1 scale (mandatory cause if we use the same tf points map,
        // values like 0 or 1 could be overwrited)
        ::fwData::TransferFunction::TFDataType newValues;
        ::fwData::TransferFunction::TFDataType::iterator TFPointIt;

        for ( TFPointIt = m_TFPoints.begin() ; TFPointIt != m_TFPoints.end() ; ++TFPointIt )
        {
            newValues[((*TFPointIt).first - min) / (max - min)] = (*TFPointIt).second;
        }

        m_TFPoints.clear();

        // Copy the new map
        m_TFPoints = newValues;
    }
}

//-----------------------------------------------------------------------------

bool TransferFunction::coordViewIsInItem( const ::scene2D::data::Coord & _coord, QGraphicsItem * _item )
{
    // Get the scene coordinates
    ::scene2D::data::Coord scenePoint = this->getScene2DRender()->mapToScene( _coord );
    QPointF sp ( scenePoint.getX(), scenePoint.getY() );
    QPointF ip = _item->mapFromScene( sp );
    // Test if the item contains the coordinates
    return _item->contains( ip );
}

//-----------------------------------------------------------------------------

::scene2D::data::Coord TransferFunction::coordViewToCoordItem( const ::scene2D::data::Coord & _coord )
{
    ::scene2D::data::Coord scenePoint = this->getScene2DRender()->mapToScene( _coord );
    return scenePoint;
}

} // namespace adaptor
} // namespace scene2D



