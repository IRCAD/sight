/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <fwData/Image.hpp>
#include <fwData/Vector.hpp>

#include <fwMedData/ImageSeries.hpp>

#include <fwMath/Compare.hpp>
//#include <fwComEd/fieldHelper/MedicalImageHelpers.hpp>

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

        for(int i = 1; i < currentSelection->size(); ++i)
        {
            ::fwMedData::ImageSeries::sptr imgSeries = ::fwMedData::ImageSeries::dynamicCast((*currentSelection)[i]);
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



