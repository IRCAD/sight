/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "validators/ModelSeries/ContainOneTool.hpp"

#include <fwActivities/validator/registry/macros.hpp>

#include <fwData/Composite.hpp>
#include <fwData/Reconstruction.hpp>
#include <fwData/Vector.hpp>

#include <fwMedData/ModelSeries.hpp>

namespace validators
{
namespace ModelSeries
{

fwActivitiesValidatorRegisterMacro(::validators::ModelSeries::ContainOneTool,
                                   "::validators::ModelSeries::ContainOneTool");

//-----------------------------------------------------------------------------

ContainOneTool::ContainOneTool(::fwActivities::IValidator::Key key)
{

}

//-----------------------------------------------------------------------------

ContainOneTool::~ContainOneTool()
{

}

//-----------------------------------------------------------------------------


::fwActivities::IValidator::ValidationType ContainOneTool::validate(const ::fwData::Object::csptr &currentData ) const
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
            if(rec->getStructureType() == "Tool")
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
                    if(rec->getStructureType() == "Tool")
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
                    if(rec->getStructureType() == "Tool")
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
