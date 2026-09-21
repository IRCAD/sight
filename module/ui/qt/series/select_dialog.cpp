/************************************************************************
 *
 * Copyright (C) 2022-2026 IRCAD France
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

#include "select_dialog.hpp"

#include <algorithm>
#include <memory>
#include <unordered_set>

namespace sight::module::ui::qt::series
{

//------------------------------------------------------------------------------

select_dialog::select_dialog()
{
    new_signal<signals::selected_t>(signals::IMAGE_SELECTED);
    new_signal<signals::selected_t>(signals::MODEL_SELECTED);
    new_slot(slots::SELECT_IMAGE, &select_dialog::select_image, this);
    new_slot(slots::SELECT_MODEL, &select_dialog::select_model, this);
    new_slot(slots::REMOVE_IMAGE, &select_dialog::remove_image, this);
    new_slot(slots::REMOVE_MODEL, &select_dialog::remove_model, this);
}

//------------------------------------------------------------------------------

void select_dialog::configuring()
{
    m_display_all_models = this->get_config().get<bool>("config.<xmlattr>.display_all_models", false);
}

//------------------------------------------------------------------------------

void select_dialog::starting()
{
}

//-----------------------------------------------------------------------------

void select_dialog::updating()
{
    const auto series_set = m_series_set.lock();

    SIGHT_THROW_IF("Missing input database series", !series_set);

    if(const auto transfer_functions = m_transfer_functions.lock(); transfer_functions)
    {
        std::unordered_set<std::string> image_ids;
        for(const auto& series : *series_set)
        {
            if(const auto image = std::dynamic_pointer_cast<data::image_series>(series); image)
            {
                image_ids.insert(image->get_id());
            }
        }

        for(auto it = transfer_functions->begin() ; it != transfer_functions->end() ; )
        {
            if(!image_ids.contains(it->first))
            {
                it = transfer_functions->erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    if(m_display_all_models)
    {
        update_display_models(*series_set);
    }

    if(series_set->empty())
    {
        m_image.reset();
        m_transfer_function.reset();
        m_model_series.reset();
        return;
    }

    bool has_current_image = false;
    if(const auto current_image = m_image.const_lock(); current_image)
    {
        has_current_image = std::ranges::any_of(
            *series_set,
            [&current_image](const data::series::csptr& _series)
            {
                return _series && _series->get_id() == current_image->get_id();
            });
    }

    bool has_current_model = false;
    if(const auto current_model = m_model_series.const_lock(); current_model)
    {
        has_current_model = std::ranges::any_of(
            *series_set,
            [&current_model](const data::series::csptr& _series)
            {
                return _series && _series->get_id() == current_model->get_id();
            });
    }

    for(const auto& series : *series_set)
    {
        if(!has_current_image)
        {
            if(const auto image_series = std::dynamic_pointer_cast<data::image_series>(series); image_series)
            {
                this->publish_image(image_series);
                has_current_image = true;
            }
        }

        if(!has_current_model)
        {
            if(const auto model_series = std::dynamic_pointer_cast<data::model_series>(series); model_series)
            {
                m_model_series    = model_series;
                has_current_model = true;
                this->async_emit(signals::MODEL_SELECTED);
            }
        }
    }

    if(!has_current_image)
    {
        m_image.reset();
        m_transfer_function.reset();
    }

    if(!has_current_model)
    {
        m_model_series.reset();
    }
}

//-----------------------------------------------------------------------------

void select_dialog::publish_image(const data::image_series::sptr& _image)
{
    data::transfer_function::sptr tf;
    if(const auto transfer_functions = m_transfer_functions.lock(); transfer_functions)
    {
        tf = transfer_functions->get<data::transfer_function>(_image->get_id());
        if(!tf)
        {
            tf                                      = data::transfer_function::create_default_tf(_image->type());
            (*transfer_functions)[_image->get_id()] = tf;
        }
    }

    if(tf && m_transfer_function.lock().get_shared() != tf)
    {
        m_transfer_function = tf;
    }

    if(m_image.const_lock().get_shared() != _image)
    {
        m_image = _image;
        this->async_emit(signals::IMAGE_SELECTED);
    }
}

//-----------------------------------------------------------------------------

void select_dialog::update_display_models(const data::series_set& _series_set)
{
    data::model_series::reconstruction_vector_t reconstructions;

    for(const auto& series : _series_set)
    {
        if(const auto model = std::dynamic_pointer_cast<data::model_series>(series); model)
        {
            const auto& source = model->get_reconstruction_db();
            reconstructions.insert(reconstructions.end(), source.begin(), source.end());
        }
    }

    if(reconstructions.empty())
    {
        m_display_models.reset();
        return;
    }

    const auto display = m_display_models.lock().get_shared();
    if(!display)
    {
        auto combined = std::make_shared<data::model_series>();
        combined->set_reconstruction_db(reconstructions);
        m_display_models = combined;
    }
    else if(display->get_reconstruction_db() != reconstructions)
    {
        display->set_reconstruction_db(reconstructions);
        display->async_emit(data::signals::MODIFIED);
    }
}

//-----------------------------------------------------------------------------

void select_dialog::select_image(std::string _id)
{
    const auto series_set = m_series_set.lock();
    if(!series_set)
    {
        return;
    }

    const auto selected = std::ranges::find_if(
        *series_set,
        [&_id](const data::series::csptr& _series)
        {
            return std::dynamic_pointer_cast<const data::image_series>(_series)
                   && _series->get_id() == _id;
        });

    if(selected != series_set->cend())
    {
        this->publish_image(std::dynamic_pointer_cast<data::image_series>(*selected));
    }
}

//-----------------------------------------------------------------------------

void select_dialog::select_model(std::string _id, bool _visible)
{
    const auto series_set = m_series_set.lock();
    if(!series_set)
    {
        return;
    }

    for(const auto& series : *series_set)
    {
        const auto model_series = std::dynamic_pointer_cast<data::model_series>(series);
        if(!model_series)
        {
            continue;
        }

        const auto& reconstructions = model_series->get_reconstruction_db();
        const auto reconstruction   = std::ranges::find_if(
            reconstructions,
            [&_id](const data::reconstruction::csptr& _reconstruction)
            {
                return _reconstruction && _reconstruction->get_id() == _id;
            });

        if(reconstruction == reconstructions.cend())
        {
            continue;
        }

        bool is_current_model = false;
        {
            const auto current_model = m_model_series.const_lock();
            is_current_model = current_model && current_model->get_id() == model_series->get_id();
        }

        if(!is_current_model)
        {
            m_model_series = model_series;
            this->async_emit(signals::MODEL_SELECTED);
        }

        if((*reconstruction)->get_is_visible() != _visible)
        {
            (*reconstruction)->set_is_visible(_visible);
            (*reconstruction)->async_emit(data::reconstruction::signals::VISIBILITY_MODIFIED, _visible);
        }

        return;
    }
}

//-----------------------------------------------------------------------------

void select_dialog::remove_image(std::string _id)
{
    bool reset_image = false;
    {
        const auto image = m_image.const_lock();
        reset_image = image && image->get_id() == _id;
    }

    {
        const auto series_set = m_series_set.lock();
        if(!series_set)
        {
            return;
        }

        const auto it = std::find_if(
            series_set->cbegin(),
            series_set->cend(),
            [&_id](const data::series::sptr& _series)
            {
                return _series && _series->get_id() == _id;
            });

        if(it != series_set->cend())
        {
            const auto scoped_emitter = series_set->scoped_emit();
            series_set->remove(*it);
        }
    }

    if(reset_image)
    {
        m_image.reset();
        m_transfer_function.reset();
    }

    this->updating();
}

//-----------------------------------------------------------------------------

void select_dialog::remove_model(std::string _id)
{
    bool removed = false;
    {
        const auto series_set = m_series_set.lock();
        if(!series_set)
        {
            return;
        }

        for(const auto& series : *series_set)
        {
            const auto model = std::dynamic_pointer_cast<data::model_series>(series);
            if(!model)
            {
                continue;
            }

            auto reconstructions = model->get_reconstruction_db();
            const auto it        = std::ranges::find_if(
                reconstructions,
                [&_id](const data::reconstruction::sptr& _reconstruction)
                {
                    return _reconstruction && _reconstruction->get_id() == _id;
                });

            if(it == reconstructions.end())
            {
                continue;
            }

            const data::model_series::reconstruction_vector_t removed_reconstructions {*it};
            reconstructions.erase(it);
            model->set_reconstruction_db(reconstructions);
            model->async_emit(data::model_series::signals::RECONSTRUCTIONS_REMOVED, removed_reconstructions);

            if(reconstructions.empty())
            {
                const auto scoped_emitter = series_set->scoped_emit();
                series_set->remove(series);
            }

            removed = true;
            break;
        }
    }

    if(removed)
    {
        this->updating();
    }
}

//-----------------------------------------------------------------------------

void select_dialog::stopping()
{
    m_display_models.reset();
    m_transfer_function.reset();
}

//-----------------------------------------------------------------------------

} // namespace sight::module::ui::qt::series
