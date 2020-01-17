/************************************************************************
 *
 * Copyright (C) 2018-2019 IRCAD France
 * Copyright (C) 2018-2019 IHU Strasbourg
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

#include "ioDicomWeb/SSliceIndexDicomPullerEditor.hpp"

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

#include <fwNetworkIO/exceptions/Base.hpp>

#include <fwPreferences/helper.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/registry/ActiveWorkers.hpp>
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

namespace ioDicomWeb
{

//------------------------------------------------------------------------------

SSliceIndexDicomPullerEditor::SSliceIndexDicomPullerEditor() noexcept :
    m_delay(500)
{
}

//------------------------------------------------------------------------------

SSliceIndexDicomPullerEditor::~SSliceIndexDicomPullerEditor() noexcept
{
}

//------------------------------------------------------------------------------

void SSliceIndexDicomPullerEditor::configuring()
{
    ::fwGui::IGuiContainerSrv::initialize();

    ::fwRuntime::ConfigurationElement::sptr config = m_configuration->findConfigurationElement("config");
    SLM_ASSERT("The service ::ioDicomWeb::SPacsConfigurationInitializer must have "
               "a \"config\" element.", config);

    bool success;

    // Reader
    std::tie(success, m_dicomReaderType) = config->getSafeAttributeValue("dicomReader");
    SLM_ASSERT("It should be a \"dicomReader\" tag in the ::ioDicomWeb::SSliceIndexDicomPullerEditor "
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
        m_delay = ::boost::lexical_cast< unsigned int >(delayStr);
    }

    if(m_delayTimer && m_delayTimer->isRunning())
    {
        m_delayTimer->stop();
        m_delayTimer.reset();
    }

    m_delayTimer = m_associatedWorker->createTimer();
    m_delayTimer->setFunction(  [ = ]()
        {
            this->triggerNewSlice();
        }  );

    m_delayTimer->setOneShot(true);
}

//------------------------------------------------------------------------------

void SSliceIndexDicomPullerEditor::starting()
{
    ::fwGui::IGuiContainerSrv::create();
    ::fwGuiQt::container::QtContainer::sptr qtContainer = fwGuiQt::container::QtContainer::dynamicCast(getContainer());

    QHBoxLayout* layout = new QHBoxLayout();

    ::fwMedData::DicomSeries::csptr dicomSeries = this->getInOut< ::fwMedData::DicomSeries >("series");
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
               "::ioDicomWeb::SSliceIndexDicomPullerEditor.", dicomReader);
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
    if(m_delayTimer)
    {
        if(m_delayTimer->isRunning())
        {
            m_delayTimer->stop();
        }

        m_delayTimer->setDuration(std::chrono::milliseconds(m_delay));
        m_delayTimer->start();
    }
    else
    {
        this->triggerNewSlice();
    }
}

//------------------------------------------------------------------------------

void SSliceIndexDicomPullerEditor::stopping()
{
    if(m_delayTimer && m_delayTimer->isRunning())
    {
        m_delayTimer->stop();
    }

    // Stop dicom reader
    if(!m_dicomReader.expired())
    {
        m_dicomReader.lock()->stop();
        ::fwServices::OSR::unregisterService(m_dicomReader.lock());
    }

    this->destroy();
}

//------------------------------------------------------------------------------

void SSliceIndexDicomPullerEditor::updating()
{
}

//------------------------------------------------------------------------------

void SSliceIndexDicomPullerEditor::changeSliceIndex(int)
{
    // Update text
    std::stringstream ss;
    ss << m_sliceIndexSlider->value() << " / " << (m_numberOfSlices-1);
    m_sliceIndexLineEdit->setText(std::string(ss.str()).c_str());

    // Get the new slice if there is no change for m_delay milliseconds
    if( m_delayTimer )
    {
        if(m_delayTimer->isRunning())
        {
            m_delayTimer->stop();
        }

        m_delayTimer->start();
    }
    else
    {
        this->triggerNewSlice();
    }
}

//------------------------------------------------------------------------------

void SSliceIndexDicomPullerEditor::triggerNewSlice()
{
    // DicomSeries
    ::fwMedData::DicomSeries::csptr dicomSeries = this->getInOut< ::fwMedData::DicomSeries >("series");
    SLM_ASSERT("DicomSeries should not be null !", dicomSeries);

    // Compute slice index
    const size_t selectedSliceIndex = static_cast<size_t>(m_sliceIndexSlider->value()) +
                                      dicomSeries->getFirstInstanceNumber();
    OSLM_TRACE("triggered new slice : " << selectedSliceIndex);
    if(!dicomSeries->isInstanceAvailable(selectedSliceIndex))
    {
        this->pullInstance();
    }
    else
    {
        this->readImage(selectedSliceIndex);
    }
}

//------------------------------------------------------------------------------

void SSliceIndexDicomPullerEditor::readImage(size_t selectedSliceIndex)
{
    // DicomSeries
    ::fwMedData::DicomSeries::csptr dicomSeries = this->getInOut< ::fwMedData::DicomSeries >("series");
    SLM_ASSERT("DicomSeries should not be null !", dicomSeries);
    if( dicomSeries->getModality() != "CT" && dicomSeries->getModality() != "MR" && dicomSeries->getModality() != "XA")
    {
        return;
    }

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

    std::filesystem::path dest = tmpPath / std::to_string(selectedSliceIndex);
    std::ofstream fs(dest, std::ios::binary|std::ios::trunc);
    FW_RAISE_IF("Can't open '" << tmpPath << "' for write.", !fs.good());

    fs.write(buffer, size);
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

    std::error_code ec;
    std::filesystem::remove_all(path, ec);
    SLM_ERROR_IF("remove_all error for path " + path.string() + ": " + ec.message(), ec.value());
}

//------------------------------------------------------------------------------

void SSliceIndexDicomPullerEditor::pullInstance()
{
    ::fwServices::IService::ConfigType configuration = this->getConfigTree();
    //Parse server port and hostname
    if(configuration.count("server"))
    {
        const std::string serverInfo               = configuration.get("server", "");
        const std::string::size_type splitPosition = serverInfo.find(':');
        SLM_ASSERT("Server info not formatted correctly", splitPosition != std::string::npos);

        m_serverHostnameKey = serverInfo.substr(0, splitPosition);
        m_serverPortKey     = serverInfo.substr(splitPosition + 1, serverInfo.size());
    }
    else
    {
        throw ::fwTools::Failed("'server' element not found");
    }

    const std::string hostname = ::fwPreferences::getValue(m_serverHostnameKey);
    if(!hostname.empty())
    {
        m_serverHostname = hostname;
    }

    const std::string port = ::fwPreferences::getValue(m_serverPortKey);
    if(!port.empty())
    {
        m_serverPort = std::stoi(port);
    }

    // Catch any errors
    try
    {
        // DicomSeries
        ::fwMedData::DicomSeries::sptr dicomSeries = this->getInOut< ::fwMedData::DicomSeries >("series");
        SLM_ASSERT("DicomSeries should not be null !", dicomSeries);

        // Get selected slice
        size_t selectedSliceIndex = static_cast<size_t>(m_sliceIndexSlider->value()) +
                                    dicomSeries->getFirstInstanceNumber();

        std::string seriesInstanceUID = dicomSeries->getInstanceUID();

        // Find Series according to SeriesInstanceUID
        QJsonObject query;
        query.insert("SeriesInstanceUID", seriesInstanceUID.c_str());

        QJsonObject body;
        body.insert("Level", "Series");
        body.insert("Query", query);
        body.insert("Limit", 0);

        /// Url PACS
        const std::string pacsServer("http://" + m_serverHostname + ":" + std::to_string(m_serverPort));

        /// Orthanc "/tools/find" route. POST a JSON to get all Series corresponding to the SeriesInstanceUID.
        ::fwNetworkIO::http::Request::sptr request = ::fwNetworkIO::http::Request::New(
            pacsServer + "/tools/find");
        QByteArray seriesAnswer;
        try
        {
            seriesAnswer = m_clientQt.post(request, QJsonDocument(body).toJson());
        }
        catch  (::fwNetworkIO::exceptions::HostNotFound& exception)
        {
            std::stringstream ss;
            ss << "Host not found:\n"
               << " Please check your configuration: \n"
               << "Pacs host name: " << m_serverHostname << "\n"
               << "Pacs port: " << m_serverPort << "\n";

            this->displayErrorMessage(ss.str());
            SLM_WARN(exception.what());
        }
        QJsonDocument jsonResponse    = QJsonDocument::fromJson(seriesAnswer);
        const QJsonArray& seriesArray = jsonResponse.array();

        // Should be one Series, so take the first of the array.
        const std::string& seriesUID = seriesArray.at(0).toString().toStdString();
        // GET all Instances by Series.
        const std::string& instancesUrl(pacsServer + "/series/" + seriesUID);

        const QByteArray& instancesAnswer =
            m_clientQt.get( ::fwNetworkIO::http::Request::New(instancesUrl));
        jsonResponse = QJsonDocument::fromJson(instancesAnswer);
        const QJsonObject& jsonObj       = jsonResponse.object();
        const QJsonArray& instancesArray = jsonObj["Instances"].toArray();
        const std::string& instanceUID   =
            instancesArray.at(static_cast<int>(selectedSliceIndex)).toString().toStdString();

        // GET frame by Slice.
        std::string instancePath;
        const std::string& instanceUrl(pacsServer + "/instances/" + instanceUID + "/file");
        try
        {
            instancePath = m_clientQt.getFile( ::fwNetworkIO::http::Request::New(instanceUrl));
        }
        catch  (::fwNetworkIO::exceptions::ContentNotFound& exception)
        {
            std::stringstream ss;
            ss << "Content not found:  \n"
               << "Unable download the DICOM instance. \n";

            this->displayErrorMessage(ss.str());
            SLM_WARN(exception.what());
        }

        // Add path and trigger reading
        dicomSeries->addDicomPath(selectedSliceIndex, instancePath);
        this->readImage(selectedSliceIndex);
    }
    catch (::fwNetworkIO::exceptions::Base& exception)
    {
        std::stringstream ss;
        ss << "Unknown error.";
        this->displayErrorMessage(ss.str());
        SLM_WARN(exception.what());
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

} // namespace ioDicomWeb
