/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDICOMIOFILTER_SORTER_ISORTER_HPP__
#define __FWDICOMIOFILTER_SORTER_ISORTER_HPP__

#include "fwDicomIOFilter/config.hpp"
#include "fwDicomIOFilter/IFilter.hpp"

#include <fwMedData/DicomSeries.hpp>

namespace fwDicomIOFilter
{
namespace sorter
{

/**
 * @brief Base class for Dicom instance sorter.
 */
class FWDICOMIOFILTER_CLASS_API ISorter : public ::fwDicomIOFilter::IFilter
{
public:
    fwCoreNonInstanciableClassDefinitionsMacro( (ISorter)(::fwDicomIOFilter::IFilter) );

    /// Destructor
    FWDICOMIOFILTER_API virtual ~ISorter();

    /// Return filter type
    FWDICOMIOFILTER_API FilterType getFilterType() const override;

protected:
    /// Constructor
    FWDICOMIOFILTER_API ISorter();

};

} // namespace sorter
} // namespace fwDicomIOFilter

#endif /* __FWDICOMIOFILTER_SORTER_ISORTER_HPP__ */
