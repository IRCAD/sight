/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "visuVTKAdaptor/SPointLabel.hpp"

#include <fwCom/Slots.hxx>

#include <fwData/Point.hpp>
#include <fwData/String.hpp>

#include <fwDataTools/fieldHelper/Image.hpp>
#include <fwDataTools/fieldHelper/MedicalImageHelpers.hpp>

#include <fwServices/macros.hpp>

#include <vtkRenderer.h>
#include <vtkTextActor.h>

#include <sstream>

fwServicesRegisterMacro(::fwRenderVTK::IAdaptor, ::visuVTKAdaptor::SPointLabel);

namespace visuVTKAdaptor
{

const ::fwServices::IService::KeyType SPointLabel::s_POINT_INPUT = "point";
const ::fwCom::Slots::SlotKeyType s_UPDATE_VISIBILITY_SLOT       = "updateVisibility";

//------------------------------------------------------------------------------

SPointLabel::SPointLabel() :
    SText()
{
    m_actor->GetPositionCoordinate()->SetCoordinateSystemToWorld();
    m_actor->GetPosition2Coordinate()->SetCoordinateSystemToWorld();
    newSlot(s_UPDATE_VISIBILITY_SLOT, &SPointLabel::updateVisibility, this);

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

        const double px = point->getCoord()[0];
        const double py = point->getCoord()[1];
        const double pz = point->getCoord()[2];

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

void SPointLabel::updateVisibility(bool isVisible)
{
    m_actor->SetVisibility(isVisible);
    this->setVtkPipelineModified();
    this->requestRender();
}
//------------------------------------------------------------------------------

} //namespace visuVTKAdaptor
