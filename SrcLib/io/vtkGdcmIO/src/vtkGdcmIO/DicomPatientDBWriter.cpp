/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>
#include <boost/bind.hpp>
#include <boost/lambda/lambda.hpp>

#include <fwTools/ClassRegistrar.hpp>

#include <fwCore/base.hpp>

#include "vtkGdcmIO/DicomPatientDBWriter.hpp"
#include "vtkGdcmIO/DicomPatientWriter.hpp"

REGISTER_BINDING_BYCLASSNAME( ::fwDataIO::writer::IObjectWriter , ::vtkGdcmIO::DicomPatientDBWriter, ::vtkGdcmIO::DicomPatientDBWriter );


namespace vtkGdcmIO
{
//------------------------------------------------------------------------------

DicomPatientDBWriter::DicomPatientDBWriter()
: ::fwData::location::enableFolder< ::fwDataIO::writer::IObjectWriter >(this)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

DicomPatientDBWriter::~DicomPatientDBWriter()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void DicomPatientDBWriter::write()
{
    assert( m_object.use_count() );
    assert( !m_object.expired() );
    assert( m_object.lock() );

    std::string patientDBPath =  this->getFolder().string();
    ::fwData::PatientDB::sptr dataPatientDB = getConcreteObject();

    SLM_ASSERT("dataPatientDB not instanced", dataPatientDB);
    int iPatient = 0;
    BOOST_FOREACH(::fwData::Patient::sptr pPatient, dataPatientDB->getPatients())
    {
        iPatient++;
        std::string tmp_path = patientDBPath;

        tmp_path += "/patient_" + ::fwTools::getString< int >(iPatient);// + pPatient->getName() + "_" + pPatient->getIDDicom();
        ::vtkGdcmIO::DicomPatientWriter::NewSptr myWriter;
        myWriter->setObject(pPatient);
        ::fwData::location::Folder::NewSptr loc;
        if ( ! ::boost::filesystem::exists( tmp_path ) )
        {
            ::boost::filesystem::create_directory( tmp_path );
        }

        loc->setFolder(tmp_path);
        myWriter->setLocation(loc);
        // forward event progress to its parents
        ::fwTools::ProgressAdviser::ProgessHandler handler = ::boost::bind( &DicomPatientDBWriter::notifyProgress,this, ::boost::lambda::_1, ::boost::lambda::_2);
        myWriter->addHandler ( handler );
        myWriter->write();
    }
}

//------------------------------------------------------------------------------

std::string  DicomPatientDBWriter::extension()
{
    return "";
}
} // namespace dicomIO
