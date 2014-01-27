/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>
#include <boost/serialization/binary_object.hpp>

#include <stdio.h>

#include <fwComEd/helper/Array.hpp>

#include <fwCore/base.hpp>

#include <fwDicomData/DicomSeries.hpp>

#include <fwDataIO/writer/registry/macros.hpp>

#include "fwDicomIOExt/gdcm/DicomSeriesWriter.hpp"

fwDataIOWriterRegisterMacro( ::fwDicomIOExt::gdcm::DicomSeriesWriter );

namespace fwDicomIOExt
{
namespace gdcm
{

//------------------------------------------------------------------------------

DicomSeriesWriter::DicomSeriesWriter(::fwDataIO::writer::IObjectWriter::Key key) :
        ::fwData::location::enableFolder< ::fwDataIO::writer::IObjectWriter >(this)
{
}

//------------------------------------------------------------------------------

void DicomSeriesWriter::write()
{
    ::fwDicomData::DicomSeries::sptr dicomSeries = this->getConcreteObject();

    FW_RAISE_IF("Dicom series should contain binaries.",
            dicomSeries->getDicomAvailability() != ::fwDicomData::DicomSeries::BINARIES);

    // Create folder
    ::boost::filesystem::path folder = this->getFolder().string() +"/";
    if (!::boost::filesystem::exists(folder))
    {
        ::boost::filesystem::create_directories(folder);
    }

    unsigned int nbInstances = dicomSeries->getNumberOfInstances();
    unsigned int count = 0;
    // Write binary files
    BOOST_FOREACH(::fwDicomData::DicomSeries::DicomBinaryContainerType::value_type value, dicomSeries->getDicomBinaries())
    {
        std::string filename = folder.string() + value.first;

        ::fwData::Array::sptr array = value.second;
        ::fwComEd::helper::Array arrayHelper(array);

        FILE* file = 0;
        file = fopen(filename.c_str(), "wb");
        FW_RAISE_IF("Unable to create files.", !file);

        char* buffer = (char*)arrayHelper.getBuffer();
        unsigned long long size = array->getSizeInBytes();
        fwrite(buffer, sizeof(char), size, file);

        fclose(file);

        this->notifyProgress(float(++count)/nbInstances, "Saving Dicom files.");

    }

}

//------------------------------------------------------------------------------

std::string  DicomSeriesWriter::extension()
{
    return "";
}

} // namespace gdcm
} // namespace fwDicomIOExt
