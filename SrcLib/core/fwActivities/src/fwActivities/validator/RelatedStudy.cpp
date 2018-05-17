/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwActivities/validator/RelatedStudy.hpp"

#include "fwActivities/validator/registry/macros.hpp"

#include <fwData/Vector.hpp>

#include <fwMedData/Series.hpp>
#include <fwMedData/Study.hpp>

#include <boost/algorithm/string.hpp>

namespace fwActivities
{
namespace validator
{

fwActivitiesValidatorRegisterMacro(
    ::fwActivities::validator::RelatedStudy, "::fwActivities::validator::RelatedStudy");

//-----------------------------------------------------------------------------

RelatedStudy::RelatedStudy(::fwActivities::IValidator::Key key)
{
}

//-----------------------------------------------------------------------------

RelatedStudy::~RelatedStudy()
{
}

//-----------------------------------------------------------------------------

IValidator::ValidationType RelatedStudy::validate(
    const ::fwActivities::registry::ActivityInfo& activityInfo,
    const ::fwData::Vector::csptr& currentSelection ) const
{
    IValidator::ValidationType validation;

    if(currentSelection->size() > 1)
    {
        validation.first  = true;
        validation.second = "Selected series refers to the same study.";

        ::fwMedData::Series::sptr seriesRef = ::fwMedData::Series::dynamicCast((*currentSelection)[0]);
        ::fwMedData::Study::sptr studyRef   = seriesRef->getStudy();

        std::string instanceUIDRef = studyRef->getInstanceUID();
        ::boost::algorithm::trim(instanceUIDRef);

        ::fwData::Vector::ContainerType::const_iterator it;
        for(it = currentSelection->begin() + 1; it != currentSelection->end(); ++it)
        {
            ::fwMedData::Series::sptr series = ::fwMedData::Series::dynamicCast(*it);
            ::fwMedData::Study::sptr study   = series->getStudy();

            std::string instanceUID = study->getInstanceUID();
            ::boost::algorithm::trim(instanceUID);

            if(instanceUIDRef != instanceUID)
            {
                validation.first  = false;
                validation.second = "Selected series do not refer to the same study.";

                break;
            }
        }
    }
    else
    {
        validation.first  = true;
        validation.second = "Only one series provided to check study affiliation, assuming validation as ok.";
    }

    return validation;
}

//-----------------------------------------------------------------------------

} // namespace validator
} // namespace fwActivities
