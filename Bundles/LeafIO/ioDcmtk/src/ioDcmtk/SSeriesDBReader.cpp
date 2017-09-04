/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ioDcmtk/SSeriesDBReader.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Signals.hpp>

#include <fwCore/base.hpp>

#include <fwData/String.hpp>

#include <fwDcmtkIO/SeriesDBReader.hpp>

#include <fwGui/Cursor.hpp>
#include <fwGui/dialog/LocationDialog.hpp>
#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/dialog/ProgressDialog.hpp>

#include <fwMedData/SeriesDB.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/op/Add.hpp>
#include <fwServices/registry/ServiceConfig.hpp>

#include <fwTools/ProgressToLogger.hpp>

#include <io/IReader.hpp>

#include <boost/bind.hpp>

namespace ioDcmtk
{

fwServicesRegisterMacro( ::io::IReader, ::ioDcmtk::SSeriesDBReader, ::fwMedData::SeriesDB );

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
    static ::boost::filesystem::path _sDefaultPath;

    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? this->getSelectorDialogTitle() : m_windowTitle);
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::READ);
    dialogFile.setType(::fwGui::dialog::LocationDialog::FOLDER);

    ::fwData::location::Folder::sptr result;
    result = ::fwData::location::Folder::dynamicCast( dialogFile.show() );
    if (result)
    {
        _sDefaultPath = result->getFolder();
        this->setFolder( result->getFolder() );
        dialogFile.saveDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    }

    // Select filter
    if(!m_filterSelectorSrvConfig.empty())
    {
        // Get the config
        ::fwRuntime::ConfigurationElement::csptr filterSelectorConfig;
        filterSelectorConfig = ::fwServices::registry::ServiceConfig::getDefault()->getServiceConfig(
            m_filterSelectorSrvConfig, "::ioDicom::SFilterSelectorDialog");

        SLM_ASSERT("There is no service configuration "
                   + m_filterSelectorSrvConfig
                   + " for ::ioDicom::SFilterSelectorDialog", filterSelectorConfig);

        // Init and execute the service
        ::fwServices::IService::sptr filterSelectorSrv;
        ::fwData::String::sptr key = ::fwData::String::New();
        filterSelectorSrv          = ::fwServices::add(key,
                                                       "::gui::editor::IDialogEditor",
                                                       "::ioDicom::SFilterSelectorDialog");
        filterSelectorSrv->setConfiguration( ::fwRuntime::ConfigurationElement::constCast(filterSelectorConfig) );
        filterSelectorSrv->configure();
        filterSelectorSrv->start();
        filterSelectorSrv->update();
        filterSelectorSrv->stop();
        ::fwServices::OSR::unregisterService( filterSelectorSrv );

        m_filterType = key->getValue();

    }
}

//------------------------------------------------------------------------------

void SSeriesDBReader::configuring()
{
    ::io::IReader::configuring();

    // Use filter selector
    ::fwRuntime::ConfigurationElement::sptr selectorConfig =
        m_configuration->findConfigurationElement("FilterSelectorSrvConfig");
    if(selectorConfig)
    {
        SLM_ASSERT("Missing 'name' attribute", selectorConfig->hasAttribute("name"));
        m_filterSelectorSrvConfig = selectorConfig->getAttributeValue("name");
    }

    // Get SOP Class selection
    const ::fwRuntime::ConfigurationElement::sptr sopClassSelection =
        m_configuration->findConfigurationElement("SOPClassSelection");
    if(sopClassSelection)
    {
        ::fwRuntime::ConfigurationElementContainer sopClassElements =
            sopClassSelection->findAllConfigurationElement("SOPClass");

        for(::fwRuntime::ConfigurationElementContainer::Iterator it = sopClassElements.begin();
            it != sopClassElements.end(); ++it)
        {
            const ::fwRuntime::ConfigurationElement::AttributePair attributePair = (*it)->getSafeAttributeValue("uid");
            if(attributePair.first)
            {
                SLM_TRACE("New SOP class supported : " + attributePair.second);
                m_supportedSOPClassSelection.push_back(attributePair.second);
            }
        }
    }

    // Set filter
    ::fwRuntime::ConfigurationElement::sptr config = m_configuration->findConfigurationElement("config");
    if(config)
    {
        m_filterType = config->getAttributeValue("filterType");
    }
}

