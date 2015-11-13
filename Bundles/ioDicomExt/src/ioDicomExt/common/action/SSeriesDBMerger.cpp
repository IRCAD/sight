/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwServices/macros.hpp>
#include <fwComEd/SeriesDBMsg.hpp>
#include <fwComEd/helper/SeriesDB.hpp>
#include <fwGui/dialog/MessageDialog.hpp>

#include "ioDicomExt/common/action/SSeriesDBMerger.hpp"

namespace ioDicomExt
{

namespace common
{

namespace action
{

fwServicesRegisterMacro( ::fwGui::IActionSrv, ::ioDicomExt::common::action::SSeriesDBMerger, ::fwData::Vector );

//------------------------------------------------------------------------------

SSeriesDBMerger::SSeriesDBMerger() throw()
{
}
//------------------------------------------------------------------------------

SSeriesDBMerger::~SSeriesDBMerger() throw()
{
}

//------------------------------------------------------------------------------

void SSeriesDBMerger::info(std::ostream &_sstream )
{
    _sstream << "SSeriesDBMerger::info";
}

//------------------------------------------------------------------------------

void SSeriesDBMerger::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    // Get Destination SeriesDB
    m_destinationSeriesDB = ::fwMedData::SeriesDB::dynamicCast(::fwTools::fwID::getObject(m_destinationSeriesDBID));
    SLM_ASSERT("The SeriesDB \"" + m_destinationSeriesDBID + "\" doesn't exist.", m_destinationSeriesDB);
}

//------------------------------------------------------------------------------

void SSeriesDBMerger::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void SSeriesDBMerger::configuring() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    ::fwRuntime::ConfigurationElement::sptr config = m_configuration->findConfigurationElement("config");
    SLM_ASSERT("The service ::ioDicomExt::common::action::SSeriesDBMerger must have a \"config\" element.",config);

    bool success;

    // Destination Series DB ID
    ::boost::tie(success, m_destinationSeriesDBID) = config->getSafeAttributeValue("destinationSeriesDBID");
    SLM_ASSERT("It should be an \"destinationSeriesDBID\" in the "
               "::ioDicomExt::common::action::SSeriesDBMerger config element.", success);


}

//------------------------------------------------------------------------------

void SSeriesDBMerger::receiving( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void SSeriesDBMerger::updating() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    ::fwComEd::helper::SeriesDB sDBhelper(m_destinationSeriesDB);
    ::fwMedData::SeriesDB::ContainerType container = m_destinationSeriesDB->getContainer();
    ::fwData::Vector::sptr selectedSeries          = this->getObject< ::fwData::Vector >();

    // Save added series in this container in order to display information on the push
    std::vector< ::fwMedData::Series::sptr > addedSeries;

    // Loop through all selected series
    for(const ::fwData::Object::sptr& obj :  selectedSeries->getContainer())
    {
        ::fwMedData::Series::sptr series = ::fwMedData::Series::dynamicCast(obj);

        if(series && std::find(container.begin(),container.end(), series) == container.end())
        {
            sDBhelper.add(series);
            addedSeries.push_back(series);
        }
    }
    sDBhelper.notify(this->getSptr());

    // Display the informations
    ::fwGui::dialog::MessageDialog messageBox;
    messageBox.setIcon(::fwGui::dialog::IMessageDialog::INFO);
    messageBox.addButton(::fwGui::dialog::IMessageDialog::OK);
    messageBox.setTitle("Push Series");

    if(selectedSeries->empty())
    {
        messageBox.setMessage( "Unable to push series, there is no series selected." );
    }
    else if(addedSeries.empty())
    {
        messageBox.setMessage( "Unable to push series, the series are already present in the database." );
    }
    else
    {
        std::stringstream ss;
        if(addedSeries.size()==1)
        {
            ss << addedSeries.size() << " series has been correctly pushed in the database:\n";
        }
        else
        {
            ss << addedSeries.size() << " series have been correctly pushed in the database:\n";
        }

        for(const ::fwMedData::Series::sptr& series :  addedSeries)
        {
            std::string description = series->getDescription();
            description = (description.empty()) ? "[No description]" : description;
            ss << "- " << description << std::endl;
        }
        messageBox.setMessage( ss.str() );
    }

    ::fwGui::dialog::IMessageDialog::Buttons answer = messageBox.show();

}



} // namespace action
} // namespace common
} // namespace ioDicomExt
