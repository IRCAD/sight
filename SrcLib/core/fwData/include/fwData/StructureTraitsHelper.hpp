/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATA_STRUCTURETRAITSHELPER_HPP__
#define __FWDATA_STRUCTURETRAITSHELPER_HPP__

#include <boost/bimap/bimap.hpp>

#include <fwData/StructureTraits.hpp>

#include "fwData/config.hpp"

namespace fwData
{
/**
 * @class   StructureTraitsHelper
 * @brief   This class defines some helper for the structure traits.
 * 
 * @date    2007-20011.
 */

class FWDATA_CLASS_API StructureTraitsHelper
{

public :

    typedef ::boost::bimaps::bimap< ::fwData::StructureTraits::StructureClass, std::string > ClassTranslatorType;
    /// Map to translate structure class (string vs enum)
    FWDATA_API static const ClassTranslatorType s_CLASSTRANSLATOR;

    typedef ::boost::bimaps::bimap< ::fwData::StructureTraits::Category, std::string > CategoryTranslatorType;
    /// Map to translate structure categories (string vs enum)
    FWDATA_API static const CategoryTranslatorType s_CATEGORYTRANSLATOR;

};

} // namespace fwData

#endif // __FWDATA_STRUCTURETRAITSHELPER_HPP__

