/************************************************************************
 *
 * Copyright (C) 2016 IRCAD France
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

#include "validators/ModelSeries/ContainOneSkin.hpp"

#include <fwActivities/validator/registry/macros.hpp>

#include <fwData/Composite.hpp>
#include <fwData/Reconstruction.hpp>
#include <fwData/Vector.hpp>

#include <fwMedData/ModelSeries.hpp>

namespace validators
{
namespace ModelSeries
{

fwActivitiesValidatorRegisterMacro(::validators::ModelSeries::ContainOneSkin,
                                   "::validators::ModelSeries::ContainOneSkin");

//-----------------------------------------------------------------------------

ContainOneSkin::ContainOneSkin(::fwActivities::IValidator::Key key)
{

}

//-----------------------------------------------------------------------------

ContainOneSkin::~ContainOneSkin()
{

}

//-----------------------------------------------------------------------------


::fwActivities::IValidator::ValidationType ContainOneSkin::validate(const ::fwData::Object::csptr &currentData ) const
{
    IValidator::ValidationType validation;

    validation.first  = true;
    validation.second = "";

    ::fwMedData::ModelSeries::csptr modelSeries = ::fwMedData::ModelSeries::dynamicConstCast(currentData);
    ::fwData::Vector::csptr vector              = ::fwData::Vector::dynamicConstCast(currentData);
    ::fwData::Composite::csptr composite        = ::fwData::Composite::dynamicConstCast(currentData);

    if (modelSeries)
    {
        size_t skinCount = 0;
        for (auto rec: modelSeries->getReconstructionDB())
        {
            if(rec->getStructureType() == "Skin")
            {
                ++skinCount;
            }
        }

        if (skinCount == 1)
        {
            validation.first = true;
        }
        else
        {
            validation.first  = false;
            validation.second = "ModelSeries should contain one and only one Skin, here : " + std::to_string(skinCount);
        }
    }
    else if (vector)
    {
        for (auto obj : *vector)
        {
            ::fwMedData::ModelSeries::sptr model = ::fwMedData::ModelSeries::dynamicCast(obj);
            if (!model)
            {
                validation.first  = false;
                validation.second = "Current object should contain only ModelSeries.";
            }
            else
            {
                size_t skinCount = 0;
                for (auto rec: model->getReconstructionDB())
                {
                    if(rec->getStructureType() == "Skin")
                    {
                        ++skinCount;
                    }
                }

                if (skinCount == 1)
                {
                    validation.first &= true;
                }
                else
                {
                    validation.first  = false;
                    validation.second = "ModelSeries should contain one and only one Skin, here : " + std::to_string(
                        skinCount);
                }
            }
        }
    }
    else if (composite)
    {
        for (auto elt : *composite)
        {
            ::fwMedData::ModelSeries::sptr model = ::fwMedData::ModelSeries::dynamicCast(elt.second);
            if (!model)
            {
                validation.first  = false;
                validation.second = "Current object should contain only ModelSeries.";
            }
            else
            {
                size_t skinCount = 0;
                for (auto rec: model->getReconstructionDB())
                {
                    if(rec->getStructureType() == "Skin")
                    {
                        ++skinCount;
                    }
                }

                if (skinCount == 1)
                {
                    validation.first &= true;
                }
                else
                {
                    validation.first  = false;
                    validation.second = "ModelSeries should contain one and only one Skin, here : " + std::to_string(
                        skinCount);
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
} // namespace validators
