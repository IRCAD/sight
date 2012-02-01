/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwComEd/CompositeMsg.hpp>
#include "scene2D/adaptor/RayCast.hpp"

REGISTER_SERVICE( ::scene2D::adaptor::IAdaptor , ::scene2D::adaptor::RayCast  , ::fwData::Composite ) ;


namespace scene2D
{
namespace adaptor
{

RayCast::RayCast() throw() : m_nbRayon(12), m_crossSize(2.f)
{
    addNewHandledEvent( ::fwComEd::CompositeMsg::ADDED_FIELDS);
    addNewHandledEvent( ::fwComEd::CompositeMsg::SWAPPED_FIELDS);
}

RayCast::~RayCast() throw()
{
}

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
}


void RayCast::doStart() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    // Initialize the layer
    m_layer = new QGraphicsItemGroup();
    m_layer->setPos( m_xAxis->getOrigin(), m_yAxis->getOrigin() );
    m_layer->setZValue( m_zValue );

    this->getScene2DRender()->getScene()->addItem(m_layer);

}

void RayCast::doUpdate() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
}

void RayCast::doUpdate( fwServices::ObjectMsg::csptr _msg) throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    ::fwData::Composite::sptr pCompositeRoot = this->getObject< ::fwData::Composite>();
    SLM_ASSERT ("no composite baby !", pCompositeRoot);

    if(_msg->hasEvent( ::fwComEd::CompositeMsg::ADDED_FIELDS))
    {
        if(pCompositeRoot->find(m_targetPointsKey) != pCompositeRoot->end())
        {
            ::fwData::Composite::sptr pComposite = ::fwData::Composite::dynamicCast((*pCompositeRoot)[m_targetPointsKey]);
            SLM_ASSERT ("Failed to retrieve key '" << m_targetPointsKey << "'", pComposite);
            fwData::PointList::sptr pointListSegment = fwData::PointList::dynamicCast(pComposite->begin()->second);

            int nbrPoint = pointListSegment->getRefPoints().size();

            SLM_ASSERT ("One segment -> 2 points !", nbrPoint==2);

            fwData::Point::NewSptr leftPoint, rightPoint, centerPoint;

            leftPoint->deepCopy(pointListSegment->getRefPoints()[0]);
            rightPoint->deepCopy(pointListSegment->getRefPoints()[1]);

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
        }
    }
}

void RayCast::doSwap() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
}

void RayCast::doStop() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    // Remove the layer (and therefore its related line item) from the scene
    this->getScene2DRender()->getScene()->removeItem(m_layer);
}

void RayCast::addLine(fwData::Point::NewSptr _point1, fwData::Point::NewSptr _point2 , QGraphicsItemGroup* _layer, QPen& _pen )
{
    QGraphicsLineItem *myLine = new QGraphicsLineItem( 
        _point1->getCRefCoord()[0], _point1->getCRefCoord()[1], 
        _point2->getCRefCoord()[0], _point2->getCRefCoord()[1] );
    
    myLine->setPen(_pen);

    _layer->addToGroup(myLine);
}

void RayCast::addCross(fwData::Point::NewSptr _point, float _size , QGraphicsItemGroup* _layer, QPen& _pen )
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


fwData::PointList::sptr RayCast::findPointRayon(fwData::Point::sptr _leftPoint, 
    fwData::Point::sptr _rightPoint, fwData::Point::sptr _centerPoint, int _rayNbr )
{

    fwVec3d pointLeft =  _leftPoint->getRefCoord();
    double pointLeftPosY  = _leftPoint->getRefCoord()[1];
    double pointRightPosY = _rightPoint->getRefCoord()[1];

    //SLM_ASSERT("point left and right positions is inapropriate", pointLeftPosY < pointRightPosY );

    double rayValue = (pointRightPosY - pointLeftPosY ) * 0.5f;

    

    for ( int numRay = 0 ; numRay < _rayNbr ; ++numRay )
    {

    }

    fwData::PointList::NewSptr ttu;

    return ttu;


}


} // namespace adaptor
} // namespace scene2D



