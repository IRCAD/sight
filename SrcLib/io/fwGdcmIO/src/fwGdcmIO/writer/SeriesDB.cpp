/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "fwGdcmIO/writer/SeriesDB.hpp"

#include <fwCore/base.hpp>

#include <fwDataIO/writer/registry/macros.hpp>

#include <fwMedData/ModelSeries.hpp>

#include <boost/filesystem.hpp>

fwDataIOWriterRegisterMacro(::fwGdcmIO::writer::SeriesDB);

namespace fwGdcmIO
{

namespace writer
{

//------------------------------------------------------------------------------

SeriesDB::SeriesDB(::fwDataIO::writer::IObjectWriter::Key key) :
    ::fwData::location::enableFolder< ::fwDataIO::writer::IObjectWriter >(this),
    m_fiducialsExportMode(::fwGdcmIO::writer::Series::SPATIAL_FIDUCIALS)
{
}

//------------------------------------------------------------------------------

SeriesDB::~SeriesDB()
{
}

//------------------------------------------------------------------------------

void SeriesDB::write()
{
//    // Disable GDCM Warnings
//    ::gdcm::Trace::SetWarning(false);

    ::fwMedData::SeriesDB::csptr seriesDB = this->getConcreteObject();
    SLM_ASSERT("SeriesDB not instanced", seriesDB);

    ::fwGdcmIO::writer::Series::sptr writer = ::fwGdcmIO::writer::Series::New();
    writer->setFiducialsExportMode(m_fiducialsExportMode);

    // Copy and sort container in order to write ImageSeries before ModelSeries
    ::fwMedData::SeriesDB::ContainerType seriesContainer = seriesDB->getContainer();
    std::sort(seriesContainer.begin(), seriesContainer.end(), SeriesDB::seriesComparator);

    // Write all patients
    for( ::fwMedData::Series::sptr series : seriesContainer)
    {
        // Create a new directory
        const ::boost::filesystem::path& seriesPath = this->getFolder() / series->getInstanceUID();
        ::boost::filesystem::create_directories(seriesPath);
        writer->setObject(series);
        writer->setFolder(seriesPath);

        // Forward event progress to its parents
        ::fwTools::ProgressAdviser::ProgessHandler handler =
            std::bind( &Series::notifyProgress, this, ::std::placeholders::_1, ::std::placeholders::_2);
        writer->addHandler(handler);

        // Write a series
        writer->write();
    }
}

//------------------------------------------------------------------------------

std::string SeriesDB::extension()
{
    return std::string("");
}

//------------------------------------------------------------------------------

bool SeriesDB::seriesComparator(const ::fwMedData::Series::csptr& a,
                                const ::fwMedData::Series::csptr& b)
{
    ::fwMedData::ModelSeries::csptr ma = ::fwMedData::ModelSeries::dynamicCast(a);
    ::fwMedData::ModelSeries::csptr mb = ::fwMedData::ModelSeries::dynamicCast(b);
    return (mb && !ma);
}

//------------------------------------------------------------------------------

} // namespace writer
} // namespace fwGdcmIO
