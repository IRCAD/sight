/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDICOMIOFILTER_CUSTOM_ICUSTOM_HPP__
#define __FWDICOMIOFILTER_CUSTOM_ICUSTOM_HPP__

#include "fwDicomIOFilter/IFilter.hpp"
#include "fwDicomIOFilter/config.hpp"

#include <fwMedData/DicomSeries.hpp>

namespace fwDicomIOFilter
{
namespace custom
{

/**
 * @brief Base class for Dicom instance custom filter.
 */
class FWDICOMIOFILTER_CLASS_API ICustom : public ::fwDicomIOFilter::IFilter
{
public:
    fwCoreNonInstanciableClassDefinitionsMacro( (ICustom)(::fwDicomIOFilter::IFilter) );

    /// Destructor
    FWDICOMIOFILTER_API virtual ~ICustom();

    /// Return filter type
    FWDICOMIOFILTER_API FilterType getFilterType() const;

protected:
    /// Constructor
    FWDICOMIOFILTER_API ICustom();

};

} // namespace custom
} // namespace fwDicomIOFilter


#endif /* __FWDICOMIOFILTER_CUSTOM_ICUSTOM_HPP__ */
