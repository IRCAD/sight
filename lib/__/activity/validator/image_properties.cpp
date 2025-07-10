/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "activity/validator/image_properties.hpp"

#include <data/image.hpp>
#include <data/image_series.hpp>
#include <data/map.hpp>
#include <data/validator/registry/macros.hpp>
#include <data/vector.hpp>

namespace sight::activity::validator
{

SIGHT_REGISTER_DATA_VALIDATOR(sight::activity::validator::image_properties);

auto f_compare = [](double _a, double _b){return std::abs(_a - _b) < 0.00001;};

//-----------------------------------------------------------------------------

data::validator::return_t image_properties::validate(const data::object::csptr& _current_data) const
{
    data::validator::return_t validation;

    data::vector::csptr vector = std::dynamic_pointer_cast<const data::vector>(_current_data);
    data::map::csptr map       = std::dynamic_pointer_cast<const data::map>(_current_data);

    validation.first  = true;
    validation.second = "Input images have the same properties.";

    data::image::csptr img0;

    if(vector)
    {
        for(const data::object::sptr& obj : *vector)
        {
            const auto img = std::dynamic_pointer_cast<const data::image>(obj);

            if(img)
            {
                if(!img0)
                {
                    img0 = img;
                }
                else
                {
                    data::image::spacing_t spacing = img0->spacing();
                    data::image::origin_t origin   = img0->origin();
                    if(img0->size() != img->size()
                       || !std::equal(spacing.begin(), spacing.end(), img->spacing().begin(), f_compare)
                       || !std::equal(origin.begin(), origin.end(), img->origin().begin(), f_compare))
                    {
                        std::string error_msg = "Images in selection have not the same properties :\n";
                        error_msg += (img0->size() != img->size()) ? "- size\n" : "";
                        error_msg += (img0->spacing() != img->spacing()) ? "- spacing\n" : "";
                        error_msg += (img0->origin() != img->origin()) ? "- origin" : "";

                        validation.first  = false;
                        validation.second = error_msg;

                        break;
                    }
                }
            }
            else
            {
                validation.first  = false;
                validation.second = "Given data does not contain valid images.";
            }
        }
    }
    else if(map)
    {
        for(const auto& elt : *map)
        {
            const auto img = std::dynamic_pointer_cast<data::image>(elt.second);

            if(img)
            {
                if(!img0)
                {
                    img0 = img;
                }
                else
                {
                    data::image::spacing_t spacing = img0->spacing();
                    data::image::origin_t origin   = img0->origin();
                    if(img0->size() != img->size()
                       || !std::equal(spacing.begin(), spacing.end(), img->spacing().begin(), f_compare)
                       || !std::equal(origin.begin(), origin.end(), img->origin().begin(), f_compare))
                    {
                        std::string error_msg = "Images in selection have not the same properties :\n";
                        error_msg += (img0->size() != img->size()) ? "- size\n" : "";
                        error_msg += (img0->spacing() != img->spacing()) ? "- spacing\n" : "";
                        error_msg += (img0->origin() != img->origin()) ? "- origin" : "";

                        validation.first  = false;
                        validation.second = error_msg;

                        break;
                    }
                }
            }
            else
            {
                validation.first  = false;
                validation.second = "Given data does not contain valid images.";
            }
        }
    }
    else
    {
        validation.first  = false;
        validation.second = "Given data is not a Vector or Map of images. The validation can not be performed.";
    }

    return validation;
}

//-----------------------------------------------------------------------------

} // namespace sight::activity::validator
