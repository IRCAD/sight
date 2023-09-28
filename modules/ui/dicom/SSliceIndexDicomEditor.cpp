/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "SSliceIndexDicomEditor.hpp"

#include <core/com/slots.hpp>
#include <core/com/slots.hxx>
#include <core/os/temp_path.hpp>
#include <core/thread/timer.hpp>

#include <data/Array.hpp>
#include <data/Composite.hpp>
#include <data/helper/MedicalImage.hpp>
#include <data/ImageSeries.hpp>
#include <data/Integer.hpp>
#include <data/SeriesSet.hpp>

#include <service/op.hpp>

#include <ui/__/dialog/message.hpp>
#include <ui/qt/container/widget.hpp>

#include <boost/lexical_cast.hpp>

#include <QApplication>
#include <QComboBox>
#include <QHBoxLayout>
#include <QMouseEvent>

#include <filesystem>
#include <fstream>
#include <iterator>

namespace sight::module::ui::dicom
{

const core::com::slots::key_t SSliceIndexDicomEditor::READ_IMAGE_SLOT      = "readImage";
const core::com::slots::key_t SSliceIndexDicomEditor::DISPLAY_MESSAGE_SLOT = "displayErrorMessage";

//------------------------------------------------------------------------------

SSliceIndexDicomEditor::SSliceIndexDicomEditor() noexcept
{
    m_slotReadImage = new_slot(READ_IMAGE_SLOT, &SSliceIndexDicomEditor::readImage, this);
    new_slot(DISPLAY_MESSAGE_SLOT, &SSliceIndexDicomEditor::displayErrorMessage);
}

//------------------------------------------------------------------------------

SSliceIndexDicomEditor::~SSliceIndexDicomEditor() noexcept =
    default;

//------------------------------------------------------------------------------

void SSliceIndexDicomEditor::configuring()
{
    sight::ui::service::initialize();

    const auto& config = this->getConfiguration();

    m_dicomReaderType = config.get<std::string>("config.<xmlattr>.dicomReader", m_dicomReaderType);
    m_delay           = config.get<std::size_t>("config.<xmlattr>.delay", m_delay);

    if(const auto readerConfig = config.get_child_optional("readerConfig"); readerConfig.has_value())
    {
        m_readerConfig = readerConfig.value();
    }
}

//------------------------------------------------------------------------------

void SSliceIndexDicomEditor::starting()
{
    m_delayTimer2 = this->worker()->create_timer();

    sight::ui::service::create();
    auto qtContainer = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(getContainer());

    auto* layout = new QHBoxLayout();

    const auto dicomSeries = m_dicomSeries.lock();
    SIGHT_ASSERT("DicomSeries should not be null !", dicomSeries);
    m_numberOfSlices = dicomSeries->numInstances();

    // Slider
    m_sliceIndexSlider = new QSlider(Qt::Horizontal);
    layout->addWidget(m_sliceIndexSlider, 1);
    m_sliceIndexSlider->setRange(0, static_cast<int>(m_numberOfSlices - 1));
    m_sliceIndexSlider->setValue(static_cast<int>(m_numberOfSlices / 2));

    // Line Edit
    m_sliceIndexLineEdit = new QLineEdit();
    layout->addWidget(m_sliceIndexLineEdit, 0);
    m_sliceIndexLineEdit->setReadOnly(true);
    m_sliceIndexLineEdit->setMaximumWidth(80);

    std::stringstream ss;
    ss << m_sliceIndexSlider->value() << " / " << (m_numberOfSlices - 1);
    m_sliceIndexLineEdit->setText(std::string(ss.str()).c_str());

    qtContainer->setLayout(layout);

    // Connect the signals
    QObject::connect(m_sliceIndexSlider, SIGNAL(valueChanged(int)), this, SLOT(changeSliceIndex(int)));

    // Create temporary SeriesSet
    m_tmp_series_set = std::make_shared<data::SeriesSet>();

    // Create reader
    auto dicomReader = sight::service::add<sight::io::service::reader>(m_dicomReaderType);
    SIGHT_ASSERT(
        "Unable to create a reader of type: \"" + m_dicomReaderType + "\" in "
                                                                      "sight::module::ui::dicom::SSliceIndexDicomEditor.",
        dicomReader
    );
    dicomReader->setInOut(m_tmp_series_set, sight::io::service::s_DATA_KEY);
    dicomReader->setConfiguration(m_readerConfig);
    dicomReader->configure();
    dicomReader->start();

    m_dicomReader = dicomReader;

    // Image Indexes
    m_axialIndex    = std::make_shared<data::Integer>(0);
    m_frontalIndex  = std::make_shared<data::Integer>(0);
    m_sagittalIndex = std::make_shared<data::Integer>(0);

    // Load a slice
    auto duration = std::chrono::milliseconds(m_delay);
    m_delayTimer2->set_function(
        [this]()
        {
            this->triggerNewSlice();
        });
    m_delayTimer2->set_duration(duration);
    m_delayTimer2->set_one_shot(true);

    this->triggerNewSlice();
}

//------------------------------------------------------------------------------

void SSliceIndexDicomEditor::stopping()
{
    // Stop dicom reader
    if(!m_dicomReader.expired())
    {
        m_dicomReader.lock()->stop();
        sight::service::remove(m_dicomReader.lock());
    }

    // Disconnect the signals
    QObject::disconnect(m_sliceIndexSlider, SIGNAL(valueChanged(int)), this, SLOT(changeSliceIndex(int)));

    this->destroy();
}

//------------------------------------------------------------------------------

void SSliceIndexDicomEditor::updating()
{
}

//------------------------------------------------------------------------------

void SSliceIndexDicomEditor::info(std::ostream& _sstream)
{
    _sstream << "SSliceIndexDicomEditor::info";
}

//------------------------------------------------------------------------------

void SSliceIndexDicomEditor::changeSliceIndex(int /*unused*/)
{
    // Update text
    std::stringstream ss;
    ss << m_sliceIndexSlider->value() << " / " << (m_numberOfSlices - 1);
    m_sliceIndexLineEdit->setText(std::string(ss.str()).c_str());

    // Get the new slice if there is no change for m_delay milliseconds
    m_delayTimer2->start();
}

//------------------------------------------------------------------------------

void SSliceIndexDicomEditor::triggerNewSlice()
{
    // DicomSeries
    const auto dicomSeries = m_dicomSeries.lock();
    SIGHT_ASSERT("DicomSeries should not be null !", dicomSeries);

    // Compute slice index
    std::size_t selectedSliceIndex = static_cast<std::size_t>(m_sliceIndexSlider->value())
                                     + dicomSeries->getFirstInstanceNumber();

    SIGHT_ERROR_IF(
        "There is no instance available for selected slice index.",
        !dicomSeries->isInstanceAvailable(selectedSliceIndex)
    );

    if(dicomSeries->isInstanceAvailable(selectedSliceIndex))
    {
        m_slotReadImage->async_run(selectedSliceIndex);
    }
}

//------------------------------------------------------------------------------

void SSliceIndexDicomEditor::readImage(std::size_t selectedSliceIndex)
{
    // DicomSeries
    const auto dicomSeries = m_dicomSeries.lock();
    SIGHT_ASSERT("DicomSeries should not be null !", dicomSeries);

    auto isModalitySupported = [](const data::Series& series)
                               {
                                   return series.getModality() == "CT"
                                          || series.getModality() == "MR"
                                          || series.getModality() == "XA";
                               };

    if(!isModalitySupported(*dicomSeries))
    {
        return;
    }

    // Clear temporary series_set
    const auto scoped_emitter = m_tmp_series_set->scoped_emit();
    m_tmp_series_set->clear();

    // Creates unique temporary folder
    core::os::temp_dir tmpDir;
    std::filesystem::path tmpPath = tmpDir / "tmp";

    SIGHT_INFO("Create " + tmpPath.string());
    std::filesystem::create_directories(tmpPath);

    const auto& binaries = dicomSeries->getDicomContainer();
    auto iter            = binaries.find(selectedSliceIndex);
    SIGHT_ASSERT("Index '" << selectedSliceIndex << "' is not found in DicomSeries", iter != binaries.end());

    const core::memory::buffer_object::sptr bufferObj = iter->second;
    const core::memory::buffer_object::lock_t lockerDest(bufferObj);
    const char* buffer     = static_cast<char*>(lockerDest.buffer());
    const std::size_t size = bufferObj->size();

    const std::filesystem::path dest = tmpPath / std::to_string(selectedSliceIndex);
    std::ofstream fs(dest, std::ios::binary | std::ios::trunc);
    SIGHT_THROW_IF("Can't open '" << tmpPath << "' for write.", !fs.good());

    fs.write(buffer, static_cast<std::int64_t>(size));
    fs.close();

    // Read image
    m_dicomReader.lock()->set_folder(tmpPath);
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
    data::ImageSeries::sptr imageSeries;

    if(!m_tmp_series_set->empty())
    {
        auto series = m_tmp_series_set->front();
        if(isModalitySupported(*series))
        {
            imageSeries = std::dynamic_pointer_cast<data::ImageSeries>(series);
        }
    }

    if(imageSeries)
    {
        const data::Image::Size newSize = imageSeries->size();

        m_frontalIndex->setValue(static_cast<int>(newSize[0] / 2));
        m_sagittalIndex->setValue(static_cast<int>(newSize[1] / 2));

        data::helper::MedicalImage::setSliceIndex(
            *imageSeries,
            data::helper::MedicalImage::orientation_t::AXIAL,
            m_axialIndex->value()
        );
        data::helper::MedicalImage::setSliceIndex(
            *imageSeries,
            data::helper::MedicalImage::orientation_t::FRONTAL,
            m_frontalIndex->value()
        );
        data::helper::MedicalImage::setSliceIndex(
            *imageSeries,
            data::helper::MedicalImage::orientation_t::SAGITTAL,
            m_sagittalIndex->value()
        );

        this->setOutput(s_IMAGE, imageSeries);
    }
}

//------------------------------------------------------------------------------

void SSliceIndexDicomEditor::displayErrorMessage(const std::string& message)
{
    SIGHT_WARN("Error: " + message);
    sight::ui::dialog::message messageBox;
    messageBox.setTitle("Error");
    messageBox.setMessage(message);
    messageBox.setIcon(sight::ui::dialog::message::CRITICAL);
    messageBox.addButton(sight::ui::dialog::message::OK);
    messageBox.show();
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::dicom
