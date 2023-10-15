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
    m_selectedFilters.clear();

    //  config Elem
    //  <selection mode="exclude">
    //  <addSelection filter="sight::filter::dicom::composite::CTImageStorageDefaultComposite" />
    //  <addSelection filter="sight::filter::dicom::composite::CTImageStorageDefaultComposite" />

    for(const auto& config : this->get_config())
    {
        SIGHT_INFO("filter_selector_dialog " + config.first);

        if(config.first == "selection")
        {
            const auto mode = config.second.get<std::string>("<xmlattr>.mode");
            m_filtersAreExcluded = (mode == "exclude");
            SIGHT_ASSERT(
                "The xml attribute <mode> must be either 'exclude' or 'include'.",
                mode == "exclude"
                || mode == "include"
            );
            SIGHT_DEBUG("mode => " + mode);
        }

        if(config.first == "addSelection")
        {
            m_selectedFilters.push_back(config.second.get<std::string>("<xmlattr>.filter"));
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
    std::vector<sight::filter::dicom::filter::sptr> registredFilters;
    for(const std::string& key : sight::filter::dicom::registry::get()->get_factory_keys())
    {
        sight::filter::dicom::filter::sptr filter = sight::filter::dicom::factory::make(key);
        registredFilters.push_back(filter);
    }

    // Filter available extensions and replace id by service description
    std::map<std::string, sight::filter::dicom::filter::sptr> availableFiltersMap;
    std::vector<std::string> availableFilterNames;

    for(const sight::filter::dicom::filter::sptr& filter : registredFilters)
    {
        const bool filterIsSelectedByUser = std::find(
            m_selectedFilters.begin(),
            m_selectedFilters.end(),
            filter->get_classname()
                                            ) != m_selectedFilters.end();

        // Test if the filter is considered here as available by users
        // excluded mode => add filters that are not selected by users
        // included mode => add filters selected by users
        if((m_filtersAreExcluded && !filterIsSelectedByUser)
           || (!m_filtersAreExcluded && filterIsSelectedByUser))
        {
            // Add this filter
            std::string filterName = filter->getName();
            filterName                      = (filterName.empty()) ? filter->get_classname() : filterName;
            availableFiltersMap[filterName] = filter;
            availableFilterNames.push_back(filterName);
        }
    }

    // Sort available services (lexical string sort)
    std::sort(availableFilterNames.begin(), availableFilterNames.end());

    // Test if we have an extension
    if(!availableFilterNames.empty())
    {
        std::string filterName         = *availableFilterNames.begin();
        bool filterSelectionIsCanceled = false;

        // Selection of extension when availableFilterNames.size() > 1
        if(availableFilterNames.size() > 1)
        {
            sight::ui::dialog::selector::sptr selector =
                std::make_shared<sight::ui::dialog::selector>();

            selector->setTitle("Filter to use");
            selector->set_choices(availableFilterNames);
            filterName                = selector->show()[0];
            filterSelectionIsCanceled = filterName.empty();

            SIGHT_ASSERT(
                "Unable to find the selected filter name in the filter map.",
                filterSelectionIsCanceled
                || availableFiltersMap.find(filterName) != availableFiltersMap.end()
            );
        }

        if(!filterSelectionIsCanceled)
        {
            sight::filter::dicom::filter::sptr filter = availableFiltersMap[filterName];

            auto obj = m_filter.lock();
            SIGHT_ASSERT("The inout key 'filter' is not correctly set.", obj);

            obj->setValue(filter->get_classname());

            auto sig =
                obj->signal<data::object::ModifiedSignalType>(data::object::MODIFIED_SIG);
            {
                core::com::connection::blocker block(sig->get_connection(slot(service::slots::UPDATE)));
                sig->async_emit();
            }
        }
    }
    else
    {
        SIGHT_WARN("filter_selector_dialog::load : availableFilters is empty.");
        sight::ui::dialog::message messageBox;
        messageBox.setTitle("Filter not found");
        messageBox.setMessage("There is no available filter for this reader.");
        messageBox.setIcon(sight::ui::dialog::message::WARNING);
        messageBox.addButton(sight::ui::dialog::message::OK);
        messageBox.show();
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
