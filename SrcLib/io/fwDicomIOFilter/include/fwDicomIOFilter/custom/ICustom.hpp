/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#ifndef __FWDICOMIOFILTER_CUSTOM_ICUSTOM_HPP__
#define __FWDICOMIOFILTER_CUSTOM_ICUSTOM_HPP__

#include "fwDicomIOFilter/config.hpp"
#include "fwDicomIOFilter/IFilter.hpp"

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
    FWDICOMIOFILTER_API FilterType getFilterType() const override;

protected:
    /// Constructor
    FWDICOMIOFILTER_API ICustom();

};

} // namespace custom
} // namespace fwDicomIOFilter

#endif /* __FWDICOMIOFILTER_CUSTOM_ICUSTOM_HPP__ */
