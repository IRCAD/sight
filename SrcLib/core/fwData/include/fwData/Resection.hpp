/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATA_RESECTION_HPP_
#define _FWDATA_RESECTION_HPP_

#include <vector>

#include "fwData/config.hpp"
#include "fwData/Object.hpp"
#include "fwData/Reconstruction.hpp"
#include "fwData/PlaneList.hpp"


namespace fwData
{
/**
 * @class       Resection
 * @brief       This class defines a resection
 * @author      IRCAD (Research and Development Team).
 * @date        2007-2009.
 */
class FWDATA_CLASS_API Resection : public ::fwData::Object
{

public :
        fwCoreClassDefinitionsWithFactoryMacro( (Resection)(::fwData::Object::Baseclass),
        (()), ::fwTools::Factory::New< Resection >) ;

        typedef std::vector< ::fwData::Reconstruction::sptr > ResectionInputs;
        typedef std::vector< ::fwData::Reconstruction::sptr > ResectionOutputs;

        /// Constructor
        FWDATA_API Resection();

        /// Destructor
        FWDATA_API virtual ~Resection();

        /// Clone method
        FWDATA_API Resection::sptr clone() const;

        /// Copy method
        FWDATA_API Resection &operator=( const Resection & _planetList ) ;

        /// PlaneList
        fwGettersSettersDocMacro(PlaneList, planeList, ::fwData::PlaneList::sptr, the plane list);
        /// Inputs
        fwGettersSettersDocMacro(Inputs, vInputs, ResectionInputs, the resection inputs);
        /// Outputs
        fwGettersSettersDocMacro(Outputs, vOutputs, ResectionOutputs, the resection outputs);

        fwGettersSettersDocMacro(IsSafePart, isSafePart, bool, the flag if the part is safe);

        fwGettersSettersDocMacro(Name, name, std::string, the resection name);

        fwGettersSettersDocMacro(IsVisible, isVisible, bool, the flag if the resection is visible);

        fwGettersSettersDocMacro(IsValid, isValid, bool, the flag if the resection is valid);

protected :
        //! Resection name
        std::string m_name;

        //! Planes list
        ::fwData::PlaneList::sptr m_planeList;

        //! Inputs (reconstructions)
        ResectionInputs m_vInputs;

        //! Outputs (reconstructions)
        ResectionOutputs m_vOutputs;

        //! flag if the part is safe
        bool m_isSafePart;

        //! flag if the resection is valid
        bool m_isValid;

        //! flag if the resection is visible
        bool m_isVisible;

}; // end class Resection

} // end namespace fwData

#endif // _FWDATA_RESECTION_HPP_
