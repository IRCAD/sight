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
 * @author  IRCAD (Research and Development Team).
 * @date    2007-20011.
 */

class FWDATA_CLASS_API StructureTraitsHelper
{

public :

    typedef ::boost::bimaps::bimap< ::fwData::StructureTraits::StructureClass, std::string > ClassTranslatorType;
    FWDATA_API static const ClassTranslatorType s_CLASSTRANSLATOR;

    typedef ::boost::bimaps::bimap< ::fwData::StructureTraits::Category, std::string > CategoryTranslatorType;
    FWDATA_API static const CategoryTranslatorType s_CATEGORYTRANSLATOR;

};

} // namespace fwData

#endif // __FWDATA_STRUCTURETRAITSHELPER_HPP__

