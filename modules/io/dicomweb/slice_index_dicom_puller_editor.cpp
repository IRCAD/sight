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

#include "slice_index_dicom_puller_editor.hpp"

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>
#include <core/os/temp_path.hpp>
#include <core/thread/timer.hpp>

#include <data/array.hpp>
#include <data/composite.hpp>
#include <data/helper/MedicalImage.hpp>
#include <data/image.hpp>
#include <data/image_series.hpp>
#include <data/integer.hpp>
#include <data/series_set.hpp>

#include <io/__/service/reader.hpp>
#include <io/http/exceptions/Base.hpp>

#include <service/op.hpp>

#include <ui/__/dialog/message.hpp>
#include <ui/__/Preferences.hpp>
#include <ui/qt/container/widget.hpp>

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

slice_index_dicom_puller_editor::slice_index_dicom_puller_editor() noexcept =
    default;

//------------------------------------------------------------------------------

slice_index_dicom_puller_editor::~slice_index_dicom_puller_editor() noexcept =
    default;

//------------------------------------------------------------------------------

void slice_index_dicom_puller_editor::configuring()
{
    sight::ui::service::initialize();

    const auto& config = this->get_config();

    m_dicomReaderType = config.get<std::string>("config.<xmlattr>.dicomReader", m_dicomReaderType);
    m_delay           = config.get<unsigned int>("config.<xmlattr>.delay", m_delay);

    if(const auto readerConfig = config.get_child_optional("readerConfig"); readerConfig.has_value())
    {
        m_readerConfig = readerConfig.value();
    }

    if(m_delayTimer && m_delayTimer->is_running())
    {
        m_delayTimer->stop();
        m_delayTimer.reset();
    }

    m_delayTimer = this->worker()->create_timer();
    m_delayTimer->set_function(
        [this]()
        {
            this->triggerNewSlice();
        });

    m_delayTimer->set_one_shot(true);
}

//------------------------------------------------------------------------------

void slice_index_dicom_puller_editor::starting()
{
    sight::ui::service::create();
    auto qtContainer = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(getContainer());

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

    // Create temporary series_set
    m_tmp_series_set = std::make_shared<data::series_set>();

    // Create reader
    auto dicomReader = sight::service::add<sight::io::service::reader>(m_dicomReaderType);
    SIGHT_ASSERT(
        "Unable to create a reader of type: \"" + m_dicomReaderType + "\" in "
                                                                      "sight::module::io::dicomweb::slice_index_dicom_puller_editor.",
        dicomReader
    );
    dicomReader->set_inout(m_tmp_series_set, sight::io::service::s_DATA_KEY);
    dicomReader->set_config(m_readerConfig);
    dicomReader->configure();
    dicomReader->start();

    m_dicomReader = dicomReader;

    // image Indexes
    m_axialIndex    = std::make_shared<data::integer>(0);
    m_frontalIndex  = std::make_shared<data::integer>(0);
    m_sagittalIndex = std::make_shared<data::integer>(0);

    // Load a slice
    if(m_delayTimer)
    {
        if(m_delayTimer->is_running())
        {
            m_delayTimer->stop();
        }

        m_delayTimer->set_duration(std::chrono::milliseconds(m_delay));
        m_delayTimer->start();
    }
    else
    {
        this->triggerNewSlice();
    }
}

//------------------------------------------------------------------------------

void slice_index_dicom_puller_editor::stopping()
{
    if(m_delayTimer && m_delayTimer->is_running())
    {
        m_delayTimer->stop();
    }

    // Stop dicom reader
    if(!m_dicomReader.expired())
    {
        m_dicomReader.lock()->stop();
        sight::service::remove(m_dicomReader.lock());
    }

    this->destroy();
}

//------------------------------------------------------------------------------

void slice_index_dicom_puller_editor::updating()
{
}

//------------------------------------------------------------------------------

