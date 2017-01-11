/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef ANDROID

#include "visuVTKAdaptor/PointLabel.hpp"

#include <fwData/Point.hpp>
#include <fwData/String.hpp>

#include <fwDataTools/fieldHelper/Image.hpp>
#include <fwDataTools/fieldHelper/MedicalImageHelpers.hpp>

#include <fwServices/macros.hpp>

#include <vtkRenderer.h>
#include <vtkTextActor.h>

#include <boost/format.hpp>

#include <sstream>


fwServicesRegisterMacro( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKAdaptor::PointLabel, ::fwData::Point );

namespace visuVTKAdaptor
{

//------------------------------------------------------------------------------

PointLabel::PointLabel() : Text()
{
    m_actor->GetPositionCoordinate()->SetCoordinateSystemToWorld();
    m_actor->GetPosition2Coordinate()->SetCoordinateSystemToWorld();
}

//------------------------------------------------------------------------------

void PointLabel::starting() throw(::fwTools::Failed)
{
    Text::starting();
    doUpdate();
}

//------------------------------------------------------------------------------

void PointLabel::stopping() throw(::fwTools::Failed)
{
    Text::stopping();
}

//------------------------------------------------------------------------------

void PointLabel::doUpdate() throw(::fwTools::Failed)
{
    ::fwData::Point::sptr point = this->getObject< ::fwData::Point >();

    std::string label = point->getField(::fwDataTools::fieldHelper::Image::m_labelId, ::fwData::String::New())->value();

    setText( label );

    double px = point->getCRefCoord()[0];
    double py = point->getCRefCoord()[1];
    double pz = point->getCRefCoord()[2];

    m_actor->GetPositionCoordinate()->SetValue(px,py,pz);
    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

void PointLabel::doSwap() throw(fwTools::Failed)
{
    this->doUpdate();
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsType PointLabel::getObjSrvConnections() const
{
    KeyConnectionsType connections;
    connections.push_back( std::make_pair( ::fwData::Point::s_MODIFIED_SIG, s_UPDATE_SLOT ) );

    return connections;
}

//------------------------------------------------------------------------------

} //namespace visuVTKAdaptor

#endif // ANDROID
