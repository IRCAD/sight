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
#include <data/helper/medical_image.hpp>
#include <data/image.hpp>
#include <data/image_series.hpp>
#include <data/integer.hpp>
#include <data/series_set.hpp>

#include <io/__/service/reader.hpp>
#include <io/http/exceptions/Base.hpp>

#include <service/op.hpp>

#include <ui/__/dialog/message.hpp>
#include <ui/__/preferences.hpp>
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

    if(const auto reader_config = config.get_child_optional("readerConfig"); reader_config.has_value())
    {
        m_readerConfig = reader_config.value();
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
    auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(getContainer());

    auto* layout = new QHBoxLayout();

    const auto dicom_series = m_series.lock();
    SIGHT_ASSERT("DicomSeries should not be null !", dicom_series);
    m_numberOfSlices = dicom_series->numInstances();

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

    qt_container->setLayout(layout);

    // Connect the signals
    QObject::connect(m_sliceIndexSlider, SIGNAL(valueChanged(int)), this, SLOT(changeSliceIndex(int)));

    // Create temporary series_set
    m_tmp_series_set = std::make_shared<data::series_set>();

    // Create reader
    auto dicom_reader = sight::service::add<sight::io::service::reader>(m_dicomReaderType);
    SIGHT_ASSERT(
        "Unable to create a reader of type: \"" + m_dicomReaderType + "\" in "
                                                                      "sight::module::io::dicomweb::slice_index_dicom_puller_editor.",
        dicom_reader
    );
    dicom_reader->set_inout(m_tmp_series_set, sight::io::service::s_DATA_KEY);
    dicom_reader->set_config(m_readerConfig);
    dicom_reader->configure();
    dicom_reader->start();

    m_dicomReader = dicom_reader;

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
    const auto dicom_series = m_series.lock();
    SIGHT_ASSERT("DicomSeries should not be null !", dicom_series);

    // Compute slice index
    const std::size_t selected_slice_index = static_cast<std::size_t>(m_sliceIndexSlider->value())
                                             + dicom_series->getFirstInstanceNumber();
    if(!dicom_series->isInstanceAvailable(selected_slice_index))
    {
        this->pullInstance(*dicom_series);
    }
    else
    {
        this->readImage(*dicom_series, selected_slice_index);
    }
}

//------------------------------------------------------------------------------

void slice_index_dicom_puller_editor::readImage(
    sight::data::dicom_series& _dicom_series,
    std::size_t _selected_slice_index
)
{
    // DicomSeries
    if(_dicom_series.getModality() != "CT" && _dicom_series.getModality() != "MR"
       && _dicom_series.getModality() != "XA")
    {
        return;
    }

    // Creates unique temporary folder
    core::os::temp_dir tmp_dir;
    std::filesystem::path tmp_path = tmp_dir / "tmp";

    SIGHT_INFO("Create " + tmp_path.string());
    std::filesystem::create_directories(tmp_path);

    const auto& binaries = _dicom_series.getDicomContainer();
    auto iter            = binaries.find(_selected_slice_index);
    SIGHT_ASSERT("Index '" << _selected_slice_index << "' is not found in DicomSeries", iter != binaries.end());

    const core::memory::buffer_object::sptr buffer_obj = iter->second;
    const core::memory::buffer_object::lock_t locker_dest(buffer_obj);
    const char* buffer     = static_cast<char*>(locker_dest.buffer());
    const std::size_t size = buffer_obj->size();

    std::filesystem::path dest = tmp_path / std::to_string(_selected_slice_index);
    std::ofstream fs(dest, std::ios::binary | std::ios::trunc);
    SIGHT_THROW_IF("Can't open '" << tmp_path << "' for write.", !fs.good());

    fs.write(buffer, static_cast<std::streamsize>(size));
    fs.close();

    // Read image
    m_dicomReader.lock()->set_folder(tmp_path);
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
    data::image_series::sptr image_series;

    if(!m_tmp_series_set->empty())
    {
        image_series = std::dynamic_pointer_cast<data::image_series>(m_tmp_series_set->front());
    }

    if(image_series)
    {
        const data::image::Size new_size = image_series->size();

        m_frontalIndex->setValue(static_cast<int>(new_size[0] / 2));
        m_sagittalIndex->setValue(static_cast<int>(new_size[1] / 2));

        data::helper::medical_image::set_slice_index(
            *image_series,
            data::helper::medical_image::orientation_t::AXIAL,
            m_axialIndex->value()
        );
        data::helper::medical_image::set_slice_index(
            *image_series,
            data::helper::medical_image::orientation_t::AXIAL,
            m_frontalIndex->value()
        );
        data::helper::medical_image::set_slice_index(
            *image_series,
            data::helper::medical_image::orientation_t::AXIAL,
            m_sagittalIndex->value()
        );

        this->set_output("image", image_series);
    }
}

