/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
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

#include <core/com/Signal.hxx>
#include <core/com/Slots.hxx>
#include <core/thread/Timer.hpp>
#include <core/tools/System.hpp>

#include <data/Array.hpp>
#include <data/Composite.hpp>
#include <data/helper/MedicalImage.hpp>
#include <data/Image.hpp>
#include <data/ImageSeries.hpp>
#include <data/Integer.hpp>
#include <data/SeriesSet.hpp>

#include <io/base/service/IReader.hpp>
#include <io/http/exceptions/Base.hpp>

#include <service/base.hpp>

#include <ui/base/dialog/MessageDialog.hpp>
#include <ui/base/Preferences.hpp>
#include <ui/qt/container/QtContainer.hpp>

#include <boost/lexical_cast.hpp>

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

SSliceIndexDicomPullerEditor::SSliceIndexDicomPullerEditor() noexcept =
    default;

//------------------------------------------------------------------------------

SSliceIndexDicomPullerEditor::~SSliceIndexDicomPullerEditor() noexcept =
    default;

//------------------------------------------------------------------------------

void SSliceIndexDicomPullerEditor::configuring()
{
    sight::ui::base::IGuiContainer::initialize();

    const auto& config = this->getConfiguration();

    m_dicomReaderType = config.get<std::string>("config.<xmlattr>.dicomReader", m_dicomReaderType);
    m_delay           = config.get<unsigned int>("config.<xmlattr>.delay", m_delay);

    if(const auto readerConfig = config.get_child_optional("readerConfig"); readerConfig.has_value())
    {
        m_readerConfig = readerConfig.value();
    }

    if(m_delayTimer && m_delayTimer->isRunning())
    {
        m_delayTimer->stop();
        m_delayTimer.reset();
    }

    m_delayTimer = this->worker()->createTimer();
    m_delayTimer->setFunction(
        [ =, this]()
        {
            this->triggerNewSlice();
        });

    m_delayTimer->setOneShot(true);
}

//------------------------------------------------------------------------------

void SSliceIndexDicomPullerEditor::starting()
{
    sight::ui::base::IGuiContainer::create();
    auto qtContainer = sight::ui::qt::container::QtContainer::dynamicCast(getContainer());

    auto* layout = new QHBoxLayout();

    const auto dicomSeries = m_series.lock();
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
    m_tmp_series_set = data::SeriesSet::New();

    // Create reader
    auto dicomReader = service::add<sight::io::base::service::IReader>(m_dicomReaderType);
    SIGHT_ASSERT(
        "Unable to create a reader of type: \"" + m_dicomReaderType + "\" in "
                                                                      "sight::module::io::dicomweb::SSliceIndexDicomPullerEditor.",
        dicomReader
    );
    dicomReader->setInOut(m_tmp_series_set, sight::io::base::service::s_DATA_KEY);
    dicomReader->setConfiguration(m_readerConfig);
    dicomReader->configure();
    dicomReader->start();

    m_dicomReader = dicomReader;

    // Image Indexes
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
        service::remove(m_dicomReader.lock());
    }

    this->destroy();
}

//------------------------------------------------------------------------------

void SSliceIndexDicomPullerEditor::updating()
{
}

//------------------------------------------------------------------------------

void SSliceIndexDicomPullerEditor::changeSliceIndex(int /*unused*/)
{
    // Update text
    std::stringstream ss;
    ss << m_sliceIndexSlider->value() << " / " << (m_numberOfSlices - 1);
    m_sliceIndexLineEdit->setText(std::string(ss.str()).c_str());

    // Get the new slice if there is no change for m_delay milliseconds
    if(m_delayTimer)
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
    const auto dicomSeries = m_series.lock();
    SIGHT_ASSERT("DicomSeries should not be null !", dicomSeries);

    // Compute slice index
    const std::size_t selectedSliceIndex = static_cast<std::size_t>(m_sliceIndexSlider->value())
                                           + dicomSeries->getFirstInstanceNumber();
    if(!dicomSeries->isInstanceAvailable(selectedSliceIndex))
    {
        this->pullInstance(*dicomSeries);
    }
    else
    {
        this->readImage(*dicomSeries, selectedSliceIndex);
    }
}

//------------------------------------------------------------------------------

