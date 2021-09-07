/************************************************************************
 *
 * Copyright (C) 2020-2021 IRCAD France
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

#include <data/Composite.hpp>
#include <data/ModelSeries.hpp>
#include <data/Reconstruction.hpp>
#include <data/Vector.hpp>

namespace sight::module::activity::validator
{

namespace ModelSeries
{

fwActivitiesValidatorRegisterMacro(::sight::module::activity::validator::ModelSeries::ContainOneTool);

//-----------------------------------------------------------------------------

ContainOneTool::ContainOneTool(::sight::activity::IValidator::Key key)
{
}

//-----------------------------------------------------------------------------

ContainOneTool::~ContainOneTool()
{
}

//-----------------------------------------------------------------------------

::sight::activity::IValidator::ValidationType ContainOneTool::validate(const data::Object::csptr& currentData) const
{
    IValidator::ValidationType validation;

    validation.first  = true;
    validation.second = "";

    data::ModelSeries::csptr modelSeries = data::ModelSeries::dynamicConstCast(currentData);
    data::Vector::csptr vector           = data::Vector::dynamicConstCast(currentData);
    data::Composite::csptr composite     = data::Composite::dynamicConstCast(currentData);

    if(modelSeries)
    {
        size_t skinCount = 0;
        for(auto rec : modelSeries->getReconstructionDB())
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
        for(auto obj : *vector)
        {
            data::ModelSeries::sptr model = data::ModelSeries::dynamicCast(obj);
            if(!model)
            {
                validation.first  = false;
                validation.second = "Current object should contain only ModelSeries.";
            }
            else
            {
                size_t skinCount = 0;
                for(auto rec : model->getReconstructionDB())
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
        for(auto elt : *composite)
        {
            data::ModelSeries::sptr model = data::ModelSeries::dynamicCast(elt.second);
            if(!model)
            {
                validation.first  = false;
                validation.second = "Current object should contain only ModelSeries.";
            }
            else
            {
                size_t skinCount = 0;
                for(auto rec : model->getReconstructionDB())
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

} // namespace ModelSeries

} // namespace sight::module::activity::validator
