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

#ifndef __FWVTKIO_HELPER_TRANSFERFUNCTION_HPP__
#define __FWVTKIO_HELPER_TRANSFERFUNCTION_HPP__

#include "fwVtkIO/config.hpp"

#include <fwData/TransferFunction.hpp>

#include <vtkLogLookupTable.h>
#include <vtkSmartPointer.h>

namespace fwVtkIO
{

namespace helper
{

/**
 * @brief   Helper to manage vtk data from ::fwData::TransferFunction.
 */
class FWVTKIO_CLASS_API TransferFunction
{

public:

    /*!
     * @brief Convert a ::fwData::TransferFunction to a vtkLookupTable.
     *
     * @param[in] tf ::fwData::TransferFunction
     * @param[in] allowTransparency Enable conversion of ::fwData::TransferFunction alpha channel
     * @param[in] size lookup table size
     * @param[out] lt vtkLookupTable
     */
    FWVTKIO_API static void toVtkLookupTable(fwData::TransferFunction::csptr tf,
                                             vtkSmartPointer<vtkLookupTable>  lt,
                                             bool allowTransparency = false,
                                             unsigned int size = 256 );

    /*!
     * @brief Convert the range of an Image or a TransferFunction to a black and white vtkLookupTable.
     *
     * @param[in] rangeMin
     * @param[in] rangeMax
     * @param[in] size lookup table size
     * @param[out] lt vtkLookupTable
     *
     */
    FWVTKIO_API static void toBWVtkLookupTable(   double rangeMin, double rangeMax,
                                                  vtkSmartPointer<vtkLookupTable> lt,
                                                  unsigned int size = 256 );

};

} // namespace helper
} // namespace fwVtkIO

#endif // __FWVTKIO_HELPER_TRANSFERFUNCTION_HPP__
