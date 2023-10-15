/************************************************************************
 *
 * Copyright (C) 2020-2023 IRCAD France
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

#include "slice_index_dicom_editor.hpp"

#include <core/com/slots.hxx>
#include <core/os/temp_path.hpp>

#include <data/helper/MedicalImage.hpp>
#include <data/image_series.hpp>
#include <data/integer.hpp>

#include <io/dimse/exceptions/Base.hpp>
#include <io/dimse/SeriesEnquirer.hpp>

#include <service/extension/config.hpp>

#include <ui/qt/container/widget.hpp>

#include <QHBoxLayout>

namespace sight::module::io::dimse
{

static const std::string s_DELAY_CONFIG        = "delay";
static const std::string s_DICOM_READER_CONFIG = "dicomReader";
static const std::string s_READER_CONFIG       = "readerConfig";

//------------------------------------------------------------------------------

slice_index_dicom_editor::slice_index_dicom_editor() noexcept :
    sight::service::notifier(m_signals)
{
}

//------------------------------------------------------------------------------

void slice_index_dicom_editor::configuring(const config_t& _config)
{
    sight::ui::service::initialize();

    const config_t config = _config.get_child("config.<xmlattr>");

    m_delay                     = config.get<unsigned>(s_DELAY_CONFIG, m_delay);
    m_dicomReaderImplementation = config.get(s_DICOM_READER_CONFIG, m_dicomReaderImplementation);
    SIGHT_ERROR_IF("'" + s_DICOM_READER_CONFIG + "' attribute not set", m_dicomReaderImplementation.empty())

    m_readerConfig = _config.get(s_READER_CONFIG, m_readerConfig);
}

//------------------------------------------------------------------------------

void slice_index_dicom_editor::starting()
{
    // Create the worker.
    m_requestWorker = core::thread::worker::make();

    // Create the DICOM reader.
    m_series_set = std::make_shared<data::series_set>();

    m_dicomReader = this->registerService<sight::io::service::reader>(m_dicomReaderImplementation);
    SIGHT_ASSERT("Unable to create a reader of type '" + m_dicomReaderImplementation + "'", m_dicomReader);
    m_dicomReader->set_worker(m_requestWorker);
    m_dicomReader->set_inout(m_series_set, "data");

    if(!m_readerConfig.empty())
    {
        const auto readerConfig =
            sight::service::extension::config::getDefault()->get_service_config(
                m_readerConfig,
                "sight::io::service::reader"
            );

        SIGHT_ASSERT(
            "No service configuration " << m_readerConfig << " for sight::io::service::reader",
            !readerConfig.empty()
        );

        m_dicomReader->set_config(readerConfig);
    }

    m_dicomReader->configure();
    m_dicomReader->start().wait();
    SIGHT_ASSERT("'" + m_dicomReaderImplementation + "' is not started", m_dicomReader->started());

    // Create the timer used to retrieve a slice.
    m_sliceTrigger = this->worker()->create_timer();
    m_sliceTrigger->set_function(
        [&]()
        {
            this->retrieveSlice();
        });
    m_sliceTrigger->set_duration(std::chrono::milliseconds(m_delay));
    m_sliceTrigger->set_one_shot(true);

    // Create the slider.
    sight::ui::service::create();
    auto qtContainer = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(getContainer());

    auto* layout = new QHBoxLayout();

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
        &module::io::dimse::slice_index_dicom_editor::changeSliceIndex
    );

    // Update informations.
    this->updating();
}

//-----------------------------------------------------------------------------

service::connections_t slice_index_dicom_editor::auto_connections() const
{
    service::connections_t connections;
    connections.push(s_DICOMSERIES_INOUT, data::dicom_series::MODIFIED_SIG, service::slots::UPDATE);

    return connections;
}

//------------------------------------------------------------------------------

void slice_index_dicom_editor::updating()
{
    // Retrieve the DICOM series and its informations.
    const auto dicomSeries        = m_series.lock();
    const std::size_t sliceNumber = dicomSeries->numInstances();

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

void slice_index_dicom_editor::stopping()
{
    this->unregisterServices();

    // Stop the worker.
    m_requestWorker->stop();
    m_requestWorker.reset();

    this->destroy();
}

//------------------------------------------------------------------------------

void slice_index_dicom_editor::changeSliceIndex(int _value)
{
    this->setSliderInformation(static_cast<unsigned int>(_value));

    // Get the new slice if there is no change for m_delay milliseconds.
    m_sliceTrigger->start();
}

//------------------------------------------------------------------------------

void slice_index_dicom_editor::setSliderInformation(unsigned _value)
{
    std::stringstream text;
    text << _value << " / " << m_slider->maximum();
    m_lineEdit->setText(std::string(text.str()).c_str());
}

//------------------------------------------------------------------------------

void slice_index_dicom_editor::retrieveSlice()
{
    // Check if the slice already exists.
    const auto dicomSeries               = m_series.lock();
    const std::size_t selectedSliceIndex = std::size_t(m_slider->value()) + dicomSeries->getFirstInstanceNumber();
    const bool isInstanceAvailable       = dicomSeries->isInstanceAvailable(selectedSliceIndex);

    // If the slice is not pulled, pull it.
    if(!isInstanceAvailable)
    {
        m_requestWorker->post([this, selectedSliceIndex](auto&& ...){pullSlice(selectedSliceIndex);});
    }
    else
    {
        this->readSlice(dicomSeries, selectedSliceIndex);
    }
}

//------------------------------------------------------------------------------

void slice_index_dicom_editor::pullSlice(std::size_t _selectedSliceIndex) const
{
    bool success = false;

    // Retrieve informations.
    const auto pacsConfig = m_config.lock();

    auto seriesEnquirer = std::make_shared<sight::io::dimse::SeriesEnquirer>();

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
        this->notifier::failure("Unable to connect to PACS");
    }

    const auto dicomSeries = m_series.lock();

    // Get selected slice.
    try
    {
        const std::string seriesInstanceUID = dicomSeries->getSeriesInstanceUID();
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
            std::filesystem::path tmpPath      = core::os::temp_dir::shared_directory() / "dicom";
            std::filesystem::path downloadPath = tmpPath / seriesInstanceUID / sopInstanceUID;
            dicomSeries->addDicomPath(_selectedSliceIndex, downloadPath);

            success = true;
        }
        else
        {
            this->notifier::failure("No instance found");
        }
    }
    catch(const sight::io::dimse::exceptions::Base& _e)
    {
        SIGHT_ERROR("Unable to execute query to the PACS: " + std::string(_e.what()));
        this->notifier::failure("Unable to execute query");
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

void slice_index_dicom_editor::readSlice(
    const data::mt::locked_ptr<data::dicom_series>& _dicomSeries,
    std::size_t _selectedSliceIndex
) const
{
    // Retrieve informations.
    const std::string modality = _dicomSeries->getModality();
    if(modality != "CT" && modality != "MR" && modality != "XA")
    {
        this->notifier::info("Unable to read the modality '" + modality + "'");
        return;
    }

    // Get the DICOM buffer to write in a temporary folder.
    const auto& binaries = _dicomSeries->getDicomContainer();
    auto iter            = binaries.find(_selectedSliceIndex);
    SIGHT_ASSERT("Index '" << _selectedSliceIndex << "' is not found in DicomSeries", iter != binaries.end());
    const core::memory::buffer_object::sptr bufferObj = iter->second;
    const core::memory::buffer_object::lock_t lockerDest(bufferObj);
    const char* buffer           = static_cast<char*>(lockerDest.buffer());
    const std::size_t bufferSize = bufferObj->size();

    // Creates unique temporary folder to save the DICOM instance.
    // Do not delete the folder, as we may use the file in the DicomReference.
    const auto tmpDir = core::os::temp_dir::shared_directory("do_not_delete_");

    // Open the temporary folder and write the buffer.
    std::filesystem::path path = tmpDir / std::to_string(_selectedSliceIndex);
    std::ofstream fs(path, std::ios::binary | std::ios::trunc);
    if(!fs.good())
    {
        SIGHT_ERROR("Unable to open '" << path << "' for write.");
        return;
    }

    fs.write(buffer, std::streamsize(bufferSize));
    fs.close();

    // Read the image.
    m_dicomReader->set_folder(tmpDir);
    m_dicomReader->update().wait();

    if(!m_dicomReader->hasFailed() && !m_series_set->empty())
    {
        // Copy the read series to the image.
        const auto imageSeries = std::dynamic_pointer_cast<data::image_series>(m_series_set->front());

        const auto image = m_image.lock();
        image->deep_copy(imageSeries);

        data::integer::sptr axialIndex   = std::make_shared<data::integer>(0);
        data::integer::sptr frontalIndex =
            std::make_shared<data::integer>(static_cast<std::int64_t>(image->size()[0] / 2));
        data::integer::sptr sagittalIndex =
            std::make_shared<data::integer>(static_cast<std::int64_t>(image->size()[1] / 2));

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
        const auto sig = image->signal<data::image::ModifiedSignalType>(data::image::MODIFIED_SIG);
        sig->async_emit();
    }
    else
    {
        SIGHT_ERROR("Unable to read the image");
        this->notifier::failure("Unable to read the image");
    }
}

} // namespace sight::module::io::dimse.
