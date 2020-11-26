/************************************************************************
 *
 * Copyright (C) 2019-2020 IRCAD France
 * Copyright (C) 2019-2020 IHU Strasbourg
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

#include "uiMeasurement/action/SAddDistance.hpp"

#include <fwCom/Signals.hpp>

#include <fwData/Boolean.hpp>
#include <fwData/Image.hpp>
#include <fwData/Point.hpp>
#include <fwData/PointList.hpp>
#include <fwData/Vector.hpp>

#include <fwDataTools/fieldHelper/Image.hpp>
#include <fwDataTools/fieldHelper/MedicalImageHelpers.hpp>

namespace uiMeasurement
{
namespace action
{

static const ::fwServices::IService::KeyType s_IMAGE_INOUT = "image";

//------------------------------------------------------------------------------

SAddDistance::SAddDistance( ) noexcept
{
}

//------------------------------------------------------------------------------

SAddDistance::~SAddDistance() noexcept
{
}

//------------------------------------------------------------------------------

void SAddDistance::configuring()
{
    this->::fwGui::IActionSrv::initialize();
}

//------------------------------------------------------------------------------

void SAddDistance::starting()
{
    this->::fwGui::IActionSrv::actionServiceStarting();
}

//------------------------------------------------------------------------------

void SAddDistance::updating()
{
    const auto image = this->getLockedInOut< ::fwData::Image >(s_IMAGE_INOUT);

    if(::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity(image.get_shared()))
    {
        const ::fwData::Point::sptr pt1 = ::fwData::Point::New();
        std::copy( image->getOrigin2().begin(), image->getOrigin2().begin() +3, pt1->getCoord().begin() );

        const ::fwData::Point::sptr pt2 = ::fwData::Point::New();
        std::copy( image->getSize2().begin(), image->getSize2().begin() +3, pt2->getCoord().begin() );

        std::transform( pt2->getCoord().begin(), pt2->getCoord().end(),
                        image->getSpacing2().begin(),
                        pt2->getCoord().begin(),
                        std::multiplies<double>() );
        std::transform( pt2->getCoord().begin(), pt2->getCoord().end(),
                        image->getOrigin2().begin(),
                        pt2->getCoord().begin(),
                        std::plus<double>() );

        const ::fwData::PointList::sptr pl = ::fwData::PointList::New();

        pl->getPoints().push_back( pt1 );
        pl->getPoints().push_back( pt2 );

        const ::fwData::Vector::sptr vectDist
            = image->setDefaultField(::fwDataTools::fieldHelper::Image::m_imageDistancesId, ::fwData::Vector::New());

        vectDist->getContainer().push_back(pl);

        // force distance to be shown
        image->setField(::fwDataTools::fieldHelper::Image::m_distanceVisibility,  ::fwData::Boolean::New(true));

        const auto sig =
            image->signal< ::fwData::Image::DistanceAddedSignalType >(::fwData::Image::s_DISTANCE_ADDED_SIG);
        sig->asyncEmit(pl);
    }
}

//------------------------------------------------------------------------------

void SAddDistance::stopping()
{
    this->::fwGui::IActionSrv::actionServiceStopping();
}

//------------------------------------------------------------------------------

} // namespace action.

} // namespace uiMeasurement.
