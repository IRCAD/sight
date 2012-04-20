/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATATOOLS_TRANSFERFUNCTION_HPP_
#define _FWDATATOOLS_TRANSFERFUNCTION_HPP_

#include <fwCore/base.hpp>

#include <fwData/TransferFunction.hpp>

#include "fwDataTools/export.hpp"

namespace fwDataTools
{


/**
 * @brief   This class contains helper to generate and compare TransferFunction.
 * @class   TransferFunction.
 * @author  IRCAD (Research and Development Team).
 * @date    2012.
 *
 */
class TransferFunction : public ::fwCore::BaseObject
{

public :

    fwCoreClassDefinitionsWithFactoryMacro((TransferFunction)( ::fwCore::BaseObject ),
                                           (()),
                                           new TransferFunction
                                          );
    fwCoreAllowSharedFromThis();

    /**
     * @brief Generate a drawing tf with value between 0 and 255.
     * @param tf  transferFunction to generate
     */
    FWDATATOOLS_API static void generateDrawingTF(::fwData::TransferFunction::sptr tf);


protected:

    /// Return a map <value, color> used to generate drawing TF
    FWDATATOOLS_API static ::fwData::TransferFunction::TFDataType getDrawingTFColors();


    //! @brief Constructor.
    FWDATATOOLS_API TransferFunction();

    //! @brief Destructor.
    FWDATATOOLS_API virtual ~TransferFunction();

};


} // namespace fwDataTools

#endif // _FWDATATOOLS_TRANSFERFUNCTION_HPP_
