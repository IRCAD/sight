/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ioGdcm/SDicomSeriesDBReader.hpp"

#include <fwCom/HasSignals.hpp>
#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>

#include <fwCore/base.hpp>

#include <fwData/mt/ObjectWriteLock.hpp>

#include <fwGdcmIO/reader/SeriesDB.hpp>

#include <fwGui/Cursor.hpp>
#include <fwGui/dialog/LocationDialog.hpp>
#include <fwGui/dialog/LoggerDialog.hpp>
#include <fwGui/dialog/MessageDialog.hpp>

#include <fwGuiQt/dialog/MessageDialog.hpp>

#include <fwJobs/IJob.hpp>
#include <fwJobs/Observer.hpp>

#include <fwMedData/SeriesDB.hpp>

#include <fwMedDataTools/helper/SeriesDB.hpp>

#include <fwServices/macros.hpp>

#include <fwTools/System.hpp>

#include <io/IReader.hpp>

#include <boost/foreach.hpp>

#include <QPointer>
#include <QPushButton>

#include <string>
#include <vector>

namespace ioGdcm
{

fwServicesRegisterMacro( ::io::IReader, ::ioGdcm::SDicomSeriesDBReader, ::fwMedData::SeriesDB );

static const ::fwCom::Signals::SignalKeyType JOB_CREATED_SIGNAL = "jobCreated";
static const ::fwCom::Signals::SignalKeyType FILES_ADDED_SIGNAL = "filesAdded";

//------------------------------------------------------------------------------

SDicomSeriesDBReader::SDicomSeriesDBReader() noexcept :
    m_sigJobCreated(JobCreatedSignal::New()),
    m_sigFilesAdded(FilesAddedSignal::New()),
    m_cancelled(false),
    m_showLogDialog(true),
    m_dicomDirSupport(USER_SELECTION),
    m_readerMode(USER_SELECTION_MODE)
{
    ::fwCom::HasSignals::m_signals
        ( JOB_CREATED_SIGNAL, m_sigJobCreated )
        ( FILES_ADDED_SIGNAL, m_sigFilesAdded );
}

//------------------------------------------------------------------------------

SDicomSeriesDBReader::~SDicomSeriesDBReader() noexcept
{
}

//------------------------------------------------------------------------------

void SDicomSeriesDBReader::configuring()
{
    ::io::IReader::configuring();

    // Show log dialog
    ::fwRuntime::ConfigurationElement::sptr logDialog = m_configuration->findConfigurationElement("showLogDialog");
    if(logDialog)
    {
        std::string logDialogStr = logDialog->getValue();
        SLM_ASSERT("<showLogDialog> value must be 'yes' or 'no'", logDialogStr == "yes" || logDialogStr == "no");
        m_showLogDialog = (logDialogStr == "yes");
    }

    // Enable dicomdir
    ::fwRuntime::ConfigurationElement::sptr dicomDir = m_configuration->findConfigurationElement("dicomdirSupport");
    if(dicomDir)
    {
        std::string dicomDirStr = dicomDir->getValue();
        SLM_ASSERT("<dicomdirSupport> value must be 'always' or 'never' or 'user_selection'",
                   dicomDirStr == "always" || dicomDirStr == "never" || dicomDirStr == "user_selection");

        if(dicomDirStr == "always")
        {
            m_dicomDirSupport = ALWAYS;
        }
        else if(dicomDirStr == "never")
        {
            m_dicomDirSupport = NEVER;
        }
        else if(dicomDirStr == "user_selection")
        {
            m_dicomDirSupport = USER_SELECTION;
        }
    }

    // Reader mode
    ::fwRuntime::ConfigurationElement::sptr mode = m_configuration->findConfigurationElement("mode");
    if(mode)
    {
        const std::string modeStr = mode->getValue();
        SLM_ASSERT("<mode> value must be 'direct' or 'copy' or 'user_selection'",
                   modeStr == "direct" || modeStr == "copy" || modeStr == "user_selection");

        if(modeStr == "direct")
        {
            m_readerMode = DIRECT;
        }
        else if(modeStr == "copy")
        {
            m_readerMode = COPY;
        }
        else if(modeStr == "user_selection")
        {
            m_readerMode = USER_SELECTION_MODE;
        }
    }

}

//------------------------------------------------------------------------------

void SDicomSeriesDBReader::configureWithIHM()
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
}

