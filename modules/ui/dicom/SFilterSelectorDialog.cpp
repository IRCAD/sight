/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "SFilterSelectorDialog.hpp"

#include <core/base.hpp>
#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
#include <core/com/Signals.hpp>

#include <data/Composite.hpp>
#include <data/String.hpp>
#include <data/tools/helper/Composite.hpp>

#include <service/macros.hpp>

#include <filter/dicom/IFilter.hpp>

#include <ui/base/Cursor.hpp>
#include <ui/base/dialog/MessageDialog.hpp>
#include <ui/base/dialog/SelectorDialog.hpp>

#include <sstream>
#include <string>

namespace sight::module::ui::dicom
{

//------------------------------------------------------------------------------


static const service::IService::KeyType s_FILTER_INOUT = "filter";

//------------------------------------------------------------------------------

SFilterSelectorDialog::SFilterSelectorDialog() :
    m_filtersAreExcluded( true )
{
}

//------------------------------------------------------------------------------

SFilterSelectorDialog::~SFilterSelectorDialog()  noexcept
{
}

//------------------------------------------------------------------------------

void SFilterSelectorDialog::configuring()
{

    m_selectedFilters.clear();

    //  Config Elem
    //  <selection mode="exclude">
    //  <addSelection filter="::sight::filter::dicom::composite::CTImageStorageDefaultComposite" />
    //  <addSelection filter="::sight::filter::dicom::composite::CTImageStorageDefaultComposite" />

    core::runtime::ConfigurationElementContainer::Iterator iter = this->m_configuration->begin();
    for(; iter != this->m_configuration->end(); ++iter )
    {
        SLM_INFO( "SFilterSelectorDialog "  + (*iter)->getName());

        if( (*iter)->getName() == "selection" )
        {
            SLM_ASSERT( "The xml element <selection> must have the attribute 'mode'.", (*iter)->hasAttribute("mode"));
            const std::string mode = (*iter)->getExistingAttributeValue("mode");
            m_filtersAreExcluded = ( mode == "exclude" );
            SLM_ASSERT( "The xml attribute <mode> must be either 'exclude' or 'include'.", mode == "exclude" ||
                        mode == "include" );
            SLM_DEBUG( "mode => " + mode );
        }

        if( (*iter)->getName() == "addSelection" )
        {
            SLM_ASSERT( "The xml element <addSelection> must have the attribute 'filter'.",
                        (*iter)->hasAttribute("filter"));
            m_selectedFilters.push_back( (*iter)->getExistingAttributeValue("filter") );
            SLM_DEBUG( "add selection => " + (*iter)->getExistingAttributeValue("filter") );
        }

    }

}

//------------------------------------------------------------------------------

void SFilterSelectorDialog::starting()
{
}

//------------------------------------------------------------------------------

void SFilterSelectorDialog::stopping()
{
}

//------------------------------------------------------------------------------

void SFilterSelectorDialog::updating()
{

    // Retrieve available filters
    std::vector< filter::dicom::IFilter::sptr > registredFilters;
    for(std::string key: filter::dicom::registry::get()->getFactoryKeys())
    {
        filter::dicom::IFilter::sptr filter = filter::dicom::factory::New(key);
        registredFilters.push_back(filter);
    }

    // Filter available extensions and replace id by service description
    std::map< std::string, filter::dicom::IFilter::sptr > availableFiltersMap;
    std::vector< std::string > availableFilterNames;

    for( filter::dicom::IFilter::sptr filter: registredFilters )
    {
        const bool filterIsSelectedByUser = std::find( m_selectedFilters.begin(), m_selectedFilters.end(),
                                                       filter->getClassname() ) != m_selectedFilters.end();

        // Test if the filter is considered here as available by users
        // excluded mode => add filters that are not selected by users
        // included mode => add filters selected by users
        if( (m_filtersAreExcluded && !filterIsSelectedByUser) ||
            (!m_filtersAreExcluded && filterIsSelectedByUser) )
        {
            // Add this filter
            std::string filterName = filter->getName();
            filterName                      = (filterName.empty()) ? filter->getClassname() : filterName;
            availableFiltersMap[filterName] = filter;
            availableFilterNames.push_back( filterName );
        }
    }

    // Sort available services (lexical string sort)
    std::sort( availableFilterNames.begin(), availableFilterNames.end() );

    // Test if we have an extension
    if ( !availableFilterNames.empty() )
    {
        std::string filterName         = *availableFilterNames.begin();
        bool filterSelectionIsCanceled = false;

        // Selection of extension when availableFilterNames.size() > 1
        if ( availableFilterNames.size() > 1 )
        {
            sight::ui::base::dialog::SelectorDialog::sptr selector = sight::ui::base::dialog::SelectorDialog::New();

            selector->setTitle("Filter to use");
            selector->setSelections(availableFilterNames);
            filterName                = selector->show();
            filterSelectionIsCanceled = filterName.empty();

            SLM_ASSERT("Unable to find the selected filter name in the filter map.",
                       filterSelectionIsCanceled || availableFiltersMap.find(filterName) != availableFiltersMap.end() );
        }

        if ( !filterSelectionIsCanceled )
        {

            filter::dicom::IFilter::sptr filter = availableFiltersMap[filterName];

            data::String::sptr obj = this->getInOut< data::String >(s_FILTER_INOUT);
            SLM_ASSERT("The inout key '" + s_FILTER_INOUT + "' is not correctly set.", obj);

            obj->setValue(filter->getClassname());

            auto sig
                = obj->signal< data::Object::ModifiedSignalType>( data::Object::s_MODIFIED_SIG );
            {
                core::com::Connection::Blocker block(sig->getConnection(m_slotUpdate));
                sig->asyncEmit();
            }
        }
    }
    else
    {
        SLM_WARN("SFilterSelectorDialog::load : availableFilters is empty.");
        sight::ui::base::dialog::MessageDialog messageBox;
        messageBox.setTitle("Filter not found");
        messageBox.setMessage( "There is no available filter for this reader." );
        messageBox.setIcon(sight::ui::base::dialog::IMessageDialog::WARNING);
        messageBox.addButton(sight::ui::base::dialog::IMessageDialog::OK);
        messageBox.show();
    }
}

//------------------------------------------------------------------------------

void SFilterSelectorDialog::info( std::ostream& _sstream )
{
    // Update message
    _sstream << "SFilterSelectorDialog";
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::dicom