//------------------------------------------------------------------------------

void slice_index_dicom_puller_editor::pullInstance(sight::data::dicom_series& _dicom_series)
{
    service::config_t configuration = this->get_config();
    //Parse server port and hostname
    if(configuration.count("server") != 0U)
    {
        const std::string server_info               = configuration.get("server", "");
        const std::string::size_type split_position = server_info.find(':');
        SIGHT_ASSERT("Server info not formatted correctly", split_position != std::string::npos);

        m_serverHostnameKey = server_info.substr(0, split_position);
        m_serverPortKey     = server_info.substr(split_position + 1, server_info.size());
    }
    else
    {
        throw core::tools::failed("'server' element not found");
    }

    ui::preferences preferences;

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
        std::size_t selected_slice_index = static_cast<std::size_t>(m_sliceIndexSlider->value())
                                           + _dicom_series.getFirstInstanceNumber();

        std::string series_instance_uid = _dicom_series.getSeriesInstanceUID();

        // Find Series according to SeriesInstanceUID
        QJsonObject query;
        query.insert("SeriesInstanceUID", series_instance_uid.c_str());

        QJsonObject body;
        body.insert("Level", "Series");
        body.insert("Query", query);
        body.insert("Limit", 0);

        /// Url PACS
        const std::string pacs_server("http://" + m_serverHostname + ":" + std::to_string(m_serverPort));

        /// Orthanc "/tools/find" route. POST a JSON to get all Series corresponding to the SeriesInstanceUID.
        sight::io::http::Request::sptr request = sight::io::http::Request::New(
            pacs_server + "/tools/find"
        );
        QByteArray series_answer;
        try
        {
            series_answer = m_clientQt.post(request, QJsonDocument(body).toJson());
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
        QJsonDocument json_response    = QJsonDocument::fromJson(series_answer);
        const QJsonArray& series_array = json_response.array();

        // Should be one Series, so take the first of the array.
        const std::string series_uid(series_array.at(0).toString().toStdString());
        // GET all Instances by Series.
        const std::string instances_url(pacs_server + "/series/" + series_uid);

        const QByteArray& instances_answer = m_clientQt.get(sight::io::http::Request::New(instances_url));
        json_response = QJsonDocument::fromJson(instances_answer);
        const QJsonObject& json_obj       = json_response.object();
        const QJsonArray& instances_array = json_obj["Instances"].toArray();
        const std::string& instance_uid   =
            instances_array.at(static_cast<int>(selected_slice_index)).toString().toStdString();

        // GET frame by Slice.
        std::string instance_path;
        const std::string instance_url(pacs_server + "/instances/" + instance_uid + "/file");
        try
        {
            instance_path = m_clientQt.get_file(sight::io::http::Request::New(instance_url));
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
        _dicom_series.addDicomPath(selected_slice_index, instance_path);
        this->readImage(_dicom_series, selected_slice_index);
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

void slice_index_dicom_puller_editor::displayErrorMessage(const std::string& _message)
{
    SIGHT_WARN("Error: " + _message);
    sight::ui::dialog::message message_box;
    message_box.setTitle("Error");
    message_box.setMessage(_message);
    message_box.setIcon(ui::dialog::message::CRITICAL);
    message_box.addButton(ui::dialog::message::OK);
    message_box.show();
}

} // namespace sight::module::io::dicomweb
