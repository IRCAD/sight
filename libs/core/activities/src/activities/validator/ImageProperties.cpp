/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "activities/validator/ImageProperties.hpp"

#include "activities/validator/registry/macros.hpp"

#include <data/Composite.hpp>
#include <data/Image.hpp>
#include <data/ImageSeries.hpp>
#include <data/Vector.hpp>

namespace sight::activities
{
namespace validator
{

fwActivitiesValidatorRegisterMacro(
    activities::validator::ImageProperties, "::activities::validator::ImageProperties");

auto fCompare = [](double a, double b) { return std::abs(a - b) < 0.00001; };

//-----------------------------------------------------------------------------

ImageProperties::ImageProperties(activities::IValidator::Key)
{
}

//-----------------------------------------------------------------------------

ImageProperties::~ImageProperties()
{
}

//-----------------------------------------------------------------------------

IValidator::ValidationType ImageProperties::validate(
    const activities::registry::ActivityInfo&,
    const data::Vector::csptr& currentSelection ) const
{
    IValidator::ValidationType validation;

    if(currentSelection->size() > 1)
    {
        validation.first  = true;
        validation.second = "Input images have the same properties.";

        data::ImageSeries::sptr imgSeries0 = data::ImageSeries::dynamicCast((*currentSelection)[0]);
        SLM_ASSERT("Failed to retrieve an image series", imgSeries0);
        data::Image::sptr img0 = imgSeries0->getImage();
        SLM_ASSERT("Failed to retrieve image from image series", img0);

        data::Image::Size size       = img0->getSize2();
        data::Image::Spacing spacing = img0->getSpacing2();
        data::Image::Origin origin   = img0->getOrigin2();

        data::Vector::ContainerType::const_iterator it;
        for(it = currentSelection->begin() + 1; it != currentSelection->end(); ++it)
        {
            data::ImageSeries::sptr imgSeries = data::ImageSeries::dynamicCast(*it);
            SLM_ASSERT("Failed to retrieve an image series", imgSeries);
            data::Image::sptr img = imgSeries->getImage();
            SLM_ASSERT("Failed to retrieve an image data", img);

            if (  size != img->getSize2() ||
                  !std::equal(spacing.begin(), spacing.end(), img->getSpacing2().begin(), fCompare) ||
                  !std::equal(origin.begin(), origin.end(), img->getOrigin2().begin(), fCompare) )
            {
                std::string errorMsg = "Images in selection have not the same properties :\n";
                errorMsg += (size != img->getSize2()) ? "- size\n" : "";
                errorMsg += (spacing != img->getSpacing2()) ? "- spacing\n" : "";
                errorMsg += (origin != img->getOrigin2()) ? "- origin" : "";

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

IValidator::ValidationType ImageProperties::validate(const data::Object::csptr& currentData ) const
{
    IValidator::ValidationType validation;

    data::Vector::csptr vector       = data::Vector::dynamicConstCast(currentData);
    data::Composite::csptr composite = data::Composite::dynamicConstCast(currentData);

    validation.first  = true;
    validation.second = "Input images have the same properties.";

    data::Image::csptr img0;

    if (vector)
    {
        for (data::Object::sptr obj : *vector)
        {
            data::ImageSeries::csptr imgSeries = data::ImageSeries::dynamicConstCast(obj);
            data::Image::csptr img             = data::Image::dynamicConstCast(obj);
            if (imgSeries)
            {
                img = imgSeries->getImage();
            }

            if (img)
            {
                if (!img0)
                {
                    img0 = img;
                }
                else
                {
                    data::Image::Spacing spacing = img0->getSpacing2();
                    data::Image::Origin origin   = img0->getOrigin2();
                    if (  img0->getSize2() != img->getSize2() ||
                          !std::equal(spacing.begin(), spacing.end(), img->getSpacing2().begin(), fCompare) ||
                          !std::equal(origin.begin(), origin.end(), img->getOrigin2().begin(), fCompare) )
                    {
                        std::string errorMsg = "Images in selection have not the same properties :\n";
                        errorMsg += (img0->getSize2() != img->getSize2()) ? "- size\n" : "";
                        errorMsg += (img0->getSpacing2() != img->getSpacing2()) ? "- spacing\n" : "";
                        errorMsg += (img0->getOrigin2() != img->getOrigin2()) ? "- origin" : "";

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
    else if (composite)
    {
        for (auto elt : *composite)
        {
            data::ImageSeries::sptr imgSeries = data::ImageSeries::dynamicCast(elt.second);
            data::Image::sptr img             = data::Image::dynamicCast(elt.second);
            if (imgSeries)
            {
                img = imgSeries->getImage();
            }

            if (img)
            {
                if (!img0)
                {
                    img0 = img;
                }
                else
                {
                    data::Image::Spacing spacing = img0->getSpacing2();
                    data::Image::Origin origin   = img0->getOrigin2();
                    if (  img0->getSize2() != img->getSize2() ||
                          !std::equal(spacing.begin(), spacing.end(), img->getSpacing2().begin(), fCompare) ||
                          !std::equal(origin.begin(), origin.end(), img->getOrigin2().begin(), fCompare) )
                    {
                        std::string errorMsg = "Images in selection have not the same properties :\n";
                        errorMsg += (img0->getSize2() != img->getSize2()) ? "- size\n" : "";
                        errorMsg += (img0->getSpacing2() != img->getSpacing2()) ? "- spacing\n" : "";
                        errorMsg += (img0->getOrigin2() != img->getOrigin2()) ? "- origin" : "";

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

} // namespace validator
} // namespace sight::activities
