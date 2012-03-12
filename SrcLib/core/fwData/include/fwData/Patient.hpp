/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef FWDATA_PATIENT_H
#define FWDATA_PATIENT_H

#include <vector>
#include <iostream>
#include <ostream>
#include <map>

#include <boost/cstdint.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "fwData/config.hpp"
#include "fwData/Object.hpp"
#include "fwData/Study.hpp"
#include "fwData/Composite.hpp"
#include "fwData/Factory.hpp"

namespace fwData
{

/** \class Patient
 *********************
 * Namespace : data
 *********************
 * @brief This class defines a medical firmware
 * object. Patient represents fundamental
 * object in the patient folder. This class
 * defines patient's name, sex, Birthdate ..
 * This class contains patient's studies.
 *
 * @see ::fwData::Study
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 */
class FWDATA_CLASS_API Patient : public Object
{

public:
    fwCoreClassDefinitionsWithFactoryMacro( (Patient)(::fwData::Object), (()), ::fwData::Factory::New< Patient >) ;

    fwDataObjectMacro();

    /// Defines shallow copy
    FWDATA_API void shallowCopy( Patient::csptr _source );

    /// Defines deep copy
    FWDATA_API void deepCopy( Patient::csptr _source );

    typedef std::vector < ::fwData::Study::sptr > StudyContainerType;

    /**
     * @brief Get/Set the study container
     */
    fwDataGetSetCRefMacro(Studies, StudyContainerType);

    /**
     * @brief Get the number of studies
     * @return number of studies
     */
    FWDATA_API StudyContainerType::size_type getNumberOfStudies() const;

    /**
     * @brief add a study
     * @param[in] _study ::fwData::Study::sptr
     */
    FWDATA_API void addStudy( ::fwData::Study::sptr _study );

    /**
     * @brief Get/Set the ToolBox container
     */
    fwDataGetSetSptrMacro(ToolBox, ::fwData::Composite::sptr);

    /**
     * @brief Get/Set the scenario container
     */
    fwDataGetSetSptrMacro(Scenarios, ::fwData::Composite::sptr);

    /**
     * @brief add/get a scenario
     * @{
     */
    FWDATA_API void addScenario( std::string _name, ::fwData::Object::sptr _scenario );
    FWDATA_API ::fwData::Object::sptr getScenario( std::string _name );
    /** @} */

    /**
     * @brief add tool in the toolBox
     * @param[in] _name tool identifier
     * @param[in] _tool ::fwData::Object::sptr
     */
    FWDATA_API void addTool( std::string _name, ::fwData::Object::sptr _tool );

    /**
     * @brief get tool from the toolBox
     * @param[in] _name tool identifier
     * @return the tool
     */
    FWDATA_API ::fwData::Object::sptr getTool( std::string _name );

    // Generator result---------------------------------------------------------

    fwGettersSettersDocMacro(Name, sName, std::string, name of the patient);

    fwGettersSettersDocMacro(Firstname, sFirstname, std::string, fisrtname of the patient );

    fwGettersSettersDocMacro(IDDicom, sIDDicom, std::string, DICOM identifiant (eg : 01254312));

    fwGettersSettersDocMacro(Birthdate, sBirthdate, ::boost::posix_time::ptime, birthdate of the patient (eg : 19771010) );

    fwGettersSettersDocMacro(IsMale, bIsMale, bool, if the patient is male (true if is male) );

    fwGettersSettersDocMacro(DbID, i32DbID, ::boost::int32_t, database identifier );

protected:

    /// Constructor
    FWDATA_API Patient ();
    /// Destructor
    FWDATA_API virtual ~Patient ();

    //! Patient's name
    std::string m_sName;

    //! Patient's first name
    std::string m_sFirstname;

    //! DICOM identifiant (eg : 01254312)
    std::string m_sIDDicom;

    //! Birthdate (eg : 19771010)
    ::boost::posix_time::ptime m_sBirthdate;

    //! true if the patient is male, false if not
    bool m_bIsMale;

    /*
     **************
     * USED IN FWDATABASE
     **************
     */

    //! Database indentifier
    ::boost::int32_t  m_i32DbID;

    StudyContainerType m_attrStudies;
    ::fwData::Composite::sptr m_attrToolBox;
    ::fwData::Composite::sptr m_attrScenarios;
};

}//end namespace fwData

#endif // FWDATA_PATIENT_H


