/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATA_RESECTIONDB_HPP_
#define _FWDATA_RESECTIONDB_HPP_


#include <vector>
#include <boost/cstdint.hpp>

// export/visibility
#include "fwData/config.hpp"

#include "fwData/Object.hpp"
#include "fwData/Resection.hpp"
#include "fwData/DownCastIterator.hpp"

namespace fwData
{
/**
 * @class       ResectionDB
 * @brief       This class defines a resection container.
 *
 * @see         ::fwData::Resection
 * @author      IRCAD (Research and Development Team).
 * @date        2007-2009.
 */
class FWDATA_CLASS_API ResectionDB : public Object
{

public:
        fwCoreClassDefinitionsWithFactoryMacro( (ResectionDB)(::fwData::Object::Baseclass), (()), ::fwTools::Factory::New< ResectionDB >) ;

        /// Constructor
        FWDATA_API ResectionDB ();

        /// Destructor
        FWDATA_API virtual ~ResectionDB ();

        /// Copy method
        FWDATA_API ResectionDB &operator=( const ResectionDB & _resectionDB ) ;

        // Patients ----------------------------------------------------------------
        /// Field identifier for patients
        FWDATA_API static const Object::FieldID ID_RESECTIONS;

        typedef ContainerCaster< Resection >::iterator                  ResectionIterator;
        typedef ContainerCaster< Resection >::const_iterator    ResectionConstIterator;

        /**
         * @brief Get the number of resections
         */
        FWDATA_API boost::uint32_t  getResectionSize() const;

        /**
         * @brief add resection
         */
        FWDATA_API void addResection( ::fwData::Resection::sptr _resection );

        /**@{
         * @brief Get iterator on the first and the last resection. Use it to browse all resections.
         * @return std::pair( patient.begin(), patient.end() )
         */
        FWDATA_API std::pair< ResectionIterator, ResectionIterator > getResections();
        FWDATA_API std::pair< ResectionConstIterator, ResectionConstIterator > getResections() const;
        //@}

        fwGettersSettersDocMacro(SafeResection, safeResection, ::fwData::Resection::sptr, Get the safe part of the resections);

protected:
        ::fwData::Resection::sptr m_safeResection;
};

}//end namespace fwData

#endif // _FWDATA_RESECTIONDB_HPP_


