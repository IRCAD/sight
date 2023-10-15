/************************************************************************
 *
 * Copyright (C) 2020-2023 IRCAD France
 * Copyright (C) 2016 IHU Strasbourg
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

#include "ContainOneTool.hpp"

#include <activity/validator/registry/macros.hpp>

#include <data/composite.hpp>
#include <data/model_series.hpp>
#include <data/reconstruction.hpp>
#include <data/vector.hpp>

namespace sight::module::activity::validator::ModelSeries
{

SIGHT_REGISTER_ACTIVITY_VALIDATOR(sight::module::activity::validator::ModelSeries::ContainOneTool);

//-----------------------------------------------------------------------------

sight::activity::validator::return_t ContainOneTool::validate(const data::object::csptr& currentData) const
{
    sight::activity::validator::return_t validation;

    validation.first  = true;
    validation.second = "";

    data::model_series::csptr modelSeries = std::dynamic_pointer_cast<const data::model_series>(currentData);
    data::vector::csptr vector            = std::dynamic_pointer_cast<const data::vector>(currentData);
    data::composite::csptr composite      = std::dynamic_pointer_cast<const data::composite>(currentData);

    if(modelSeries)
    {
        std::size_t skinCount = 0;
        for(const auto& rec : modelSeries->getReconstructionDB())
        {
            if(rec->getStructureType() == "Tool")
            {
                ++skinCount;
            }
        }

        if(skinCount == 1)
        {
            validation.first = true;
        }
        else
        {
            validation.first  = false;
            validation.second = "ModelSeries should contain one and only one Skin, here : " + std::to_string(skinCount);
        }
    }
    else if(vector)
    {
        for(const auto& obj : *vector)
        {
            data::model_series::sptr model = std::dynamic_pointer_cast<data::model_series>(obj);
            if(!model)
            {
                validation.first  = false;
                validation.second = "Current object should contain only ModelSeries.";
            }
            else
            {
                std::size_t skinCount = 0;
                for(const auto& rec : model->getReconstructionDB())
                {
                    if(rec->getStructureType() == "Tool")
                    {
                        ++skinCount;
                    }
                }

                if(skinCount == 1)
                {
                    validation.first &= true;
                }
                else
                {
                    validation.first  = false;
                    validation.second = "ModelSeries should contain one and only one Skin, here : " + std::to_string(
                        skinCount
                    );
                }
            }
        }
    }
    else if(composite)
    {
        for(const auto& elt : *composite)
        {
            data::model_series::sptr model = std::dynamic_pointer_cast<data::model_series>(elt.second);
            if(!model)
            {
                validation.first  = false;
                validation.second = "Current object should contain only ModelSeries.";
            }
            else
            {
                std::size_t skinCount = 0;
                for(const auto& rec : model->getReconstructionDB())
                {
                    if(rec->getStructureType() == "Tool")
                    {
                        ++skinCount;
                    }
                }

                if(skinCount == 1)
                {
                    validation.first &= true;
                }
                else
                {
                    validation.first  = false;
                    validation.second = "ModelSeries should contain one and only one Skin, here : " + std::to_string(
                        skinCount
                    );
                }
            }
        }
    }
    else
    {
        validation.first  = false;
        validation.second = "Current object should be a ModelSeries, a Vector or a Composite.";
    }

    return validation;
}

//-----------------------------------------------------------------------------

} // namespace sight::module::activity::validator::ModelSeries
