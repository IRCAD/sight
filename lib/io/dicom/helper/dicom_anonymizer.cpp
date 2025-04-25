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

#include "io/dicom/helper/dicom_anonymizer.hpp"

#include "io/__/reader/csv_reader.hpp"
#include "io/dicom/exception/invalid_tag.hpp"
#include "io/dicom/helper/dicom_search.hpp"
#include "io/dicom/helper/tags.hpp"

#include <core/base.hpp>
#include <core/jobs/base.hpp>
#include <core/jobs/observer.hpp>
#include <core/os/temp_path.hpp>
#include <core/runtime/path.hpp>

#include <boost/algorithm/string/join.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/exception/all.hpp>
#include <boost/range/algorithm/for_each.hpp>

#include <gdcmGlobal.h>
#include <gdcmReader.h>
#include <gdcmUIDGenerator.h>
#include <gdcmWriter.h>

#include <cstdint>
#include <ctime>
#include <filesystem>
#include <iomanip>

namespace sight::io::dicom::helper
{

const std::string C_MIN_DATE_STRING = "19000101";

gdcm::UIDGenerator generator;

dicom_anonymizer::dicom_anonymizer() :
    m_public_dictionary(gdcm::Global::GetInstance().GetDicts().GetPublicDict()),
    m_observer(std::make_shared<core::jobs::observer>("Anonymization process")),
    m_reference_date(boost::gregorian::from_undelimited_string(C_MIN_DATE_STRING))
{
    const std::filesystem::path tags_path = core::runtime::get_library_resource_file_path(
        "io_dicom/tags.csv"
    );
    SIGHT_ASSERT(
        "File '" + tags_path.string() + "' must exists",
        std::filesystem::is_regular_file(tags_path)
    );
    io::reader::csv_reader csv_reader(tags_path);
    io::reader::csv_reader::token_container_t tag_vec = csv_reader.get_line();

    while(!tag_vec.empty())
    {
        if(tag_vec.size() < 3)
        {
            const std::string error_message = "Error when loading tag '" + boost::algorithm::join(tag_vec, ", ") + "'";
            SIGHT_WARN_IF(error_message, tag_vec.size() != 4);
            SIGHT_THROW_EXCEPTION(io::dicom::exception::invalid_tag(error_message));
        }

        const std::string& action_code = tag_vec[0];
        gdcm::Tag tag                  = io::dicom::helper::get_gdcm_tag(tag_vec[1], tag_vec[2]);

        if(action_code == "D")
        {
            m_action_code_d_tags.insert(tag);
        }
        else if(action_code == "Z" || action_code == "Z/D")
        {
            m_action_code_z_tags.insert(tag);
        }
        else if(action_code == "X"
                || action_code == "X/Z"
                || action_code == "X/D"
                || action_code == "X/Z/D"
                || action_code == "X/Z/U*")
        {
            m_action_code_x_tags.insert(tag);
        }
        else if(action_code == "K")
        {
            m_action_code_k_tags.insert(tag);
        }
        else if(action_code == "C")
        {
            m_action_code_c_tags.insert(tag);
        }
        else if(action_code == "U")
        {
            m_action_code_u_tags.insert(tag);
        }
        else
        {
            SIGHT_ERROR("action code '" + action_code + "' is not managed.");
        }

        tag_vec = csv_reader.get_line();
    }
}

//------------------------------------------------------------------------------

dicom_anonymizer::~dicom_anonymizer()
= default;

//------------------------------------------------------------------------------

void dicom_anonymizer::set_reference_date(const boost::gregorian::date& _reference_date)
{
    m_reference_date = _reference_date;
}

//------------------------------------------------------------------------------

void dicom_anonymizer::anonymize(const std::filesystem::path& _dir_path)
{
    m_archiving = false;
    m_observer->set_total_work_units(100);
    this->anonymization_process(_dir_path);
    m_observer->finish();
}

//------------------------------------------------------------------------------

void move_directory(
    const std::filesystem::path& _input,
    const std::filesystem::path& _output
)
{
    namespace fs = std::filesystem;
    std::error_code ec;
    fs::copy(_input, _output, fs::copy_options::overwrite_existing | fs::copy_options::recursive, ec);
    SIGHT_THROW_IF(
        "copy_directory " << _input.string() << " " << _output.string()
        << " error : " << ec.message(),
        ec.value()
    );

    fs::directory_iterator it(_input);
    fs::directory_iterator end;
    fs::permissions(_output, fs::perms::owner_all, ec);
    SIGHT_ERROR_IF("set " << _output.string() << " permission error : " << ec.message(), ec.value());

    for( ; it != end ; ++it)
    {
        fs::path dest = _output / it->path().filename();
        if(fs::is_directory(*it))
        {
            move_directory(*it, dest);
        }
        else
        {
            fs::rename(*it, dest, ec);
            SIGHT_THROW_IF(
                "rename " << it->path().string() << " " << dest.string()
                << " error : " << ec.message(),
                ec.value()
            );
        }

        fs::permissions(dest, fs::perms::owner_all, ec);
        SIGHT_ERROR_IF("set " << dest.string() << " permission error : " << ec.message(), ec.value());
    }
}

//------------------------------------------------------------------------------

void dicom_anonymizer::remove_anonymize_tag(const gdcm::Tag& _tag)
{
    m_action_code_d_tags.erase(_tag);
    m_action_code_z_tags.erase(_tag);
    m_action_code_x_tags.erase(_tag);
    m_action_code_k_tags.erase(_tag);
    m_action_code_c_tags.erase(_tag);
    m_action_code_u_tags.erase(_tag);
}

//------------------------------------------------------------------------------

const dicom_anonymizer::tag_container_t& dicom_anonymizer::get_action_code_d_tags()
{
    return m_action_code_d_tags;
}

//------------------------------------------------------------------------------

const dicom_anonymizer::tag_container_t& dicom_anonymizer::get_action_code_z_tags()
{
    return m_action_code_z_tags;
}

//------------------------------------------------------------------------------

const dicom_anonymizer::tag_container_t& dicom_anonymizer::get_action_code_x_tags()
{
    return m_action_code_x_tags;
}

//------------------------------------------------------------------------------

const dicom_anonymizer::tag_container_t& dicom_anonymizer::get_action_code_k_tags()
{
    return m_action_code_k_tags;
}

//------------------------------------------------------------------------------

const dicom_anonymizer::tag_container_t& dicom_anonymizer::get_action_code_c_tags()
{
    return m_action_code_c_tags;
}

//------------------------------------------------------------------------------

const dicom_anonymizer::tag_container_t& dicom_anonymizer::get_action_code_u_tags()
{
    return m_action_code_u_tags;
}

//------------------------------------------------------------------------------

void dicom_anonymizer::anonymization_process(const std::filesystem::path& _dir_path)
{
    // Create temporary directory
    core::os::temp_dir tmp_path;

    // Doesn't use std::filesystem::rename because of potential issues when moving folders across volumes
    move_directory(_dir_path, tmp_path);

    std::error_code ec;

    std::filesystem::directory_iterator it(_dir_path);
    std::filesystem::directory_iterator end;

    for( ; it != end ; ++it)
    {
        if(std::filesystem::is_directory(*it))
        {
            std::filesystem::remove_all((*it), ec);
            SIGHT_THROW_IF("remove_all " + _dir_path.string() + " error : " + ec.message(), ec.value());
        }
    }

    std::vector<std::filesystem::path> dicom_files;
    io::dicom::helper::dicom_search::search_recursively(tmp_path, dicom_files, false);

    unsigned int file_index = 0;
    for(const auto& file : dicom_files)
    {
        if(m_observer->cancel_requested())
        {
            break;
        }

        std::ifstream in_stream(file, std::ios::binary);

        std::stringstream ss;
        ss << std::setfill('0') << std::setw(7) << file_index++;

        std::ofstream out_stream(_dir_path / ss.str(), std::ios::binary | std::ios::trunc);

        this->anonymize(in_stream, out_stream);

        auto progress = static_cast<std::uint64_t>(
            ((m_archiving) ? 50.F : 100.F) * static_cast<float>(file_index) / static_cast<float>(dicom_files.size()));
        m_observer->done_work(progress);
    }
}

//------------------------------------------------------------------------------

void dicom_anonymizer::reset_index()
{
    m_file_index = 0;
}

//------------------------------------------------------------------------------

unsigned int dicom_anonymizer::get_next_index()
{
    return m_file_index++;
}

//------------------------------------------------------------------------------

void dicom_anonymizer::anonymize(std::istream& _input_stream, std::ostream& _output_stream)
{
    // File Reader
    gdcm::Reader reader;
    reader.SetStream(_input_stream);
    SIGHT_THROW_IF("Unable to anonymize (file read failed)", !reader.Read());

    // String filter
    m_string_filter.SetFile(reader.GetFile());

    // Objects used to scan groups of elements
    gdcm::DataElement data_element;
    const gdcm::File& dataset_file = reader.GetFile();
    gdcm::DataSet dataset          = dataset_file.GetDataSet();

    std::vector<gdcm::DataElement> preserved_tags;
    for(const gdcm::Tag& t : m_private_tags)
    {
        if(dataset.FindDataElement(t))
        {
            preserved_tags.push_back(dataset.GetDataElement(t));
        }
    }

    m_anonymizer.SetFile(dataset_file);

    m_anonymizer.RemoveGroupLength();
    m_anonymizer.RemoveRetired();

    for(const auto& exception : m_exception_tag_map)
    {
        m_anonymizer.Replace(exception.first, exception.second.c_str());
    }

    // Shift dates
    for(const auto& date_tag : m_action_shift_date_tags)
    {
        this->apply_action_shift_date(date_tag);
    }

    for(const auto& tag : m_action_code_d_tags)
    {
        this->apply_action_code_d(tag);
    }

    for(const auto& tag : m_action_code_z_tags)
    {
        this->apply_action_code_z(tag);
    }

    for(const auto& tag : m_action_code_x_tags)
    {
        this->apply_action_code_x(tag);
    }

    for(const auto& tag : m_action_code_k_tags)
    {
        this->apply_action_code_k(tag);
    }

    for(const auto& tag : m_action_code_c_tags)
    {
        sight::io::dicom::helper::dicom_anonymizer::apply_action_code_c(tag);
    }

    for(const auto& tag : m_action_code_u_tags)
    {
        this->apply_action_code_u(tag);
    }

    auto apply_action_code_x_with_exception = [this](const gdcm::Tag& _tag)
                                              {
                                                  if(m_exception_tag_map.find(_tag) == m_exception_tag_map.end())
                                                  {
                                                      this->apply_action_code_x(_tag);
                                                  }
                                              };

    const auto data_element_set = dataset.GetDES();
    gdcm::Tag tag;

    // Curve Data (0x50xx,0xxxxx)
    auto element = data_element_set.lower_bound(gdcm::Tag(0x5000, 0x0));
    auto end     = data_element_set.upper_bound(gdcm::Tag(0x50ff, 0xffff));
    for( ; element != end ; ++element)
    {
        tag = element->GetTag();
        apply_action_code_x_with_exception(tag);
    }

    // Overlay Data (0x60xx,0x3000) && Overlay Comments (0x60xx,0x4000)
    element = data_element_set.lower_bound(gdcm::Tag(0x6000, 0x3000));
    end     = data_element_set.upper_bound(gdcm::Tag(0x60ff, 0x4000));
    for( ; element != end ; ++element)
    {
        tag = element->GetTag();
        if(tag.GetElement() == 0x3000 || tag.GetElement() == 0x4000)
        {
            apply_action_code_x_with_exception(tag);
        }
    }

    m_anonymizer.RemovePrivateTags(); // Private attributes (X)

    for(const gdcm::DataElement& de : preserved_tags)
    {
        dataset.Insert(de);
    }

    // Write file
    gdcm::Writer writer;
    writer.SetStream(_output_stream);
    writer.SetFile(dataset_file);

    SIGHT_THROW_IF("Unable to anonymize (file write failed)", !writer.Write());
}

//------------------------------------------------------------------------------

void dicom_anonymizer::add_exception_tag(uint16_t _group, uint16_t _element, const std::string& _value)
{
    gdcm::Tag tag(_group, _element);

    m_exception_tag_map[tag] = _value;

    m_action_code_d_tags.erase(tag);
    m_action_code_z_tags.erase(tag);
    m_action_code_x_tags.erase(tag);
    m_action_code_k_tags.erase(tag);
    m_action_code_c_tags.erase(tag);
    m_action_code_u_tags.erase(tag);
}

//------------------------------------------------------------------------------

void dicom_anonymizer::preserve_private_tag(const gdcm::Tag& _tag)
{
    const bool found = std::find(m_private_tags.begin(), m_private_tags.end(), _tag) != m_private_tags.end();
    SIGHT_WARN_IF("Private tag " << _tag.GetGroup() << ", " << _tag.GetElement() << " has already been added", !found);

    if(!found)
    {
        m_private_tags.push_back(_tag);
    }
}

//------------------------------------------------------------------------------

void dicom_anonymizer::apply_action_code_d(const gdcm::Tag& _tag)
{
    // Sequence of Items
    if(m_public_dictionary.GetDictEntry(_tag).GetVR() == gdcm::VR::SQ)
    {
        m_anonymizer.Empty(_tag);
    }
    else
    {
        this->generate_dummy_value(_tag);
    }
}

//------------------------------------------------------------------------------

void dicom_anonymizer::apply_action_code_z(const gdcm::Tag& _tag)
{
    this->apply_action_code_d(_tag);
}

//------------------------------------------------------------------------------

void dicom_anonymizer::apply_action_code_x(const gdcm::Tag& _tag)
{
    m_anonymizer.Remove(_tag);
}

//------------------------------------------------------------------------------

void dicom_anonymizer::apply_action_code_k(const gdcm::Tag& _tag)
{
    // Sequence of Items
    if(m_public_dictionary.GetDictEntry(_tag).GetVR() == gdcm::VR::SQ)
    {
        m_anonymizer.Empty(_tag);
    }
}

//------------------------------------------------------------------------------

void dicom_anonymizer::apply_action_code_c(const gdcm::Tag& /*tag*/)
{
    SIGHT_FATAL(
        "Basic profile \"C\" is not supported yet: "
        "Only basic profile is supported by the current implementation."
    );
}

//------------------------------------------------------------------------------

void dicom_anonymizer::apply_action_code_u(const gdcm::Tag& _tag)
{
    const std::string old_uid = m_string_filter.ToString(_tag);
    if(!old_uid.empty())
    {
        auto it = m_uid_map.find(old_uid);
        std::string uid;

        if(it == m_uid_map.end())
        {
            uid = generator.Generate();
            m_uid_map.insert(std::pair<std::string, std::string>(old_uid, uid));
        }
        else
        {
            uid = it->second;
        }

        m_anonymizer.Replace(_tag, uid.c_str());
    }
}

//------------------------------------------------------------------------------

void dicom_anonymizer::add_shift_date_tag(const gdcm::Tag& _tag)
{
    m_action_code_d_tags.erase(_tag);
    m_action_code_z_tags.erase(_tag);
    m_action_code_x_tags.erase(_tag);
    m_action_code_k_tags.erase(_tag);
    m_action_code_c_tags.erase(_tag);
    m_action_code_u_tags.erase(_tag);

    m_action_shift_date_tags.insert(_tag);
}

//------------------------------------------------------------------------------

void dicom_anonymizer::apply_action_shift_date(const gdcm::Tag& _tag)
{
    const std::string old_date        = m_string_filter.ToString(_tag);
    const boost::gregorian::date date = boost::gregorian::from_undelimited_string(old_date);

    const auto shift = date - m_reference_date;

    //Minimum date
    const boost::gregorian::date min_date = boost::gregorian::from_undelimited_string(C_MIN_DATE_STRING);

    const auto shifted_date = min_date + shift;

    m_anonymizer.Replace(_tag, boost::gregorian::to_iso_string(shifted_date).c_str());
}

//------------------------------------------------------------------------------

void dicom_anonymizer::generate_dummy_value(const gdcm::Tag& _tag)
{
    switch(m_public_dictionary.GetDictEntry(_tag).GetVR())
    {
        case gdcm::VR::AE:
            m_anonymizer.Replace(_tag, "ANONYMIZED");
            break;

        case gdcm::VR::AS:
            m_anonymizer.Replace(_tag, "000Y");
            break;

        case gdcm::VR::AT:
            m_anonymizer.Replace(_tag, "00H,00H,00H,00H");
            break;

        case gdcm::VR::CS:
            // Patient's sex
            if(_tag == gdcm::Tag(0x0010, 0x0040))
            {
                m_anonymizer.Replace(_tag, "O");
            }
            else
            {
                m_anonymizer.Replace(_tag, "ANONYMIZED");
            }

            break;

        case gdcm::VR::DA:
            m_anonymizer.Replace(_tag, C_MIN_DATE_STRING.c_str());
            break;

        case gdcm::VR::DS:
            m_anonymizer.Replace(_tag, "0");
            break;

        case gdcm::VR::DT:
            m_anonymizer.Replace(_tag, std::string(C_MIN_DATE_STRING + "000000.000000").c_str());
            break;

        case gdcm::VR::FD:
        case gdcm::VR::FL:
        case gdcm::VR::IS:
            m_anonymizer.Replace(_tag, "0");
            break;

        case gdcm::VR::LO:
        case gdcm::VR::LT:
            m_anonymizer.Replace(_tag, "ANONYMIZED");
            break;

        case gdcm::VR::OB:
            m_anonymizer.Replace(_tag, "00H00H");
            break;

        case gdcm::VR::OF:
        case gdcm::VR::OW:
            m_anonymizer.Replace(_tag, "0");
            break;

        case gdcm::VR::PN:
            m_anonymizer.Replace(_tag, "ANONYMIZED^ANONYMIZED");
            break;

        case gdcm::VR::SH:
            m_anonymizer.Replace(_tag, "ANONYMIZED");
            break;

        case gdcm::VR::SL:
            m_anonymizer.Replace(_tag, "0");
            break;

        case gdcm::VR::SQ:
            m_anonymizer.Empty(_tag);
            break;

        case gdcm::VR::SS:
            m_anonymizer.Replace(_tag, "0");
            break;

        case gdcm::VR::ST:
            m_anonymizer.Replace(_tag, "ANONYMIZED");
            break;

        case gdcm::VR::TM:
            m_anonymizer.Replace(_tag, "000000.000000");
            break;

        case gdcm::VR::UI:
            m_anonymizer.Replace(_tag, "ANONYMIZED");
            break;

        case gdcm::VR::UL:
            m_anonymizer.Replace(_tag, "0");
            break;

        case gdcm::VR::UN:
            m_anonymizer.Replace(_tag, "ANONYMIZED");
            break;

        case gdcm::VR::US:
            m_anonymizer.Replace(_tag, "0");
            break;

        case gdcm::VR::UT:
            m_anonymizer.Replace(_tag, "ANONYMIZED");
            break;

        default:
            SIGHT_ERROR(_tag << " is not supported. Emptied value. ");
            m_anonymizer.Empty(_tag);
            break;
    }
}

//------------------------------------------------------------------------------

void dicom_anonymizer::copy_directory(
    const std::filesystem::path& _input,
    const std::filesystem::path& _output
)
{
    namespace fs = std::filesystem;
    std::error_code ec;
    fs::copy(_input, _output, fs::copy_options::overwrite_existing | fs::copy_options::recursive, ec);
    SIGHT_THROW_IF(
        "copy_directory " << _input.string() << " " << _output.string()
        << " error : " << ec.message(),
        ec.value()
    );

    fs::directory_iterator it(_input);
    fs::directory_iterator end;

    ec.clear();
    fs::permissions(_output, fs::perms::owner_all, ec);
    SIGHT_ERROR_IF("set " + _output.string() + " permission error : " + ec.message(), ec.value());

    for( ; it != end ; ++it)
    {
        fs::path dest = _output / it->path().filename();
        if(fs::is_directory(*it))
        {
            dicom_anonymizer::copy_directory(*it, dest);
        }
        else
        {
            // Use stream instead of boost::copy_file (Unix c++11 issue)
            std::ifstream in_stream(it->path(), std::ios::binary);
            SIGHT_THROW_IF("Unable to read file :" << it->path().string(), !in_stream.good());
            std::ofstream out_stream(dest, std::ios::binary);
            SIGHT_THROW_IF("Unable to write file :" << dest.string(), !out_stream.good());

            out_stream << in_stream.rdbuf();

            in_stream.close();
            out_stream.close();
        }

        ec.clear();
        fs::permissions(dest, fs::perms::owner_all, ec);
        SIGHT_ERROR_IF("set " + dest.string() + " permission error : " + ec.message(), ec.value());
    }
}

//------------------------------------------------------------------------------

SPTR(core::jobs::base) dicom_anonymizer::get_job() const
{
    return m_observer;
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::helper
