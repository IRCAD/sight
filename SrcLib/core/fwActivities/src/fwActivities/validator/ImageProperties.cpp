/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwData/Image.hpp>
#include <fwData/Vector.hpp>

#include <fwMedData/ImageSeries.hpp>

#include <fwMath/Compare.hpp>

#include "fwActivities/validator/registry/macros.hpp"
#include "fwActivities/validator/ImageProperties.hpp"

namespace fwActivities
{
namespace validator
{

fwActivitiesValidatorRegisterMacro(
        ::fwActivities::validator::ImageProperties, "::fwActivities::validator::ImageProperties");

//-----------------------------------------------------------------------------

ImageProperties::ImageProperties(::fwActivities::IValidator::Key key)
{}

//-----------------------------------------------------------------------------

ImageProperties::~ImageProperties()
{}

//-----------------------------------------------------------------------------

IValidator::ValidationType ImageProperties::validate(
        const ::fwActivities::registry::ActivityInfo& activityInfo,
        ::fwData::Vector::sptr currentSelection ) const
{
    IValidator::ValidationType validation;

    if(currentSelection->size() > 1)
    {
        validation.first = true;
        validation.second = "Input images have the same properties.";

        ::fwMedData::ImageSeries::sptr imgSeries0 = ::fwMedData::ImageSeries::dynamicCast((*currentSelection)[0]);
        SLM_ASSERT("Failed to retrieve an image series", imgSeries0);
        ::fwData::Image::sptr img0 = imgSeries0->getImage();
        SLM_ASSERT("Failed to retrieve image from image series", img0);

        ::fwData::Image::SizeType size = img0->getSize();
        ::fwData::Image::SpacingType spacing = img0->getSpacing();
        ::fwData::Image::OriginType origin = img0->getOrigin();

        ::fwData::Vector::ContainerType::const_iterator it; 
        for(it = currentSelection->begin() + 1; it != currentSelection->end(); ++it)
        {
            ::fwMedData::ImageSeries::sptr imgSeries = ::fwMedData::ImageSeries::dynamicCast(*it);
            SLM_ASSERT("Failed to retrieve an image series", imgSeries);
            ::fwData::Image::sptr img = imgSeries->getImage();
            SLM_ASSERT("Failed to retrieve an image data", img);

            if (  size != img->getSize() ||
                    !::fwMath::isContainerEqual< const ::fwData::Image::SpacingType >(spacing, img->getSpacing()) ||
                    !::fwMath::isContainerEqual< const ::fwData::Image::OriginType >(origin, img->getOrigin()) )
            {
                std::string errorMsg = "Images in selection have not the same properties :\n";
                errorMsg += (size != img->getSize()) ? "- size\n":"";
                errorMsg += (spacing != img->getSpacing())?"- spacing\n":"";
                errorMsg += (origin != img->getOrigin())?"- origin":"";

                validation.first = false;
                validation.second = errorMsg;

                break;
            }
        }
    }
    else
    {
        validation.first = true;
        validation.second = "Only one data provided to check images properties, assuming validation as ok.";
    }

    return validation;
}

//-----------------------------------------------------------------------------

} // namespace validator
} // namespace fwActivities


