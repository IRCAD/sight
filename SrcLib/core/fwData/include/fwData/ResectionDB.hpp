/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATA_RESECTIONDB_HPP_
#define _FWDATA_RESECTIONDB_HPP_

#include <boost/cstdint.hpp>

#include "fwData/config.hpp"
#include "fwData/Factory.hpp"
#include "fwData/Resection.hpp"

namespace fwData
{
/**
 * @class   ResectionDB
 * @brief   This class defines a resection container.
 *
 * @see     ::fwData::Resection
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 */
class FWDATA_CLASS_API ResectionDB : public Object
{

public:
    fwCoreClassDefinitionsWithFactoryMacro( (ResectionDB)(::fwData::Object), (()), ::fwData::Factory::New< ResectionDB >) ;

    fwDataObjectMacro();

    /// Defines shallow copy
    FWDATA_API void shallowCopy( ResectionDB::csptr _source );

    /// Defines deep copy
    FWDATA_API void deepCopy( ResectionDB::csptr _source );

    typedef std::vector< ::fwData::Resection::sptr > ResectionContainerType;

    /**
     * @brief Get the number of resections
     */
    FWDATA_API ResectionContainerType::size_type getNumberOfResections() const;

    /**
     * @brief add resection
     */
    FWDATA_API void addResection( ::fwData::Resection::sptr resection );

    fwDataGetSetCRefMacro(Resections, ResectionContainerType);

    fwGettersSettersDocMacro(SafeResection, safeResection, ::fwData::Resection::sptr, Get the safe part of the resections);

protected:

    /// Constructor
    FWDATA_API ResectionDB ();

    /// Destructor
    FWDATA_API virtual ~ResectionDB ();

    ::fwData::Resection::sptr m_safeResection;

    ResectionContainerType m_attrResections;
};

}//end namespace fwData

#endif // _FWDATA_RESECTIONDB_HPP_


