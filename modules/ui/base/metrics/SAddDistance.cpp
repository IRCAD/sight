/************************************************************************
 *
 * Copyright (C) 2019-2022 IRCAD France
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

#include "SAddDistance.hpp"

#include <core/com/Signals.hpp>

#include <data/Boolean.hpp>
#include <data/helper/MedicalImage.hpp>
#include <data/Point.hpp>
#include <data/PointList.hpp>
#include <data/Vector.hpp>

namespace sight::module::ui::base::metrics
{

//------------------------------------------------------------------------------

SAddDistance::SAddDistance() noexcept
{
}

//------------------------------------------------------------------------------

SAddDistance::~SAddDistance() noexcept
{
}

//------------------------------------------------------------------------------

void SAddDistance::configuring()
{
    this->sight::ui::base::IAction::initialize();
}

//------------------------------------------------------------------------------

void SAddDistance::starting()
{
    this->sight::ui::base::IAction::actionServiceStarting();
}

//------------------------------------------------------------------------------

void SAddDistance::updating()
{
    const auto image = m_image.lock();

    if(data::helper::MedicalImage::checkImageValidity(image.get_shared()))
    {
        const data::Point::sptr pt1 = data::Point::New();
        std::copy(image->getOrigin().begin(), image->getOrigin().begin() + 3, pt1->getCoord().begin());

        const data::Point::sptr pt2 = data::Point::New();
        std::transform(
            image->getSize().begin(),
            image->getSize().begin() + 3,
            pt2->getCoord().begin(),
            boost::numeric_cast<data::Point::PointCoordType, data::Image::Size::value_type>
        );

        std::transform(
            pt2->getCoord().begin(),
            pt2->getCoord().end(),
            image->getSpacing().begin(),
            pt2->getCoord().begin(),
            std::multiplies<double>()
        );
        std::transform(
            pt2->getCoord().begin(),
            pt2->getCoord().end(),
            image->getOrigin().begin(),
            pt2->getCoord().begin(),
            std::plus<double>()
        );

        const data::PointList::sptr pl = data::PointList::New();

        pl->getPoints().push_back(pt1);
        pl->getPoints().push_back(pt2);

        data::Vector::sptr vectDist = data::helper::MedicalImage::getDistances(*image);
        if(!vectDist)
        {
            vectDist = data::Vector::New();
            data::helper::MedicalImage::setDistances(*image, vectDist);
        }

        vectDist->getContainer().push_back(pl);

        const auto sig =
            image->signal<data::Image::DistanceAddedSignalType>(data::Image::s_DISTANCE_ADDED_SIG);
        sig->asyncEmit(pl);
    }
}

//------------------------------------------------------------------------------

void SAddDistance::stopping()
{
    this->sight::ui::base::IAction::actionServiceStopping();
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::base::metrics
