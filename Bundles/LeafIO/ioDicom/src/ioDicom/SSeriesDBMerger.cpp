/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ioDicom/SSeriesDBMerger.hpp"

#include <fwData/Vector.hpp>

#include <fwGui/dialog/MessageDialog.hpp>

#include <fwMedDataTools/helper/SeriesDB.hpp>

#include <fwServices/macros.hpp>

#include <boost/foreach.hpp>

namespace ioDicom
{

fwServicesRegisterMacro( ::fwGui::IActionSrv, ::ioDicom::SSeriesDBMerger, ::fwData::Vector );

//------------------------------------------------------------------------------

SSeriesDBMerger::SSeriesDBMerger() noexcept
{
}

//------------------------------------------------------------------------------

SSeriesDBMerger::~SSeriesDBMerger() noexcept
{
}

//------------------------------------------------------------------------------

void SSeriesDBMerger::info(std::ostream& _sstream )
{
    _sstream << "SSeriesDBMerger::info";
}

//------------------------------------------------------------------------------

void SSeriesDBMerger::starting()
{
}

//------------------------------------------------------------------------------

void SSeriesDBMerger::stopping()
{
}

//------------------------------------------------------------------------------

void SSeriesDBMerger::configuring()
{
}

//------------------------------------------------------------------------------

void SSeriesDBMerger::updating()
{
    auto destinationSeriesDB = this->getInOut< ::fwMedData::SeriesDB>("seriesDB");
    SLM_ASSERT("'seriesDB' key is not found.", destinationSeriesDB);

    ::fwMedDataTools::helper::SeriesDB sDBhelper(destinationSeriesDB);
    ::fwMedData::SeriesDB::ContainerType container = destinationSeriesDB->getContainer();
    ::fwData::Vector::csptr selectedSeries         = this->getInput< ::fwData::Vector >("selectedSeries");

    // Save added series in this container in order to display information on the push
    std::vector< ::fwMedData::Series::sptr > addedSeries;

    // Loop through all selected series
    for(const ::fwData::Object::sptr& obj: selectedSeries->getContainer())
    {
        ::fwMedData::Series::sptr series = ::fwMedData::Series::dynamicCast(obj);

        if(series && std::find(container.begin(),container.end(), series) == container.end())
        {
            sDBhelper.add(series);
            addedSeries.push_back(series);
        }
    }
    sDBhelper.notify();

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

        for(const ::fwMedData::Series::sptr& series: addedSeries)
        {
            std::string description = series->getDescription();
            description = (description.empty()) ? "[No description]" : description;
            ss << "- " << description << std::endl;
        }
        messageBox.setMessage( ss.str() );
    }

    messageBox.show();
}

//------------------------------------------------------------------------------

} // namespace ioDicom
