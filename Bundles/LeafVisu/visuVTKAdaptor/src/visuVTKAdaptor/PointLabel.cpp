/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include <boost/format.hpp>

#include <fwServices/macros.hpp>
#include <fwData/Point.hpp>
#include <fwData/String.hpp>
#include <fwComEd/Dictionary.hpp>
#include <fwComEd/fieldHelper/MedicalImageHelpers.hpp>
#include <fwComEd/PointMsg.hpp>

#include <vtkRenderer.h>
#include <vtkTextActor.h>

#include "visuVTKAdaptor/PointLabel.hpp"

#include <sstream>


REGISTER_SERVICE( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKAdaptor::PointLabel, ::fwData::Point ) ;

namespace visuVTKAdaptor
{


PointLabel::PointLabel() : Text()
{
    m_actor->GetPositionCoordinate()->SetCoordinateSystemToWorld();
    m_actor->GetPosition2Coordinate()->SetCoordinateSystemToWorld();
    addNewHandlingEvent( ::fwComEd::PointMsg::POINT_IS_MODIFIED );
}

void PointLabel::starting() throw(::fwTools::Failed)
{
    Text::starting();
    doUpdate();
}

void PointLabel::stopping() throw(::fwTools::Failed)
{
    Text::stopping();
}


void PointLabel::doUpdate() throw(::fwTools::Failed)
{
    ::fwData::Point::sptr point = this->getObject< ::fwData::Point >();

    std::string label;
    if ( point->getFieldSize( ::fwComEd::Dictionary::m_labelId ) )
    {
        label = point->getFieldSingleElement< ::fwData::String>( ::fwComEd::Dictionary::m_labelId )->value();
    }

    setText( label );

    double px = point->getCRefCoord()[0];
    double py = point->getCRefCoord()[1];
    double pz = point->getCRefCoord()[2];

    m_actor->GetPositionCoordinate()->SetValue(px,py,pz);
    this->setVtkPipelineModified();
}

void PointLabel::doUpdate( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed)
{
    SLM_ASSERT("ACH : recieve a msg that no concern his object", _msg->getSubject().lock() == this->getObject() );

    ::fwComEd::PointMsg::csptr pointMsg = ::fwComEd::PointMsg::dynamicConstCast( _msg );
    if ( pointMsg && pointMsg->hasEvent( ::fwComEd::PointMsg::POINT_IS_MODIFIED ) )
    {
        this->doUpdate();
    }
}


void PointLabel::doSwap() throw(fwTools::Failed)
{
    this->doUpdate();
}







} //namespace visuVTKAdaptor
