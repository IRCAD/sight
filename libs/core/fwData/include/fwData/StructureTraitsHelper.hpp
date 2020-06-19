/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

#ifndef __FWDATA_STRUCTURETRAITSHELPER_HPP__
#define __FWDATA_STRUCTURETRAITSHELPER_HPP__

#include "fwData/config.hpp"
#include "fwData/StructureTraits.hpp"

#include <boost/bimap/bimap.hpp>

namespace fwData
{

/**
 * @brief This class defines some helper for the structure traits.
 */
class FWDATA_CLASS_API StructureTraitsHelper
{

public:

    typedef ::boost::bimaps::bimap< ::fwData::StructureTraits::StructureClass, std::string > ClassTranslatorType;
    /// Map to translate structure class (string vs enum)
    FWDATA_API static const ClassTranslatorType s_CLASSTRANSLATOR;

    typedef ::boost::bimaps::bimap< ::fwData::StructureTraits::Category, std::string > CategoryTranslatorType;
    /// Map to translate structure categories (string vs enum)
    FWDATA_API static const CategoryTranslatorType s_CATEGORYTRANSLATOR;

};

} // namespace fwData

#endif // __FWDATA_STRUCTURETRAITSHELPER_HPP__

