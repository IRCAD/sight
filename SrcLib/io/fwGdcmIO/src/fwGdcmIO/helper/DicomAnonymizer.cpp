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

#include "fwGdcmIO/helper/DicomAnonymizer.hpp"

#include "fwGdcmIO/exception/InvalidTag.hpp"
#include "fwGdcmIO/helper/CsvIO.hpp"
#include "fwGdcmIO/helper/DicomSearch.hpp"
#include "fwGdcmIO/helper/tags.hpp"

#include <fwCore/base.hpp>

#include <fwJobs/IJob.hpp>
#include <fwJobs/Observer.hpp>

#include <fwRuntime/operations.hpp>

#include <fwTools/System.hpp>

#include <boost/algorithm/string/join.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/exception/all.hpp>
#include <boost/range/algorithm/for_each.hpp>

#include <gdcmGlobal.h>
#include <gdcmReader.h>
#include <gdcmUIDGenerator.h>
#include <gdcmWriter.h>
#include <time.h>

#include <cstdint>
#include <filesystem>
#include <iomanip>

namespace fwGdcmIO
{
namespace helper
{

const std::string c_MIN_DATE_STRING = "19000101";

::gdcm::UIDGenerator GENERATOR;

DicomAnonymizer::DicomAnonymizer() :
    m_publicDictionary(::gdcm::Global::GetInstance().GetDicts().GetPublicDict()),
    m_observer(::fwJobs::Observer::New("Anonymization process")),
    m_archiving(false),
    m_fileIndex(0),
    m_referenceDate(::boost::gregorian::from_undelimited_string(c_MIN_DATE_STRING))
{
    const std::filesystem::path tagsPathStr = ::fwRuntime::getLibraryResourceFilePath(
        "fwGdcmIO-" FWGDCMIO_VER "/tags.csv");
    std::filesystem::path tagsPath = tagsPathStr;
    SLM_ASSERT("File '" + tagsPath.string() + "' must exists",
               std::filesystem::is_regular_file(tagsPath));

    auto csvStream = std::ifstream(tagsPath.string());
    ::fwGdcmIO::helper::CsvIO csvReader(csvStream);
    ::fwGdcmIO::helper::CsvIO::TokenContainerType tagVec = csvReader.getLine();

    while(!tagVec.empty())
    {
        if(tagVec.size() < 3)
        {
            const std::string errorMessage = "Error when loading tag '" + ::boost::algorithm::join(tagVec, ", ") + "'";
            OSLM_WARN_IF(errorMessage, tagVec.size() != 4);
            FW_RAISE_EXCEPTION(::fwGdcmIO::exception::InvalidTag(errorMessage));
        }

        const std::string& actionCode = tagVec[0];
        ::gdcm::Tag tag = ::fwGdcmIO::helper::getGdcmTag(tagVec[1], tagVec[2]);

        if(actionCode == "D")
        {
            m_actionCodeDTags.insert(tag);
        }
        else if(actionCode == "Z" || actionCode == "Z/D")
        {
            m_actionCodeZTags.insert(tag);
        }
        else if(actionCode == "X"
                || actionCode == "X/Z"
                || actionCode == "X/D"
                || actionCode == "X/Z/D"
                || actionCode == "X/Z/U*")
        {
            m_actionCodeXTags.insert(tag);
        }
        else if(actionCode == "K")
        {
            m_actionCodeKTags.insert(tag);
        }
        else if(actionCode == "C")
        {
            m_actionCodeCTags.insert(tag);
        }
        else if(actionCode == "U")
        {
            m_actionCodeUTags.insert(tag);
        }
        else
        {
            OSLM_ERROR("Action code '" + actionCode + "' is not managed.");
        }

        tagVec = csvReader.getLine();
    }
}

//------------------------------------------------------------------------------

DicomAnonymizer::~DicomAnonymizer()
{
}

//------------------------------------------------------------------------------

void DicomAnonymizer::setReferenceDate(const ::boost::gregorian::date& referenceDate)
{
    m_referenceDate = referenceDate;
}

//------------------------------------------------------------------------------

void DicomAnonymizer::anonymize(const std::filesystem::path& dirPath)
{
    m_archiving = false;
    m_observer->setTotalWorkUnits(100);
    this->anonymizationProcess(dirPath);
    m_observer->finish();
}

//------------------------------------------------------------------------------

void moveDirectory(const std::filesystem::path& input,
                   const std::filesystem::path& output)
{
    namespace fs = std::filesystem;
    std::error_code ec;
    fs::copy(input, output, fs::copy_options::overwrite_existing | fs::copy_options::recursive, ec);
    FW_RAISE_IF("copy_directory " << input.string() << " " << output.string()
                                  << " error : " << ec.message(), ec.value());

    fs::directory_iterator it(input);
    fs::directory_iterator end;
    fs::permissions(output, fs::perms::owner_all, ec);
    OSLM_ERROR_IF("set " << output.string() << " permission error : " << ec.message(), ec.value());

    for(; it != end; ++it)
    {
        fs::path dest = output / it->path().filename();
        if(fs::is_directory(*it))
        {
            moveDirectory(*it, dest);
        }
        else
        {
            fs::rename(*it, dest, ec);
            FW_RAISE_IF("rename " << it->path().string() << " " << dest.string()
                                  << " error : " << ec.message(), ec.value());
        }

        fs::permissions(dest, fs::perms::owner_all, ec);
        OSLM_ERROR_IF("set " << dest.string() << " permission error : " << ec.message(), ec.value());
    }
}

//------------------------------------------------------------------------------

void DicomAnonymizer::removeAnonymizeTag(const ::gdcm::Tag& tag)
{
    m_actionCodeDTags.erase(tag);
    m_actionCodeZTags.erase(tag);
    m_actionCodeXTags.erase(tag);
    m_actionCodeKTags.erase(tag);
    m_actionCodeCTags.erase(tag);
    m_actionCodeUTags.erase(tag);
}

//------------------------------------------------------------------------------

const DicomAnonymizer::TagContainerType& DicomAnonymizer::getActionCodeDTags()
{
    return m_actionCodeDTags;
}

//------------------------------------------------------------------------------

const DicomAnonymizer::TagContainerType& DicomAnonymizer::getActionCodeZTags()
{
    return m_actionCodeZTags;
}

//------------------------------------------------------------------------------

const DicomAnonymizer::TagContainerType& DicomAnonymizer::getActionCodeXTags()
{
    return m_actionCodeXTags;
}

//------------------------------------------------------------------------------

const DicomAnonymizer::TagContainerType& DicomAnonymizer::getActionCodeKTags()
{
    return m_actionCodeKTags;
}

//------------------------------------------------------------------------------

const DicomAnonymizer::TagContainerType& DicomAnonymizer::getActionCodeCTags()
{
    return m_actionCodeCTags;
}

//------------------------------------------------------------------------------

const DicomAnonymizer::TagContainerType& DicomAnonymizer::getActionCodeUTags()
{
    return m_actionCodeUTags;
}

//------------------------------------------------------------------------------

void DicomAnonymizer::anonymizationProcess(const std::filesystem::path& dirPath)
{
    // Create temporary directory
    std::filesystem::path tmpPath = ::fwTools::System::getTemporaryFolder("DicomAnonymizer");
    tmpPath /= "tmp";

    // Doesn't use std::filesystem::rename because of potential issues when moving folders across volumes
    moveDirectory(dirPath, tmpPath);

    std::error_code ec;

    std::filesystem::directory_iterator it(dirPath);
    std::filesystem::directory_iterator end;

    for(; it != end; ++it)
    {
        if(std::filesystem::is_directory(*it))
        {
            std::filesystem::remove_all((*it), ec);
            FW_RAISE_IF("remove_all " + dirPath.string() + " error : " + ec.message(), ec.value());
        }
    }

    std::vector< std::filesystem::path > dicomFiles;
    ::fwGdcmIO::helper::DicomSearch::searchRecursively(tmpPath, dicomFiles, false);

    unsigned int fileIndex = 0;
    for(const auto file : dicomFiles)
    {
        if(m_observer->cancelRequested())
        {
            break;
        }

        std::ifstream inStream(file, std::ios::binary);

        std::stringstream ss;
        ss << std::setfill('0') << std::setw(7) << fileIndex++;

        std::ofstream outStream(dirPath / ss.str(), std::ios::binary | std::ios::trunc);

        this->anonymize(inStream, outStream);

        std::uint64_t progress = static_cast<std::uint64_t>(
            ((m_archiving) ? 50 : 100) * static_cast<float>(fileIndex) / static_cast<float>(dicomFiles.size()));
        m_observer->doneWork(progress);
    }

}

//------------------------------------------------------------------------------

void DicomAnonymizer::resetIndex()
{
    m_fileIndex = 0;
}

//------------------------------------------------------------------------------

unsigned int DicomAnonymizer::getNextIndex()
{
    return m_fileIndex++;
}

//------------------------------------------------------------------------------

void DicomAnonymizer::anonymize(std::istream& inputStream, std::ostream& outputStream)
{
    // File Reader
    ::gdcm::Reader reader;
    reader.SetStream(inputStream);
    FW_RAISE_IF("Unable to anonymize (file read failed)", !reader.Read());

    // String filter
    m_stringFilter.SetFile(reader.GetFile());

    // Objects used to scan groups of elements
    ::gdcm::Tag tag;
    ::gdcm::DataElement dataElement;
    const ::gdcm::File& datasetFile = reader.GetFile();
    ::gdcm::DataSet dataset = datasetFile.GetDataSet();

    std::vector< ::gdcm::DataElement > preservedTags;
    for(const ::gdcm::Tag& t : m_privateTags)
    {
        if(dataset.FindDataElement(t))
        {
            preservedTags.push_back(dataset.GetDataElement(t));
        }
    }

    m_anonymizer.SetFile( datasetFile );

    m_anonymizer.RemoveGroupLength();
    m_anonymizer.RemoveRetired();

    for(ExceptionTagMapType::value_type exception : m_exceptionTagMap)
    {
        m_anonymizer.Replace(exception.first, exception.second.c_str());
    }

    // Shift dates
    for(const auto& dateTag : m_actionShiftDateTags)
    {
        this->applyActionShiftDate(dateTag);
    }

    for(const auto& tag : m_actionCodeDTags)
    {
        this->applyActionCodeD(tag);
    }
    for(const auto& tag : m_actionCodeZTags)
    {
        this->applyActionCodeZ(tag);
    }
    for(const auto& tag : m_actionCodeXTags)
    {
        this->applyActionCodeX(tag);
    }
    for(const auto& tag : m_actionCodeKTags)
    {
        this->applyActionCodeK(tag);
    }
    for(const auto& tag : m_actionCodeCTags)
    {
        this->applyActionCodeC(tag);
    }
    for(const auto& tag : m_actionCodeUTags)
    {
        this->applyActionCodeU(tag);
    }

    auto applyActionCodeXWithException = [this](const ::gdcm::Tag& tag)
                                         {
                                             if(m_exceptionTagMap.find(tag) == m_exceptionTagMap.end())
                                             {
                                                 this->applyActionCodeX(tag);
                                             }
                                         };

    const auto dataElementSet = dataset.GetDES();

    // Curve Data (0x50xx,0xxxxx)
    auto element = dataElementSet.lower_bound(::gdcm::Tag(0x5000, 0x0));
    auto end     = dataElementSet.upper_bound(::gdcm::Tag(0x50ff, 0xffff));
    for(; element != end; ++element)
    {
        tag = element->GetTag();
        applyActionCodeXWithException(tag);
    }

    // Overlay Data (0x60xx,0x3000) && Overlay Comments (0x60xx,0x4000)
    element = dataElementSet.lower_bound(::gdcm::Tag(0x6000, 0x3000));
    end     = dataElementSet.upper_bound(::gdcm::Tag(0x60ff, 0x4000));
    for(; element != end; ++element)
    {
        tag = element->GetTag();
        if(tag.GetElement() == 0x3000 || tag.GetElement() == 0x4000)
        {
            applyActionCodeXWithException(tag);
        }
    }

    m_anonymizer.RemovePrivateTags();   // Private attributes (X)

    for(const ::gdcm::DataElement& de : preservedTags)
    {
        dataset.Insert(de);
    }

    // Write file
    ::gdcm::Writer writer;
    writer.SetStream(outputStream);
    writer.SetFile(datasetFile);

    FW_RAISE_IF("Unable to anonymize (file write failed)", !writer.Write());
}

//------------------------------------------------------------------------------

void DicomAnonymizer::addExceptionTag(uint16_t group, uint16_t element, const std::string& value)
{
    ::gdcm::Tag tag(group, element);

    m_exceptionTagMap[tag] = value;

    m_actionCodeDTags.erase(tag);
    m_actionCodeZTags.erase(tag);
    m_actionCodeXTags.erase(tag);
    m_actionCodeKTags.erase(tag);
    m_actionCodeCTags.erase(tag);
    m_actionCodeUTags.erase(tag);
}

//------------------------------------------------------------------------------

void DicomAnonymizer::preservePrivateTag(const ::gdcm::Tag& tag)
{
    const bool found = std::find(m_privateTags.begin(), m_privateTags.end(), tag) != m_privateTags.end();
    OSLM_WARN_IF("Private tag " << tag.GetGroup() << ", " << tag.GetElement() << " has already been added", !found);

    if(!found)
    {
        m_privateTags.push_back(tag);
    }
}

//------------------------------------------------------------------------------

void DicomAnonymizer::applyActionCodeD(const ::gdcm::Tag& tag)
{
    // Sequence of Items
    if(m_publicDictionary.GetDictEntry(tag).GetVR() == ::gdcm::VR::SQ)
    {
        m_anonymizer.Empty(tag);
    }
    else
    {
        this->generateDummyValue(tag);
    }
}

//------------------------------------------------------------------------------

void DicomAnonymizer::applyActionCodeZ(const ::gdcm::Tag& tag)
{
    this->applyActionCodeD(tag);
}

//------------------------------------------------------------------------------

void DicomAnonymizer::applyActionCodeX(const ::gdcm::Tag& tag)
{
    m_anonymizer.Remove(tag);
}

//------------------------------------------------------------------------------

void DicomAnonymizer::applyActionCodeK(const ::gdcm::Tag& tag)
{
    // Sequence of Items
    if(m_publicDictionary.GetDictEntry(tag).GetVR() == ::gdcm::VR::SQ)
    {
        m_anonymizer.Empty(tag);
    }
}

//------------------------------------------------------------------------------

void DicomAnonymizer::applyActionCodeC(const ::gdcm::Tag& tag)
{
    SLM_FATAL("Basic profile \"C\" is not supported yet: "
              "Only basic profile is supported by the current implementation.");
}

//------------------------------------------------------------------------------

void DicomAnonymizer::applyActionCodeU(const ::gdcm::Tag& tag)
{
    const std::string oldUID = m_stringFilter.ToString(tag);
    if(!oldUID.empty())
    {
        auto it = m_uidMap.find(oldUID);
        std::string uid;

        if(it == m_uidMap.end())
        {
            uid = GENERATOR.Generate();
            m_uidMap.insert(std::pair<std::string, std::string>(oldUID, uid));
        }
        else
        {
            uid = it->second;
        }

        m_anonymizer.Replace(tag, uid.c_str());
    }
}

//------------------------------------------------------------------------------

void DicomAnonymizer::addShiftDateTag(const ::gdcm::Tag& tag)
{
    m_actionCodeDTags.erase(tag);
    m_actionCodeZTags.erase(tag);
    m_actionCodeXTags.erase(tag);
    m_actionCodeKTags.erase(tag);
    m_actionCodeCTags.erase(tag);
    m_actionCodeUTags.erase(tag);

    m_actionShiftDateTags.insert(tag);
}

//------------------------------------------------------------------------------

void DicomAnonymizer::applyActionShiftDate(const ::gdcm::Tag& tag)
{
    const std::string oldDate           = m_stringFilter.ToString(tag);
    const ::boost::gregorian::date date = ::boost::gregorian::from_undelimited_string(oldDate);

    const auto shift = date - m_referenceDate;

    //Minimum date
    const ::boost::gregorian::date min_date = ::boost::gregorian::from_undelimited_string(c_MIN_DATE_STRING);

    const auto shiftedDate = min_date + shift;

    m_anonymizer.Replace(tag, ::boost::gregorian::to_iso_string(shiftedDate).c_str());
}

//------------------------------------------------------------------------------

void DicomAnonymizer::generateDummyValue(const ::gdcm::Tag& tag)
{

    switch (m_publicDictionary.GetDictEntry(tag).GetVR())
    {
        case ::gdcm::VR::AE:
        {
            m_anonymizer.Replace(tag, "ANONYMIZED");
            break;
        }
        case ::gdcm::VR::AS:
        {
            m_anonymizer.Replace(tag, "000Y");
            break;
        }
        case ::gdcm::VR::AT:
        {
            m_anonymizer.Replace(tag, "00H,00H,00H,00H");
            break;
        }
        case ::gdcm::VR::CS:
        {
            // Patient's sex
            if(tag == ::gdcm::Tag(0x0010, 0x0040))
            {
                m_anonymizer.Replace(tag, "O");
            }
            else
            {
                m_anonymizer.Replace(tag, "ANONYMIZED");
            }
            break;
        }
        case ::gdcm::VR::DA:
        {
            m_anonymizer.Replace(tag, c_MIN_DATE_STRING.c_str());
            break;
        }
        case ::gdcm::VR::DS:
        {
            m_anonymizer.Replace(tag, "0");
            break;
        }
        case ::gdcm::VR::DT:
        {
            m_anonymizer.Replace(tag, std::string(c_MIN_DATE_STRING + "000000.000000").c_str());
            break;
        }
        case ::gdcm::VR::FD:
        {
            m_anonymizer.Replace(tag, "0");
            break;
        }
        case ::gdcm::VR::FL:
        {
            m_anonymizer.Replace(tag, "0");
            break;
        }
        case ::gdcm::VR::IS:
        {
            m_anonymizer.Replace(tag, "0");
            break;
        }
        case ::gdcm::VR::LO:
        {
            m_anonymizer.Replace(tag, "ANONYMIZED");
            break;
        }
        case ::gdcm::VR::LT:
        {
            m_anonymizer.Replace(tag, "ANONYMIZED");
            break;
        }
        case ::gdcm::VR::OB:
        {
            m_anonymizer.Replace(tag, "00H00H");
            break;
        }
        case ::gdcm::VR::OF:
        {
            m_anonymizer.Replace(tag, "0");
            break;
        }
        case ::gdcm::VR::OW:
        {
            m_anonymizer.Replace(tag, "0");
            break;
        }
        case ::gdcm::VR::PN:
        {
            m_anonymizer.Replace(tag, "ANONYMIZED^ANONYMIZED");
            break;
        }
        case ::gdcm::VR::SH:
        {
            m_anonymizer.Replace(tag, "ANONYMIZED");
            break;
        }
        case ::gdcm::VR::SL:
        {
            m_anonymizer.Replace(tag, "0");
            break;
        }
        case ::gdcm::VR::SQ:
        {
            m_anonymizer.Empty(tag);
            break;
        }
        case ::gdcm::VR::SS:
        {
            m_anonymizer.Replace(tag, "0");
            break;
        }
        case ::gdcm::VR::ST:
        {
            m_anonymizer.Replace(tag, "ANONYMIZED");
            break;
        }
        case ::gdcm::VR::TM:
        {
            m_anonymizer.Replace(tag, "000000.000000");
            break;
        }
        case ::gdcm::VR::UI:
        {
            m_anonymizer.Replace(tag, "ANONYMIZED");
            break;
        }
        case ::gdcm::VR::UL:
        {
            m_anonymizer.Replace(tag, "0");
            break;
        }
        case ::gdcm::VR::UN:
        {
            m_anonymizer.Replace(tag, "ANONYMIZED");
            break;
        }
        case ::gdcm::VR::US:
        {
            m_anonymizer.Replace(tag, "0");
            break;
        }
        case ::gdcm::VR::UT:
        {
            m_anonymizer.Replace(tag, "ANONYMIZED");
            break;
        }
        default:
        {
            OSLM_ERROR(tag<<" is not supported. Emptied value. ");
            m_anonymizer.Empty(tag);
            break;
        }
    }
}

//------------------------------------------------------------------------------

void DicomAnonymizer::copyDirectory(const std::filesystem::path& input,
                                    const std::filesystem::path& output)
{
    namespace fs = std::filesystem;
    std::error_code ec;
    fs::copy(input, output, fs::copy_options::overwrite_existing | fs::copy_options::recursive, ec);
    FW_RAISE_IF("copy_directory " << input.string() << " " << output.string()
                                  << " error : " << ec.message(), ec.value());

    fs::directory_iterator it(input);
    fs::directory_iterator end;

    ec.clear();
    fs::permissions(output, fs::perms::owner_all, ec);
    SLM_ERROR_IF("set " + output.string() + " permission error : " + ec.message(), ec.value());

    for(; it != end; ++it)
    {
        fs::path dest = output / it->path().filename();
        if(fs::is_directory(*it))
        {
            DicomAnonymizer::copyDirectory(*it, dest);
        }
        else
        {
            // Use stream instead of boost::copy_file (Unix c++11 issue)
            std::ifstream inStream(it->path(), std::ios::binary);
            FW_RAISE_IF("Unable to read file :" << it->path().string(), !inStream.good());
            std::ofstream outStream(dest, std::ios::binary);
            FW_RAISE_IF("Unable to write file :" << dest.string(), !outStream.good());

            outStream << inStream.rdbuf();

            inStream.close();
            outStream.close();
        }

        ec.clear();
        fs::permissions(dest, fs::perms::owner_all, ec);
        SLM_ERROR_IF("set " + dest.string() + " permission error : " + ec.message(), ec.value());
    }
}

//------------------------------------------------------------------------------

SPTR(::fwJobs::IJob) DicomAnonymizer::getJob() const
{
    return m_observer;
}
//------------------------------------------------------------------------------

} // namespace helper
} // namespace fwGdcmIO
