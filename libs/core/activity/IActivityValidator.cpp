/************************************************************************
 *
 * Copyright (C) 2016-2022 IRCAD France
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

#include "activity/IActivityValidator.hpp"

#include "activity/IObjectValidator.hpp"

#include <data/Activity.hpp>
#include <data/Composite.hpp>
#include <data/Vector.hpp>

namespace sight::activity
{

//------------------------------------------------------------------------------

IValidator::ValidationType IActivityValidator::checkRequirements(const data::Activity::csptr& activity)
{
    IValidator::ValidationType validation;
    validation.first  = true;
    validation.second = "";

    activity::extension::ActivityInfo info;
    info = activity::extension::Activity::getDefault()->getInfo(activity->getActivityConfigId());

    data::Composite::csptr composite = activity->getData();

    for(const activity::extension::ActivityRequirement& req : info.requirements)
    {
        if((req.minOccurs == 1 && req.maxOccurs == 1)
           || (req.minOccurs == 0 && req.maxOccurs == 0)
           || req.create) // One object is required
        {
            data::Object::csptr obj = composite->get(req.name);
            if(!obj)
            {
                validation.first   = false;
                validation.second += "\n - The parameter '" + req.name + "' is required but is not defined.";
            }
            else if(obj->getClassname() != req.type)
            {
                validation.first   = false;
                validation.second += "\n - The parameter '" + req.name + "' must be a '" + req.type + "'.";
            }
            else
            {
                IValidator::ValidationType val = sight::activity::IActivityValidator::checkObject(obj, req.validator);
                if(!val.first)
                {
                    validation.first   = false;
                    validation.second += "\n - The parameter '" + req.name + "' is not valid: " + val.second;
                }
            }
        }
        else if(req.container == "vector")
        {
            data::Vector::csptr vector = data::Vector::dynamicConstCast(composite->get(req.name));
            if(!vector)
            {
                validation.first   = false;
                validation.second += "\n - The parameter '" + req.name + "' must be a Vector of '" + req.type + "'.";
            }
            else
            {
                auto nbObj = static_cast<unsigned int>(vector->size());
                if(nbObj < req.minOccurs)
                {
                    validation.first   = false;
                    validation.second += "\n - The parameter '" + req.name + "' must contain at least "
                                         + std::to_string(req.minOccurs) + " objects.";
                }
                else if(nbObj > req.maxOccurs)
                {
                    validation.first   = false;
                    validation.second += "\n - The parameter '" + req.name + "' must contain at most "
                                         + std::to_string(req.maxOccurs) + " objects.";
                }
                else
                {
                    bool isValid = true;
                    for(data::Object::csptr obj : *vector)
                    {
                        if(!obj)
                        {
                            validation.first   = false;
                            validation.second += "\n - The parameter '" + req.name
                                                 + "' must contain valid objects of type '" + req.type + "'.";
                            isValid = false;
                        }

                        if(obj->getClassname() != req.type)
                        {
                            validation.first   = false;
                            validation.second += "\n - The parameter '" + req.name
                                                 + "' must contain only objects of type '" + req.type + "'.";
                            isValid = false;
                        }
                    }

                    if(isValid)
                    {
                        IValidator::ValidationType val = sight::activity::IActivityValidator::checkObject(
                            vector,
                            req.validator
                        );
                        if(!val.first)
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
            auto currentComposite = data::Composite::dynamicConstCast(composite->get(req.name));
            if(!currentComposite)
            {
                validation.first   = false;
                validation.second += "\n - The parameter '" + req.name + "' must be a Composite of '"
                                     + req.type + "'.";
            }
            else
            {
                auto nbObj = static_cast<unsigned int>(currentComposite->size());
                if(nbObj < req.minOccurs)
                {
                    validation.first   = false;
                    validation.second += "\n - The parameter '" + req.name + "' must contain at least "
                                         + std::to_string(req.minOccurs) + " objects.";
                }
                else if(nbObj > req.maxOccurs)
                {
                    validation.first   = false;
                    validation.second += "\n - The parameter '" + req.name + "' must contain at most "
                                         + std::to_string(req.minOccurs) + " objects.";
                }
                else
                {
                    bool isValid = true;

                    for(const auto& elt : *currentComposite)
                    {
                        std::string key        = elt.first;
                        data::Object::sptr obj = elt.second;
                        activity::extension::ActivityRequirementKey reqKey;
                        bool keyIsFound = false;
                        for(const activity::extension::ActivityRequirementKey& keyElt : req.keys)
                        {
                            if(key == keyElt.key)
                            {
                                reqKey     = keyElt;
                                keyIsFound = true;
                            }
                        }

                        if(!keyIsFound)
                        {
                            validation.first   = false;
                            validation.second += "\n - The parameter '" + req.name
                                                 + "' has an invalid key : '" + key + "'.";
                            isValid = false;
                        }
                        else if(!obj)
                        {
                            validation.first   = false;
                            validation.second += "\n - The parameter '" + req.name
                                                 + "' must contain valid objects of type '" + req.type + "'.";
                            isValid = false;
                        }
                        else if(obj->getClassname() != req.type)
                        {
                            validation.first   = false;
                            validation.second += "\n - The parameter '" + req.name
                                                 + "' must contain only objects of type '" + req.type + "'.";
                            isValid = false;
                        }
                    }

                    if(isValid)
                    {
                        IValidator::ValidationType val = sight::activity::IActivityValidator::checkObject(
                            currentComposite,
                            req.validator
                        );
                        if(!val.first)
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

IValidator::ValidationType IActivityValidator::checkObject(
    const data::Object::csptr& object,
    const std::string& validatorImpl
)
{
    activity::IValidator::ValidationType validation;
    validation.first  = true;
    validation.second = "";

    if(validatorImpl.empty())
    {
        validation.first  = true;
        validation.second = "Validator implementation is empty, assuming it is valid.";
    }
    else
    {
        /// Process object validator
        activity::IValidator::sptr validator           = activity::validator::factory::New(validatorImpl);
        activity::IObjectValidator::sptr dataValidator = activity::IObjectValidator::dynamicCast(validator);

        if(!dataValidator)
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

} // namespace sight::activity
