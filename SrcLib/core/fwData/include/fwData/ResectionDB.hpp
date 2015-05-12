/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATA_RESECTIONDB_HPP__
#define __FWDATA_RESECTIONDB_HPP__

#include <boost/cstdint.hpp>

#include "fwData/config.hpp"
#include "fwData/factory/new.hpp"
#include "fwData/Resection.hpp"

fwCampAutoDeclareDataMacro((fwData)(ResectionDB), FWDATA_API);

namespace fwData
{
/**
 * @brief   This class defines a resection container.
 *
 * @see     ::fwData::Resection
 */
class FWDATA_CLASS_API ResectionDB : public Object
{

public:
    fwCoreClassDefinitionsWithFactoryMacro( (ResectionDB)(::fwData::Object), (()),
                                            ::fwData::factory::New< ResectionDB >);
    fwCampMakeFriendDataMacro((fwData)(ResectionDB));

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWDATA_API ResectionDB(::fwData::Object::Key key);

    /// Destructor
    FWDATA_API virtual ~ResectionDB();


    /// Defines shallow copy
    FWDATA_API void shallowCopy( const Object::csptr& _source );

    /// Defines deep copy
    FWDATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType &cache);

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

    fwGettersSettersDocMacro(SafeResection, safeResection, ::fwData::Resection::sptr,
                             "Get the safe part of the resections");

protected:

    ::fwData::Resection::sptr m_safeResection;

    ResectionContainerType m_attrResections;
};

} // namespace fwData

#endif // __FWDATA_RESECTIONDB_HPP__

