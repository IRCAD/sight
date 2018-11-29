/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "ioDicom/SFilterSelectorDialog.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Signals.hpp>

#include <fwCore/base.hpp>

#include <fwData/Composite.hpp>
#include <fwData/String.hpp>

#include <fwDataTools/helper/Composite.hpp>

#include <fwDicomIOFilter/IFilter.hpp>

#include <fwGui/Cursor.hpp>
#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/dialog/SelectorDialog.hpp>

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/helper.hpp>

#include <fwServices/macros.hpp>

#include <boost/foreach.hpp>

#include <sstream>
#include <string>

namespace ioDicom
{

//------------------------------------------------------------------------------

fwServicesRegisterMacro( ::fwGui::editor::IDialogEditor, ::ioDicom::SFilterSelectorDialog, ::fwData::String );

static const ::fwServices::IService::KeyType s_FILTER_INOUT = "filter";

//------------------------------------------------------------------------------

SFilterSelectorDialog::SFilterSelectorDialog() :
    m_filtersAreExcluded( true )
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

SFilterSelectorDialog::~SFilterSelectorDialog()  noexcept
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void SFilterSelectorDialog::configuring()
{
    SLM_TRACE_FUNC();

    m_selectedFilters.clear();

    //  Config Elem
    //  <selection mode="exclude">
    //  <addSelection filter="::fwDicomIOFilter::composite::CTImageStorageDefaultComposite" />
    //  <addSelection filter="::fwDicomIOFilter::composite::CTImageStorageDefaultComposite" />

    ::fwRuntime::ConfigurationElementContainer::Iterator iter = this->m_configuration->begin();
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
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void SFilterSelectorDialog::stopping()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void SFilterSelectorDialog::updating()
{
    SLM_TRACE_FUNC();

    // Retrieve available filters
    std::vector< ::fwDicomIOFilter::IFilter::sptr > registredFilters;
    for(std::string key: ::fwDicomIOFilter::registry::get()->getFactoryKeys())
    {
        ::fwDicomIOFilter::IFilter::sptr filter = ::fwDicomIOFilter::factory::New(key);
        registredFilters.push_back(filter);
    }

    // Filter available extensions and replace id by service description
    std::map< std::string, ::fwDicomIOFilter::IFilter::sptr > availableFiltersMap;
    std::vector< std::string > availableFilterNames;

    for( ::fwDicomIOFilter::IFilter::sptr filter: registredFilters )
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
            ::fwGui::dialog::SelectorDialog::sptr selector = ::fwGui::dialog::SelectorDialog::New();

            selector->setTitle("Filter to use");
            selector->setSelections(availableFilterNames);
            filterName                = selector->show();
            filterSelectionIsCanceled = filterName.empty();

            SLM_ASSERT("Unable to find the selected filter name in the filter map.",
                       filterSelectionIsCanceled || availableFiltersMap.find(filterName) != availableFiltersMap.end() );
        }

        if ( !filterSelectionIsCanceled )
        {

            ::fwDicomIOFilter::IFilter::sptr filter = availableFiltersMap[filterName];

            ::fwData::String::sptr obj = this->getInOut< ::fwData::String >(s_FILTER_INOUT);
            if (!obj)
            {
                FW_DEPRECATED_KEY(s_FILTER_INOUT, "inout", "fw4spl_18.0");
                obj = this->getObject< ::fwData::String >();
            }
            SLM_ASSERT("The filter selector service must work on a ::fwData::String object.", obj);
            obj->setValue(filter->getClassname());

            auto sig
                = obj->signal< ::fwData::Object::ModifiedSignalType>( ::fwData::Object::s_MODIFIED_SIG );
            {
                ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
                sig->asyncEmit();
            }
        }
    }
    else
    {
        SLM_WARN("SFilterSelectorDialog::load : availableFilters is empty.");
        ::fwGui::dialog::MessageDialog messageBox;
        messageBox.setTitle("Filter not found");
        messageBox.setMessage( "There is no available filter for this reader." );
        messageBox.setIcon(::fwGui::dialog::IMessageDialog::WARNING);
        messageBox.addButton(::fwGui::dialog::IMessageDialog::OK);
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

} // namespace ioDicom
