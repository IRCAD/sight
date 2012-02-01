/* ***** BEGIN LICENSE BLOCK *****
* FW4SPL - Copyright (C) IRCAD, 2009-2010.
* Distributed under the terms of the GNU Lesser General Public License (LGPL) as
* published by the Free Software Foundation.
* ****** END LICENSE BLOCK ****** */

#include <fwComEd/CompositeMsg.hpp>
#include <fwComEd/ImageMsg.hpp>

#include "scene2D/adaptor/RayCast.hpp"
#include "scene2D/data/InitQtPen.hpp"

#include "fwMath/VectorFunctions.hpp"

#ifndef M_PI
#define M_PI           3.14159265358979323846
#endif

REGISTER_SERVICE( ::scene2D::adaptor::IAdaptor , ::scene2D::adaptor::RayCast  , ::fwData::Composite ) ;


namespace scene2D
{
namespace adaptor
{

//---------------------------------------------------------------------------------------------------

RayCast::RayCast() throw()
: m_nbRayon(12), m_crossSize(2.f), m_scaleRatio(1.1f), m_isMoving(false), m_widgetOffset(0, 0)

{
    addNewHandledEvent( ::fwComEd::CompositeMsg::ADDED_FIELDS);
    addNewHandledEvent( ::fwComEd::CompositeMsg::SWAPPED_FIELDS);
    addNewHandledEvent( ::fwComEd::ImageMsg::SLICE_INDEX );
}

//---------------------------------------------------------------------------------------------------

RayCast::~RayCast() throw()
{
}

//---------------------------------------------------------------------------------------------------

void RayCast::configuring() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    SLM_ASSERT("\"config\" tag missing", m_configuration->getName() == "config");

    this->IAdaptor::configuring();

    SLM_TRACE("IAdaptor configuring ok");

    if (!m_configuration->getAttributeValue("targetPoints").empty())
    {
        m_targetPointsKey = m_configuration->getAttributeValue("targetPoints");
    }

    if (!m_configuration->getAttributeValue("color").empty())
    {
        ::scene2D::data::InitQtPen::setPenColor(m_pen, m_configuration->getAttributeValue("color"));
    }

    if( !m_configuration->getAttributeValue("scaleRatio").empty() )
    {
        m_scaleRatio = ::boost::lexical_cast< float >( m_configuration->getAttributeValue("scaleRatio") );
    }

    if( !m_configuration->getAttributeValue("imageKey").empty() )
    {
        m_imageKey = m_configuration->getAttributeValue("imageKey");
    }
}

//---------------------------------------------------------------------------------------------------

void RayCast::doStart() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    m_layer = new QGraphicsItemGroup();

    m_layer->setPos( m_xAxis->getOrigin(), m_yAxis->getOrigin() );
    m_layer->setZValue( m_zValue );

    this->getScene2DRender()->getScene()->addItem(m_layer);

    m_baseLine = new QGraphicsLineItem();
    m_baseLine->setPen( QPen(Qt::red) );

    // Add base line
    m_layer->addToGroup(m_baseLine);
}

//---------------------------------------------------------------------------------------------------

void RayCast::doUpdate() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
}

//---------------------------------------------------------------------------------------------------