//------------------------------------------------------------------------------

void SSeriesDBReader::starting()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void SSeriesDBReader::stopping()
{
    SLM_TRACE_FUNC();
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

::fwMedData::SeriesDB::sptr SSeriesDBReader::createSeriesDB(const ::boost::filesystem::path& dicomDir)
{
    SLM_TRACE_FUNC();
    ::fwDcmtkIO::SeriesDBReader::sptr myLoader = ::fwDcmtkIO::SeriesDBReader::New();
    ::fwMedData::SeriesDB::sptr dummy          = ::fwMedData::SeriesDB::New();
    myLoader->setObject(dummy);
    myLoader->setFolder(dicomDir);
    myLoader->setDicomFilterType(m_filterType);
    myLoader->setSupportedSOPClassContainer(m_supportedSOPClassSelection);

    if(myLoader->isDicomDirAvailable())
    {
        ::fwGui::dialog::MessageDialog messageBox;
        messageBox.setTitle("Dicomdir file");
        messageBox.setMessage( "There is a dicomdir file in the root folder. "
                               "Would you like to use it for the reading process ?" );
        messageBox.setIcon(::fwGui::dialog::IMessageDialog::QUESTION);
        messageBox.addButton(::fwGui::dialog::IMessageDialog::YES_NO);
        ::fwGui::dialog::IMessageDialog::Buttons button = messageBox.show();

        myLoader->setDicomdirActivated(button == ::fwGui::dialog::IMessageDialog::YES);
    }

    try
    {
        ::fwGui::dialog::ProgressDialog progressMeterGUI("Loading Dicom Image");
        myLoader->addHandler( progressMeterGUI );
        myLoader->read();
    }
    catch (const std::exception& e)
    {
        std::stringstream ss;
        ss << "Warning during loading : " << e.what();
        ::fwGui::dialog::MessageDialog::showMessageDialog(
            "Warning", ss.str(), ::fwGui::dialog::IMessageDialog::WARNING);
    }
    catch( ... )
    {
        ::fwGui::dialog::MessageDialog::showMessageDialog(
            "Warning", "Warning during loading", ::fwGui::dialog::IMessageDialog::WARNING);
    }

    return myLoader->getConcreteObject();
}

//------------------------------------------------------------------------------

void SSeriesDBReader::updating()
{
    SLM_TRACE_FUNC();
    if( this->hasLocationDefined() )
    {
        ::fwMedData::SeriesDB::sptr seriesDB = createSeriesDB( this->getFolder() );

        if( seriesDB->size() > 0 )
        {
            // Retrieve dataStruct associated with this service
            ::fwMedData::SeriesDB::sptr associatedSeriesDB = this->getInOut< ::fwMedData::SeriesDB >(::io::s_DATA_KEY);
            SLM_ASSERT("associated SeriesDB not instanced", associatedSeriesDB);
            associatedSeriesDB->shallowCopy( seriesDB );

            ::fwGui::Cursor cursor;
            cursor.setCursor(::fwGui::ICursor::BUSY);
            this->notificationOfDBUpdate();
            cursor.setDefaultCursor();
        }
        else
        {
            ::fwGui::dialog::MessageDialog::showMessageDialog(
                "Image Reader", "This file can not be read. Retry with another file reader.",
                ::fwGui::dialog::IMessageDialog::WARNING);
        }
    }
}

//------------------------------------------------------------------------------

void SSeriesDBReader::notificationOfDBUpdate()
{
    ::fwMedData::SeriesDB::sptr seriesDB = this->getInOut< ::fwMedData::SeriesDB >(::io::s_DATA_KEY);
    SLM_ASSERT("Unable to get seriesDB", seriesDB);

    ::fwMedData::SeriesDB::ContainerType addedSeries;
    for(const ::fwMedData::Series::sptr& s :  seriesDB->getContainer() )
    {
        addedSeries.push_back(s);
    }

    auto sig = seriesDB->signal< ::fwMedData::SeriesDB::AddedSeriesSignalType >(
        ::fwMedData::SeriesDB::s_ADDED_SERIES_SIG);
    sig->asyncEmit(addedSeries);
}

//-----------------------------------------------------------------------------

::io::IOPathType SSeriesDBReader::getIOPathType() const
{
    return ::io::FOLDER;
}

//------------------------------------------------------------------------------

} // namespace ioDcmtk
