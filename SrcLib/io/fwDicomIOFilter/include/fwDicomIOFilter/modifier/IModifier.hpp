/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDICOMIOFILTER_MODIFIER_IMODIFIER_HPP__
#define __FWDICOMIOFILTER_MODIFIER_IMODIFIER_HPP__

#include "fwDicomIOFilter/IFilter.hpp"
#include "fwDicomIOFilter/config.hpp"

#include <fwMedData/DicomSeries.hpp>

namespace fwDicomIOFilter
{
namespace modifier
{

/**
 * @brief Base class for Dicom instance modifier.
 */
class FWDICOMIOFILTER_CLASS_API IModifier : public ::fwDicomIOFilter::IFilter
{
public:
    fwCoreNonInstanciableClassDefinitionsMacro( (IModifier)(::fwDicomIOFilter::IFilter) );

    /// Destructor
    FWDICOMIOFILTER_API virtual ~IModifier();

    /// Return filter type
    FWDICOMIOFILTER_API FilterType getFilterType() const;

protected:
    /// Constructor
    FWDICOMIOFILTER_API IModifier();

};

} // namespace modifier
} // namespace fwDicomIOFilter


#endif /* __FWDICOMIOFILTER_MODIFIER_IMODIFIER_HPP__ */
