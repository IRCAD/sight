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

#include "uiMeasurement/action/AddDistance.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Signals.hpp>

#include <fwCore/base.hpp>

#include <fwData/Boolean.hpp>
#include <fwData/Image.hpp>
#include <fwData/Point.hpp>
#include <fwData/PointList.hpp>
#include <fwData/Vector.hpp>

#include <fwDataTools/fieldHelper/Image.hpp>
#include <fwDataTools/fieldHelper/MedicalImageHelpers.hpp>

#include <fwServices/macros.hpp>

#include <exception>

namespace uiMeasurement
{
namespace action
{

fwServicesRegisterMacro( ::fwGui::IActionSrv, ::uiMeasurement::action::AddDistance, ::fwData::Image );

static const ::fwServices::IService::KeyType s_IMAGE_INOUT = "image";

//------------------------------------------------------------------------------

AddDistance::AddDistance( ) noexcept
{
}

//------------------------------------------------------------------------------

AddDistance::~AddDistance() noexcept
{
}

//------------------------------------------------------------------------------

void AddDistance::updating()
{
    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    if (!::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity(image))
    {
        SLM_WARN("The image '" + s_IMAGE_INOUT + "' is invalid.");
        return;
    }
    ::fwData::Point::sptr pt1 = ::fwData::Point::New();
    std::copy( image->getOrigin().begin(),  image->getOrigin().begin() +3, pt1->getCoord().begin() );

    ::fwData::Point::sptr pt2 = ::fwData::Point::New();
    std::copy( image->getSize().begin(),  image->getSize().begin() +3, pt2->getCoord().begin() );

    std::transform( pt2->getCoord().begin(), pt2->getCoord().end(),
                    image->getSpacing().begin(),
                    pt2->getCoord().begin(),
                    std::multiplies<double>() );
    std::transform( pt2->getCoord().begin(), pt2->getCoord().end(),
                    image->getOrigin().begin(),
                    pt2->getCoord().begin(),
                    std::plus<double>() );

    ::fwData::PointList::sptr pl = ::fwData::PointList::New();

    pl->getPoints().push_back( pt1 );
    pl->getPoints().push_back( pt2 );

    ::fwData::Vector::sptr vectDist;
    vectDist = image->setDefaultField(::fwDataTools::fieldHelper::Image::m_imageDistancesId, ::fwData::Vector::New());

    vectDist->getContainer().push_back(pl);

    // force distance to be shown
    image->setField("ShowDistances",  ::fwData::Boolean::New(true));

    auto sig = image->signal< ::fwData::Image::DistanceAddedSignalType >(::fwData::Image::s_DISTANCE_ADDED_SIG);
    sig->asyncEmit(pl);
}

//------------------------------------------------------------------------------

void AddDistance::configuring()
{
    this->::fwGui::IActionSrv::initialize();
}

//------------------------------------------------------------------------------

void AddDistance::starting()
{
    this->::fwGui::IActionSrv::actionServiceStarting();
}

//------------------------------------------------------------------------------

void AddDistance::stopping()
{
    this->::fwGui::IActionSrv::actionServiceStopping();
}

//------------------------------------------------------------------------------

} // namespace action

} // namespace uiMeasurement
