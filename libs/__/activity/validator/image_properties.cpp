/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include "activity/validator/registry/macros.hpp"

#include <data/image.hpp>
#include <data/image_series.hpp>
#include <data/map.hpp>
#include <data/vector.hpp>

namespace sight::activity::validator
{

SIGHT_REGISTER_ACTIVITY_VALIDATOR(sight::activity::validator::image_properties);

auto f_compare = [](double _a, double _b){return std::abs(_a - _b) < 0.00001;};

//-----------------------------------------------------------------------------

validator::return_t image_properties::validate(
    const activity::extension::activity_info& /*unused*/,
    const data::vector::csptr& _current_selection
) const
{
    validator::return_t validation;

    if(_current_selection->size() > 1)
    {
        validation.first  = true;
        validation.second = "Input images have the same properties.";

        data::image_series::sptr img_series0 = std::dynamic_pointer_cast<data::image_series>((*_current_selection)[0]);
        SIGHT_ASSERT("Failed to retrieve an image series", img_series0);

        data::image::size_t size       = img_series0->size();
        data::image::spacing_t spacing = img_series0->spacing();
        data::image::origin_t origin   = img_series0->origin();

        data::vector::container_t::const_iterator it;
        for(it = _current_selection->begin() + 1 ; it != _current_selection->end() ; ++it)
        {
            data::image_series::sptr img_series = std::dynamic_pointer_cast<data::image_series>(*it);
            SIGHT_ASSERT("Failed to retrieve an image series", img_series);

            if(size != img_series->size()
               || !std::equal(spacing.begin(), spacing.end(), img_series->spacing().begin(), f_compare)
               || !std::equal(origin.begin(), origin.end(), img_series->origin().begin(), f_compare))
            {
                std::string error_msg = "Images in selection have not the same properties :\n";
                error_msg += (size != img_series->size()) ? "- size\n" : "";
                error_msg += (spacing != img_series->spacing()) ? "- spacing\n" : "";
                error_msg += (origin != img_series->origin()) ? "- origin" : "";

                validation.first  = false;
                validation.second = error_msg;

                break;
            }
        }
    }
    else
    {
        validation.first  = true;
        validation.second = "Only one data provided to check images properties, assuming validation as ok.";
    }

    return validation;
}

//-----------------------------------------------------------------------------

validator::return_t image_properties::validate(const data::object::csptr& _current_data) const
{
    validator::return_t validation;

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