void RayCast::doUpdate( fwServices::ObjectMsg::csptr _msg) throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    ::fwData::Composite::sptr pCompositeRoot = this->getObject< ::fwData::Composite>();
    SLM_ASSERT ("no composite baby !", pCompositeRoot);

    if(_msg->hasEvent( ::fwComEd::CompositeMsg::ADDED_FIELDS)
            || _msg->hasEvent( ::fwComEd::CompositeMsg::SWAPPED_FIELDS))
    {
        if(pCompositeRoot->find(m_targetPointsKey) != pCompositeRoot->end())
        {
            ::fwData::Composite::sptr pComposite =
                ::fwData::Composite::dynamicCast((*pCompositeRoot)[m_targetPointsKey]);
            SLM_ASSERT ("Failed to retrieve key '" << m_targetPointsKey << "'", pComposite);

            fwData::PointList::sptr pointListSegment = fwData::PointList::dynamicCast(pComposite->begin()->second);

            const int nbrPoint = pointListSegment->getRefPoints().size();

            SLM_ASSERT ("One segment -> 2 points !", nbrPoint==2);

            fwData::Point::NewSptr leftPoint, rightPoint, centerPoint;

            leftPoint->deepCopy(pointListSegment->getRefPoints()[1]);
            rightPoint->deepCopy(pointListSegment->getRefPoints()[0]);

            leftPoint  = axialToSagittal(leftPoint);
            rightPoint = axialToSagittal(rightPoint);

            addLine(leftPoint, rightPoint , m_layer, QPen(Qt::red ));

            addCross(leftPoint, m_crossSize , m_layer , QPen(Qt::yellow ));
            addCross(rightPoint, m_crossSize , m_layer , QPen(Qt::yellow ));

            fwData::PointList::NewSptr listPoint;
            listPoint->getRefPoints().push_back(leftPoint);
            listPoint->getRefPoints().push_back(rightPoint);

            centerPoint->deepCopy( listPointAveragePosition(listPoint) );

            addCross(centerPoint, m_crossSize , m_layer , QPen(Qt::yellow ));

            fwData::PointList::NewSptr listPointRayon;

            listPointRayon->deepCopy( findPointRayon(leftPoint, rightPoint, centerPoint, m_nbRayon) );

            addLinesFromPoint(centerPoint, listPointRayon , m_layer, QPen(Qt::gray) );

            addCross(listPointRayon, m_crossSize , m_layer , QPen(Qt::blue));
        }
    }
    else if(_msg->hasEvent( ::fwComEd::ImageMsg::SLICE_INDEX))
    {
        ::fwData::Image::sptr img = ::fwData::Image::dynamicCast((*pCompositeRoot)[m_imageKey]);  
        SLM_ASSERT("No image for key '" << m_imageKey << "'", img);

        const unsigned int saggital = img->getFieldSingleElement< ::fwData::Integer>(
                ::fwComEd::Dictionary::m_sagittalSliceIndexId )->value();

        const std::string saggitalAsString = ::boost::lexical_cast< std::string >(saggital);

        fwData::PointList::sptr segment = fwData::PointList::dynamicCast( (*pCompositeRoot)[saggitalAsString] );
        SLM_ASSERT("At least 2 points are required", segment->getRefPoints().size());

        const double y = img->getSize()[1];

        m_baseLine->setLine(
                segment->getRefPoints()[0]->getRefCoord()[0],
                y,
                segment->getRefPoints()[0]->getRefCoord()[0],
                y );
    }
}

//---------------------------------------------------------------------------------------------------
void RayCast::doSwap() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
}

//---------------------------------------------------------------------------------------------------

void RayCast::processInteraction( ::scene2D::data::Event::sptr _event)
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
    
        const QLineF line = m_baseLine->line();
        m_baseLine->setLine( QLineF( 
                line.x1() + offsetX, line.y1() + offsetY,
                line.x2() + offsetX, line.y2() + offsetY) );

        m_pos.setX(coord.getX());
        m_pos.setY(coord.getY());

        this->syncLines();
    }
}

//---------------------------------------------------------------------------------------------------

void RayCast::doStop() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    // Remove the layer (and therefore its related line item) from the scene
    this->getScene2DRender()->getScene()->removeItem(m_layer);
}

//---------------------------------------------------------------------------------------------------
void RayCast::addLine(fwData::Point::NewSptr _point1, fwData::Point::NewSptr _point2 , QGraphicsItemGroup* _layer, QPen _pen )
{
    QGraphicsLineItem *myLine = new QGraphicsLineItem( 
        _point1->getCRefCoord()[0], _point1->getCRefCoord()[1], 
        _point2->getCRefCoord()[0], _point2->getCRefCoord()[1] );

    myLine->setPen(_pen);

    _layer->addToGroup(myLine);
}

//---------------------------------------------------------------------------------------------------
void RayCast::addLinesFromPoint(fwData::Point::NewSptr _point, fwData::PointList::NewSptr _pointList , QGraphicsItemGroup* _layer, QPen _pen )
{
    int nbrPoint = _pointList->getRefPoints().size();

    for ( int numPoint = 0 ; numPoint < nbrPoint ; ++numPoint )
    {
        fwData::Point::sptr monPoint = _pointList->getRefPoints()[numPoint];
        addLine(_point, monPoint, _layer, _pen );
    }
}

