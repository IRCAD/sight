/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _UIPATIENTDB_SPATIENTDBINSERTER_HPP_
#define _UIPATIENTDB_SPATIENTDBINSERTER_HPP_

// Service associated data
#include <fwData/PatientDB.hpp>
#include <fwData/Patient.hpp>
#include <fwData/Study.hpp>
#include <fwData/Acquisition.hpp>
#include <fwData/Image.hpp>

#include <io/IWriter.hpp>

#include "uiPatientDB/config.hpp"

namespace uiPatientDB
{

/**
 * @brief   A writer to push associated object in a patientDB
 * @note    Associated object must be a patientDB, patient, study, acquisition or image.
 * @class   SPatientDBInserter.
 * @author  IRCAD (Research and Development Team).
 * @date    2012.
 */
class UIPATIENTDB_CLASS_API SPatientDBInserter : public ::io::IWriter
{

public:

    fwCoreServiceClassDefinitionsMacro ( (SPatientDBInserter)(::io::IWriter) ) ;

    /**
     * @brief Overrides
     @verbatim
     <service ... >
        <patientDB fwID="myPatientDB" />
     </service>
     @endverbatim
     *
     * Set the object uid of PatientDB.
     */
    UIPATIENTDB_API virtual void configuring() throw ( ::fwTools::Failed );

    UIPATIENTDB_API void configureWithIHM();

    /// Overrides, does nothing
    UIPATIENTDB_API virtual void starting() throw ( ::fwTools::Failed );

    /// Overrides, does nothing
    UIPATIENTDB_API virtual void stopping() throw ( ::fwTools::Failed );

    /// Create a patient from associated object (patientDB, patient, study, acquisition or image), and push it in the patientDB
    UIPATIENTDB_API virtual void updating() throw ( ::fwTools::Failed );

    /// Overrides, does nothing
    UIPATIENTDB_API virtual void updating( ::fwServices::ObjectMsg::csptr _msg ) throw ( ::fwTools::Failed );

    /// Just call stopping/starting
    UIPATIENTDB_API virtual void swapping() throw ( ::fwTools::Failed );

protected :

    /// Constructor
    UIPATIENTDB_API SPatientDBInserter();

    /// Destructor
    UIPATIENTDB_API virtual ~SPatientDBInserter() throw();

private :

    /**
     * @brief Retrieves selected image, if we have not selected image or if the
     * buffer is null, the method returns null sptr
     */
    ::fwData::PatientDB::sptr getPatientDB() const;

    /// Create a PDB from patient
    ::fwData::PatientDB::sptr createPDB(::fwData::Patient::sptr patient) const;
    /// Create a PDB from study
    ::fwData::PatientDB::sptr createPDB(::fwData::Study::sptr study) const;
    /// Create a PDB from acquisition
    ::fwData::PatientDB::sptr createPDB(::fwData::Acquisition::sptr acq) const;
    /// Create a PDB from image
    ::fwData::PatientDB::sptr createPDB(::fwData::Image::sptr img) const;

    /// PatientDB fwID
    std::string m_patientDBfwID;
};

} // namespace uiPatientDB

#endif /*_UIPATIENTDB_SPATIENTDBINSERTER_HPP_*/

