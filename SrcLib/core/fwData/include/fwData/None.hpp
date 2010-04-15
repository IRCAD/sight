/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATA_NONE_HPP_
#define _FWDATA_NONE_HPP_

// export/visibility
#include "fwData/config.hpp"

#include "fwData/Object.hpp"


namespace fwData
{
/**
 * @class   None
 * @brief   This class represent the absence of data.
 *
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 */
class FWDATA_CLASS_API None : public Object
{

public :
    fwCoreClassDefinitionsWithFactoryMacro( (None)(::fwData::Object), (()), ::fwTools::Factory::New< None >) ;

    /// Constructor
    FWDATA_API None();
    /// Destructor
    FWDATA_API virtual ~None();

    FWDATA_API static bool isNone(::fwData::Object::sptr object);
}; // end class None

} // end namespace fwData

#endif // _FWDATA_NONE_HPP_
