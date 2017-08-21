/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UIMEDDATAQT_INSERTSERIES_HPP__
#define __UIMEDDATAQT_INSERTSERIES_HPP__

#include "uiMedDataQt/config.hpp"

#include <fwData/factory/new.hpp>

#include <fwMedData/Series.hpp>

namespace uiMedDataQt
{

/**
 * @brief Used as a placeholder in ::uiMedDataQt::editor::SSelector UI to insert create and insert new series in a
 * series
 * DB.
 */
class UIMEDDATAQT_CLASS_API InsertSeries : public ::fwMedData::Series
{

public:
    fwCoreClassDefinitionsWithFactoryMacro( (InsertSeries)(::fwData::Object), (()),
                                            ::fwData::factory::New< InsertSeries >);

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    UIMEDDATAQT_API InsertSeries(::fwData::Object::Key key);

    /// Destructor
    UIMEDDATAQT_API virtual ~InsertSeries();

    /// Defines shallow copy
    UIMEDDATAQT_API void shallowCopy( const ::fwData::Object::csptr& _source );

    /// Defines deep copy
    UIMEDDATAQT_API void cachedDeepCopy( const ::fwData::Object::csptr& _source, DeepCopyCacheType& cache );

};

}   //end namespace uiMedDataQt

#endif // __UIMEDDATAQT_INSERTSERIES_HPP__

