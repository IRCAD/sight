/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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
#include <data/Study.hpp>
#include <data/Vector.hpp>

#include <boost/algorithm/string.hpp>

namespace sight::activity
{
namespace validator
{

fwActivitiesValidatorRegisterMacro(::sight::activity::validator::RelatedStudy);

//-----------------------------------------------------------------------------

RelatedStudy::RelatedStudy(activity::IValidator::Key key)
{
}

//-----------------------------------------------------------------------------

RelatedStudy::~RelatedStudy()
{
}

//-----------------------------------------------------------------------------

IValidator::ValidationType RelatedStudy::validate(
    const activity::extension::ActivityInfo& activityInfo,
    const data::Vector::csptr& currentSelection ) const
{
    IValidator::ValidationType validation;

    if(currentSelection->size() > 1)
    {
        validation.first  = true;
        validation.second = "Selected series refers to the same study.";

        data::Series::sptr seriesRef = data::Series::dynamicCast((*currentSelection)[0]);
        data::Study::sptr studyRef   = seriesRef->getStudy();

        std::string instanceUIDRef = studyRef->getInstanceUID();
        ::boost::algorithm::trim(instanceUIDRef);

        data::Vector::ContainerType::const_iterator it;
        for(it = currentSelection->begin() + 1; it != currentSelection->end(); ++it)
        {
            data::Series::sptr series = data::Series::dynamicCast(*it);
            data::Study::sptr study   = series->getStudy();

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
} // namespace sight::activity
