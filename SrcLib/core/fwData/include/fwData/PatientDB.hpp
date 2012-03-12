/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATA_PATIENTDB_HPP_
#define _FWDATA_PATIENTDB_HPP_


#include <vector>
#include <boost/cstdint.hpp>


#include "fwData/config.hpp"
#include "fwData/Object.hpp"
#include "fwData/Factory.hpp"
#include "fwData/Patient.hpp"

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
    fwCoreClassDefinitionsWithFactoryMacro( (PatientDB)(::fwData::Object), (()), ::fwData::Factory::New< PatientDB >) ;

    fwDataObjectMacro();

    /// Defines shallow copy
    FWDATA_API void shallowCopy( PatientDB::csptr _source );

    /// Defines deep copy
    FWDATA_API void deepCopy( PatientDB::csptr _source );

    typedef std::vector< ::fwData::Patient::sptr > PatientContainerType;

    /**
     * @brief Get/Set patients
     */
    fwDataGetSetCRefMacro(Patients, PatientContainerType);

    /**
     * @brief Get the number of patients
     */
    FWDATA_API PatientContainerType::size_type getNumberOfPatients() const;

    /**
     * @brief add patient
     */
    FWDATA_API void addPatient( ::fwData::Patient::sptr _patient );

protected:
    /// Constructor
    FWDATA_API PatientDB ();

    /// Destructor
    FWDATA_API virtual ~PatientDB ();

    PatientContainerType m_attrPatients;
};

}//end namespace fwData

#endif // _FWDATA_PATIENTDB_HPP_


