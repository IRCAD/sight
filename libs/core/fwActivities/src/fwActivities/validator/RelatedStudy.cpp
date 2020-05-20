/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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