void SSliceIndexDicomPullerEditor::readImage(sight::data::DicomSeries& dicomSeries, std::size_t selectedSliceIndex)
{
    // DicomSeries
    if(dicomSeries.getModality() != "CT" && dicomSeries.getModality() != "MR" && dicomSeries.getModality() != "XA")
    {
        return;
    }

    // Creates unique temporary folder, no need to check if exists before (see core::tools::System::getTemporaryFolder)
    std::filesystem::path path    = core::tools::System::getTemporaryFolder("dicom");
    std::filesystem::path tmpPath = path / "tmp";

    SIGHT_INFO("Create " + tmpPath.string());
    std::filesystem::create_directories(tmpPath);

    const auto& binaries = dicomSeries.getDicomContainer();
    auto iter            = binaries.find(selectedSliceIndex);
    SIGHT_ASSERT("Index '" << selectedSliceIndex << "' is not found in DicomSeries", iter != binaries.end());

    const core::memory::BufferObject::sptr bufferObj = iter->second;
    const core::memory::BufferObject::Lock lockerDest(bufferObj);
    const char* buffer     = static_cast<char*>(lockerDest.getBuffer());
    const std::size_t size = bufferObj->getSize();

    std::filesystem::path dest = tmpPath / std::to_string(selectedSliceIndex);
    std::ofstream fs(dest, std::ios::binary | std::ios::trunc);
    SIGHT_THROW_IF("Can't open '" << tmpPath << "' for write.", !fs.good());

    fs.write(buffer, static_cast<std::streamsize>(size));
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

    if(!m_tmp_series_set->empty())
    {
        imageSeries = data::ImageSeries::dynamicCast(m_tmp_series_set->front());
    }

    if(imageSeries)
    {
        const data::Image::Size newSize = imageSeries->getSize();

        m_frontalIndex->setValue(static_cast<int>(newSize[0] / 2));
        m_sagittalIndex->setValue(static_cast<int>(newSize[1] / 2));

        data::helper::MedicalImage::setSliceIndex(
            *imageSeries,
            data::helper::MedicalImage::orientation_t::AXIAL,
            m_axialIndex->value()
        );
        data::helper::MedicalImage::setSliceIndex(
            *imageSeries,
            data::helper::MedicalImage::orientation_t::AXIAL,
            m_frontalIndex->value()
        );
        data::helper::MedicalImage::setSliceIndex(
            *imageSeries,
            data::helper::MedicalImage::orientation_t::AXIAL,
            m_sagittalIndex->value()
        );

        this->setOutput("image", imageSeries);
    }

    std::error_code ec;
    std::filesystem::remove_all(path, ec);
    SIGHT_ERROR_IF("remove_all error for path " + path.string() + ": " + ec.message(), ec.value());
}

//------------------------------------------------------------------------------

void SSliceIndexDicomPullerEditor::pullInstance(sight::data::DicomSeries& dicomSeries)
{
    service::IService::ConfigType configuration = this->getConfiguration();
    //Parse server port and hostname
    if(configuration.count("server") != 0U)
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

    ui::base::Preferences preferences;

    try
    {
        m_serverPort = preferences.delimited_get(m_serverPortKey, m_serverPort);
    }
    catch(...)
    {
        // Do nothing
    }

    try
    {
        m_serverHostname = preferences.delimited_get(m_serverHostnameKey, m_serverHostname);
    }
    catch(...)
    {
        // Do nothing
    }

    // Catch any errors
    try
    {
        // Get selected slice
        std::size_t selectedSliceIndex = static_cast<std::size_t>(m_sliceIndexSlider->value())
                                         + dicomSeries.getFirstInstanceNumber();

        std::string seriesInstanceUID = dicomSeries.getSeriesInstanceUID();

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
            pacsServer + "/tools/find"
        );
        QByteArray seriesAnswer;
        try
        {
            seriesAnswer = m_clientQt.post(request, QJsonDocument(body).toJson());
        }
        catch(sight::io::http::exceptions::HostNotFound& exception)
        {
            std::stringstream ss;
            ss << "Host not found:\n"
            << " Please check your configuration: \n"
            << "Pacs host name: " << m_serverHostname << "\n"
            << "Pacs port: " << m_serverPort << "\n";

            sight::module::io::dicomweb::SSliceIndexDicomPullerEditor::displayErrorMessage(ss.str());
            SIGHT_WARN(exception.what());
        }
        QJsonDocument jsonResponse    = QJsonDocument::fromJson(seriesAnswer);
        const QJsonArray& seriesArray = jsonResponse.array();

        // Should be one Series, so take the first of the array.
        const std::string seriesUID(seriesArray.at(0).toString().toStdString());
        // GET all Instances by Series.
        const std::string instancesUrl(pacsServer + "/series/" + seriesUID);

        const QByteArray& instancesAnswer = m_clientQt.get(sight::io::http::Request::New(instancesUrl));
        jsonResponse = QJsonDocument::fromJson(instancesAnswer);
        const QJsonObject& jsonObj       = jsonResponse.object();
        const QJsonArray& instancesArray = jsonObj["Instances"].toArray();
        const std::string& instanceUID   =
            instancesArray.at(static_cast<int>(selectedSliceIndex)).toString().toStdString();

        // GET frame by Slice.
        std::string instancePath;
        const std::string instanceUrl(pacsServer + "/instances/" + instanceUID + "/file");
        try
        {
            instancePath = m_clientQt.getFile(sight::io::http::Request::New(instanceUrl));
        }
        catch(sight::io::http::exceptions::ContentNotFound& exception)
        {
            std::stringstream ss;
            ss << "Content not found:  \n"
            << "Unable download the DICOM instance. \n";

            sight::module::io::dicomweb::SSliceIndexDicomPullerEditor::displayErrorMessage(ss.str());
            SIGHT_WARN(exception.what());
        }

        // Add path and trigger reading
        dicomSeries.addDicomPath(selectedSliceIndex, instancePath);
        this->readImage(dicomSeries, selectedSliceIndex);
    }
    catch(sight::io::http::exceptions::Base& exception)
    {
        std::stringstream ss;
        ss << "Unknown error.";
        sight::module::io::dicomweb::SSliceIndexDicomPullerEditor::displayErrorMessage(ss.str());
        SIGHT_WARN(exception.what());
    }
}

//------------------------------------------------------------------------------

void SSliceIndexDicomPullerEditor::displayErrorMessage(const std::string& message)
{
    SIGHT_WARN("Error: " + message);
    sight::ui::base::dialog::MessageDialog messageBox;
    messageBox.setTitle("Error");
    messageBox.setMessage(message);
    messageBox.setIcon(ui::base::dialog::IMessageDialog::CRITICAL);
    messageBox.addButton(ui::base::dialog::IMessageDialog::OK);
    messageBox.show();
}

} // namespace sight::module::io::dicomweb