//---------------------------------------------------------------------------------------------------
void RayCast::addCross(fwData::Point::NewSptr _point, float _size , QGraphicsItemGroup* _layer, QPen _pen )
{
    QGraphicsLineItem *myLine1 = new QGraphicsLineItem( 
        _point->getCRefCoord()[0]+_size, _point->getCRefCoord()[1], 
        _point->getCRefCoord()[0]-_size, _point->getCRefCoord()[1] );

    QGraphicsLineItem *myLine2 = new QGraphicsLineItem( 
        _point->getCRefCoord()[0], _point->getCRefCoord()[1]+_size, 
        _point->getCRefCoord()[0], _point->getCRefCoord()[1]-_size );

    myLine1->setPen(_pen);
    myLine2->setPen(_pen);

    _layer->addToGroup(myLine1);
    _layer->addToGroup(myLine2);
}

//---------------------------------------------------------------------------------------------------
fwData::Point::sptr RayCast::listPointAveragePosition(fwData::PointList::sptr _pointList)
{
    int nbrPoint = _pointList->getRefPoints().size();

    SLM_ASSERT("nbrPoint must be > 0", nbrPoint > 0);

    double posX = 0.f;
    double posY = 0.f;
    double posZ = 0.f;

    for (int numPoint = 0 ; numPoint < nbrPoint ; ++numPoint )
    {
        fwData::Point::sptr monPoint = _pointList->getRefPoints()[numPoint];

        posX += monPoint->getRefCoord()[0];
        posY += monPoint->getRefCoord()[1];
        posZ += monPoint->getRefCoord()[2];
    }
    posX /= (double) nbrPoint;
    posY /= (double) nbrPoint;
    posZ /= (double) nbrPoint;

    fwData::Point::NewSptr pointAveragePosition(posX, posY, posZ);

    return pointAveragePosition;

}

//---------------------------------------------------------------------------------------------------
void RayCast::addCross(fwData::PointList::NewSptr _pointList, float _size , QGraphicsItemGroup* _layer, QPen _pen )
{
    int nbrPoint = _pointList->getRefPoints().size();

    for ( int numPoint = 0 ; numPoint < nbrPoint ; ++numPoint )
    {
        fwData::Point::sptr monPoint = _pointList->getRefPoints()[numPoint];
        addCross(monPoint, _size , _layer, _pen );
    }
}

//---------------------------------------------------------------------------------------------------
fwData::PointList::sptr RayCast::findPointRayon(fwData::Point::sptr _leftPoint, 
    fwData::Point::sptr _rightPoint, fwData::Point::sptr _centerPoint, int _rayNbr )
{
    fwVec3d pointLeft  =  _leftPoint->getRefCoord();
    fwVec3d pointRight = _rightPoint->getRefCoord();

    fwVec3d pointCenter = 0.5f * (pointLeft + pointRight);

    //SLM_ASSERT("point left and right positions is inapropriate", pointLeftPosY < pointRightPosY );
    double rayValue = (pointRight[0] - pointLeft[0] ) * 0.5f;

    fwData::PointList::NewSptr pointListOut;

    int nbrPortionAngle = _rayNbr +1;

    float valeurDeltaAngle = M_PI / ((float)nbrPortionAngle);

    for ( int numRay = 0 ; numRay < _rayNbr ; ++numRay )
    {
        float angle = (numRay+1) * valeurDeltaAngle;
        float contribX = cos(angle);
        float contribY = -sin(angle);

        fwVec3d directionLive;
        directionLive[0] = contribX;
        directionLive[1] = contribY;
        directionLive[2] = 0;

        fwVec3d translationCentre = directionLive * rayValue;

        fwVec3d positionLive = pointCenter + translationCentre;

        fwData::Point::NewSptr pointLive(positionLive[0], positionLive[1], positionLive[2]);
        pointListOut->getRefPoints().push_back(pointLive);
    }

    return pointListOut;
}

//---------------------------------------------------------------------------------------------------

void RayCast::syncLines()
{
    

}

//---------------------------------------------------------------------------------------------------

void RayCast::syncCrosses()
{
    

}

//---------------------------------------------------------------------------------------------------
fwVec3d RayCast::axialToSagittal(fwVec3d _vecIn)
{
    fwVec3d vecOut;
    vecOut[0] = _vecIn[1];
    vecOut[1] = _vecIn[2];
    vecOut[2] = _vecIn[0];
    return vecOut;
}

//---------------------------------------------------------------------------------------------------
fwData::Point::sptr RayCast::axialToSagittal(fwData::Point::sptr _pointIn)
{
    return fwData::Point::New( _pointIn->getRefCoord()[1], _pointIn->getRefCoord()[2], _pointIn->getRefCoord()[0] );
}

//---------------------------------------------------------------------------------------------------
} // namespace adaptor} // namespace scene2D
}
