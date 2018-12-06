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

#ifndef __FWDATATOOLS_TRANSFERFUNCTION_HPP__
#define __FWDATATOOLS_TRANSFERFUNCTION_HPP__

#include <fwCore/base.hpp>

#include <fwData/TransferFunction.hpp>

#include "fwDataTools/config.hpp"

namespace fwDataTools
{


/**
 * @brief   This class contains helper to generate and compare TransferFunction.
 * @class   TransferFunction
 */
class TransferFunction
{

public:

    /**
     * @brief Generate a drawing tf with value between 0 and 255.
     * @param tf  transferFunction to generate
     */
    FWDATATOOLS_API static void generateDrawingTF(::fwData::TransferFunction::sptr tf);

protected:

    /// Return a map <value, color> used to generate drawing TF
    FWDATATOOLS_API static ::fwData::TransferFunction::TFDataType getDrawingTFColors();

};


} // namespace fwDataTools

#endif // __FWDATATOOLS_TRANSFERFUNCTION_HPP__
