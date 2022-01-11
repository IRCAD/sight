/************************************************************************
 *
 * Copyright (C) 2020-2022 IRCAD France
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

#include "SSliceIndexDicomEditor.hpp"

#include <core/com/Slots.hxx>
#include <core/tools/System.hpp>

#include <data/helper/MedicalImage.hpp>
#include <data/ImageSeries.hpp>
#include <data/Integer.hpp>

#include <io/dimse/exceptions/Base.hpp>
#include <io/dimse/SeriesEnquirer.hpp>

#include <service/extension/Config.hpp>

#include <ui/qt/container/QtContainer.hpp>

#include <QHBoxLayout>

namespace sight::module::io::dimse
{

static const std::string s_DELAY_CONFIG        = "delay";
static const std::string s_DICOM_READER_CONFIG = "dicomReader";
static const std::string s_READER_CONFIG       = "readerConfig";

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
    sight::ui::base::IGuiContainer::initialize();

    const ConfigType configType = this->getConfigTree();
    const ConfigType config     = configType.get_child("config.<xmlattr>");

    m_delay                     = config.get<unsigned>(s_DELAY_CONFIG, m_delay);
    m_dicomReaderImplementation = config.get(s_DICOM_READER_CONFIG, m_dicomReaderImplementation);
    SIGHT_ERROR_IF("'" + s_DICOM_READER_CONFIG + "' attribute not set", m_dicomReaderImplementation.empty())

    m_readerConfig = configType.get(s_READER_CONFIG, m_readerConfig);
}

//------------------------------------------------------------------------------

void SSliceIndexDicomEditor::starting()
{
    // Create the worker.
    m_requestWorker = core::thread::Worker::New();

    // Create the DICOM reader.
    m_seriesDB = data::SeriesDB::New();

    m_dicomReader = this->registerService<sight::io::base::service::IReader>(m_dicomReaderImplementation);
    SIGHT_ASSERT("Unable to create a reader of type '" + m_dicomReaderImplementation + "'", m_dicomReader);
    m_dicomReader->setWorker(m_requestWorker);
    m_dicomReader->setInOut(m_seriesDB, "data");

    if(!m_readerConfig.empty())
    {
        core::runtime::ConfigurationElement::csptr readerConfig =
            service::extension::Config::getDefault()->getServiceConfig(
                m_readerConfig,
                "sight::io::base::service::IReader"
            );

        SIGHT_ASSERT(
            "No service configuration " << m_readerConfig << " for sight::io::base::service::IReader",
            readerConfig
        );

        m_dicomReader->setConfiguration(core::runtime::ConfigurationElement::constCast(readerConfig));
    }

    m_dicomReader->configure();
    m_dicomReader->start().wait();
    SIGHT_ASSERT("'" + m_dicomReaderImplementation + "' is not started", m_dicomReader->isStarted());

    // Create the timer used to retrieve a slice.
    m_sliceTriggerer = m_associatedWorker->createTimer();
    m_sliceTriggerer->setFunction(
        [&]()
        {
            this->retrieveSlice();
        });
    m_sliceTriggerer->setDuration(std::chrono::milliseconds(m_delay));
    m_sliceTriggerer->setOneShot(true);

    // Create the slider.
    sight::ui::base::IGuiContainer::create();
    auto qtContainer = sight::ui::qt::container::QtContainer::dynamicCast(getContainer());

    QHBoxLayout* layout = new QHBoxLayout();

    m_slider = new QSlider(Qt::Horizontal);
    layout->addWidget(m_slider, 1);

    m_lineEdit = new QLineEdit();
    layout->addWidget(m_lineEdit, 0);
    m_lineEdit->setReadOnly(true);
    m_lineEdit->setMaximumWidth(80);

    qtContainer->setLayout(layout);

    // Connect the slider to the slot.
    QObject::connect(
        m_slider,
        &QSlider::valueChanged,
        this,
        &module::io::dimse::SSliceIndexDicomEditor::changeSliceIndex
    );

    // Update informations.
    this->updating();
}

//-----------------------------------------------------------------------------

service::IService::KeyConnectionsMap SSliceIndexDicomEditor::getAutoConnections() const
{
    service::IService::KeyConnectionsMap connections;
    connections.push(s_DICOMSERIES_INOUT, data::DicomSeries::s_MODIFIED_SIG, s_UPDATE_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

void SSliceIndexDicomEditor::updating()
{
    // Retrieve the DICOM series and its informations.
    const auto dicomSeries   = m_series.lock();
    const size_t sliceNumber = dicomSeries->numInstances();

    if(sliceNumber > 0)
    {
        // If the current slice index is the initial value of the slider, we just send a signal to trigger other
        // services.
        const int currentSlice = m_slider->value();
        if(currentSlice == static_cast<int>(sliceNumber / 2))
        {
            this->changeSliceIndex(currentSlice);
        }
        else
        {
            // Fill slider informations.
            m_slider->setRange(0, static_cast<int>(sliceNumber - 1));
            m_slider->setValue(static_cast<int>(sliceNumber / 2));

            this->setSliderInformation(static_cast<unsigned int>(sliceNumber / 2));
        }
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
    const auto dicomSeries          = m_series.lock();
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
    const auto pacsConfig = m_config.lock();

    auto seriesEnquirer = sight::io::dimse::SeriesEnquirer::New();

    // Initialize connection.
    try
    {
        seriesEnquirer->initialize(
            pacsConfig->getLocalApplicationTitle(),
            pacsConfig->getPacsHostName(),
            pacsConfig->getPacsApplicationPort(),
            pacsConfig->getPacsApplicationTitle(),
            pacsConfig->getMoveApplicationTitle()
        );
        seriesEnquirer->connect();
    }
    catch(const sight::io::dimse::exceptions::Base& _e)
    {
        SIGHT_ERROR("Unable to establish a connection with the PACS: " + std::string(_e.what()));
        this->notify(NotificationType::FAILURE, "Unable to connect to PACS");
    }

    const auto dicomSeries = m_series.lock();

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
            switch(pacsConfig->getRetrieveMethod())
            {
                case sight::io::dimse::data::PacsConfiguration::GET_RETRIEVE_METHOD:
                    seriesEnquirer->pullInstanceUsingGetRetrieveMethod(seriesInstanceUID, sopInstanceUID);
                    break;

                case sight::io::dimse::data::PacsConfiguration::MOVE_RETRIEVE_METHOD:
                    seriesEnquirer->pullInstanceUsingMoveRetrieveMethod(seriesInstanceUID, sopInstanceUID);
                    break;

                default:
                    SIGHT_ERROR("Unknown retrieve method, 'get' will be used");
                    seriesEnquirer->pullInstanceUsingGetRetrieveMethod(seriesInstanceUID, sopInstanceUID);
                    break;
            }

            // Compute the path and add it to the DICOM series.
            std::filesystem::path tmpPath      = core::tools::System::getTemporaryFolder() / "dicom";
            std::filesystem::path downloadPath = tmpPath / seriesInstanceUID / sopInstanceUID;
            dicomSeries->addDicomPath(_selectedSliceIndex, downloadPath);

            success = true;
        }
        else
        {
            this->notify(NotificationType::FAILURE, "No instance found");
        }
    }
    catch(const sight::io::dimse::exceptions::Base& _e)
    {
        SIGHT_ERROR("Unable to execute query to the PACS: " + std::string(_e.what()));
        this->notify(NotificationType::FAILURE, "Unable to execute query");
    }
    catch(const std::filesystem::filesystem_error& _e)
    {
        SIGHT_ERROR(std::string(_e.what()));
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

void SSliceIndexDicomEditor::readSlice(
    const data::mt::locked_ptr<data::DicomSeries>& _dicomSeries,
    std::size_t _selectedSliceIndex
) const
{
    // Retrieve informations.
    const std::string modality = _dicomSeries->getModality();
    if(modality != "CT" && modality != "MR" && modality != "XA")
    {
        this->notify(NotificationType::INFO, "Unable to read the modality '" + modality + "'");
        return;
    }

    // Get the DICOM buffer to write in a temporary folder.
    const auto& binaries = _dicomSeries->getDicomContainer();
    auto iter            = binaries.find(_selectedSliceIndex);
    SIGHT_ASSERT("Index '" << _selectedSliceIndex << "' is not found in DicomSeries", iter != binaries.end());
    const core::memory::BufferObject::sptr bufferObj = iter->second;
    const core::memory::BufferObject::Lock lockerDest(bufferObj);
    const char* buffer      = static_cast<char*>(lockerDest.getBuffer());
    const size_t bufferSize = bufferObj->getSize();

    // Creates unique temporary folder to save the DICOM instance.
    std::filesystem::path tmpPath = core::tools::System::getTemporaryFolder("dicom");
    std::filesystem::create_directories(tmpPath);

    // Open the temporary folder and write the buffer.
    std::filesystem::path path = tmpPath / std::to_string(_selectedSliceIndex);
    std::ofstream fs(path, std::ios::binary | std::ios::trunc);
    if(!fs.good())
    {
        SIGHT_ERROR("Unable to open '" << path << "' for write.");
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
        const data::ImageSeries::sptr imageSeries =
            data::ImageSeries::dynamicCast(*(m_seriesDB->getContainer().begin()));
        const data::Image::sptr newImage = imageSeries->getImage();

        const auto image = m_image.lock();
        image->deepCopy(newImage);

        data::Integer::sptr axialIndex    = data::Integer::New(0);
        data::Integer::sptr frontalIndex  = data::Integer::New(image->getSize()[0] / 2);
        data::Integer::sptr sagittalIndex = data::Integer::New(image->getSize()[1] / 2);

        data::helper::MedicalImage::setSliceIndex(
            *image,
            data::helper::MedicalImage::orientation_t::AXIAL,
            axialIndex->value()
        );
        data::helper::MedicalImage::setSliceIndex(
            *image,
            data::helper::MedicalImage::orientation_t::FRONTAL,
            frontalIndex->value()
        );
        data::helper::MedicalImage::setSliceIndex(
            *image,
            data::helper::MedicalImage::orientation_t::SAGITTAL,
            sagittalIndex->value()
        );

        // Send the signal
        const auto sig = image->signal<data::Image::ModifiedSignalType>(data::Image::s_MODIFIED_SIG);
        sig->asyncEmit();
    }
    else
    {
        SIGHT_ERROR("Unable to read the image");
        this->notify(NotificationType::FAILURE, "Unable to read the image");
    }
}

} // namespace sight::module::io::dimse.
