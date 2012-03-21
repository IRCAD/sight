/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _VTKIO_HELPER_TRANSFERTFUNCTION_HPP_
#define _VTKIO_HELPER_TRANSFERTFUNCTION_HPP_

#include <vtkSmartPointer.h>
#include <vtkLogLookupTable.h>

#include <fwData/TransferFunction.hpp>

#include "vtkIO/config.hpp"

namespace vtkIO
{

namespace helper
{

/**
 * @brief   Helper to manage vtk data from ::fwData::TransfertFunction.
 * @class   TransfertFunction.
 * @author  IRCAD (Research and Development Team).
 * @date    2012.
 *
 */
class VTKIO_CLASS_API TransfertFunction
{

public :

    /*!
     * @brief Convert a ::fwData::TransfertFunction to a vtkLookupTable.
     *
     * @param[in] tf ::fwData::TransfertFunction
     * @param[in] allow_transparency Enable convertion of ::fwData::TransfertFunction alpha channel
     * @param[in] size lookup table size
     * @param[out] lt vtkLookupTable
     */
    VTKIO_API static void toVtkLookupTable(
            ::fwData::TransferFunction::sptr tf,
             vtkSmartPointer<vtkLookupTable>  lt,
            bool allowTransparency = false,
            unsigned int size = 256 );

    /*!
     * @brief Convert the range of an Image or a TransfertFunction to a black and white vtkLookupTable.
     *
     * @param[in] rangeMin
     * @param[in] rangeMax
     * @param[in] size lookup table size
     * @param[out] lt vtkLookupTable
     *
     */
    VTKIO_API static void toBWVtkLookupTable(   double rangeMin, double rangeMax,
                                                vtkSmartPointer<vtkLookupTable> lt,
                                                unsigned int size = 256 );

  };

} // namespace helper
} // namespace vtkIO

#endif // _VTKIO_HELPER_TRANSFERTFUNCTION_HPP_
