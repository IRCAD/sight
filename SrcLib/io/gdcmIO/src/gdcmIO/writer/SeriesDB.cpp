/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <boost/bind.hpp>           // for ProgessHandler
#include <boost/lambda/lambda.hpp>  // for ProgessHandler

#include <fwCore/base.hpp>

#include <fwTools/Stringizer.hpp>

#include <fwMedData/Series.hpp>

#include <fwDataIO/writer/registry/macros.hpp>

#include "gdcmIO/writer/SeriesDB.hpp"
#include "gdcmIO/writer/Series.hpp"


fwDataIOWriterRegisterMacro( ::gdcmIO::writer::SeriesDB );


namespace gdcmIO
{

namespace writer
{

//------------------------------------------------------------------------------

SeriesDB::SeriesDB(::fwDataIO::writer::IObjectWriter::Key key):
        ::fwData::location::enableFolder< ::fwDataIO::writer::IObjectWriter >(this)
{
}

//------------------------------------------------------------------------------

SeriesDB::~SeriesDB()
{
}

//------------------------------------------------------------------------------

void SeriesDB::write()
{
    ::fwMedData::SeriesDB::sptr seriesDB = this->getConcreteObject();
    SLM_ASSERT("dataPatientDB not instanced", seriesDB);

    // Used to set name folder of the current patient.
    size_t seriesID  = 1;
    ::gdcmIO::writer::Series::sptr writer = ::gdcmIO::writer::Series::New();

    // Write all patients
    BOOST_FOREACH( ::fwMedData::Series::sptr series, seriesDB->getContainer())
    {
        // Make a new directory
        const ::boost::filesystem::path& seriesPath =
                this->getFolder() / (std::string("series_") + ::fwTools::getString<size_t>(seriesID));
        ::boost::filesystem::create_directory(seriesPath);
        writer->setObject(series);
        writer->setFolder(seriesPath);

        // forward event progress to its parents
        ::fwTools::ProgressAdviser::ProgessHandler handler =
                ::boost::bind( &Series::notifyProgress, this, ::boost::lambda::_1, ::boost::lambda::_2);
        writer->addHandler (handler);

        // Write one patient
        writer->write();
        seriesID++;
    }
}

//------------------------------------------------------------------------------

std::string SeriesDB::extension()
{
    return std::string("");
};

}   // namespace writer

}   // namespace gdcmIO
