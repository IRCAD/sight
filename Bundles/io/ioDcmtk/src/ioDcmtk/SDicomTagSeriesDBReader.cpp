/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ioDcmtk/SDicomTagSeriesDBReader.hpp"

#include <fwCore/base.hpp>

#include <fwDcmtkIO/DicomSeriesDBReader.hpp>

#include <fwGui/Cursor.hpp>
#include <fwGui/dialog/LocationDialog.hpp>
#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/dialog/ProgressDialog.hpp>

#include <fwIO/IReader.hpp>

#include <fwMedData/SeriesDB.hpp>

#include <fwServices/macros.hpp>

#include <fwTools/ProgressToLogger.hpp>

namespace ioDcmtk
{

fwServicesRegisterMacro( ::fwIO::IReader, ::ioDcmtk::SDicomTagSeriesDBReader, ::fwMedData::SeriesDB );

//------------------------------------------------------------------------------

SDicomTagSeriesDBReader::SDicomTagSeriesDBReader() noexcept
{
}

//------------------------------------------------------------------------------

SDicomTagSeriesDBReader::~SDicomTagSeriesDBReader() noexcept
{
}

//------------------------------------------------------------------------------

void SDicomTagSeriesDBReader::configuring()
{
    ::fwIO::IReader::configuring();
}

//------------------------------------------------------------------------------

void SDicomTagSeriesDBReader::configureWithIHM()
{
    static ::boost::filesystem::path _sDefaultPath;

    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? this->getSelectorDialogTitle() : m_windowTitle);
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::READ);
    dialogFile.setType(::fwGui::dialog::ILocationDialog::FOLDER);

    ::fwData::location::Folder::sptr result;
    result = ::fwData::location::Folder::dynamicCast( dialogFile.show() );
    if (result)
    {
        _sDefaultPath = result->getFolder();
        this->setFolder( result->getFolder() );
        dialogFile.saveDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    }
}

//------------------------------------------------------------------------------

void SDicomTagSeriesDBReader::starting()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void SDicomTagSeriesDBReader::stopping()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void SDicomTagSeriesDBReader::info(std::ostream& _sstream )
{
    _sstream << "SDicomTagSeriesDBReader::info";
}

//------------------------------------------------------------------------------

std::string SDicomTagSeriesDBReader::getSelectorDialogTitle()
{
    return "Choose a directory with DICOM images";
}

//------------------------------------------------------------------------------

::fwMedData::SeriesDB::sptr SDicomTagSeriesDBReader::createSeriesDB(const ::boost::filesystem::path& dicomDir)
{
    SLM_TRACE_FUNC();
    ::fwDcmtkIO::DicomSeriesDBReader::sptr myLoader = ::fwDcmtkIO::DicomSeriesDBReader::New();

    ::fwMedData::SeriesDB::sptr dummy = ::fwMedData::SeriesDB::New();
    myLoader->setObject(dummy);
    myLoader->setFolder(dicomDir);

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
        ::fwGui::dialog::ProgressDialog progressMeterGUI("Loading Dicom");
        myLoader->addHandler( progressMeterGUI );
        myLoader->read();
    }
    catch (const std::runtime_error& e)
    {
        throw e;
    }

    return myLoader->getConcreteObject();
}

//------------------------------------------------------------------------------

void SDicomTagSeriesDBReader::updating()
{
    SLM_TRACE_FUNC();
    if( this->hasLocationDefined() )
    {
        try
        {
            ::fwMedData::SeriesDB::sptr seriesDB = createSeriesDB( this->getFolder() );
            if( seriesDB->size() > 0 )
            {
                // Retrieve dataStruct associated with this service
                auto associatedSeriesDB = this->getInOut< ::fwMedData::SeriesDB >(::fwIO::s_DATA_KEY);
                associatedSeriesDB->shallowCopy( seriesDB );

                ::fwGui::Cursor cursor;
                cursor.setCursor(::fwGui::ICursor::BUSY);
                this->notificationOfDBUpdate();
                cursor.setDefaultCursor();
            }
            else
            {
                throw std::runtime_error("An uncaught exception occurred while reading file.");
            }
        }
        catch (const std::exception& e)
        {
            std::stringstream ss;
            ss << "Warning during loading : " << e.what();
            ::fwGui::dialog::MessageDialog::showMessageDialog(
                "Warning", ss.str(), ::fwGui::dialog::IMessageDialog::WARNING);
        }
    }
}

//------------------------------------------------------------------------------

void SDicomTagSeriesDBReader::notificationOfDBUpdate()
{
    ::fwMedData::SeriesDB::csptr seriesDB = this->getInOut< ::fwMedData::SeriesDB >(::fwIO::s_DATA_KEY);
    SLM_ASSERT("Unable to get seriesDB", seriesDB);

    ::fwMedData::SeriesDB::ContainerType addedSeries;
    for( ::fwMedData::Series::sptr s :  seriesDB->getContainer() )
    {
        addedSeries.push_back(s);
    }

    auto sig = seriesDB->signal< ::fwMedData::SeriesDB::AddedSeriesSignalType >(
        ::fwMedData::SeriesDB::s_ADDED_SERIES_SIG);
    sig->asyncEmit(addedSeries);
}

//-----------------------------------------------------------------------------

::fwIO::IOPathType SDicomTagSeriesDBReader::getIOPathType() const
{
    return ::fwIO::FOLDER;
}

//------------------------------------------------------------------------------

} // namespace ioDcmtk
