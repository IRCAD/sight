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

#include "ContainOneSkin.hpp"

#include <activity/validator/registry/macros.hpp>

#include <data/Composite.hpp>
#include <data/ModelSeries.hpp>
#include <data/Reconstruction.hpp>
#include <data/Vector.hpp>

namespace sight::module::activity::validator::ModelSeries
{

SIGHT_REGISTER_ACTIVITY_VALIDATOR(sight::module::activity::validator::ModelSeries::ContainOneSkin);

//-----------------------------------------------------------------------------

ContainOneSkin::~ContainOneSkin()
= default;

//-----------------------------------------------------------------------------

sight::activity::validator::return_t ContainOneSkin::validate(const data::Object::csptr& currentData) const
{
    sight::activity::validator::return_t validation;

    validation.first  = true;
    validation.second = "";

    data::ModelSeries::csptr modelSeries = std::dynamic_pointer_cast<const data::ModelSeries>(currentData);
    data::Vector::csptr vector           = std::dynamic_pointer_cast<const data::Vector>(currentData);
    data::Composite::csptr composite     = std::dynamic_pointer_cast<const data::Composite>(currentData);

    if(modelSeries)
    {
        std::size_t skinCount = 0;
        for(const auto& rec : modelSeries->getReconstructionDB())
        {
            if(rec->getStructureType() == "Skin")
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
            data::ModelSeries::sptr model = std::dynamic_pointer_cast<data::ModelSeries>(obj);
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
                    if(rec->getStructureType() == "Skin")
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
            data::ModelSeries::sptr model = std::dynamic_pointer_cast<data::ModelSeries>(elt.second);
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
                    if(rec->getStructureType() == "Skin")
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
