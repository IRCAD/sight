/************************************************************************
 *
 * Copyright (C) 2020-2021 IRCAD France
 * Copyright (C) 2020 IHU Strasbourg
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

#include "ioPacs/SSliceIndexDicomEditor.hpp"

#include <core/com/Slots.hxx>
#include <core/tools/System.hpp>

#include <fwData/Image.hpp>
#include <fwData/Integer.hpp>

#include <fwDataTools/fieldHelper/Image.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwMedData/ImageSeries.hpp>

#include <fwPacsIO/data/PacsConfiguration.hpp>
#include <fwPacsIO/exceptions/Base.hpp>
#include <fwPacsIO/SeriesEnquirer.hpp>

#include <fwServices/registry/ServiceConfig.hpp>

#include <QHBoxLayout>

namespace ioPacs
{

static const std::string s_DELAY_CONFIG        = "delay";
static const std::string s_DICOM_READER_CONFIG = "dicomReader";
static const std::string s_READER_CONFIG       = "readerConfig";

static const ::fwServices::IService::KeyType s_DICOMSERIES_INOUT = "series";
static const ::fwServices::IService::KeyType s_IMAGE_INOUT       = "image";

static const ::fwServices::IService::KeyType s_PACS_INPUT = "pacsConfig";

//------------------------------------------------------------------------------

SSliceIndexDicomEditor::SSliceIndexDicomEditor() noexcept
{
}

//------------------------------------------------------------------------------

SSliceIndexDicomEditor::~SSliceIndexDicomEditor() noexcept
{
}

//------------------------------------------------------------------------------

void SSliceIndexDicomEditor::configuring()
{
    ::fwGui::IGuiContainerSrv::initialize();

    const ConfigType configType = this->getConfigTree();
    const ConfigType config     = configType.get_child("config.<xmlattr>");

    m_delay                     = config.get<unsigned>(s_DELAY_CONFIG, m_delay);
    m_dicomReaderImplementation = config.get(s_DICOM_READER_CONFIG, m_dicomReaderImplementation);
    SLM_ERROR_IF("'" + s_DICOM_READER_CONFIG + "' attribute not set", m_dicomReaderImplementation.empty())

    m_readerConfig = configType.get(s_READER_CONFIG, m_readerConfig);
}

//------------------------------------------------------------------------------

void SSliceIndexDicomEditor::starting()
{
    // Create the worker.
    m_requestWorker = core::thread::Worker::New();

    // Create the DICOM reader.
    m_seriesDB = ::fwMedData::SeriesDB::New();

    m_dicomReader = this->registerService< ::fwIO::IReader >(m_dicomReaderImplementation);
    SLM_ASSERT("Unable to create a reader of type '" + m_dicomReaderImplementation + "'", m_dicomReader);
    m_dicomReader->setWorker(m_requestWorker);
    m_dicomReader->registerInOut(m_seriesDB, "data");

    if(!m_readerConfig.empty())
    {
        core::runtime::ConfigurationElement::csptr readerConfig =
            ::fwServices::registry::ServiceConfig::getDefault()->getServiceConfig(
                m_readerConfig, "::fwIO::IReader");

        SLM_ASSERT("No service configuration " << m_readerConfig << " for ::fwIO::IReader", readerConfig);

        m_dicomReader->setConfiguration( core::runtime::ConfigurationElement::constCast(readerConfig) );
    }

    m_dicomReader->configure();
    m_dicomReader->start().wait();
    SLM_ASSERT("'" + m_dicomReaderImplementation + "' is not started", m_dicomReader->isStarted());

    // Create the timer used to retrieve a slice.
    m_sliceTriggerer = m_associatedWorker->createTimer();
    m_sliceTriggerer->setFunction([&]()
        {
            this->retrieveSlice();
        });
    m_sliceTriggerer->setDuration(std::chrono::milliseconds(m_delay));
    m_sliceTriggerer->setOneShot(true);

    // Create the slider.
    ::fwGui::IGuiContainerSrv::create();
    ::fwGuiQt::container::QtContainer::sptr qtContainer = fwGuiQt::container::QtContainer::dynamicCast(getContainer());

    QHBoxLayout* layout = new QHBoxLayout();

    m_slider = new QSlider(Qt::Horizontal);
    layout->addWidget(m_slider, 1);

    m_lineEdit = new QLineEdit();
    layout->addWidget(m_lineEdit, 0);
    m_lineEdit->setReadOnly(true);
    m_lineEdit->setMaximumWidth(80);

    qtContainer->setLayout(layout);

    // Connect the slider to the slot.
    QObject::connect(m_slider, &QSlider::valueChanged, this, &::ioPacs::SSliceIndexDicomEditor::changeSliceIndex);

    // Update informations.
    this->updating();
}

//-----------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SSliceIndexDicomEditor::getAutoConnections() const
{
    ::fwServices::IService::KeyConnectionsMap connections;
    connections.push(s_DICOMSERIES_INOUT, ::fwMedData::DicomSeries::s_MODIFIED_SIG, s_UPDATE_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

void SSliceIndexDicomEditor::updating()
{
    // Retrieve the DICOM series and its informations.
    const auto dicomSeries   = this->getLockedInOut< const ::fwMedData::DicomSeries >(s_DICOMSERIES_INOUT);
    const size_t sliceNumber = dicomSeries->getNumberOfInstances();

    // If the current slice index is the initial value of the slider, we just send a signal to trigger other services.
    const int currentSlice = m_slider->value();
    if(currentSlice == static_cast<int>(sliceNumber/2))
    {
        this->changeSliceIndex(currentSlice);
    }
    else
    {
        // Fill slider informations.
        m_slider->setRange(0, static_cast<int>(sliceNumber-1));
        m_slider->setValue(static_cast<int>(sliceNumber/2));

        this->setSliderInformation(static_cast<unsigned int>(sliceNumber/2));
    }
}

//------------------------------------------------------------------------------

void SSliceIndexDicomEditor::stopping()
{
    this->unregisterServices();

    // Stop the worker.
    m_requestWorker->stop();
    m_requestWorker.reset();

    this->destroy();
}

//------------------------------------------------------------------------------

void SSliceIndexDicomEditor::changeSliceIndex(int _value)
{
    this->setSliderInformation(static_cast<unsigned int>(_value));

    // Get the new slice if there is no change for m_delay milliseconds.
    m_sliceTriggerer->start();
}

//------------------------------------------------------------------------------

void SSliceIndexDicomEditor::setSliderInformation(unsigned _value)
{
    std::stringstream text;
    text << _value << " / " << m_slider->maximum();
    m_lineEdit->setText(std::string(text.str()).c_str());
}

//------------------------------------------------------------------------------

void SSliceIndexDicomEditor::retrieveSlice()
{
    // Check if the slice already exists.
    const auto dicomSeries          = this->getLockedInOut< ::fwMedData::DicomSeries >(s_DICOMSERIES_INOUT);
    const size_t selectedSliceIndex = m_slider->value() + dicomSeries->getFirstInstanceNumber();
    const bool isInstanceAvailable  = dicomSeries->isInstanceAvailable(selectedSliceIndex);

    // If the slice is not pulled, pull it.
    if(!isInstanceAvailable)
    {
        m_requestWorker->post(std::bind(&SSliceIndexDicomEditor::pullSlice, this, selectedSliceIndex));
    }
    else
    {
        this->readSlice(dicomSeries, selectedSliceIndex);
    }
}

//------------------------------------------------------------------------------

void SSliceIndexDicomEditor::pullSlice(std::size_t _selectedSliceIndex) const
{
    bool success = false;

    // Retrieve informations.
    const auto pacsConfig = this->getLockedInput< const ::fwPacsIO::data::PacsConfiguration >(s_PACS_INPUT);

    ::fwPacsIO::SeriesEnquirer::sptr seriesEnquirer = ::fwPacsIO::SeriesEnquirer::New();

    // Initialize connection.
    try
    {
        seriesEnquirer->initialize(
            pacsConfig->getLocalApplicationTitle(),
            pacsConfig->getPacsHostName(),
            pacsConfig->getPacsApplicationPort(),
            pacsConfig->getPacsApplicationTitle(),
            pacsConfig->getMoveApplicationTitle());
        seriesEnquirer->connect();
    }
    catch(const ::fwPacsIO::exceptions::Base& _e)
    {
        SLM_ERROR("Unable to establish a connection with the PACS: " + std::string(_e.what()));
        const auto notif = this->signal< ::fwServices::IService::FailureNotifiedSignalType >(
            ::fwServices::IService::s_FAILURE_NOTIFIED_SIG);
        notif->asyncEmit("Unable to connect to PACS");
    }

    const auto dicomSeries = this->getLockedInOut< ::fwMedData::DicomSeries >(s_DICOMSERIES_INOUT);

    // Get selected slice.
    try
    {
        const std::string seriesInstanceUID = dicomSeries->getInstanceUID();
        const std::string sopInstanceUID    =
            seriesEnquirer->findSOPInstanceUID(seriesInstanceUID, static_cast<unsigned int>(_selectedSliceIndex));

        // Check if an instance with the selected Instance Number is found on the PACS.
        if(!sopInstanceUID.empty())
        {
            // Pull selected series and save it to the temporary folder.
            switch (pacsConfig->getRetrieveMethod())
            {
                case ::fwPacsIO::data::PacsConfiguration::GET_RETRIEVE_METHOD:
                    seriesEnquirer->pullInstanceUsingGetRetrieveMethod(seriesInstanceUID, sopInstanceUID);
                    break;
                case ::fwPacsIO::data::PacsConfiguration::MOVE_RETRIEVE_METHOD:
                    seriesEnquirer->pullInstanceUsingMoveRetrieveMethod(seriesInstanceUID, sopInstanceUID);
                    break;
                default:
                    SLM_ERROR("Unknown retrieve method, 'get' will be used");
                    seriesEnquirer->pullInstanceUsingGetRetrieveMethod(seriesInstanceUID, sopInstanceUID);
                    break;
            }

            // Compute the path and add it to the DICOM series.
            std::filesystem::path tmpPath      = core::tools::System::getTemporaryFolder() / "dicom/";
            std::filesystem::path downloadPath = tmpPath.string() + seriesInstanceUID + "/" + sopInstanceUID;
            dicomSeries->addDicomPath(_selectedSliceIndex, downloadPath);

            success = true;
        }
        else
        {
            const auto notif = this->signal< ::fwServices::IService::FailureNotifiedSignalType >(
                ::fwServices::IService::s_FAILURE_NOTIFIED_SIG);
            notif->asyncEmit("No instance found");
        }

    }
    catch(const ::fwPacsIO::exceptions::Base& _e)
    {
        SLM_ERROR("Unable to execute query to the PACS: " + std::string(_e.what()));
        const auto notif = this->signal< ::fwServices::IService::FailureNotifiedSignalType >(
            ::fwServices::IService::s_FAILURE_NOTIFIED_SIG);
        notif->asyncEmit("Unable to execute query");
    }

    // Disconnect the series enquirer.
    if(seriesEnquirer->isConnectedToPacs())
    {
        seriesEnquirer->disconnect();
    }

    if(success)
    {
        this->readSlice(dicomSeries, _selectedSliceIndex);
    }
}

//------------------------------------------------------------------------------

void SSliceIndexDicomEditor::readSlice(const ::fwData::mt::locked_ptr< ::fwMedData::DicomSeries >& _dicomSeries,
                                       std::size_t _selectedSliceIndex) const
{
    // Retrieve informations.
    const std::string modality = _dicomSeries->getModality();
    if(modality != "CT" && modality != "MR" && modality != "XA")
    {
        const auto notif = this->signal< ::fwServices::IService::InfoNotifiedSignalType >(
            ::fwServices::IService::s_INFO_NOTIFIED_SIG);
        notif->asyncEmit("Unable to read the modality '" + modality + "'");
        return;
    }

    // Get the DICOM buffer to write in a temporary folder.
    const auto& binaries = _dicomSeries->getDicomContainer();
    auto iter            = binaries.find(_selectedSliceIndex);
    SLM_ASSERT("Index '" << _selectedSliceIndex << "' is not found in DicomSeries", iter != binaries.end());
    const core::memory::BufferObject::sptr bufferObj = iter->second;
    const core::memory::BufferObject::Lock lockerDest(bufferObj);
    const char* buffer      = static_cast<char*>(lockerDest.getBuffer());
    const size_t bufferSize = bufferObj->getSize();

    // Creates unique temporary folder to save the DICOM instance.
    std::filesystem::path tmpPath = core::tools::System::getTemporaryFolder("dicom");
    std::filesystem::create_directories(tmpPath);

    // Open the temporary folder and write the buffer.
    std::filesystem::path path = tmpPath / std::to_string(_selectedSliceIndex);
    std::ofstream fs(path, std::ios::binary|std::ios::trunc);
    if(!fs.good())
    {
        SLM_ERROR("Unable to open '" << path << "' for write.");
        return;
    }
    fs.write(buffer, bufferSize);
    fs.close();

    // Read the image.
    m_dicomReader->setFolder(tmpPath);
    m_dicomReader->update();

    if(!m_dicomReader->hasFailed() && m_seriesDB->getContainer().size() > 0)
    {
        // Copy the read serie to the image.
        const ::fwMedData::ImageSeries::sptr imageSeries =
            ::fwMedData::ImageSeries::dynamicCast(*(m_seriesDB->getContainer().begin()));
        const ::fwData::Image::sptr newImage = imageSeries->getImage();

        const auto image = this->getLockedInOut< ::fwData::Image >(s_IMAGE_INOUT);
        image->deepCopy(newImage);

        ::fwData::Integer::sptr axialIndex    = ::fwData::Integer::New(0);
        ::fwData::Integer::sptr frontalIndex  = ::fwData::Integer::New(image->getSize2()[0]/2);
        ::fwData::Integer::sptr sagittalIndex = ::fwData::Integer::New(image->getSize2()[1]/2);

        image->setField(::fwDataTools::fieldHelper::Image::m_axialSliceIndexId, axialIndex);
        image->setField(::fwDataTools::fieldHelper::Image::m_frontalSliceIndexId, frontalIndex);
        image->setField(::fwDataTools::fieldHelper::Image::m_sagittalSliceIndexId, sagittalIndex);

        // Send the signal
        const auto sig = image->signal< ::fwData::Image::ModifiedSignalType >(::fwData::Image::s_MODIFIED_SIG);
        sig->asyncEmit();
    }
    else
    {
        SLM_ERROR("Unable to read the image");
        const auto notif = this->signal< ::fwServices::IService::FailureNotifiedSignalType >(
            ::fwServices::IService::s_FAILURE_NOTIFIED_SIG);
        notif->asyncEmit("Unable to read the image");
    }
}

} // namespace ioPacs.
