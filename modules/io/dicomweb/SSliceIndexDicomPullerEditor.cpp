/************************************************************************
 *
 * Copyright (C) 2018-2021 IRCAD France
 * Copyright (C) 2018-2020 IHU Strasbourg
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

#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
#include <core/com/Signals.hpp>
#include <core/com/Slot.hpp>
#include <core/com/Slots.hpp>
#include <core/com/Slots.hxx>
#include <core/thread/ActiveWorkers.hpp>
#include <core/thread/Timer.hpp>
#include <core/tools/System.hpp>

#include <data/Array.hpp>
#include <data/Composite.hpp>
#include <data/DicomSeries.hpp>
#include <data/fieldHelper/Image.hpp>
#include <data/helper/Composite.hpp>
#include <data/helper/SeriesDB.hpp>
#include <data/Image.hpp>
#include <data/ImageSeries.hpp>
#include <data/Integer.hpp>
#include <data/SeriesDB.hpp>

#include <io/base/service/IReader.hpp>
#include <io/http/exceptions/Base.hpp>

#include <service/macros.hpp>
#include <service/registry/ObjectService.hpp>

#include <ui/base/dialog/MessageDialog.hpp>
#include <ui/base/preferences/helper.hpp>
#include <ui/qt/container/QtContainer.hpp>

#include <QApplication>
#include <QComboBox>
#include <QHBoxLayout>
#include <QMouseEvent>

#include <filesystem>
#include <fstream>
#include <iterator>

namespace sight::module::io::dicomweb
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
    sight::ui::base::IGuiContainer::initialize();

    core::runtime::ConfigurationElement::sptr config = m_configuration->findConfigurationElement("config");
    SIGHT_ASSERT("The service module::io::dicomweb::SPacsConfigurationInitializer must have "
                 "a \"config\" element.", config);

    bool success;

    // Reader
    std::tie(success, m_dicomReaderType) = config->getSafeAttributeValue("dicomReader");
    SIGHT_ASSERT("It should be a \"dicomReader\" tag in the module::io::dicomweb::SSliceIndexDicomPullerEditor "
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
    sight::ui::base::IGuiContainer::create();
    auto qtContainer = sight::ui::qt::container::QtContainer::dynamicCast(getContainer());

    QHBoxLayout* layout = new QHBoxLayout();

    data::DicomSeries::csptr dicomSeries = this->getInOut< data::DicomSeries >("series");
    SIGHT_ASSERT("DicomSeries should not be null !", dicomSeries);
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
    m_tempSeriesDB = data::SeriesDB::New();

    // Create reader
    service::extension::Factory::sptr srvFactory = service::extension::Factory::getDefault();

    sight::io::base::service::IReader::sptr dicomReader;
    dicomReader = sight::io::base::service::IReader::dynamicCast(srvFactory->create(m_dicomReaderType));
    SIGHT_ASSERT("Unable to create a reader of type: \"" + m_dicomReaderType + "\" in "
                 "::sight::module::io::dicomweb::SSliceIndexDicomPullerEditor.", dicomReader);
    service::OSR::registerService(m_tempSeriesDB, sight::io::base::service::s_DATA_KEY,
                                  service::IService::AccessType::INOUT, dicomReader);
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
        service::OSR::unregisterService(m_dicomReader.lock());
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
    data::DicomSeries::csptr dicomSeries = this->getInOut< data::DicomSeries >("series");
    SIGHT_ASSERT("DicomSeries should not be null !", dicomSeries);

    // Compute slice index
    const size_t selectedSliceIndex = static_cast<size_t>(m_sliceIndexSlider->value()) +
                                      dicomSeries->getFirstInstanceNumber();
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
    data::DicomSeries::csptr dicomSeries = this->getInOut< data::DicomSeries >("series");
    SIGHT_ASSERT("DicomSeries should not be null !", dicomSeries);
    if( dicomSeries->getModality() != "CT" && dicomSeries->getModality() != "MR" && dicomSeries->getModality() != "XA")
    {
        return;
    }

    // Creates unique temporary folder, no need to check if exists before (see core::tools::System::getTemporaryFolder)
    std::filesystem::path path    = core::tools::System::getTemporaryFolder("dicom");
    std::filesystem::path tmpPath = path / "tmp";

    SIGHT_INFO("Create " + tmpPath.string());
    std::filesystem::create_directories(tmpPath);

    const auto& binaries = dicomSeries->getDicomContainer();
    auto iter            = binaries.find(selectedSliceIndex);
    SIGHT_ASSERT("Index '"<<selectedSliceIndex<<"' is not found in DicomSeries", iter != binaries.end());

    const core::memory::BufferObject::sptr bufferObj = iter->second;
    const core::memory::BufferObject::Lock lockerDest(bufferObj);
    const char* buffer = static_cast<char*>(lockerDest.getBuffer());
    const size_t size  = bufferObj->getSize();

    std::filesystem::path dest = tmpPath / std::to_string(selectedSliceIndex);
    std::ofstream fs(dest, std::ios::binary|std::ios::trunc);
    SIGHT_THROW_IF("Can't open '" << tmpPath << "' for write.", !fs.good());

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
    data::ImageSeries::sptr imageSeries;

    if(m_tempSeriesDB->getContainer().size() > 0)
    {
        imageSeries = data::ImageSeries::dynamicCast(*(m_tempSeriesDB->getContainer().begin()));
    }

    if(imageSeries)
    {
        data::Image::sptr newImage      = imageSeries->getImage();
        const data::Image::Size newSize = newImage->getSize2();

        newImage->setField(data::fieldHelper::Image::m_axialSliceIndexId, m_axialIndex);
        m_frontalIndex->setValue(static_cast<int>(newSize[0]/2));
        newImage->setField(data::fieldHelper::Image::m_frontalSliceIndexId, m_frontalIndex);
        m_sagittalIndex->setValue(static_cast<int>(newSize[1]/2));
        newImage->setField(data::fieldHelper::Image::m_sagittalSliceIndexId, m_sagittalIndex);

        this->setOutput("image", newImage);
    }

    std::error_code ec;
    std::filesystem::remove_all(path, ec);
    SIGHT_ERROR_IF("remove_all error for path " + path.string() + ": " + ec.message(), ec.value());
}

//------------------------------------------------------------------------------

void SSliceIndexDicomPullerEditor::pullInstance()
{
    service::IService::ConfigType configuration = this->getConfigTree();
    //Parse server port and hostname
    if(configuration.count("server"))
    {
        const std::string serverInfo               = configuration.get("server", "");
        const std::string::size_type splitPosition = serverInfo.find(':');
        SIGHT_ASSERT("Server info not formatted correctly", splitPosition != std::string::npos);

        m_serverHostnameKey = serverInfo.substr(0, splitPosition);
        m_serverPortKey     = serverInfo.substr(splitPosition + 1, serverInfo.size());
    }
    else
    {
        throw core::tools::Failed("'server' element not found");
    }

    const std::string hostname = ui::base::preferences::getValue(m_serverHostnameKey);
    if(!hostname.empty())
    {
        m_serverHostname = hostname;
    }

    const std::string port = ui::base::preferences::getValue(m_serverPortKey);
    if(!port.empty())
    {
        m_serverPort = std::stoi(port);
    }

    // Catch any errors
    try
    {
        // DicomSeries
        data::DicomSeries::sptr dicomSeries = this->getInOut< data::DicomSeries >("series");
        SIGHT_ASSERT("DicomSeries should not be null !", dicomSeries);

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
        sight::io::http::Request::sptr request = sight::io::http::Request::New(
            pacsServer + "/tools/find");
        QByteArray seriesAnswer;
        try
        {
            seriesAnswer = m_clientQt.post(request, QJsonDocument(body).toJson());
        }
        catch  (sight::io::http::exceptions::HostNotFound& exception)
        {
            std::stringstream ss;
            ss << "Host not found:\n"
               << " Please check your configuration: \n"
               << "Pacs host name: " << m_serverHostname << "\n"
               << "Pacs port: " << m_serverPort << "\n";

            this->displayErrorMessage(ss.str());
            SIGHT_WARN(exception.what());
        }
        QJsonDocument jsonResponse    = QJsonDocument::fromJson(seriesAnswer);
        const QJsonArray& seriesArray = jsonResponse.array();

        // Should be one Series, so take the first of the array.
        const std::string& seriesUID = seriesArray.at(0).toString().toStdString();
        // GET all Instances by Series.
        const std::string& instancesUrl(pacsServer + "/series/" + seriesUID);

        const QByteArray& instancesAnswer =
            m_clientQt.get( sight::io::http::Request::New(instancesUrl));
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
            instancePath = m_clientQt.getFile( sight::io::http::Request::New(instanceUrl));
        }
        catch(sight::io::http::exceptions::ContentNotFound& exception)
        {
            std::stringstream ss;
            ss << "Content not found:  \n"
               << "Unable download the DICOM instance. \n";

            this->displayErrorMessage(ss.str());
            SIGHT_WARN(exception.what());
        }

        // Add path and trigger reading
        dicomSeries->addDicomPath(selectedSliceIndex, instancePath);
        this->readImage(selectedSliceIndex);
    }
    catch (sight::io::http::exceptions::Base& exception)
    {
        std::stringstream ss;
        ss << "Unknown error.";
        this->displayErrorMessage(ss.str());
        SIGHT_WARN(exception.what());
    }
}

//------------------------------------------------------------------------------

void SSliceIndexDicomPullerEditor::displayErrorMessage(const std::string& message) const
{
    SIGHT_WARN("Error: " + message);
    sight::ui::base::dialog::MessageDialog messageBox;
    messageBox.setTitle("Error");
    messageBox.setMessage( message );
    messageBox.setIcon(ui::base::dialog::IMessageDialog::CRITICAL);
    messageBox.addButton(ui::base::dialog::IMessageDialog::OK);
    messageBox.show();
}

} // namespace sight::module::io::dicomweb
