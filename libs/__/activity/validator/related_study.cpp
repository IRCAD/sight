/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "activity/validator/related_study.hpp"

#include "activity/validator/registry/macros.hpp"

#include <data/series.hpp>
#include <data/vector.hpp>

#include <boost/algorithm/string.hpp>

namespace sight::activity::validator
{

SIGHT_REGISTER_ACTIVITY_VALIDATOR(sight::activity::validator::related_study);

//-----------------------------------------------------------------------------

validator::return_t related_study::validate(
    const activity::extension::activity_info& /*activity_info*/,
    const data::vector::csptr& _current_selection
) const
{
    validator::return_t validation;

    if(_current_selection->size() > 1)
    {
        validation.first  = true;
        validation.second = "Selected series refers to the same study.";

        auto series_ref                    = std::dynamic_pointer_cast<data::series>((*_current_selection)[0]);
        std::string study_instance_uid_ref = series_ref->getStudyInstanceUID();
        boost::algorithm::trim(study_instance_uid_ref);

        data::vector::container_type::const_iterator it;
        for(it = _current_selection->begin() + 1 ; it != _current_selection->end() ; ++it)
        {
            auto series = std::dynamic_pointer_cast<data::series>(*it);

            std::string study_instance_uid = series->getStudyInstanceUID();
            boost::algorithm::trim(study_instance_uid);

            if(study_instance_uid_ref != study_instance_uid)
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
