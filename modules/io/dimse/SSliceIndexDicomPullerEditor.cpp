/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "SSliceIndexDicomPullerEditor.hpp"

#include <core/com/Slots.hpp>
#include <core/com/Slots.hxx>
#include <core/thread/Timer.hpp>
#include <core/tools/System.hpp>

#include <data/Array.hpp>
#include <data/Composite.hpp>
#include <data/DicomSeries.hpp>
#include <data/Image.hpp>
#include <data/ImageSeries.hpp>
#include <data/Integer.hpp>
#include <data/SeriesDB.hpp>
#include <data/tools/fieldHelper/Image.hpp>
#include <data/tools/helper/Composite.hpp>
#include <data/tools/helper/SeriesDB.hpp>

#include <services/macros.hpp>
#include <services/registry/ObjectService.hpp>

#include <io/dimse/exceptions/Base.hpp>

#include <QApplication>
#include <QComboBox>
#include <QHBoxLayout>
#include <QMouseEvent>

#include <ui/base/dialog/MessageDialog.hpp>
#include <ui/qt/container/QtContainer.hpp>

#include <filesystem>
#include <iterator>

namespace sight::modules::io::dimse
{

fwServicesRegisterMacro( ::sight::ui::base::editor::IEditor, ::sight::modules::io::dimse::SSliceIndexDicomPullerEditor,
                         data::DicomSeries )

static const std::string s_DICOM_READER_CONFIG = "dicomReader";
static const std::string s_READER_CONFIG_CONFIG = "dicomReaderConfig";
static const std::string s_DELAY_CONFIG         = "delay";

static const services::IService::KeyType s_PACS_INPUT = "pacsConfig";

static const services::IService::KeyType s_DICOMSERIES_INOUT = "series";

static const services::IService::KeyType s_IMAGE_OUTPUT = "image";

const core::com::Slots::SlotKeyType s_DISPLAY_MESSAGE_SLOT = "displayErrorMessage";

//------------------------------------------------------------------------------

SSliceIndexDicomPullerEditor::SSliceIndexDicomPullerEditor() noexcept
{
    FW_DEPRECATED_MSG(
        "::modules::io::dimse::SSliceIndexDicomPullerEditor will be removed in sight 21.0, use modules::io::dimse::SSliceIndexDicomEditor instead",
        "21.0");

    m_slotDisplayMessage = core::com::newSlot(&SSliceIndexDicomPullerEditor::displayErrorMessage, this);
    core::com::HasSlots::m_slots(s_DISPLAY_MESSAGE_SLOT, m_slotDisplayMessage);

    core::com::HasSlots::m_slots.setWorker(m_associatedWorker);
}
//------------------------------------------------------------------------------

SSliceIndexDicomPullerEditor::~SSliceIndexDicomPullerEditor() noexcept
{
}

//------------------------------------------------------------------------------

void SSliceIndexDicomPullerEditor::configuring()
{
    sight::ui::base::IGuiContainerSrv::initialize();

    core::runtime::ConfigurationElement::sptr config = m_configuration->findConfigurationElement("config");
    SLM_ASSERT("The service modules::io::dimse::SPacsConfigurationInitializer must have "
               "a \"config\" element.", config);

    bool success;

    // Reader
    std::tie(success, m_dicomReaderType) = config->getSafeAttributeValue("dicomReader");
    SLM_ASSERT("It should be a \"dicomReader\" tag in the modules::io::dimse::SSliceIndexDicomPullerEditor "
               "config element.", success);

    // Reader configuration
    core::runtime::ConfigurationElement::sptr readerConfig = config->findConfigurationElement("dicomReaderConfig");
    m_readerConfig =
        (readerConfig && readerConfig->size() == 1) ? readerConfig->getElements()[0] : nullptr;

    // Delay
    std::string delayStr;
    std::tie(success, delayStr) = config->getSafeAttributeValue("delay");
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
    m_pacsConfiguration = this->getInput< sight::io::dimse::data::PacsConfiguration>(s_PACS_INPUT);

    sight::ui::base::IGuiContainerSrv::create();
    auto qtContainer = sight::ui::qt::container::QtContainer::dynamicCast(getContainer());

    QHBoxLayout* layout = new QHBoxLayout();

    data::DicomSeries::csptr dicomSeries = this->getInOut< data::DicomSeries >(s_DICOMSERIES_INOUT);
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
    m_tempSeriesDB = data::SeriesDB::New();

    // Create reader
    services::registry::ServiceFactory::sptr srvFactory = services::registry::ServiceFactory::getDefault();

    sight::io::base::services::IReader::sptr dicomReader;
    dicomReader = sight::io::base::services::IReader::dynamicCast(srvFactory->create(m_dicomReaderType));
    SLM_ASSERT("Unable to create a reader of type: \"" + m_dicomReaderType + "\" in "
               "::modules::io::dimse::SSliceIndexDicomPullerEditor.", dicomReader);
    services::OSR::registerService(m_tempSeriesDB, sight::io::base::services::s_DATA_KEY,
                                   services::IService::AccessType::INOUT, dicomReader);
    if(m_readerConfig)
    {
        dicomReader->setConfiguration(m_readerConfig);
    }

    dicomReader->configure();
    dicomReader->start();

    m_dicomReader = dicomReader;

    // Worker
    m_pullSeriesWorker = core::thread::Worker::New();

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

void SSliceIndexDicomPullerEditor::updating()
{
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
        services::OSR::unregisterService(m_dicomReader.lock());
    }

    // Disconnect the signals
    QObject::disconnect(m_sliceIndexSlider, SIGNAL(valueChanged(int)), this, SLOT(changeSliceIndex(int)));

    this->destroy();
}

//------------------------------------------------------------------------------

void SSliceIndexDicomPullerEditor::changeSliceIndex(int)
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
    data::DicomSeries::csptr dicomSeries = this->getInOut< data::DicomSeries >(s_DICOMSERIES_INOUT);
    SLM_ASSERT("DicomSeries should not be null !", dicomSeries);

