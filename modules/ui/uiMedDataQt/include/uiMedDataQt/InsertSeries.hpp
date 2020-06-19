/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#pragma once

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
    fwCoreClassMacro(InsertSeries, ::fwData::Object, ::fwData::factory::New< InsertSeries >);

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    UIMEDDATAQT_API InsertSeries(::fwData::Object::Key key);

    /// Destructor
    UIMEDDATAQT_API virtual ~InsertSeries();

    /// Defines shallow copy
    UIMEDDATAQT_API void shallowCopy( const ::fwData::Object::csptr& _source ) override;

    /// Defines deep copy
    UIMEDDATAQT_API void cachedDeepCopy( const ::fwData::Object::csptr& _source, DeepCopyCacheType& cache ) override;

};

}   //end namespace uiMedDataQt