//------------------------------------------------------------------------------

void SDicomSeriesDBReader::starting()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void SDicomSeriesDBReader::stopping()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void SDicomSeriesDBReader::info(std::ostream& _sstream )
{
    _sstream << "SDicomSeriesDBReader::info";
}

//------------------------------------------------------------------------------

std::string SDicomSeriesDBReader::getSelectorDialogTitle()
{
    return "Choose a directory with DICOM images";
}

//------------------------------------------------------------------------------

::fwMedData::SeriesDB::sptr SDicomSeriesDBReader::createSeriesDB(bool checkIsDicom,
                                                                 const ::boost::filesystem::path& dicomDir)
{
    SLM_TRACE_FUNC();
    ::fwGdcmIO::reader::SeriesDB::sptr reader = ::fwGdcmIO::reader::SeriesDB::New();
    ::fwMedData::SeriesDB::sptr dummy         = ::fwMedData::SeriesDB::New();
    reader->setObject(dummy);
    reader->setFolder(dicomDir);

    auto job = reader->getJob();
    m_sigJobCreated->emit(job);

    if(m_dicomDirSupport == USER_SELECTION && reader->isDicomDirAvailable())
    {
        ::fwGui::dialog::MessageDialog messageBox;
        messageBox.setTitle("Dicomdir file");
        messageBox.setMessage( "There is a dicomdir file in the root folder. "
                               "Would you like to use it for the reading process ?" );
        messageBox.setIcon(::fwGui::dialog::IMessageDialog::QUESTION);
        messageBox.addButton(::fwGui::dialog::IMessageDialog::YES_NO);
        ::fwGui::dialog::IMessageDialog::Buttons button = messageBox.show();

        reader->setDicomdirActivated(button == ::fwGui::dialog::IMessageDialog::YES);
    }
    else if(m_dicomDirSupport == ALWAYS)
    {
        reader->setDicomdirActivated(true);
    }
    else //m_dicomDirSupport == NEVER
    {
        reader->setDicomdirActivated(false);
    }

    try
    {
        reader->readDicomSeries(checkIsDicom);

        // Retrieve logger
        ::fwLog::Logger::sptr logger = reader->getLogger();
        logger->sort();

        // Display logger dialog if enabled
        if(m_showLogDialog && !logger->empty())
        {

            std::stringstream ss;
            if(dummy->size() > 1)
            {
                ss << "The reading process is over : <b>" << dummy->size() << " series</b> have been found. "
                    "<br>Please verify the log report to be informed of the potential errors.";
            }
            else
            {
                ss << "The reading process is over : <b>" << dummy->size() << " series</b> has been found. "
                    "<br>Please verify the log report to be informed of the potential errors.";
            }

            bool result = false;
            if(!reader->getJob()->cancelRequested())
            {
                result = ::fwGui::dialog::LoggerDialog::showLoggerDialog("Reading process over", ss.str(), logger);
            }

            // If the user cancel the reading process we delete the loaded series
            if(!result || reader->getJob()->cancelRequested())
            {
                ::fwMedDataTools::helper::SeriesDB sDBhelper(dummy);
                sDBhelper.clear();
            }
        }
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

    m_cancelled = job->cancelRequested();

    return dummy;
}

//------------------------------------------------------------------------------

bool mustCopyDialog()
{
    ::fwGuiQt::dialog::MessageDialog::sptr dialog = ::fwGuiQt::dialog::MessageDialog::New();

    QPointer<QPushButton> copyButton   = new QPushButton("Copy");
    QPointer<QPushButton> directButton = new QPushButton("Direct read");
    dialog->addCustomButton(copyButton.data());
    dialog->addCustomButton(directButton.data());

    const std::string message = "Do you want to copy files before the reading process ?<br />"
                                "<b>Warning :</b> reading files directly may be slow on external media (DVD, USB key, etc.).";
    dialog->setMessage(message);

    bool copy = false;
    QObject::connect(copyButton.data(), &QPushButton::clicked, [dialog, &copy]()
        {
            copy = true;
        });
    QObject::connect(directButton.data(), &QPushButton::clicked, [dialog, &copy]()
        {
            copy = false;
        });

    dialog->show();

    return copy;

}

//------------------------------------------------------------------------------

bool hasEnoughSpaceToCopy(const std::vector< ::boost::filesystem::path >& files, unsigned int safetyCoefficient = 1)
{
    std::size_t size = 0;
    for(auto file: files)
    {
        size += ::boost::filesystem::file_size(file);
    }

    ::boost::filesystem::path destinationFolder = ::fwTools::System::getTemporaryFolder();
    auto available = ::boost::filesystem::space(destinationFolder).available;

    OSLM_TRACE("Copying file - Size : " << size << " Available : " << available);

    return (safetyCoefficient * size) < available;
}

//------------------------------------------------------------------------------

void SDicomSeriesDBReader::updating()
{
    SLM_TRACE_FUNC();
    if( this->hasLocationDefined() )
    {
        ::fwGui::Cursor cursor;
        cursor.setCursor(::fwGui::ICursor::BUSY);

        bool mustCopy = (m_readerMode == COPY);
        if(m_readerMode == USER_SELECTION_MODE)
        {
            mustCopy = mustCopyDialog();
        }

        ::fwMedData::SeriesDB::sptr seriesDB = createSeriesDB(!mustCopy, this->getFolder() );

        if( seriesDB->size() > 0 && !m_cancelled)
        {
            // Retrieve dataStruct associated with this service
            ::fwMedData::SeriesDB::sptr associatedSeriesDB = this->getObject< ::fwMedData::SeriesDB >();
            SLM_ASSERT("associated SeriesDB not instanced", associatedSeriesDB);

            // Retrieve added files
            std::vector< ::boost::filesystem::path > files;
            for(auto series: seriesDB->getContainer())
            {
                ::fwMedData::DicomSeries::sptr dicomSeries = ::fwMedData::DicomSeries::dynamicCast(series);
                SLM_ASSERT("The reader should retrieve only DicomSeries.", dicomSeries);
                for(auto entry: dicomSeries->getLocalDicomPaths())
                {
                    files.push_back(entry.second);
                }
            }

            // If we must copy, check that we have enough space to copy and
            // to anonymize files
            if(!mustCopy || hasEnoughSpaceToCopy(files, 2))
            {
                // Add series to SeriesDB
                ::fwMedDataTools::helper::SeriesDB sDBhelper(associatedSeriesDB);
                ::fwData::mt::ObjectWriteLock lock(associatedSeriesDB);
                sDBhelper.merge(seriesDB);

                // Notify SeriesDB
                sDBhelper.notify();

                // Notify added files
                m_sigFilesAdded->asyncEmit(mustCopy, files);
            }
            else
            {
                ::fwGui::dialog::MessageDialog::showMessageDialog(
                    "Copy error", "There is not enough space on your main hard drive to copy those files.",
                    ::fwGui::dialog::IMessageDialog::CRITICAL);
            }

        }

        cursor.setDefaultCursor();
    }
}

//-----------------------------------------------------------------------------

::io::IOPathType SDicomSeriesDBReader::getIOPathType() const
{
    return ::io::FOLDER;
}

//------------------------------------------------------------------------------

} // namespace ioGdcm
