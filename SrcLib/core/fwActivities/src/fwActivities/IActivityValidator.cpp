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

#include "fwActivities/IActivityValidator.hpp"
#include "fwActivities/IObjectValidator.hpp"

#include <fwData/Composite.hpp>
#include <fwData/Vector.hpp>

#include <fwDataCamp/getObject.hpp>

#include <fwMedData/ActivitySeries.hpp>


namespace fwActivities
{

//------------------------------------------------------------------------------

IValidator::ValidationType IActivityValidator::checkRequirements(const ::fwMedData::ActivitySeries::csptr &activity)
const
{
    IValidator::ValidationType validation;
    validation.first  = true;
    validation.second = "";

    ::fwActivities::registry::ActivityInfo info;
    info = ::fwActivities::registry::Activities::getDefault()->getInfo(activity->getActivityConfigId());

    ::fwData::Composite::sptr composite = activity->getData();

    for (::fwActivities::registry::ActivityRequirement req: info.requirements)
    {
        if ((req.minOccurs == 1 && req.maxOccurs == 1) ||
            (req.minOccurs == 0 && req.maxOccurs == 0) ||
            req.create)          // One object is required
        {
            ::fwData::Object::sptr obj = composite->at< ::fwData::Object >(req.name);
            if (!obj)
            {
                validation.first   = false;
                validation.second += "\n - The parameter '" + req.name + "' is required but is not defined.";
            }
            else if (obj->getClassname() != req.type)
            {
                validation.first   = false;
                validation.second += "\n - The parameter '" + req.name + "' must be a '" + req.type + "'.";
            }
            else
            {
                IValidator::ValidationType val = this->checkObject(obj, req.validator);
                if (!val.first)
                {
                    validation.first   = false;
                    validation.second += "\n - The parameter '" + req.name + "' is not valid: " + val.second;
                }
            }
        }
        else if (req.container == "vector")
        {
            ::fwData::Vector::sptr vector = composite->at< ::fwData::Vector >(req.name);
            if (!vector)
            {
                validation.first   = false;
                validation.second += "\n - The parameter '" + req.name + "' must be a Vector of '" + req.type + "'.";
            }
            else
            {
                unsigned int nbObj = static_cast<unsigned int >(vector->size());
                if (nbObj < req.minOccurs)
                {
                    validation.first   = false;
                    validation.second += "\n - The parameter '" + req.name + "' must contain at least " +
                                         std::to_string(req.minOccurs) + " objects.";
                }
                else if (nbObj > req.maxOccurs)
                {
                    validation.first   = false;
                    validation.second += "\n - The parameter '" + req.name + "' must contain at most " +
                                         std::to_string(req.maxOccurs) + " objects.";
                }
                else
                {
                    bool isValid = true;
                    for (::fwData::Object::sptr obj : *vector)
                    {
                        if (!obj)
                        {
                            validation.first   = false;
                            validation.second += "\n - The parameter '" + req.name +
                                                 "' must contain valid objects of type '" + req.type + "'.";
                            isValid = false;
                        }
                        if (obj->getClassname() != req.type)
                        {
                            validation.first   = false;
                            validation.second += "\n - The parameter '" + req.name +
                                                 "' must contain only objects of type '" + req.type + "'.";
                            isValid = false;
                        }
                    }
                    if (isValid)
                    {
                        IValidator::ValidationType val = this->checkObject(vector, req.validator);
                        if (!val.first)
                        {
                            validation.first   = false;
                            validation.second += "\n - The parameter '" + req.name + "' is not valid: " + val.second;
                        }
                    }
                }
            }
        }
        else // container == composite
        {
            ::fwData::Composite::sptr currentComposite = composite->at< ::fwData::Composite >(req.name);
            if (!currentComposite)
            {
                validation.first   = false;
                validation.second += "\n - The parameter '" + req.name + "' must be a Composite of '" +
                                     req.type + "'.";
            }
            else
            {
                unsigned int nbObj = static_cast<unsigned int>(currentComposite->size());
                if (nbObj < req.minOccurs)
                {
                    validation.first   = false;
                    validation.second += "\n - The parameter '" + req.name + "' must contain at least " +
                                         std::to_string(req.minOccurs) + " objects.";
                }
                else if (nbObj > req.maxOccurs)
                {
                    validation.first   = false;
                    validation.second += "\n - The parameter '" + req.name + "' must contain at most " +
                                         std::to_string(req.minOccurs) + " objects.";
                }
                else
                {
                    bool isValid = true;

                    for (auto elt : *currentComposite)
                    {
                        std::string key = elt.first;
                        ::fwData::Object::sptr obj = elt.second;
                        ::fwActivities::registry::ActivityRequirementKey reqKey;
                        bool keyIsFound = false;
                        for (::fwActivities::registry::ActivityRequirementKey keyElt: req.keys)
                        {
                            if (key == keyElt.key)
                            {
                                reqKey     = keyElt;
                                keyIsFound = true;
                            }
                        }
                        if (!keyIsFound)
                        {
                            validation.first   = false;
                            validation.second += "\n - The parameter '" + req.name +
                                                 "' has an invalid key : '" + key + "'.";
                            isValid = false;

                        }
                        else if (!obj)
                        {
                            validation.first   = false;
                            validation.second += "\n - The parameter '" + req.name +
                                                 "' must contain valid objects of type '" + req.type + "'.";
                            isValid = false;
                        }
                        // FIXME We can not validate the type of object extracted with 'camp' path.
                        else if (reqKey.path.empty() && obj->getClassname() != req.type)
                        {
                            validation.first   = false;
                            validation.second += "\n - The parameter '" + req.name +
                                                 "' must contain only objects of type '" + req.type + "'.";
                            isValid = false;
                        }
                    }

                    if (isValid)
                    {
                        IValidator::ValidationType val = this->checkObject(currentComposite, req.validator);
                        if (!val.first)
                        {
                            validation.first   = false;
                            validation.second += "\n - The parameter '" + req.name + "' is not valid: " + val.second;
                        }

                    }
                }
            }
        }
    }

    return validation;
}

//------------------------------------------------------------------------------

IValidator::ValidationType IActivityValidator::checkParameters(const ::fwMedData::ActivitySeries::csptr &activity) const
{
    IValidator::ValidationType validation;
    validation.first  = true;
    validation.second = "";

    ::fwActivities::registry::ActivityInfo info;
    info = ::fwActivities::registry::Activities::getDefault()->getInfo(activity->getActivityConfigId());

    ::fwData::Composite::sptr composite = activity->getData();

    // Check if all the activity config parameters are present
    ::fwActivities::registry::ActivityAppConfig appConfigInfo = info.appConfig;
    for (auto param : appConfigInfo.parameters)
    {
        if (param.isSeshat())
        {
            std::string path = param.by;
            if (path.substr(0,1) == "!")
            {
                path.replace(0, 1, "@");
            }
            ::fwData::Object::sptr obj = ::fwDataCamp::getObject(composite, path);
            if (!obj)
            {
                validation.first   = false;
                validation.second += "\n - invalid sesh@ path : '" + path + "'";
            }
        }
    }

    return validation;
}

//------------------------------------------------------------------------------

IValidator::ValidationType IActivityValidator::checkObject(const ::fwData::Object::csptr &object,
                                                           const std::string &validatorImpl) const
{
    ::fwActivities::IValidator::ValidationType validation;
    validation.first  = true;
    validation.second = "";

    if (validatorImpl.empty())
    {
        validation.first  = true;
        validation.second = "Validator implementation is empty, assuming it is valid.";
    }
    else
    {
        /// Process object validator
        ::fwActivities::IValidator::sptr validator           = ::fwActivities::validator::factory::New(validatorImpl);
        ::fwActivities::IObjectValidator::sptr dataValidator = ::fwActivities::IObjectValidator::dynamicCast(validator);

        if (!dataValidator)
        {
            validation.first  = false;
            validation.second = "Validator '" + validatorImpl + "' cannot be instantiated";
        }
        else
        {
            validation = dataValidator->validate(object);
        }
    }

    return validation;
}

//------------------------------------------------------------------------------

} // namespace fwActivities

