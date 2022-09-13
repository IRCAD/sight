/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "activity/validator/ImageProperties.hpp"

#include "activity/validator/registry/macros.hpp"

#include <data/Composite.hpp>
#include <data/Image.hpp>
#include <data/ImageSeries.hpp>
#include <data/Vector.hpp>

namespace sight::activity::validator
{

fwActivitiesValidatorRegisterMacro(sight::activity::validator::ImageProperties);

auto fCompare = [](double a, double b){return std::abs(a - b) < 0.00001;};

//-----------------------------------------------------------------------------

ImageProperties::ImageProperties(activity::IValidator::Key /*unused*/)
{
}

//-----------------------------------------------------------------------------

ImageProperties::~ImageProperties()
= default;

//-----------------------------------------------------------------------------

IValidator::ValidationType ImageProperties::validate(
    const activity::extension::ActivityInfo& /*unused*/,
    const data::Vector::csptr& currentSelection
) const
{
    IValidator::ValidationType validation;

    if(currentSelection->size() > 1)
    {
        validation.first  = true;
        validation.second = "Input images have the same properties.";

        data::ImageSeries::sptr imgSeries0 = data::ImageSeries::dynamicCast((*currentSelection)[0]);
        SIGHT_ASSERT("Failed to retrieve an image series", imgSeries0);

        data::Image::Size size       = imgSeries0->getSize();
        data::Image::Spacing spacing = imgSeries0->getSpacing();
        data::Image::Origin origin   = imgSeries0->getOrigin();

        data::Vector::container_type::const_iterator it;
        for(it = currentSelection->begin() + 1 ; it != currentSelection->end() ; ++it)
        {
            data::ImageSeries::sptr imgSeries = data::ImageSeries::dynamicCast(*it);
            SIGHT_ASSERT("Failed to retrieve an image series", imgSeries);

            if(size != imgSeries->getSize()
               || !std::equal(spacing.begin(), spacing.end(), imgSeries->getSpacing().begin(), fCompare)
               || !std::equal(origin.begin(), origin.end(), imgSeries->getOrigin().begin(), fCompare))
            {
                std::string errorMsg = "Images in selection have not the same properties :\n";
                errorMsg += (size != imgSeries->getSize()) ? "- size\n" : "";
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

IValidator::ValidationType ImageProperties::validate(const data::Object::csptr& currentData) const
{
    IValidator::ValidationType validation;

    data::Vector::csptr vector       = data::Vector::dynamicConstCast(currentData);
    data::Composite::csptr composite = data::Composite::dynamicConstCast(currentData);

    validation.first  = true;
    validation.second = "Input images have the same properties.";

    data::Image::csptr img0;

    if(vector)
    {
        for(const data::Object::sptr& obj : *vector)
        {
            const auto img = data::Image::dynamicConstCast(obj);

            if(img)
            {
                if(!img0)
                {
                    img0 = img;
                }
                else
                {
                    data::Image::Spacing spacing = img0->getSpacing();
                    data::Image::Origin origin   = img0->getOrigin();
                    if(img0->getSize() != img->getSize()
                       || !std::equal(spacing.begin(), spacing.end(), img->getSpacing().begin(), fCompare)
                       || !std::equal(origin.begin(), origin.end(), img->getOrigin().begin(), fCompare))
                    {
                        std::string errorMsg = "Images in selection have not the same properties :\n";
                        errorMsg += (img0->getSize() != img->getSize()) ? "- size\n" : "";
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
            const auto img = data::Image::dynamicCast(elt.second);

            if(img)
            {
                if(!img0)
                {
                    img0 = img;
                }
                else
                {
                    data::Image::Spacing spacing = img0->getSpacing();
                    data::Image::Origin origin   = img0->getOrigin();
                    if(img0->getSize() != img->getSize()
                       || !std::equal(spacing.begin(), spacing.end(), img->getSpacing().begin(), fCompare)
                       || !std::equal(origin.begin(), origin.end(), img->getOrigin().begin(), fCompare))
                    {
                        std::string errorMsg = "Images in selection have not the same properties :\n";
                        errorMsg += (img0->getSize() != img->getSize()) ? "- size\n" : "";
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
