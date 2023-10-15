/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "activity/validator/image_properties.hpp"

#include "activity/validator/registry/macros.hpp"

#include <data/composite.hpp>
#include <data/image.hpp>
#include <data/image_series.hpp>
#include <data/vector.hpp>

namespace sight::activity::validator
{

SIGHT_REGISTER_ACTIVITY_VALIDATOR(sight::activity::validator::image_properties);

auto fCompare = [](double a, double b){return std::abs(a - b) < 0.00001;};

//-----------------------------------------------------------------------------

validator::return_t image_properties::validate(
    const activity::extension::activity_info& /*unused*/,
    const data::vector::csptr& currentSelection
) const
{
    validator::return_t validation;

    if(currentSelection->size() > 1)
    {
        validation.first  = true;
        validation.second = "Input images have the same properties.";

        data::image_series::sptr imgSeries0 = std::dynamic_pointer_cast<data::image_series>((*currentSelection)[0]);
        SIGHT_ASSERT("Failed to retrieve an image series", imgSeries0);

        data::image::Size size       = imgSeries0->size();
        data::image::Spacing spacing = imgSeries0->getSpacing();
        data::image::Origin origin   = imgSeries0->getOrigin();

        data::vector::container_type::const_iterator it;
        for(it = currentSelection->begin() + 1 ; it != currentSelection->end() ; ++it)
        {
            data::image_series::sptr imgSeries = std::dynamic_pointer_cast<data::image_series>(*it);
            SIGHT_ASSERT("Failed to retrieve an image series", imgSeries);

            if(size != imgSeries->size()
               || !std::equal(spacing.begin(), spacing.end(), imgSeries->getSpacing().begin(), fCompare)
               || !std::equal(origin.begin(), origin.end(), imgSeries->getOrigin().begin(), fCompare))
            {
                std::string errorMsg = "Images in selection have not the same properties :\n";
                errorMsg += (size != imgSeries->size()) ? "- size\n" : "";
                errorMsg += (spacing != imgSeries->getSpacing()) ? "- spacing\n" : "";
                errorMsg += (origin != imgSeries->getOrigin()) ? "- origin" : "";

                validation.first  = false;
                validation.second = errorMsg;

                break;
            }
        }
    }
    else
    {
        validation.first  = true;
        validation.second = "Only one data provided to check images properties, assuming validation as ok.";
    }

    return validation;
}

//-----------------------------------------------------------------------------

validator::return_t image_properties::validate(const data::object::csptr& currentData) const
{
    validator::return_t validation;

    data::vector::csptr vector       = std::dynamic_pointer_cast<const data::vector>(currentData);
    data::composite::csptr composite = std::dynamic_pointer_cast<const data::composite>(currentData);

    validation.first  = true;
    validation.second = "Input images have the same properties.";

    data::image::csptr img0;

    if(vector)
    {
        for(const data::object::sptr& obj : *vector)
        {
            const auto img = std::dynamic_pointer_cast<const data::image>(obj);

            if(img)
            {
                if(!img0)
                {
                    img0 = img;
                }
                else
                {
                    data::image::Spacing spacing = img0->getSpacing();
                    data::image::Origin origin   = img0->getOrigin();
                    if(img0->size() != img->size()
                       || !std::equal(spacing.begin(), spacing.end(), img->getSpacing().begin(), fCompare)
                       || !std::equal(origin.begin(), origin.end(), img->getOrigin().begin(), fCompare))
                    {
                        std::string errorMsg = "Images in selection have not the same properties :\n";
                        errorMsg += (img0->size() != img->size()) ? "- size\n" : "";
                        errorMsg += (img0->getSpacing() != img->getSpacing()) ? "- spacing\n" : "";
                        errorMsg += (img0->getOrigin() != img->getOrigin()) ? "- origin" : "";

                        validation.first  = false;
                        validation.second = errorMsg;

                        break;
                    }
                }
            }
            else
            {
                validation.first  = false;
                validation.second = "Given data does not contain valid images.";
            }
        }
    }
    else if(composite)
    {
        for(const auto& elt : *composite)
        {
            const auto img = std::dynamic_pointer_cast<data::image>(elt.second);

            if(img)
            {
                if(!img0)
                {
                    img0 = img;
                }
                else
                {
                    data::image::Spacing spacing = img0->getSpacing();
                    data::image::Origin origin   = img0->getOrigin();
                    if(img0->size() != img->size()
                       || !std::equal(spacing.begin(), spacing.end(), img->getSpacing().begin(), fCompare)
                       || !std::equal(origin.begin(), origin.end(), img->getOrigin().begin(), fCompare))
                    {
                        std::string errorMsg = "Images in selection have not the same properties :\n";
                        errorMsg += (img0->size() != img->size()) ? "- size\n" : "";
                        errorMsg += (img0->getSpacing() != img->getSpacing()) ? "- spacing\n" : "";
                        errorMsg += (img0->getOrigin() != img->getOrigin()) ? "- origin" : "";

                        validation.first  = false;
                        validation.second = errorMsg;

                        break;
                    }
                }
            }
            else
            {
                validation.first  = false;
                validation.second = "Given data does not contain valid images.";
            }
        }
    }
    else
    {
        validation.first  = false;
        validation.second = "Given data is not a Vector or Composite of images. The validation can not be performed.";
    }

    return validation;
}

//-----------------------------------------------------------------------------

} // namespace sight::activity::validator
