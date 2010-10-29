/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include "fwData/GenericFieldBase.hpp"


namespace fwData 
{

std::ostream& operator<<( std::ostream &_os, const ::fwData::GenericFieldBase &lf )
{
    return lf.toOStream(_os);
}

} //namespace fwData
