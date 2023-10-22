/************************************************************************
 *
 * Copyright (C) 2016-2023 IRCAD France
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

#include "activity/validator/activity.hpp"

#include "activity/validator/object.hpp"

#include <data/activity.hpp>
#include <data/composite.hpp>
#include <data/vector.hpp>

namespace sight::activity::validator
{

//------------------------------------------------------------------------------

validator::return_t activity::checkRequirements(const data::activity::csptr& _activity)
{
    validator::return_t validation;
    validation.first  = true;
    validation.second = "";

    extension::activity_info info;
    info = extension::activity::getDefault()->getInfo(_activity->getActivityConfigId());

    for(const extension::activity_requirement& req : info.requirements)
    {
        if((req.minOccurs == 1 && req.maxOccurs == 1)
           || (req.minOccurs == 0 && req.maxOccurs == 0)
           || req.create) // One object is required
        {
            data::object::csptr obj = _activity->get(req.name);
            if(!obj)
            {
                validation.first   = false;
                validation.second += "\n - The parameter '" + req.name + "' is required but is not defined.";
            }
            else if(obj->get_classname() != req.type)
            {
                validation.first   = false;
                validation.second += "\n - The parameter '" + req.name + "' must be a '" + req.type + "'.";
            }
            else
            {
                validator::return_t val = sight::activity::validator::activity::checkObject(obj, req.validator);
                if(!val.first)
                {
                    validation.first   = false;
                    validation.second += "\n - The parameter '" + req.name + "' is not valid: " + val.second;
                }
            }
        }
        else if(req.container == "vector")
        {
            data::vector::csptr vector = std::dynamic_pointer_cast<const data::vector>(_activity->get(req.name));
            if(!vector)
            {
                validation.first   = false;
                validation.second += "\n - The parameter '" + req.name + "' must be a Vector of '" + req.type + "'.";
            }
            else
            {
                auto nb_obj = static_cast<unsigned int>(vector->size());
                if(nb_obj < req.minOccurs)
                {
                    validation.first   = false;
                    validation.second += "\n - The parameter '" + req.name + "' must contain at least "
                                         + std::to_string(req.minOccurs) + " objects.";
                }
                else if(nb_obj > req.maxOccurs)
                {
                    validation.first   = false;
                    validation.second += "\n - The parameter '" + req.name + "' must contain at most "
                                         + std::to_string(req.maxOccurs) + " objects.";
                }
                else
                {
                    bool is_valid = true;
                    for(data::object::csptr obj : *vector)
                    {
                        if(!obj)
                        {
                            validation.first   = false;
                            validation.second += "\n - The parameter '" + req.name
                                                 + "' must contain valid objects of type '" + req.type + "'.";
                            is_valid = false;
                        }

                        if(obj->get_classname() != req.type)
                        {
                            validation.first   = false;
                            validation.second += "\n - The parameter '" + req.name
                                                 + "' must contain only objects of type '" + req.type + "'.";
                            is_valid = false;
                        }
                    }

                    if(is_valid)
                    {
                        validator::return_t val = sight::activity::validator::activity::checkObject(
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
            auto current_composite = std::dynamic_pointer_cast<const data::composite>(_activity->get(req.name));
            if(!current_composite)
            {
                validation.first   = false;
                validation.second += "\n - The parameter '" + req.name + "' must be a Composite of '"
                                     + req.type + "'.";
            }
            else
            {
                auto nb_obj = static_cast<unsigned int>(current_composite->size());
                if(nb_obj < req.minOccurs)
                {
                    validation.first   = false;
                    validation.second += "\n - The parameter '" + req.name + "' must contain at least "
                                         + std::to_string(req.minOccurs) + " objects.";
                }
                else if(nb_obj > req.maxOccurs)
                {
                    validation.first   = false;
                    validation.second += "\n - The parameter '" + req.name + "' must contain at most "
                                         + std::to_string(req.minOccurs) + " objects.";
                }
                else
                {
                    bool is_valid = true;

                    for(const auto& elt : *current_composite)
                    {
                        std::string key        = elt.first;
                        data::object::sptr obj = elt.second;
                        extension::activity_requirement_key req_key;
                        bool key_is_found = false;
                        for(const extension::activity_requirement_key& key_elt : req.keys)
                        {
                            if(key == key_elt.key)
                            {
                                req_key      = key_elt;
                                key_is_found = true;
                            }
                        }

                        if(!key_is_found)
                        {
                            validation.first   = false;
                            validation.second += "\n - The parameter '" + req.name
                                                 + "' has an invalid key : '" + key + "'.";
                            is_valid = false;
                        }
                        else if(!obj)
                        {
                            validation.first   = false;
                            validation.second += "\n - The parameter '" + req.name
                                                 + "' must contain valid objects of type '" + req.type + "'.";
                            is_valid = false;
                        }
                        else if(obj->get_classname() != req.type)
                        {
                            validation.first   = false;
                            validation.second += "\n - The parameter '" + req.name
                                                 + "' must contain only objects of type '" + req.type + "'.";
                            is_valid = false;
                        }
                    }

                    if(is_valid)
                    {
                        validator::return_t val = sight::activity::validator::activity::checkObject(
                            current_composite,
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

validator::return_t activity::checkObject(
    const data::object::csptr& _object,
    const std::string& _validator_impl
)
{
    sight::activity::validator::return_t validation;
    validation.first  = true;
    validation.second = "";

    if(_validator_impl.empty())
    {
        validation.first  = true;
        validation.second = "Validator implementation is empty, assuming it is valid.";
    }
    else
    {
        /// Process object validator
        auto validator      = sight::activity::validator::factory::make(_validator_impl);
        auto data_validator = std::dynamic_pointer_cast<sight::activity::validator::object>(validator);

        if(!data_validator)
        {
            validation.first  = false;
            validation.second = "Validator '" + _validator_impl + "' cannot be instantiated";
        }
        else
        {
            validation = data_validator->validate(_object);
        }
    }

    return validation;
}

//------------------------------------------------------------------------------

} // namespace sight::activity::validator
