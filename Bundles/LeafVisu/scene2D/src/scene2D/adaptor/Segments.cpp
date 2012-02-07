/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwComEd/PointListMsg.hpp>

#include "scene2D/data/Event.hpp"
#include "scene2D/adaptor/Segments.hpp"
#include "scene2D/data/InitQtPen.hpp"


REGISTER_SERVICE( ::scene2D::adaptor::IAdaptor , ::scene2D::adaptor::Segments  , ::fwData::Vector ) ;


namespace scene2D
{
namespace adaptor
{

Segments::Segments() throw() : m_pen(QColor(Qt::blue)), m_scaleRatio(1.1f), m_isMoving(false), m_widgetOffset(0, 0)
{
    addNewHandledEvent( ::fwComEd::PointListMsg::ELEMENT_ADDED);
    addNewHandledEvent( ::fwComEd::PointListMsg::ELEMENT_REMOVED);
    addNewHandledEvent( ::fwComEd::PointListMsg::ELEMENT_MODIFIED);
}

Segments::~Segments() throw()
{
}

void Segments::configuring() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    SLM_ASSERT("\"config\" tag missing", m_configuration->getName() == "config");

    this->IAdaptor::configuring();

    SLM_TRACE("IAdaptor configuring ok");

    // If the corresponding attributes are present in the config, set the color of the line
    if (!m_configuration->getAttributeValue("color").empty())
    {
        ::scene2D::data::InitQtPen::setPenColor(m_pen, m_configuration->getAttributeValue("color"));
    }

    if( !m_configuration->getAttributeValue("scaleRatio").empty() )
    {
        m_scaleRatio = ::boost::lexical_cast< float >( m_configuration->getAttributeValue("scaleRatio") );
    }
}


void Segments::doStart() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    //get object
    ::fwData::Vector::sptr pVector = this->getObject< ::fwData::Vector>();
    SLM_ASSERT ("no vector baby !", pVector);

    SLM_TRACE_FUNC();

    // Initialize the layer
    m_layer = new QGraphicsItemGroup();
    m_layer->setPos( m_xAxis->getOrigin(), m_yAxis->getOrigin() );
    m_layer->setZValue( m_zValue );
    this->getScene2DRender()->getScene()->addItem(m_layer);

    m_pen.setWidth(1.f);
    m_pen.setCosmetic(true);
    m_pos = m_layer->pos();
}

void Segments::doUpdate() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    ::fwData::Vector::sptr pVector = this->getObject< ::fwData::Vector>();
    SLM_ASSERT ("no vector baby !", pVector);

    m_vecQLine.clear();

    int nbrLineBrised = pVector->getRefContainer().size();

    for ( int numLineBrised = 0 ; numLineBrised < nbrLineBrised ; ++numLineBrised )
    {
        fwData::PointList::sptr maPointList = fwData::PointList::dynamicCast(pVector->getRefContainer()[numLineBrised]);
        int nbrPoint = maPointList->getRefPoints().size();

        SLM_ASSERT("nbrPoint < 2 dans lapointList representing segments", nbrPoint > 1);

        int nbrSegment = nbrPoint-1;

         for ( int numSegment = 0 ; numSegment < nbrSegment ; ++numSegment )
        {
            fwData::Point::sptr point1 = maPointList->getRefPoints()[numSegment];
            fwData::Point::sptr point2 = maPointList->getRefPoints()[numSegment+1];

            QPointF qPoint1(point1->getRefCoord()[0] + m_widgetOffset.first, point1->getRefCoord()[1] + m_widgetOffset.second);
            QPointF qPoint2(point2->getRefCoord()[0] + m_widgetOffset.first, point2->getRefCoord()[1] + m_widgetOffset.second);

            m_vecQLine.push_back(QLineF(qPoint1, qPoint2));
        }
    }

    this->syncLines();
}

void Segments::processInteraction( ::scene2D::data::Event::sptr _event)
{
    const float scale = m_layer->scale();

    ::scene2D::data::Coord coord = this->getScene2DRender()->mapToScene( _event->getCoord() );
    coord.setX( coord.getX() / scale );
    coord.setY( coord.getY() / scale );

    if(_event->getType() == ::scene2D::data::Event::MouseWheelUp)
    {
        m_layer->setScale(m_layer->scale() * m_scaleRatio);
    }
    else if(_event->getType() == ::scene2D::data::Event::MouseWheelDown)
    {
        m_layer->setScale(m_layer->scale() / m_scaleRatio);
    }
    else if(_event->getButton() == ::scene2D::data::Event::MidButton
            && _event->getModifier() == ::scene2D::data::Event::NoModifier)
    {
        if(_event->getType() == ::scene2D::data::Event::MouseButtonPress)
        {
            m_isMoving = true;
            m_pos.setX( coord.getX() );
            m_pos.setY( coord.getY() );
        }
        else if(_event->getType() == ::scene2D::data::Event::MouseButtonRelease)
        {
            m_isMoving = false;
        }
    }
    else if(_event->getType() == ::scene2D::data::Event::MouseMove && m_isMoving)
    {
        const double offsetX = coord.getX() - m_pos.x();
        const double offsetY = coord.getY() - m_pos.y();

        m_widgetOffset.first += offsetX;
        m_widgetOffset.second += offsetY;

        std::vector< QLineF > newLines;
        BOOST_FOREACH(QLineF line, m_vecQLine)
        {
            newLines.push_back(QLineF(
                line.x1() + offsetX, line.y1() + offsetY,
                line.x2() + offsetX, line.y2() + offsetY));
        }

        m_vecQLine = newLines;

        m_pos.setX(coord.getX());
        m_pos.setY(coord.getY());

        this->syncLines();
    }
}

void Segments::syncLines()
{
    BOOST_FOREACH( QGraphicsLineItem* line, m_graphicLines )
    {
        m_layer->removeFromGroup(line);
        delete line;
    }

    m_graphicLines.clear();

    BOOST_FOREACH( QLineF line, m_vecQLine )
    {
        QGraphicsLineItem* graphicLine = new QGraphicsLineItem(line);
        graphicLine->setPen(m_pen);
        graphicLine->setScale(m_layer->scale());

        m_graphicLines.push_back(graphicLine);
        m_layer->addToGroup(graphicLine);
    }

}

void Segments::doUpdate( fwServices::ObjectMsg::csptr _msg) throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    if( _msg->hasEvent( ::fwComEd::PointListMsg::ELEMENT_ADDED))
    {
        this->doUpdate();
    }
}

void Segments::doSwap() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
}

void Segments::doStop() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    // Remove the layer (and therefore its related line item) from the scene
    this->getScene2DRender()->getScene()->removeItem(m_layer);
}

} // namespace adaptor
} // namespace scene2D