    // Compute slice index
    std::size_t selectedSliceIndex = m_sliceIndexSlider->value() + dicomSeries->getFirstInstanceNumber();
    if(!dicomSeries->isInstanceAvailable(selectedSliceIndex))
    {
        if(m_pacsConfiguration)
        {
            m_pullSeriesWorker->post(std::bind(&modules::io::dimse::SSliceIndexDicomPullerEditor::pullInstance, this));
        }
        else
        {
            SLM_ERROR("There is no instance available for selected slice index.");
        }
    }
    else
    {
        this->readImage(selectedSliceIndex);
    }
}

//------------------------------------------------------------------------------

void SSliceIndexDicomPullerEditor::readImage(std::size_t _selectedSliceIndex)
{
    // DicomSeries
    data::DicomSeries::csptr dicomSeries = this->getInOut< data::DicomSeries >(s_DICOMSERIES_INOUT);
    SLM_ASSERT("DicomSeries should not be null !", dicomSeries);
    if( dicomSeries->getModality() != "CT" && dicomSeries->getModality() != "MR" && dicomSeries->getModality() != "XA")
    {
        return;
    }

    // Clear temporary seriesDB
    data::tools::helper::SeriesDB sDBTempohelper(m_tempSeriesDB);
    sDBTempohelper.clear();

    // Creates unique temporary folder, no need to check if exists before (see core::tools::System::getTemporaryFolder)
    std::filesystem::path path    = core::tools::System::getTemporaryFolder("dicom");
    std::filesystem::path tmpPath = path / "tmp";

    SLM_INFO("Create " + tmpPath.string());
    std::filesystem::create_directories(tmpPath);

    const auto& binaries = dicomSeries->getDicomContainer();
    auto iter            = binaries.find(_selectedSliceIndex);
    SLM_ASSERT("Index '"<<_selectedSliceIndex<<"' is not found in DicomSeries", iter != binaries.end());

    const core::memory::BufferObject::sptr bufferObj = iter->second;
    const core::memory::BufferObject::Lock lockerDest(bufferObj);
    const char* buffer = static_cast<char*>(lockerDest.getBuffer());
    const size_t size  = bufferObj->getSize();

    std::filesystem::path dest = tmpPath / std::to_string(_selectedSliceIndex);
    std::ofstream fs(dest, std::ios::binary|std::ios::trunc);
    FW_RAISE_IF("Can't open '" << tmpPath << "' for write.", !fs.good());

    fs.write(buffer, size);
    fs.close();

    // Read image
    sight::io::base::services::IReader::sptr dicomReader = m_dicomReader.lock();
    if(dicomReader)
    {
        dicomReader->setFolder(tmpPath);

        dicomReader->update();

        if(dicomReader->isStopped())
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
        imageSeries = data::ImageSeries::dynamicCast(*(m_tempSeriesDB->getContainer().begin()));
    }

    if(imageSeries)
    {
        data::Image::sptr newImage      = imageSeries->getImage();
        const data::Image::Size newSize = newImage->getSize2();

        data::Integer::sptr axialIndex    = data::Integer::New(0);
        data::Integer::sptr frontalIndex  = data::Integer::New(newSize[0]/2);
        data::Integer::sptr sagittalIndex = data::Integer::New(newSize[1]/2);

        newImage->setField(data::tools::fieldHelper::Image::m_axialSliceIndexId, axialIndex);
        newImage->setField(data::tools::fieldHelper::Image::m_frontalSliceIndexId, frontalIndex);
        newImage->setField(data::tools::fieldHelper::Image::m_sagittalSliceIndexId, sagittalIndex);

        this->setOutput(s_IMAGE_OUTPUT, newImage);
    }
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
            data::DicomSeries::sptr dicomSeries =
                this->getInOut< data::DicomSeries >(s_DICOMSERIES_INOUT);
            SLM_ASSERT("DicomSeries should not be null !", dicomSeries);

            // Get selected slice
            std::size_t selectedSliceIndex = m_sliceIndexSlider->value() + dicomSeries->getFirstInstanceNumber();

            auto seriesEnquirer = sight::io::dimse::SeriesEnquirer::New();

            seriesEnquirer->initialize(m_pacsConfiguration->getLocalApplicationTitle(),
                                       m_pacsConfiguration->getPacsHostName(),
                                       m_pacsConfiguration->getPacsApplicationPort(),
                                       m_pacsConfiguration->getPacsApplicationTitle(),
                                       m_pacsConfiguration->getMoveApplicationTitle());

            seriesEnquirer->connect();
            std::string seriesInstanceUID = dicomSeries->getInstanceUID();
            std::string sopInstanceUID    =
                seriesEnquirer->findSOPInstanceUID(seriesInstanceUID, static_cast<unsigned int>(selectedSliceIndex));

            // Check if an instance with the selected Instance Number is found on the PACS
            if(!sopInstanceUID.empty())
            {
                // Pull Selected Series using C-GET Requests
                seriesEnquirer->pullInstanceUsingGetRetrieveMethod(seriesInstanceUID, sopInstanceUID);

                // Add path and trigger reading
                std::filesystem::path path     = core::tools::System::getTemporaryFolder() / "dicom/";
                std::filesystem::path filePath = path.string() + seriesInstanceUID + "/" + sopInstanceUID;
                dicomSeries->addDicomPath(selectedSliceIndex, filePath);
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
            seriesEnquirer->disconnect();

        }
        catch (sight::io::dimse::exceptions::Base& exception)
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

void SSliceIndexDicomPullerEditor::displayErrorMessage(const std::string& _message) const
{
    SLM_WARN("Error: " + _message);
    sight::ui::base::dialog::MessageDialog messageBox;
    messageBox.setTitle("Error");
    messageBox.setMessage(_message);
    messageBox.setIcon(ui::base::dialog::IMessageDialog::CRITICAL);
    messageBox.addButton(ui::base::dialog::IMessageDialog::OK);
    messageBox.show();
}

} // namespace sight::modules::io::dimse
