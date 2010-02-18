/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATA_PATIENTDB_HPP_
#define _FWDATA_PATIENTDB_HPP_


#include <vector>
#include <boost/cstdint.hpp>

// export/visibility
#include "fwData/config.hpp"

#include "fwData/Object.hpp"
#include "fwData/Patient.hpp"
#include "fwData/DownCastIterator.hpp"

namespace fwData
{
/**
 * @class   PatientDB
 * @brief   This class defines a patients container.
 *
 * @see     ::fwData::Patient
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 */
class FWDATA_CLASS_API PatientDB : public Object
{

public:
    fwCoreClassDefinitionsWithFactoryMacro( (PatientDB)(::fwData::Object::Baseclass), (()), ::fwTools::Factory::New< PatientDB >) ;

    /// Constructor
    FWDATA_API PatientDB ();

    /// Destructor
    FWDATA_API virtual ~PatientDB ();


    fwDataObjectMacro();

    /// Defines shallow copy
    FWDATA_API void shallowCopy( PatientDB::csptr _source );

    /// Defines deep copy
    FWDATA_API void deepCopy( PatientDB::csptr _source );

    /// Copy method
    //FWDATA_API PatientDB &operator=( const PatientDB & _patientDB ) ;

    // Patients ----------------------------------------------------------------
    /// Field identifier for patients
    FWDATA_API static const Object::FieldID ID_PATIENTS;

    typedef ContainerCaster< Patient >::iterator        PatientIterator;
    typedef ContainerCaster< Patient >::const_iterator  PatientConstIterator;

    /**
     * @brief Get the number of patients
     */
    FWDATA_API boost::uint32_t  getPatientSize() const;

    /**
     * @brief add patient
     */
    FWDATA_API void addPatient( ::fwData::Patient::sptr _patient );

    /**@{
     * @brief Get iterator on the first and the last patient. Use it to browse all patients.
     * @return std::pair( patient.begin(), patient.end() )
     */
    FWDATA_API std::pair< PatientIterator, PatientIterator > getPatients();
    FWDATA_API std::pair< PatientConstIterator, PatientConstIterator > getPatients() const;
    //@]

};

}//end namespace fwData

#endif // _FWDATA_PATIENTDB_HPP_


