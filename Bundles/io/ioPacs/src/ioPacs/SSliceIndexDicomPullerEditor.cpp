/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ioPacs/SSliceIndexDicomPullerEditor.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Signals.hpp>
#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwData/Array.hpp>
#include <fwData/Composite.hpp>
#include <fwData/Image.hpp>
#include <fwData/Integer.hpp>

#include <fwDataTools/fieldHelper/Image.hpp>
#include <fwDataTools/helper/Array.hpp>
#include <fwDataTools/helper/Composite.hpp>

#include <fwGui/dialog/MessageDialog.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwMedData/DicomSeries.hpp>
#include <fwMedData/ImageSeries.hpp>
#include <fwMedData/SeriesDB.hpp>

#include <fwMedDataTools/helper/SeriesDB.hpp>

#include <fwPacsIO/exceptions/Base.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/registry/ActiveWorkers.hpp>
#include <fwServices/registry/ObjectService.hpp>

#include <fwThread/Timer.hpp>

#include <fwTools/System.hpp>

#include <boost/asio/placeholders.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/foreach.hpp>

#include <QApplication>
#include <QComboBox>
#include <QHBoxLayout>
#include <QMouseEvent>

#include <iterator>

namespace ioPacs
{

fwServicesRegisterMacro( ::fwGui::editor::IEditor, ::ioPacs::SSliceIndexDicomPullerEditor,
                         ::fwMedData::DicomSeries );

const ::fwCom::Slots::SlotKeyType SSliceIndexDicomPullerEditor::s_READ_IMAGE_SLOT      = "readImage";
const ::fwCom::Slots::SlotKeyType SSliceIndexDicomPullerEditor::s_DISPLAY_MESSAGE_SLOT = "displayErrorMessage";

//------------------------------------------------------------------------------

SSliceIndexDicomPullerEditor::SSliceIndexDicomPullerEditor() noexcept :
    m_delay(500)
{
    m_slotReadImage = ::fwCom::newSlot(&SSliceIndexDicomPullerEditor::readImage, this);
    ::fwCom::HasSlots::m_slots(s_READ_IMAGE_SLOT, m_slotReadImage);

    m_slotDisplayMessage = ::fwCom::newSlot(&SSliceIndexDicomPullerEditor::displayErrorMessage, this);
    ::fwCom::HasSlots::m_slots(s_DISPLAY_MESSAGE_SLOT, m_slotDisplayMessage);

    ::fwCom::HasSlots::m_slots.setWorker( m_associatedWorker );
}
//------------------------------------------------------------------------------

SSliceIndexDicomPullerEditor::~SSliceIndexDicomPullerEditor() noexcept
{
}

//------------------------------------------------------------------------------

void SSliceIndexDicomPullerEditor::info(std::ostream& _sstream )
{
    _sstream << "SSliceIndexDicomPullerEditor::info";
}

//------------------------------------------------------------------------------

void SSliceIndexDicomPullerEditor::configuring()
{
    ::fwGui::IGuiContainerSrv::initialize();

    ::fwRuntime::ConfigurationElement::sptr config = m_configuration->findConfigurationElement("config");
    SLM_ASSERT("The service ::ioPacs::SPacsConfigurationInitializer must have "
               "a \"config\" element.", config);

    bool success;

    // Reader
    ::boost::tie(success, m_dicomReaderType) = config->getSafeAttributeValue("dicomReader");
    SLM_ASSERT("It should be a \"dicomReader\" tag in the ::ioPacs::SSliceIndexDicomPullerEditor "
               "config element.", success);

    // Reader configuration
    ::fwRuntime::ConfigurationElement::sptr readerConfig = config->findConfigurationElement("dicomReaderConfig");
    m_readerConfig                                       =
        (readerConfig && readerConfig->size() == 1) ? readerConfig->getElements()[0] : nullptr;

    // Delay
    std::string delayStr;
    ::boost::tie(success, delayStr) = config->getSafeAttributeValue("delay");
    if(success)
    {
        m_delay = ::boost::lexical_cast< unsigned int >(delayStr);
    }
}

//------------------------------------------------------------------------------

void SSliceIndexDicomPullerEditor::starting()
{
    m_delayTimer2 = m_associatedWorker->createTimer();

    // Get pacs configuration
    m_pacsConfiguration = this->getInput< ::fwPacsIO::data::PacsConfiguration>("pacsConfig");

    ::fwGui::IGuiContainerSrv::create();
    ::fwGuiQt::container::QtContainer::sptr qtContainer = fwGuiQt::container::QtContainer::dynamicCast(getContainer());

    QHBoxLayout* layout = new QHBoxLayout();

    ::fwMedData::DicomSeries::csptr dicomSeries = this->getInOut< ::fwMedData::DicomSeries >("series");
    SLM_ASSERT("DicomSeries should not be null !", dicomSeries);
    m_numberOfSlices = dicomSeries->getNumberOfInstances();

    // Slider
    m_sliceIndexSlider = new QSlider(Qt::Horizontal);
    layout->addWidget(m_sliceIndexSlider, 1);
    m_sliceIndexSlider->setRange(0, static_cast<unsigned int>(m_numberOfSlices-1));
    m_sliceIndexSlider->setValue(static_cast<unsigned int>(m_numberOfSlices/2));

    // Line Edit
    m_sliceIndexLineEdit = new QLineEdit();
    layout->addWidget(m_sliceIndexLineEdit, 0);
    m_sliceIndexLineEdit->setReadOnly(true);
    m_sliceIndexLineEdit->setMaximumWidth(80);

    std::stringstream ss;
    ss << m_sliceIndexSlider->value() << " / " << (m_numberOfSlices-1);
    m_sliceIndexLineEdit->setText(std::string(ss.str()).c_str());

    qtContainer->setLayout(layout);

    // Connect the signals
    QObject::connect(m_sliceIndexSlider, SIGNAL(valueChanged(int)), this, SLOT(changeSliceIndex(int)));

    // Create temporary SeriesDB
    m_tempSeriesDB = ::fwMedData::SeriesDB::New();

    // Create reader
    ::fwServices::registry::ServiceFactory::sptr srvFactory = ::fwServices::registry::ServiceFactory::getDefault();

    ::fwIO::IReader::sptr dicomReader;
    dicomReader = ::fwIO::IReader::dynamicCast(srvFactory->create(m_dicomReaderType));
    SLM_ASSERT("Unable to create a reader of type: \"" + m_dicomReaderType + "\" in "
               "::ioPacs::SSliceIndexDicomPullerEditor.", dicomReader);
    ::fwServices::OSR::registerService(m_tempSeriesDB, dicomReader);

    if(m_readerConfig)
    {
        dicomReader->setConfiguration(m_readerConfig);
    }

    dicomReader->configure();
    dicomReader->start();

    m_dicomReader = dicomReader;

    // Image Indecies
    m_axialIndex    = ::fwData::Integer::New(0);
    m_frontalIndex  = ::fwData::Integer::New(0);
    m_sagittalIndex = ::fwData::Integer::New(0);

    // Worker
    m_pullSeriesWorker = ::fwThread::Worker::New();

    // Create enquirer
    m_seriesEnquirer = ::fwPacsIO::SeriesEnquirer::New();

    // Load a slice
    std::chrono::milliseconds duration = std::chrono::milliseconds(m_delay);
    m_delayTimer2->setFunction(  [ = ]()
        {
            this->triggerNewSlice();
        }  );
    m_delayTimer2->setDuration(duration);
    m_delayTimer2->setOneShot(true);

    this->triggerNewSlice();

}

//------------------------------------------------------------------------------

void SSliceIndexDicomPullerEditor::stopping()
{
    // Worker
    m_pullSeriesWorker->stop();
    m_pullSeriesWorker.reset();

    // Stop dicom reader
    if(!m_dicomReader.expired())
    {
        m_dicomReader.lock()->stop();
        ::fwServices::OSR::unregisterService(m_dicomReader.lock());
    }

    // Disconnect the signals
    QObject::disconnect(m_sliceIndexSlider, SIGNAL(valueChanged(int)), this, SLOT(changeSliceIndex(int)));

    this->destroy();
}

//------------------------------------------------------------------------------

void SSliceIndexDicomPullerEditor::updating()
{
}

//------------------------------------------------------------------------------

void SSliceIndexDicomPullerEditor::changeSliceIndex(int value)
{
    // Update text
    std::stringstream ss;
    ss << m_sliceIndexSlider->value() << " / " << (m_numberOfSlices-1);
    m_sliceIndexLineEdit->setText(std::string(ss.str()).c_str());

    // Get the new slice if there is no change for m_delay milliseconds
    m_delayTimer2->start();

}

//------------------------------------------------------------------------------

void SSliceIndexDicomPullerEditor::triggerNewSlice()
{
    // DicomSeries
    ::fwMedData::DicomSeries::csptr dicomSeries = this->getInOut< ::fwMedData::DicomSeries >("series");
    SLM_ASSERT("DicomSeries should not be null !", dicomSeries);

    // Compute slice index
    std::size_t selectedSliceIndex = m_sliceIndexSlider->value() + dicomSeries->getFirstInstanceNumber();
    OSLM_TRACE("triggered new slice : " << selectedSliceIndex);
    if(!dicomSeries->isInstanceAvailable(selectedSliceIndex))
    {
        if(m_pacsConfiguration)
        {
            m_pullSeriesWorker->post(std::bind(&::ioPacs::SSliceIndexDicomPullerEditor::pullInstance, this));
        }
        else
        {
            SLM_ERROR("There is no instance available for selected slice index.");
        }
    }
    else
    {
        //m_slotReadImage->asyncRun(selectedSliceIndex);
        this->readImage(selectedSliceIndex);
    }
}

//------------------------------------------------------------------------------

void SSliceIndexDicomPullerEditor::readImage(std::size_t selectedSliceIndex)
{
    // DicomSeries
    ::fwMedData::DicomSeries::csptr dicomSeries = this->getInOut< ::fwMedData::DicomSeries >("series");
    SLM_ASSERT("DicomSeries should not be null !", dicomSeries);
    if( dicomSeries->getModality() != "CT" && dicomSeries->getModality() != "MR" && dicomSeries->getModality() != "XA")
    {
        return;
    }

    // Clear temporary seriesDB
    ::fwMedDataTools::helper::SeriesDB sDBTempohelper(m_tempSeriesDB);
    sDBTempohelper.clear();

    // Creates unique temporary folder, no need to check if exists before (see ::fwTools::System::getTemporaryFolder)
    ::boost::filesystem::path path    = ::fwTools::System::getTemporaryFolder("dicom");
    ::boost::filesystem::path tmpPath = path / "tmp";

    SLM_INFO("Create " + tmpPath.string());
    ::boost::filesystem::create_directories(tmpPath);

    SLM_ASSERT("Dicom data shall be available before reading them.",
               dicomSeries->getDicomAvailability() != ::fwMedData::DicomSeries::NONE
               || dicomSeries->isInstanceAvailable(selectedSliceIndex));

    if(dicomSeries->getDicomAvailability() != ::fwMedData::DicomSeries::BINARIES )
    {
        ::fwMedData::DicomSeries::DicomPathContainerType paths = dicomSeries->getLocalDicomPaths();
        ::boost::filesystem::path src                          = paths[selectedSliceIndex];
        ::boost::filesystem::path dest                         = tmpPath / src.filename();

        ::boost::system::error_code err;
        ::boost::filesystem::create_hard_link( src, dest, err );
        if (err.value() != 0)
        {
            SLM_INFO("Copying " + src.string() + " to " + dest.string());
            ::boost::filesystem::copy( src, dest );

            ::boost::system::error_code errPerm;
            ::boost::filesystem::permissions(dest, ::boost::filesystem::owner_all, errPerm);
            SLM_ERROR_IF("set permission error : " + errPerm.message(), errPerm.value());
        }
    }
    else if(dicomSeries->getDicomAvailability() == ::fwMedData::DicomSeries::BINARIES)
    {

        const ::fwMedData::DicomSeries::DicomBinaryContainerType& binaries = dicomSeries->getDicomBinaries();
        ::fwMedData::DicomSeries::DicomBinaryContainerType::const_iterator binary = binaries.begin();
        std::advance(binary, selectedSliceIndex);

        ::fwData::Array::sptr array = binary->second;
        ::fwDataTools::helper::Array arrayHelper(array);
        char* buffer = static_cast<char*>(arrayHelper.getBuffer());
        size_t size  = array->getSizeInBytes();

        ::boost::filesystem::path dest = tmpPath / binary->first;
        ::boost::filesystem::ofstream fs(dest, std::ios::binary|std::ios::trunc);
        FW_RAISE_IF("Can't open '" << tmpPath << "' for write.", !fs.good());

        fs.write(buffer, size);
        fs.close();
    }

    // Read image

    m_dicomReader.lock()->setFolder(tmpPath);
    if(!m_dicomReader.expired())
    {
        m_dicomReader.lock()->update();

        if(m_dicomReader.expired() || m_dicomReader.lock()->isStopped())
        {
            return;
        }
    }
    else
    {
        return;
    }

    //Copy image
    ::fwMedData::ImageSeries::sptr imageSeries;

    if(m_tempSeriesDB->getContainer().size() > 0)
    {
        imageSeries = ::fwMedData::ImageSeries::dynamicCast(*(m_tempSeriesDB->getContainer().begin()));
    }

    if(imageSeries)
    {
        ::fwData::Image::sptr newImage    = imageSeries->getImage();
        ::fwData::Image::SizeType newSize = newImage->getSize();

        newImage->setField(::fwDataTools::fieldHelper::Image::m_axialSliceIndexId, m_axialIndex);
        m_frontalIndex->setValue(static_cast<int>(newSize[0]/2));
        newImage->setField(::fwDataTools::fieldHelper::Image::m_frontalSliceIndexId, m_frontalIndex);
        m_sagittalIndex->setValue(static_cast<int>(newSize[1]/2));
        newImage->setField(::fwDataTools::fieldHelper::Image::m_sagittalSliceIndexId, m_sagittalIndex);

        this->setOutput("image", newImage);
    }

    ::boost::system::error_code ec;
    ::boost::filesystem::remove_all(path, ec);
    SLM_ERROR_IF("remove_all error for path " + path.string() + ": " + ec.message(), ec.value());
}

//------------------------------------------------------------------------------

void SSliceIndexDicomPullerEditor::pullInstance()
{
    SLM_ASSERT("Pacs not configured.", m_pacsConfiguration);

    if( m_pacsConfiguration )
    {
        // Catch any errors
        try
        {
            // DicomSeries
            ::fwMedData::DicomSeries::sptr dicomSeries = this->getInOut< ::fwMedData::DicomSeries >("series");
            SLM_ASSERT("DicomSeries should not be null !", dicomSeries);

            // Get selected slice
            std::size_t selectedSliceIndex = m_sliceIndexSlider->value() + dicomSeries->getFirstInstanceNumber();

            m_seriesEnquirer->initialize(m_pacsConfiguration->getLocalApplicationTitle(),
                                         m_pacsConfiguration->getPacsHostName(),
                                         m_pacsConfiguration->getPacsApplicationPort(),
                                         m_pacsConfiguration->getPacsApplicationTitle(),
                                         m_pacsConfiguration->getMoveApplicationTitle());

            m_seriesEnquirer->connect();
            std::string seriesInstanceUID = dicomSeries->getInstanceUID();
            std::string sopInstanceUID    =
                m_seriesEnquirer->findSOPInstanceUID(seriesInstanceUID, static_cast<unsigned int>(selectedSliceIndex));

            // Check if an instance with the selected Instance Number is found on the PACS
            if(!sopInstanceUID.empty())
            {
                // Pull Selected Series using C-GET Requests
                m_seriesEnquirer->pullInstanceUsingGetRetrieveMethod(seriesInstanceUID, sopInstanceUID);

                // Add path and trigger reading
                ::boost::filesystem::path path     = ::fwTools::System::getTemporaryFolder() / "dicom/";
                ::boost::filesystem::path filePath = path.string() + seriesInstanceUID + "/" + sopInstanceUID;
                dicomSeries->addDicomPath(selectedSliceIndex, filePath);
                //m_slotReadImage->asyncRun(selectedSliceIndex);
                this->readImage(selectedSliceIndex);
            }
            else
            {
                std::stringstream ss;
                ss << "The selected series does not have an instance matching the selected instance number (" <<
                    selectedSliceIndex << ").";
                m_slotDisplayMessage->asyncRun(ss.str());
            }

            // Close connection
            m_seriesEnquirer->disconnect();

        }
        catch (::fwPacsIO::exceptions::Base& exception)
        {
            std::stringstream ss;
            ss << "Unable to connect to the pacs. Please check your configuration: \n"
               << "Pacs host name: " << m_pacsConfiguration->getPacsHostName() << "\n"
               << "Pacs application title: " << m_pacsConfiguration->getPacsApplicationTitle() << "\n"
               << "Pacs port: " << m_pacsConfiguration->getPacsApplicationPort() << "\n";
            m_slotDisplayMessage->asyncRun(ss.str());
            SLM_WARN(exception.what());
        }

    }
    else
    {
        SLM_ERROR("Pacs pull aborted : no pacs configuration found.");
    }
}

//------------------------------------------------------------------------------

void SSliceIndexDicomPullerEditor::displayErrorMessage(const std::string& message) const
{
    SLM_WARN("Error: " + message);
    ::fwGui::dialog::MessageDialog messageBox;
    messageBox.setTitle("Error");
    messageBox.setMessage( message );
    messageBox.setIcon(::fwGui::dialog::IMessageDialog::CRITICAL);
    messageBox.addButton(::fwGui::dialog::IMessageDialog::OK);
    messageBox.show();
}

} // namespace ioPacs
