/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATA_STUDY_HPP_
#define _FWDATA_STUDY_HPP_

#include <vector>
#include <boost/cstdint.hpp>

#include "fwData/config.hpp"
#include "fwData/Object.hpp"
#include "fwData/Acquisition.hpp"
#include "fwData/DownCastIterator.hpp"

namespace fwData
{

/**
 * @class   Study
 * @brief   This class defines a medical firmware object. Study represents fundamental
 * object in the patient's study. This class defines study's hospital, modality,
 * acquisition zone..
 * a study contains acquisitions.
 *
 * @see     ::fwData::Acquisition
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 */
class FWDATA_CLASS_API Study : public Object
{


public:
    fwCoreClassDefinitionsWithFactoryMacro( (Study)(::fwData::Object), (()), ::fwTools::Factory::New< Study >) ;

    /// @brief Constructor
    FWDATA_API Study();
    /// @brief Destructor
    FWDATA_API virtual ~Study();


    fwDataObjectMacro();

    /// Defines shallow copy
    FWDATA_API void shallowCopy( Study::csptr _source );

    /// Defines deep copy
    FWDATA_API void deepCopy( Study::csptr _source );

    /// Field identifier for acquisitions
    FWDATA_API static const Object::FieldID ID_ACQUISITIONS;

    typedef ContainerCaster< Acquisition >::iterator         AcquisitionIterator;
    typedef ContainerCaster< Acquisition >::const_iterator AcquisitionConstIterator;

    /**
     * @brief Get the number of acquisitions
     * @return acquisition number
     */
    FWDATA_API ::boost::uint32_t  getAcquisitionSize() const;

    /**
     * add Acquisition
     */
    FWDATA_API void addAcquisition( ::fwData::Acquisition::sptr _acquisition );

    /**@{
     * Get iterator on the first and the last study. Use it to browse all acquisitons.
     * @return std::pair( study.begin(), study.end() )
     */
    FWDATA_API std::pair< AcquisitionIterator, AcquisitionIterator > getAcquisitions();
    FWDATA_API std::pair< AcquisitionConstIterator, AcquisitionConstIterator > getAcquisitions() const;
    //@}

    // Generator result---------------------------------------------------------
    fwGettersSettersDocMacro(Hospital, sHospital, std::string, the name of the hospital where the study is made. \n (eg : RADIOLOGIE URGENCE CHUV LAUSANNE) );

    fwGettersSettersDocMacro(Modality, sModality, std::string, the study modality. (MT or CT) );

    fwGettersSettersDocMacro(AcquisitionZone, sAcquisitionZone, std::string, the acquisition zone (eg : CT ABDOMEN COMPLET) );

    fwGettersSettersDocMacro(RISId, sRISId, std::string, the RIS Identifier );

    fwGettersSettersDocMacro(UID, sUID, std::string, the unique Identifier DICOM );

    fwGettersSettersDocMacro(DbID, i32DbID, ::boost::int32_t, the database indentifier );

    fwGettersSettersDocMacro(Date, date, std::string, the study date );
    fwGettersSettersDocMacro(Time, time, std::string, the study time );
    fwGettersSettersDocMacro(Description, description, std::string, the study description );

protected :


    /// Hospital name (eg : RADIOLOGIE URGENCE CHUV LAUSANNE)
    std::string m_sHospital;

    /// Acquisition modality (MT or CT)
    std::string m_sModality;

    /// Acquisition Zone (eg : CT ABDOMEN COMPLET)
    std::string m_sAcquisitionZone;

    /// The RIS Id
    std::string m_sRISId;

    /// Unique Identifier DICOM
    std::string m_sUID;

    /// Database indentifier
    ::boost::int32_t  m_i32DbID;

    std::string m_date;
    std::string m_time;
    std::string m_description;

};

} // namespace fwData

#endif // _FWDATA_STUDY_HPP_
