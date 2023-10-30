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

#include "contain_one_skin.hpp"

#include <activity/validator/registry/macros.hpp>

#include <data/composite.hpp>
#include <data/model_series.hpp>
#include <data/reconstruction.hpp>
#include <data/vector.hpp>

namespace sight::module::activity::validator::model_series
{

SIGHT_REGISTER_ACTIVITY_VALIDATOR(sight::module::activity::validator::model_series::contain_one_skin);

//-----------------------------------------------------------------------------

contain_one_skin::~contain_one_skin()
= default;

//-----------------------------------------------------------------------------

sight::activity::validator::return_t contain_one_skin::validate(const data::object::csptr& _current_data) const
{
    sight::activity::validator::return_t validation;

    validation.first  = true;
    validation.second = "";

    data::model_series::csptr model_series = std::dynamic_pointer_cast<const data::model_series>(_current_data);
    data::vector::csptr vector             = std::dynamic_pointer_cast<const data::vector>(_current_data);
    data::composite::csptr composite       = std::dynamic_pointer_cast<const data::composite>(_current_data);

    if(model_series)
    {
        std::size_t skin_count = 0;
        for(const auto& rec : model_series->get_reconstruction_db())
        {
            if(rec->get_structure_type() == "Skin")
            {
                ++skin_count;
            }
        }

        if(skin_count == 1)
        {
            validation.first = true;
        }
        else
        {
            validation.first  = false;
            validation.second = "ModelSeries should contain one and only one Skin, here : "
                                + std::to_string(skin_count);
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
                std::size_t skin_count = 0;
                for(const auto& rec : model->get_reconstruction_db())
                {
                    if(rec->get_structure_type() == "Skin")
                    {
                        ++skin_count;
                    }
                }

                if(skin_count == 1)
                {
                    validation.first &= true;
                }
                else
                {
                    validation.first  = false;
                    validation.second = "ModelSeries should contain one and only one Skin, here : " + std::to_string(
                        skin_count
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
                std::size_t skin_count = 0;
                for(const auto& rec : model->get_reconstruction_db())
                {
                    if(rec->get_structure_type() == "Skin")
                    {
                        ++skin_count;
                    }
                }

                if(skin_count == 1)
                {
                    validation.first &= true;
                }
                else
                {
                    validation.first  = false;
                    validation.second = "ModelSeries should contain one and only one Skin, here : " + std::to_string(
                        skin_count
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

} // namespace sight::module::activity::validator::model_series
