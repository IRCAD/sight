/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include <core/com/Slots.hpp>
#include <core/com/Slots.hxx>
#include <core/thread/Timer.hpp>
#include <core/tools/System.hpp>

#include <data/Array.hpp>
#include <data/Composite.hpp>
#include <data/helper/Composite.hpp>
#include <data/helper/MedicalImage.hpp>
#include <data/helper/SeriesDB.hpp>
#include <data/ImageSeries.hpp>
#include <data/Integer.hpp>
#include <data/SeriesDB.hpp>

#include <service/base.hpp>

#include <ui/base/dialog/MessageDialog.hpp>
#include <ui/qt/container/QtContainer.hpp>

#include <QApplication>
#include <QComboBox>
#include <QHBoxLayout>
#include <QMouseEvent>

#include <filesystem>
#include <fstream>
#include <iterator>

namespace sight::module::ui::dicom
{

const core::com::Slots::SlotKeyType SSliceIndexDicomEditor::s_READ_IMAGE_SLOT      = "readImage";
const core::com::Slots::SlotKeyType SSliceIndexDicomEditor::s_DISPLAY_MESSAGE_SLOT = "displayErrorMessage";

//------------------------------------------------------------------------------

SSliceIndexDicomEditor::SSliceIndexDicomEditor() noexcept :
    m_delay(500)
{
    m_slotReadImage = newSlot(s_READ_IMAGE_SLOT, &SSliceIndexDicomEditor::readImage, this);
    newSlot(s_DISPLAY_MESSAGE_SLOT, &SSliceIndexDicomEditor::displayErrorMessage, this);
}

//------------------------------------------------------------------------------

SSliceIndexDicomEditor::~SSliceIndexDicomEditor() noexcept
{
}

//------------------------------------------------------------------------------

void SSliceIndexDicomEditor::configuring()
{
    sight::ui::base::IGuiContainer::initialize();

    core::runtime::ConfigurationElement::sptr config = m_configuration->findConfigurationElement("config");
    SIGHT_ASSERT(
        "The service module::ui::dicom::SSliceIndexDicomEditor must have "
        "a \"config\" element.",
        config
    );

    bool success;

    // Reader
    std::tie(success, m_dicomReaderType) = config->getSafeAttributeValue("dicomReader");
    SIGHT_ASSERT(
        "It should be a \"dicomReader\" tag in the module::ui::dicom::SSliceIndexDicomEditor "
        "config element.",
        success
    );

    // Reader configuration
    core::runtime::ConfigurationElement::sptr readerConfig = config->findConfigurationElement("readerConfig");
    m_readerConfig =
        (readerConfig && readerConfig->size() == 1) ? readerConfig->getElements()[0] : nullptr;

    // Delay
    std::string delayStr;
    std::tie(success, delayStr) = config->getSafeAttributeValue("delay");
    if(success)
    {
        m_delay = boost::lexical_cast<std::size_t>(delayStr);
    }
}

//------------------------------------------------------------------------------

void SSliceIndexDicomEditor::starting()
{
    m_delayTimer2 = m_associatedWorker->createTimer();

    sight::ui::base::IGuiContainer::create();
    auto qtContainer = sight::ui::qt::container::QtContainer::dynamicCast(getContainer());

    QHBoxLayout* layout = new QHBoxLayout();

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

    // Create temporary SeriesDB
    m_tempSeriesDB = data::SeriesDB::New();

    // Create reader
    auto dicomReader = service::add<sight::io::base::service::IReader>(m_dicomReaderType);
    SIGHT_ASSERT(
        "Unable to create a reader of type: \"" + m_dicomReaderType + "\" in "
                                                                      "sight::module::ui::dicom::SSliceIndexDicomEditor.",
        dicomReader
    );
    dicomReader->setInOut(m_tempSeriesDB, sight::io::base::service::s_DATA_KEY);

    if(m_readerConfig)
    {
        dicomReader->setConfiguration(m_readerConfig);
    }

    dicomReader->configure();
    dicomReader->start();

    m_dicomReader = dicomReader;

    // Image Indecies
    m_axialIndex    = data::Integer::New(0);
    m_frontalIndex  = data::Integer::New(0);
    m_sagittalIndex = data::Integer::New(0);

    // Load a slice
    std::chrono::milliseconds duration = std::chrono::milliseconds(m_delay);
    m_delayTimer2->setFunction(
        [ = ]()
        {
            this->triggerNewSlice();
        });
    m_delayTimer2->setDuration(duration);
    m_delayTimer2->setOneShot(true);

    this->triggerNewSlice();
}

//------------------------------------------------------------------------------

void SSliceIndexDicomEditor::stopping()
{
    // Stop dicom reader
    if(!m_dicomReader.expired())
    {
        m_dicomReader.lock()->stop();
        service::remove(m_dicomReader.lock());
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

void SSliceIndexDicomEditor::changeSliceIndex(int)
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
    size_t selectedSliceIndex = static_cast<size_t>(m_sliceIndexSlider->value())
                                + dicomSeries->getFirstInstanceNumber();

    SIGHT_ERROR_IF(
        "There is no instance available for selected slice index.",
        !dicomSeries->isInstanceAvailable(selectedSliceIndex)
    );

    if(dicomSeries->isInstanceAvailable(selectedSliceIndex))
    {
        m_slotReadImage->asyncRun(selectedSliceIndex);
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

    // Clear temporary seriesDB
    data::helper::SeriesDB sDBTempohelper(*m_tempSeriesDB);
    sDBTempohelper.clear();

    // Creates unique temporary folder, no need to check if exists before (see core::tools::System::getTemporaryFolder)
    std::filesystem::path path    = core::tools::System::getTemporaryFolder("dicom");
    std::filesystem::path tmpPath = path / "tmp";

    SIGHT_INFO("Create " + tmpPath.string());
    std::filesystem::create_directories(tmpPath);

    const auto& binaries = dicomSeries->getDicomContainer();
    auto iter            = binaries.find(selectedSliceIndex);
    SIGHT_ASSERT("Index '" << selectedSliceIndex << "' is not found in DicomSeries", iter != binaries.end());

    const core::memory::BufferObject::sptr bufferObj = iter->second;
    const core::memory::BufferObject::Lock lockerDest(bufferObj);
    const char* buffer = static_cast<char*>(lockerDest.getBuffer());
    const size_t size  = bufferObj->getSize();

    const std::filesystem::path dest = tmpPath / std::to_string(selectedSliceIndex);
    std::ofstream fs(dest, std::ios::binary | std::ios::trunc);
    SIGHT_THROW_IF("Can't open '" << tmpPath << "' for write.", !fs.good());

    fs.write(buffer, static_cast<long>(size));
    fs.close();

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
    data::ImageSeries::sptr imageSeries;

    if(m_tempSeriesDB->getContainer().size() > 0)
    {
        auto series = *(m_tempSeriesDB->getContainer().begin());
        if(isModalitySupported(*series))
        {
            imageSeries = data::ImageSeries::dynamicCast(series);
        }
    }

    if(imageSeries)
    {
        data::Image::sptr newImage      = imageSeries->getImage();
        const data::Image::Size newSize = newImage->getSize();

        m_frontalIndex->setValue(static_cast<int>(newSize[0] / 2));
        m_sagittalIndex->setValue(static_cast<int>(newSize[1] / 2));

        data::helper::MedicalImage::setSliceIndex(
            *newImage,
            data::helper::MedicalImage::orientation_t::AXIAL,
            m_axialIndex->value()
        );
        data::helper::MedicalImage::setSliceIndex(
            *newImage,
            data::helper::MedicalImage::orientation_t::FRONTAL,
            m_frontalIndex->value()
        );
        data::helper::MedicalImage::setSliceIndex(
            *newImage,
            data::helper::MedicalImage::orientation_t::SAGITTAL,
            m_sagittalIndex->value()
        );

        this->setOutput(s_IMAGE, newImage);
    }

    std::error_code ec;
    std::filesystem::remove_all(path, ec);
    SIGHT_ERROR_IF("remove_all error for path " + path.string() + ": " + ec.message(), ec.value());
}

//------------------------------------------------------------------------------

void SSliceIndexDicomEditor::displayErrorMessage(const std::string& message) const
{
    SIGHT_WARN("Error: " + message);
    sight::ui::base::dialog::MessageDialog messageBox;
    messageBox.setTitle("Error");
    messageBox.setMessage(message);
    messageBox.setIcon(sight::ui::base::dialog::IMessageDialog::CRITICAL);
    messageBox.addButton(sight::ui::base::dialog::IMessageDialog::OK);
    messageBox.show();
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::dicom
