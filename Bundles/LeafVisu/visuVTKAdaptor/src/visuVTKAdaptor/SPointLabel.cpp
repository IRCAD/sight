/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef ANDROID

#include "visuVTKAdaptor/SPointLabel.hpp"

#include <fwData/Point.hpp>
#include <fwData/String.hpp>

#include <fwDataTools/fieldHelper/Image.hpp>
#include <fwDataTools/fieldHelper/MedicalImageHelpers.hpp>

#include <fwServices/macros.hpp>

#include <boost/format.hpp>

#include <vtkRenderer.h>
#include <vtkTextActor.h>

#include <sstream>

fwServicesRegisterMacro(::fwRenderVTK::IAdaptor, ::visuVTKAdaptor::SPointLabel);

namespace visuVTKAdaptor
{

const ::fwServices::IService::KeyType SPointLabel::s_POINT_INPUT = "point";

//------------------------------------------------------------------------------

SPointLabel::SPointLabel() :
    SText()
{
    m_actor->GetPositionCoordinate()->SetCoordinateSystemToWorld();
    m_actor->GetPosition2Coordinate()->SetCoordinateSystemToWorld();
}

//------------------------------------------------------------------------------

void SPointLabel::starting()
{
    this->SText::starting();
    this->updating();
}

//------------------------------------------------------------------------------

void SPointLabel::stopping()
{
    this->SText::stopping();
}

//------------------------------------------------------------------------------

void SPointLabel::updating()
{
    ::fwData::Point::csptr point = this->getInput< ::fwData::Point >(s_POINT_INPUT);
    SLM_ASSERT("Missing point.", point);

    ::fwData::String::sptr strField = point->getField< ::fwData::String >(::fwDataTools::fieldHelper::Image::m_labelId);

    if (strField)
    {
        std::string label = strField->value();

        this->setText( label );

        const double px = point->getCRefCoord()[0];
        const double py = point->getCRefCoord()[1];
        const double pz = point->getCRefCoord()[2];

        m_actor->GetPositionCoordinate()->SetValue(px, py, pz);
        this->setVtkPipelineModified();
        this->requestRender();
    }
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SPointLabel::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push(s_POINT_INPUT, ::fwData::Point::s_MODIFIED_SIG, s_UPDATE_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

} //namespace visuVTKAdaptor

#endif // ANDROID
