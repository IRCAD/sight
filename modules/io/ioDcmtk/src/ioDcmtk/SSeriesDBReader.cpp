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

#include "ioDcmtk/SSeriesDBReader.hpp"

#include <core/base.hpp>
#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
#include <core/com/Signals.hpp>
#include <core/tools/ProgressToLogger.hpp>

#include <data/SeriesDB.hpp>
#include <data/String.hpp>

#include <fwDcmtkIO/SeriesDBReader.hpp>

#include <services/macros.hpp>
#include <services/op/Add.hpp>
#include <services/registry/ServiceConfig.hpp>

#include <io/base/services/IReader.hpp>

#include <ui/base/Cursor.hpp>
#include <ui/base/dialog/LocationDialog.hpp>
#include <ui/base/dialog/MessageDialog.hpp>
#include <ui/base/dialog/ProgressDialog.hpp>

namespace ioDcmtk
{

fwServicesRegisterMacro( ::sight::io::base::services::IReader, ::ioDcmtk::SSeriesDBReader, ::sight::data::SeriesDB )

//------------------------------------------------------------------------------

SSeriesDBReader::SSeriesDBReader() noexcept
{
}

//------------------------------------------------------------------------------

SSeriesDBReader::~SSeriesDBReader() noexcept
{
}

//------------------------------------------------------------------------------

void SSeriesDBReader::configureWithIHM()
{
    this->openLocationDialog();
}
//------------------------------------------------------------------------------

void SSeriesDBReader::openLocationDialog()
{
    static std::filesystem::path _sDefaultPath;

    ui::base::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? this->getSelectorDialogTitle() : m_windowTitle);
    dialogFile.setDefaultLocation( data::location::Folder::New(_sDefaultPath) );
    dialogFile.setOption(ui::base::dialog::ILocationDialog::READ);
    dialogFile.setType(ui::base::dialog::LocationDialog::FOLDER);

    data::location::Folder::sptr result;
    result = data::location::Folder::dynamicCast( dialogFile.show() );
    if (result)
    {
        _sDefaultPath = result->getFolder();
        this->setFolder( result->getFolder() );
        dialogFile.saveDefaultLocation( data::location::Folder::New(_sDefaultPath) );
    }

    // Select filter
    if(!m_filterSelectorSrvConfig.empty())
    {
        // Get the config
        core::runtime::ConfigurationElement::csptr filterSelectorConfig;
        filterSelectorConfig = services::registry::ServiceConfig::getDefault()->getServiceConfig(
            m_filterSelectorSrvConfig, "::ioDicom::SFilterSelectorDialog");
        SLM_ASSERT("There is no service configuration "
                   + m_filterSelectorSrvConfig
                   + " for ::ioDicom::SFilterSelectorDialog", filterSelectorConfig);

        // Init and execute the service
        services::IService::sptr filterSelectorSrv;
        data::String::sptr key = data::String::New();
        filterSelectorSrv = services::add("::ioDicom::SFilterSelectorDialog");
        filterSelectorSrv->registerInOut(key, "filter");
        filterSelectorSrv->setConfiguration( core::runtime::ConfigurationElement::constCast(filterSelectorConfig) );
        filterSelectorSrv->configure();
        filterSelectorSrv->start();
        filterSelectorSrv->update();
        filterSelectorSrv->stop();
        services::OSR::unregisterService( filterSelectorSrv );

        m_filterType = key->getValue();

    }
}

//------------------------------------------------------------------------------

void SSeriesDBReader::configuring()
{
    sight::io::base::services::IReader::configuring();

    // Use filter selector
    core::runtime::ConfigurationElement::sptr selectorConfig =
        m_configuration->findConfigurationElement("FilterSelectorSrvConfig");
    if(selectorConfig)
    {
        SLM_ASSERT("Missing 'name' attribute", selectorConfig->hasAttribute("name"));
        m_filterSelectorSrvConfig = selectorConfig->getAttributeValue("name");
    }

    // Get SOP Class selection
    const core::runtime::ConfigurationElement::sptr sopClassSelection =
        m_configuration->findConfigurationElement("SOPClassSelection");
    if(sopClassSelection)
    {
        core::runtime::ConfigurationElementContainer sopClassElements =
            sopClassSelection->findAllConfigurationElement("SOPClass");

        for(core::runtime::ConfigurationElementContainer::Iterator it = sopClassElements.begin();
            it != sopClassElements.end(); ++it)
        {
            const core::runtime::ConfigurationElement::AttributePair attributePair =
                (*it)->getSafeAttributeValue("uid");
            if(attributePair.first)
            {
                m_supportedSOPClassSelection.push_back(attributePair.second);
            }
        }
    }

    // Set filter
    core::runtime::ConfigurationElement::sptr config = m_configuration->findConfigurationElement("config");
    if(config)
    {
        m_filterType = config->getAttributeValue("filterType");
    }
}

