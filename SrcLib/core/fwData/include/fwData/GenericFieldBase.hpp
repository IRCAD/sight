/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATA_GENERICFIELDBASE_HPP__
#define __FWDATA_GENERICFIELDBASE_HPP__

#include "fwData/config.hpp"
#include "fwData/Object.hpp"

#include <fwCore/base.hpp>

#include <string>
#include <ostream>

namespace fwData
{

/**
 * @brief GenericFieldBase define common operation on simple data.
 */
class FWDATA_CLASS_API GenericFieldBase : public Object
{
public:

    fwCoreNonInstanciableClassDefinitionsMacro( (GenericFieldBase) );

    FWDATA_API virtual bool operator== (const GenericFieldBase &lf ) = 0;
    FWDATA_API virtual bool operator!= (const GenericFieldBase &lf ) = 0;
    FWDATA_API virtual bool operator<  (const GenericFieldBase &lf ) = 0;
    FWDATA_API virtual bool operator>  (const GenericFieldBase &lf ) = 0;
    FWDATA_API virtual bool operator<= (const GenericFieldBase &lf ) = 0;
    FWDATA_API virtual bool operator>= (const GenericFieldBase &lf ) = 0;
    FWDATA_API friend std::ostream& operator<<( std::ostream &, const GenericFieldBase & );

    FWDATA_API virtual ::std::string toString() const               = 0;
    FWDATA_API virtual void fromString(const ::std::string &_value) = 0;


private:
    FWDATA_API virtual std::ostream & toOStream( std::ostream &_os ) const = 0;
};

} // namespace fwData

#endif // __FWDATA_GENERICFIELDBASE_HPP__
