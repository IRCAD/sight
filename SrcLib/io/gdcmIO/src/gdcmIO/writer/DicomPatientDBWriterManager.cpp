/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2012-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>
#include <boost/bind.hpp>           // for ProgessHandler
#include <boost/lambda/lambda.hpp>  // for ProgessHandler

#include <fwCore/base.hpp>

#include <fwTools/ClassRegistrar.hpp>

#include "gdcmIO/writer/DicomPatientDBWriterManager.hpp"
#include "gdcmIO/writer/DicomGlobalWriterManager.hpp"


REGISTER_BINDING_BYCLASSNAME( ::fwDataIO::writer::IObjectWriter , ::gdcmIO::writer::DicomPatientDBWriterManager, ::gdcmIO::writer::DicomPatientDBWriterManager );


namespace gdcmIO
{

namespace writer
{

//------------------------------------------------------------------------------

DicomPatientDBWriterManager::DicomPatientDBWriterManager():
        ::fwData::location::enableFolder< ::fwDataIO::writer::IObjectWriter >(this)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

DicomPatientDBWriterManager::~DicomPatientDBWriterManager()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void DicomPatientDBWriterManager::write()
{
    SLM_TRACE_FUNC();

    ::fwData::PatientDB::sptr dataPatientDB = this->getConcreteObject();
    SLM_ASSERT("dataPatientDB not instanced", dataPatientDB);

    // Init
    unsigned int                patientID       = 1;    // Used to set name folder of the current patient.
    const std::string &         patientDBPath   = this->getFolder().string();
    DicomGlobalWriterManager::NewSptr    myWriter;

    // Write all patients
    BOOST_FOREACH( ::fwData::Patient::sptr patient, dataPatientDB->getPatients())
    {
        myWriter->setObject( patient );
        myWriter->setFolder( patientDBPath );
        myWriter->setPatientID( patientID++ );

        // forward event progress to its parents
        ::fwTools::ProgressAdviser::ProgessHandler handler = ::boost::bind( &DicomGlobalWriterManager::notifyProgress,this, ::boost::lambda::_1, ::boost::lambda::_2);
        myWriter->addHandler ( handler );

        // Write one patient
        myWriter->write();
    }
}

//------------------------------------------------------------------------------

std::string DicomPatientDBWriterManager::extension()
{
    return std::string("");
};

}   // namespace writer

}   // namespace gdcmIO
