/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "ioDicom/SSliceIndexDicomEditor.hpp"

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

#include <fwServices/macros.hpp>
#include <fwServices/registry/ObjectService.hpp>

#include <fwThread/Timer.hpp>

#include <fwTools/System.hpp>

#include <filesystem>
#include <QApplication>
#include <QComboBox>
#include <QHBoxLayout>
#include <QMouseEvent>

#include <fstream>
#include <iterator>

namespace ioDicom
{

fwServicesRegisterMacro( ::fwGui::editor::IEditor, ::ioDicom::SSliceIndexDicomEditor, ::fwMedData::DicomSeries );

const ::fwCom::Slots::SlotKeyType SSliceIndexDicomEditor::s_READ_IMAGE_SLOT      = "readImage";
const ::fwCom::Slots::SlotKeyType SSliceIndexDicomEditor::s_DISPLAY_MESSAGE_SLOT = "displayErrorMessage";

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
    SLM_TRACE_FUNC();
    ::fwGui::IGuiContainerSrv::initialize();

    ::fwRuntime::ConfigurationElement::sptr config = m_configuration->findConfigurationElement("config");
    SLM_ASSERT("The service ::ioDicom::SSliceIndexDicomEditor must have "
               "a \"config\" element.", config);

    bool success;

    // Reader
    std::tie(success, m_dicomReaderType) = config->getSafeAttributeValue("dicomReader");
    SLM_ASSERT("It should be a \"dicomReader\" tag in the ::ioDicom::SSliceIndexDicomEditor "
               "config element.", success);

    // Reader configuration
    ::fwRuntime::ConfigurationElement::sptr readerConfig = config->findConfigurationElement("dicomReaderConfig");
    m_readerConfig                                       =
        (readerConfig && readerConfig->size() == 1) ? readerConfig->getElements()[0] : nullptr;

    // Delay
    std::string delayStr;
    std::tie(success, delayStr) = config->getSafeAttributeValue("delay");
    if(success)
    {
        m_delay = ::boost::lexical_cast< std::size_t >(delayStr);
    }
}

//------------------------------------------------------------------------------

void SSliceIndexDicomEditor::starting()
{
    m_delayTimer2 = m_associatedWorker->createTimer();

    ::fwGui::IGuiContainerSrv::create();
    ::fwGuiQt::container::QtContainer::sptr qtContainer = fwGuiQt::container::QtContainer::dynamicCast(getContainer());

    QHBoxLayout* layout = new QHBoxLayout();

    ::fwMedData::DicomSeries::csptr dicomSeries = this->getInput< ::fwMedData::DicomSeries >("series");
    SLM_ASSERT("DicomSeries should not be null !", dicomSeries);
    m_numberOfSlices = dicomSeries->getNumberOfInstances();

    // Slider
    m_sliceIndexSlider = new QSlider(Qt::Horizontal);
    layout->addWidget(m_sliceIndexSlider, 1);
    m_sliceIndexSlider->setRange(0, static_cast<int>(m_numberOfSlices-1));
    m_sliceIndexSlider->setValue(static_cast<int>(m_numberOfSlices/2));

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
               "::ioDicom::SSliceIndexDicomEditor.", dicomReader);
    ::fwServices::OSR::registerService(m_tempSeriesDB, ::fwIO::s_DATA_KEY,
                                       ::fwServices::IService::AccessType::INOUT, dicomReader);

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

