/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "activity/validator/RelatedStudy.hpp"

#include "activity/validator/registry/macros.hpp"

#include <data/Series.hpp>
#include <data/Vector.hpp>

#include <boost/algorithm/string.hpp>

namespace sight::activity::validator
{

fwActivitiesValidatorRegisterMacro(sight::activity::validator::RelatedStudy);

//-----------------------------------------------------------------------------

RelatedStudy::RelatedStudy(activity::IValidator::Key /*unused*/)
{
}

//-----------------------------------------------------------------------------

RelatedStudy::~RelatedStudy()
= default;

//-----------------------------------------------------------------------------

IValidator::ValidationType RelatedStudy::validate(
    const activity::extension::ActivityInfo& /*activityInfo*/,
    const data::Vector::csptr& currentSelection
) const
{
    IValidator::ValidationType validation;

    if(currentSelection->size() > 1)
    {
        validation.first  = true;
        validation.second = "Selected series refers to the same study.";

        auto seriesRef                  = data::Series::dynamicCast((*currentSelection)[0]);
        std::string studyInstanceUIDRef = seriesRef->getStudyInstanceUID();
        boost::algorithm::trim(studyInstanceUIDRef);

        data::Vector::container_type::const_iterator it;
        for(it = currentSelection->begin() + 1 ; it != currentSelection->end() ; ++it)
        {
            auto series = data::Series::dynamicCast(*it);

            std::string studyInstanceUID = series->getStudyInstanceUID();
            boost::algorithm::trim(studyInstanceUID);

            if(studyInstanceUIDRef != studyInstanceUID)
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

} // namespace sight::activity::validator