//------------------------------------------------------------------------------

void SSeriesDBReader::starting()
{
}

//------------------------------------------------------------------------------

void SSeriesDBReader::stopping()
{
}

//------------------------------------------------------------------------------

void SSeriesDBReader::info(std::ostream& _sstream )
{
    _sstream << "SSeriesDBReader::info";
}

//------------------------------------------------------------------------------

std::string SSeriesDBReader::getSelectorDialogTitle()
{
    return "Choose a directory with DICOM images";
}

//------------------------------------------------------------------------------

data::SeriesDB::sptr SSeriesDBReader::createSeriesDB(const std::filesystem::path& dicomDir)
{
    ::fwDcmtkIO::SeriesDBReader::sptr myLoader = ::fwDcmtkIO::SeriesDBReader::New();
    data::SeriesDB::sptr dummy = data::SeriesDB::New();
    myLoader->setObject(dummy);
    myLoader->setFolder(dicomDir);
    myLoader->setDicomFilterType(m_filterType);
    myLoader->setSupportedSOPClassContainer(m_supportedSOPClassSelection);

    if(myLoader->isDicomDirAvailable())
    {
        ui::base::dialog::MessageDialog messageBox;
        messageBox.setTitle("Dicomdir file");
        messageBox.setMessage( "There is a dicomdir file in the root folder. "
                               "Would you like to use it for the reading process ?" );
        messageBox.setIcon(ui::base::dialog::IMessageDialog::QUESTION);
        messageBox.addButton(ui::base::dialog::IMessageDialog::YES_NO);
        ui::base::dialog::IMessageDialog::Buttons button = messageBox.show();

        myLoader->setDicomdirActivated(button == ui::base::dialog::IMessageDialog::YES);
    }

    try
    {
        ui::base::dialog::ProgressDialog progressMeterGUI("Loading Dicom Image");
        myLoader->addHandler( progressMeterGUI );
        myLoader->read();
    }
    catch (const std::exception& e)
    {
        std::stringstream ss;
        ss << "Warning during loading : " << e.what();
        ui::base::dialog::MessageDialog::show(
            "Warning", ss.str(), ui::base::dialog::IMessageDialog::WARNING);
    }
    catch( ... )
    {
        ui::base::dialog::MessageDialog::show(
            "Warning", "Warning during loading", ui::base::dialog::IMessageDialog::WARNING);
    }

    return myLoader->getConcreteObject();
}

//------------------------------------------------------------------------------

void SSeriesDBReader::updating()
{
    if( this->hasLocationDefined() )
    {
        data::SeriesDB::sptr seriesDB = createSeriesDB( this->getFolder() );

        if( seriesDB->size() > 0 )
        {
            // Retrieve dataStruct associated with this service
            data::SeriesDB::sptr associatedSeriesDB =
                this->getInOut< data::SeriesDB >(sight::io::base::services::s_DATA_KEY);
            SLM_ASSERT("associated SeriesDB not instanced", associatedSeriesDB);
            associatedSeriesDB->shallowCopy( seriesDB );

            ui::base::Cursor cursor;
            cursor.setCursor(ui::base::ICursor::BUSY);
            this->notificationOfDBUpdate();
            cursor.setDefaultCursor();

            m_readFailed = false;
        }
        else
        {
            m_readFailed = true;
            ui::base::dialog::MessageDialog::show(
                "Image Reader", "This file can not be read. Retry with another file reader.",
                ui::base::dialog::IMessageDialog::WARNING);
        }
    }
    else
    {
        m_readFailed = true;
    }
}

//------------------------------------------------------------------------------

void SSeriesDBReader::notificationOfDBUpdate()
{
    data::SeriesDB::sptr seriesDB = this->getInOut< data::SeriesDB >(sight::io::base::services::s_DATA_KEY);
    SLM_ASSERT("Unable to get seriesDB", seriesDB);

    data::SeriesDB::ContainerType addedSeries;
    for(const data::Series::sptr& s :  seriesDB->getContainer() )
    {
        addedSeries.push_back(s);
    }

    auto sig = seriesDB->signal< data::SeriesDB::AddedSeriesSignalType >(
        data::SeriesDB::s_ADDED_SERIES_SIG);
    sig->asyncEmit(addedSeries);
}

//-----------------------------------------------------------------------------

sight::io::base::services::IOPathType SSeriesDBReader::getIOPathType() const
{
    return sight::io::base::services::FOLDER;
}

//------------------------------------------------------------------------------

} // namespace ioDcmtk
