/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATATOOLS_TRANSFERFUNCTION_HPP_
#define _FWDATATOOLS_TRANSFERFUNCTION_HPP_

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

public :

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

#endif // _FWDATATOOLS_TRANSFERFUNCTION_HPP_