void SSliceIndexDicomEditor::stopping()
{
    SLM_TRACE_FUNC();

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

void SSliceIndexDicomEditor::updating()
{
}

//------------------------------------------------------------------------------

void SSliceIndexDicomEditor::info(std::ostream& _sstream )
{
    _sstream << "SSliceIndexDicomEditor::info";
}

//------------------------------------------------------------------------------

void SSliceIndexDicomEditor::changeSliceIndex(int value)
{
    // Update text
    std::stringstream ss;
    ss << m_sliceIndexSlider->value() << " / " << (m_numberOfSlices-1);
    m_sliceIndexLineEdit->setText(std::string(ss.str()).c_str());

    // Get the new slice if there is no change for m_delay milliseconds
    m_delayTimer2->start();

}

//------------------------------------------------------------------------------

void SSliceIndexDicomEditor::triggerNewSlice()
{
    // DicomSeries
    ::fwMedData::DicomSeries::csptr dicomSeries = this->getInput< ::fwMedData::DicomSeries >("series");
    SLM_ASSERT("DicomSeries should not be null !", dicomSeries);

    // Compute slice index
    size_t selectedSliceIndex = static_cast<size_t>(m_sliceIndexSlider->value()) +
                                dicomSeries->getFirstInstanceNumber();
    OSLM_TRACE("triggered new slice : " << selectedSliceIndex);

    SLM_ERROR_IF("There is no instance available for selected slice index.",
                 !dicomSeries->isInstanceAvailable(selectedSliceIndex));

    if(dicomSeries->isInstanceAvailable(selectedSliceIndex))
    {
        m_slotReadImage->asyncRun(selectedSliceIndex);
    }
}

//------------------------------------------------------------------------------

void SSliceIndexDicomEditor::readImage(std::size_t selectedSliceIndex)
{
    // DicomSeries
    ::fwMedData::DicomSeries::csptr dicomSeries = this->getInput< ::fwMedData::DicomSeries >("series");
    SLM_ASSERT("DicomSeries should not be null !", dicomSeries);

    auto isModalitySupported = [](const ::fwMedData::Series::csptr& series )
                               {
                                   return series->getModality() == "CT" ||
                                          series->getModality() == "MR" ||
                                          series->getModality() == "XA";
                               };

    if( !isModalitySupported(dicomSeries) )
    {
        return;
    }

    // Clear temporary seriesDB
    ::fwMedDataTools::helper::SeriesDB sDBTempohelper(m_tempSeriesDB);
    sDBTempohelper.clear();

    // Creates unique temporary folder, no need to check if exists before (see ::fwTools::System::getTemporaryFolder)
    std::filesystem::path path    = ::fwTools::System::getTemporaryFolder("dicom");
    std::filesystem::path tmpPath = path / "tmp";

    SLM_INFO("Create " + tmpPath.string());
    std::filesystem::create_directories(tmpPath);

    const auto& binaries = dicomSeries->getDicomContainer();
    auto iter            = binaries.find(selectedSliceIndex);
    OSLM_ASSERT("Index '"<<selectedSliceIndex<<"' is not found in DicomSeries", iter != binaries.end());

    const ::fwMemory::BufferObject::sptr bufferObj = iter->second;
    const ::fwMemory::BufferObject::Lock lockerDest(bufferObj);
    const char* buffer = static_cast<char*>(lockerDest.getBuffer());
    const size_t size  = bufferObj->getSize();

    const std::filesystem::path dest = tmpPath / std::to_string(selectedSliceIndex);
    std::ofstream fs(dest, std::ios::binary|std::ios::trunc);
    FW_RAISE_IF("Can't open '" << tmpPath << "' for write.", !fs.good());

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
    ::fwMedData::ImageSeries::sptr imageSeries;

    if(m_tempSeriesDB->getContainer().size() > 0)
    {
        auto series = *(m_tempSeriesDB->getContainer().begin());
        if( isModalitySupported(series) )
        {
            imageSeries = ::fwMedData::ImageSeries::dynamicCast(series);
        }
    }

    if(imageSeries)
    {
        ::fwData::Image::sptr newImage    = imageSeries->getImage();
        ::fwData::Image::SizeType newSize = newImage->getSize();

        newImage->setField(::fwDataTools::fieldHelper::Image::m_axialSliceIndexId, m_axialIndex);
        m_frontalIndex->setValue(static_cast< int >(newSize[0]/2));
        newImage->setField(::fwDataTools::fieldHelper::Image::m_frontalSliceIndexId, m_frontalIndex);
        m_sagittalIndex->setValue(static_cast< int >(newSize[1]/2));
        newImage->setField(::fwDataTools::fieldHelper::Image::m_sagittalSliceIndexId, m_sagittalIndex);

        this->setOutput("image", newImage);
    }

    std::error_code ec;
    std::filesystem::remove_all(path, ec);
    SLM_ERROR_IF("remove_all error for path " + path.string() + ": " + ec.message(), ec.value());
}

//------------------------------------------------------------------------------

void SSliceIndexDicomEditor::displayErrorMessage(const std::string& message) const
{
    SLM_WARN("Error: " + message);
    ::fwGui::dialog::MessageDialog messageBox;
    messageBox.setTitle("Error");
    messageBox.setMessage( message );
    messageBox.setIcon(::fwGui::dialog::IMessageDialog::CRITICAL);
    messageBox.addButton(::fwGui::dialog::IMessageDialog::OK);
    messageBox.show();
}

//------------------------------------------------------------------------------

} // namespace ioDicom
