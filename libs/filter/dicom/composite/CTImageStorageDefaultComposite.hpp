/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "filter/dicom/composite/IComposite.hpp"
#include "filter/dicom/config.hpp"

#include <data/DicomSeries.hpp>

namespace sight::filter::dicom
{
namespace composite
{

/**
 * @brief Default filter used to read CTImageStorage SOP class.
 */
class FILTER_DICOM_CLASS_API CTImageStorageDefaultComposite : public IComposite
{
public:
    fwCoreClassMacro(CTImageStorageDefaultComposite, IComposite,
                     filter::dicom::factory::New< CTImageStorageDefaultComposite >);

    /// Constructor
    FILTER_DICOM_API CTImageStorageDefaultComposite(filter::dicom::IFilter::Key key);

    /// Destructor
    FILTER_DICOM_API virtual ~CTImageStorageDefaultComposite();

    /// Return the name of the filter
    FILTER_DICOM_API virtual std::string getName() const override;

    /// Return the description of the filter
    FILTER_DICOM_API virtual std::string getDescription() const override;

protected:

    /// Filter name
    static const std::string s_FILTER_NAME;

    /// Filter description
    static const std::string s_FILTER_DESCRIPTION;
};

} // namespace composite
} // namespace sight::filter::dicom
