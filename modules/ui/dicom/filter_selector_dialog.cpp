/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "filter_selector_dialog.hpp"

#include <core/com/signal.hpp>
#include <core/com/signal.hxx>
#include <core/com/signals.hpp>

#include <data/composite.hpp>
#include <data/string.hpp>

#include <filter/dicom/filter.hpp>

#include <ui/__/cursor.hpp>
#include <ui/__/dialog/message.hpp>
#include <ui/__/dialog/selector.hpp>

namespace sight::module::ui::dicom
{

//------------------------------------------------------------------------------

filter_selector_dialog::filter_selector_dialog()
= default;

//------------------------------------------------------------------------------

filter_selector_dialog::~filter_selector_dialog() noexcept =
    default;

//------------------------------------------------------------------------------

void filter_selector_dialog::configuring()
{
    m_selected_filters.clear();

    //  config Elem
    //  <selection mode="exclude">
    //  <addSelection filter="sight::filter::dicom::composite::ct_image_storage_default_composite" />
    //  <addSelection filter="sight::filter::dicom::composite::ct_image_storage_default_composite" />

    for(const auto& config : this->get_config())
    {
        SIGHT_INFO("filter_selector_dialog " + config.first);

        if(config.first == "selection")
        {
            const auto mode = config.second.get<std::string>("<xmlattr>.mode");
            m_filters_are_excluded = (mode == "exclude");
            SIGHT_ASSERT(
                "The xml attribute <mode> must be either 'exclude' or 'include'.",
                mode == "exclude"
                || mode == "include"
            );
            SIGHT_DEBUG("mode => " + mode);
        }

        if(config.first == "addSelection")
        {
            m_selected_filters.push_back(config.second.get<std::string>("<xmlattr>.filter"));
        }
    }
}

//------------------------------------------------------------------------------

void filter_selector_dialog::starting()
{
}

//------------------------------------------------------------------------------

void filter_selector_dialog::stopping()
{
}

//------------------------------------------------------------------------------

void filter_selector_dialog::updating()
{
    // Retrieve available filters
    std::vector<sight::filter::dicom::filter::sptr> registred_filters;
    for(const std::string& key : sight::filter::dicom::registry::get()->get_factory_keys())
    {
        sight::filter::dicom::filter::sptr filter = sight::filter::dicom::factory::make(key);
        registred_filters.push_back(filter);
    }

    // filter available extensions and replace id by service description
    std::map<std::string, sight::filter::dicom::filter::sptr> available_filters_map;
    std::vector<std::string> available_filter_names;

    for(const sight::filter::dicom::filter::sptr& filter : registred_filters)
    {
        const bool filter_is_selected_by_user = std::find(
            m_selected_filters.begin(),
            m_selected_filters.end(),
            filter->get_classname()
                                                ) != m_selected_filters.end();

        // Test if the filter is considered here as available by users
        // excluded mode => add filters that are not selected by users
        // included mode => add filters selected by users
        if((m_filters_are_excluded && !filter_is_selected_by_user)
           || (!m_filters_are_excluded && filter_is_selected_by_user))
        {
            // Add this filter
            std::string filter_name = filter->get_name();
            filter_name                        = (filter_name.empty()) ? filter->get_classname() : filter_name;
            available_filters_map[filter_name] = filter;
            available_filter_names.push_back(filter_name);
        }
    }

    // Sort available services (lexical string sort)
    std::sort(available_filter_names.begin(), available_filter_names.end());

    // Test if we have an extension
    if(!available_filter_names.empty())
    {
        std::string filter_name           = *available_filter_names.begin();
        bool filter_selection_is_canceled = false;

        // Selection of extension when availableFilterNames.size() > 1
        if(available_filter_names.size() > 1)
        {
            sight::ui::dialog::selector selector;

            selector.set_title("filter to use");
            selector.set_choices(available_filter_names);

            if(const auto& choices = selector.show(); choices.empty())
            {
                filter_selection_is_canceled = true;
            }
            else
            {
                filter_name = choices.front();
            }

            SIGHT_ASSERT(
                "Unable to find the selected filter name in the filter map.",
                filter_selection_is_canceled
                || available_filters_map.find(filter_name) != available_filters_map.end()
            );
        }

        if(!filter_selection_is_canceled)
        {
            sight::filter::dicom::filter::sptr filter = available_filters_map[filter_name];

            auto obj = m_filter.lock();
            SIGHT_ASSERT("The inout key 'filter' is not correctly set.", obj);

            obj->set_value(filter->get_classname());

            auto sig =
                obj->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
            {
                core::com::connection::blocker block(sig->get_connection(slot(service::slots::UPDATE)));
                sig->async_emit();
            }
        }
    }
    else
    {
        SIGHT_WARN("filter_selector_dialog::load : availableFilters is empty.");
        sight::ui::dialog::message message_box;
        message_box.set_title("filter not found");
        message_box.set_message("There is no available filter for this reader.");
        message_box.set_icon(sight::ui::dialog::message::warning);
        message_box.add_button(sight::ui::dialog::message::ok);
        message_box.show();
    }
}

//------------------------------------------------------------------------------

void filter_selector_dialog::info(std::ostream& _sstream)
{
    // Update message
    _sstream << "filter_selector_dialog";
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::dicom