void slice_index_dicom_puller_editor::changeSliceIndex(int /*unused*/)
{
    // Update text
    std::stringstream ss;
    ss << m_sliceIndexSlider->value() << " / " << (m_numberOfSlices - 1);
    m_sliceIndexLineEdit->setText(std::string(ss.str()).c_str());

    // Get the new slice if there is no change for m_delay milliseconds
    if(m_delayTimer)
    {
        if(m_delayTimer->is_running())
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

void slice_index_dicom_puller_editor::triggerNewSlice()
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

void slice_index_dicom_puller_editor::readImage(sight::data::dicom_series& dicomSeries, std::size_t selectedSliceIndex)
{
    // DicomSeries
    if(dicomSeries.getModality() != "CT" && dicomSeries.getModality() != "MR" && dicomSeries.getModality() != "XA")
    {
        return;
    }

    // Creates unique temporary folder
    core::os::temp_dir tmpDir;
    std::filesystem::path tmpPath = tmpDir / "tmp";

    SIGHT_INFO("Create " + tmpPath.string());
    std::filesystem::create_directories(tmpPath);

    const auto& binaries = dicomSeries.getDicomContainer();
    auto iter            = binaries.find(selectedSliceIndex);
    SIGHT_ASSERT("Index '" << selectedSliceIndex << "' is not found in DicomSeries", iter != binaries.end());

    const core::memory::buffer_object::sptr bufferObj = iter->second;
    const core::memory::buffer_object::lock_t lockerDest(bufferObj);
    const char* buffer     = static_cast<char*>(lockerDest.buffer());
    const std::size_t size = bufferObj->size();

    std::filesystem::path dest = tmpPath / std::to_string(selectedSliceIndex);
    std::ofstream fs(dest, std::ios::binary | std::ios::trunc);
    SIGHT_THROW_IF("Can't open '" << tmpPath << "' for write.", !fs.good());

    fs.write(buffer, static_cast<std::streamsize>(size));
    fs.close();

    // Read image
    m_dicomReader.lock()->set_folder(tmpPath);
    if(!m_dicomReader.expired())
    {
        m_dicomReader.lock()->update();

        if(m_dicomReader.expired() || m_dicomReader.lock()->stopped())
        {
            return;
        }
    }
    else
    {
        return;
    }

    //Copy image
    data::image_series::sptr imageSeries;

    if(!m_tmp_series_set->empty())
    {
        imageSeries = std::dynamic_pointer_cast<data::image_series>(m_tmp_series_set->front());
    }

    if(imageSeries)
    {
        const data::image::Size newSize = imageSeries->size();

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

        this->set_output("image", imageSeries);
    }
}

//------------------------------------------------------------------------------

void slice_index_dicom_puller_editor::pullInstance(sight::data::dicom_series& dicomSeries)
{
    service::config_t configuration = this->get_config();
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
        throw core::tools::failed("'server' element not found");
    }

    ui::Preferences preferences;

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

            sight::module::io::dicomweb::slice_index_dicom_puller_editor::displayErrorMessage(ss.str());
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
            instancePath = m_clientQt.get_file(sight::io::http::Request::New(instanceUrl));
        }
        catch(sight::io::http::exceptions::ContentNotFound& exception)
        {
            std::stringstream ss;
            ss << "Content not found:  \n"
            << "Unable download the DICOM instance. \n";

            sight::module::io::dicomweb::slice_index_dicom_puller_editor::displayErrorMessage(ss.str());
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
        sight::module::io::dicomweb::slice_index_dicom_puller_editor::displayErrorMessage(ss.str());
        SIGHT_WARN(exception.what());
    }
}

//------------------------------------------------------------------------------

void slice_index_dicom_puller_editor::displayErrorMessage(const std::string& message)
{
    SIGHT_WARN("Error: " + message);
    sight::ui::dialog::message messageBox;
    messageBox.setTitle("Error");
    messageBox.setMessage(message);
    messageBox.setIcon(ui::dialog::message::CRITICAL);
    messageBox.addButton(ui::dialog::message::OK);
    messageBox.show();
}

} // namespace sight::module::io::dicomweb
