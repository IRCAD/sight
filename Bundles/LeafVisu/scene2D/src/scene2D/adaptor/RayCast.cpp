/* ***** BEGIN LICENSE BLOCK *****
* FW4SPL - Copyright (C) IRCAD, 2009-2010.
* Distributed under the terms of the GNU Lesser General Public License (LGPL) as
* published by the Free Software Foundation.
* ****** END LICENSE BLOCK ****** */

#include <fwComEd/CompositeMsg.hpp>
#include "scene2D/adaptor/RayCast.hpp"
#include "fwMath/VectorFunctions.hpp"
#ifndef M_PI
#define M_PI           3.14159265358979323846
#endif

REGISTER_SERVICE( ::scene2D::adaptor::IAdaptor , ::scene2D::adaptor::RayCast  , ::fwData::Composite ) ;


namespace scene2D
{
    namespace adaptor{
//-------------------------------------------------------------------------------------------------------------------------------
        RayCast::RayCast() throw() : m_nbRayon(12), m_crossSize(2.f)
        {
            addNewHandledEvent( ::fwComEd::CompositeMsg::ADDED_FIELDS);
            addNewHandledEvent( ::fwComEd::CompositeMsg::SWAPPED_FIELDS);
        }

//-------------------------------------------------------------------------------------------------------------------------------
        RayCast::~RayCast() throw()
        {
        }

//-------------------------------------------------------------------------------------------------------------------------------
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


//-------------------------------------------------------------------------------------------------------------------------------
        void RayCast::doStart() throw ( ::fwTools::Failed )
        {
            SLM_TRACE_FUNC();

            m_layer = new QGraphicsItemGroup();
            // Initialize the layer    m_layer = new QGraphicsItemGroup();
            m_layer->setPos( m_xAxis->getOrigin(), m_yAxis->getOrigin() );
            m_layer->setZValue( m_zValue );

            this->getScene2DRender()->getScene()->addItem(m_layer);


        }
//-------------------------------------------------------------------------------------------------------------------------------
        void RayCast::doUpdate() throw ( ::fwTools::Failed )
        {
            SLM_TRACE_FUNC();
        }

//-------------------------------------------------------------------------------------------------------------------------------
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
        }

//-------------------------------------------------------------------------------------------------------------------------------
        void RayCast::doSwap() throw ( ::fwTools::Failed )
        {
            SLM_TRACE_FUNC();
        }

//-------------------------------------------------------------------------------------------------------------------------------
        void RayCast::doStop() throw ( ::fwTools::Failed )
        {
            SLM_TRACE_FUNC();

            // Remove the layer (and therefore its related line item) from the scene
            this->getScene2DRender()->getScene()->removeItem(m_layer);
        }

//-------------------------------------------------------------------------------------------------------------------------------
        void RayCast::addLine(fwData::Point::NewSptr _point1, fwData::Point::NewSptr _point2 , QGraphicsItemGroup* _layer, QPen _pen )
        {
            QGraphicsLineItem *myLine = new QGraphicsLineItem( 
                _point1->getCRefCoord()[0], _point1->getCRefCoord()[1], 
                _point2->getCRefCoord()[0], _point2->getCRefCoord()[1] );

            myLine->setPen(_pen);

            _layer->addToGroup(myLine);
        }

//-------------------------------------------------------------------------------------------------------------------------------
        void RayCast::addLinesFromPoint(fwData::Point::NewSptr _point, fwData::PointList::NewSptr _pointList , QGraphicsItemGroup* _layer, QPen _pen )
        {
            int nbrPoint = _pointList->getRefPoints().size();

            for ( int numPoint = 0 ; numPoint < nbrPoint ; ++numPoint )
            {
                fwData::Point::sptr monPoint = _pointList->getRefPoints()[numPoint];
                addLine(_point, monPoint, _layer, _pen );
            }
        }

//-------------------------------------------------------------------------------------------------------------------------------
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

//-------------------------------------------------------------------------------------------------------------------------------
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

//-------------------------------------------------------------------------------------------------------------------------------
        void RayCast::addCross(fwData::PointList::NewSptr _pointList, float _size , QGraphicsItemGroup* _layer, QPen _pen )
        {
            int nbrPoint = _pointList->getRefPoints().size();

            for ( int numPoint = 0 ; numPoint < nbrPoint ; ++numPoint )
            {
                fwData::Point::sptr monPoint = _pointList->getRefPoints()[numPoint];
                addCross(monPoint, _size , _layer, _pen );
            }
        }

//-------------------------------------------------------------------------------------------------------------------------------
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



//-------------------------------------------------------------------------------------------------------------------------------
        fwVec3d RayCast::axialToSagittal(fwVec3d _vecIn)
        {
            fwVec3d vecOut;
            vecOut[0] = _vecIn[1];
            vecOut[1] = _vecIn[2];
            vecOut[2] = _vecIn[0];
            return vecOut;
        }

//-------------------------------------------------------------------------------------------------------------------------------
        fwData::Point::sptr RayCast::axialToSagittal(fwData::Point::sptr _pointIn)
        {
            return fwData::Point::New( _pointIn->getRefCoord()[1], _pointIn->getRefCoord()[2], _pointIn->getRefCoord()[0] );
        }

//-------------------------------------------------------------------------------------------------------------------------------
    } // namespace adaptor} // namespace scene2D
}
