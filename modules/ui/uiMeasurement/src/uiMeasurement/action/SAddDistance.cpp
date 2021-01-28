/************************************************************************
 *
 * Copyright (C) 2019-2021 IRCAD France
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

#include <core/com/Signals.hpp>

#include <data/Boolean.hpp>
#include <data/Image.hpp>
#include <data/Point.hpp>
#include <data/PointList.hpp>
#include <data/tools/fieldHelper/Image.hpp>
#include <data/tools/fieldHelper/MedicalImageHelpers.hpp>
#include <data/Vector.hpp>

namespace uiMeasurement
{
namespace action
{

static const services::IService::KeyType s_IMAGE_INOUT = "image";

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
    this->::ui::base::IActionSrv::initialize();
}

//------------------------------------------------------------------------------

void SAddDistance::starting()
{
    this->::ui::base::IActionSrv::actionServiceStarting();
}

//------------------------------------------------------------------------------

void SAddDistance::updating()
{
    const auto image = this->getLockedInOut< data::Image >(s_IMAGE_INOUT);

    if(data::tools::fieldHelper::MedicalImageHelpers::checkImageValidity(image.get_shared()))
    {
        const data::Point::sptr pt1 = data::Point::New();
        std::copy( image->getOrigin2().begin(), image->getOrigin2().begin() +3, pt1->getCoord().begin() );

        const data::Point::sptr pt2 = data::Point::New();
        std::copy( image->getSize2().begin(), image->getSize2().begin() +3, pt2->getCoord().begin() );

        std::transform( pt2->getCoord().begin(), pt2->getCoord().end(),
                        image->getSpacing2().begin(),
                        pt2->getCoord().begin(),
                        std::multiplies<double>() );
        std::transform( pt2->getCoord().begin(), pt2->getCoord().end(),
                        image->getOrigin2().begin(),
                        pt2->getCoord().begin(),
                        std::plus<double>() );

        const data::PointList::sptr pl = data::PointList::New();

        pl->getPoints().push_back( pt1 );
        pl->getPoints().push_back( pt2 );

        const data::Vector::sptr vectDist
            = image->setDefaultField(data::tools::fieldHelper::Image::m_imageDistancesId, data::Vector::New());

        vectDist->getContainer().push_back(pl);

        const auto sig =
            image->signal< data::Image::DistanceAddedSignalType >(data::Image::s_DISTANCE_ADDED_SIG);
        sig->asyncEmit(pl);
    }
}

//------------------------------------------------------------------------------

void SAddDistance::stopping()
{
    this->::ui::base::IActionSrv::actionServiceStopping();
}

//------------------------------------------------------------------------------

} // namespace action.

} // namespace uiMeasurement.
