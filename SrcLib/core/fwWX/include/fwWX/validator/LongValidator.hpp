/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWWX_VALIDATOR_LONGVALIDATOR_HPP_
#define _FWWX_VALIDATOR_LONGVALIDATOR_HPP_

#include <wx/wx.h>

#include "fwWX/config.hpp"

namespace fwWX
{

namespace validator
{

/**
 * @brief       This class allows us to long value in wx widgets.
 * @class       LongValidator.
 * @author      IRCAD (Research and Development Team).
 * @date        2009.
 * @note        This class is not commented, because it is not used in the open framework tutorial.
 * @todo        LongValidator is not commented.
 */
class FWWX_CLASS_API LongValidator : public wxTextValidator
{

public:
        FWWX_API LongValidator( const LongValidator & validator );

        FWWX_API LongValidator( long & value );

        FWWX_API LongValidator( long & value, long minValue );

        FWWX_API LongValidator( long & value, long minValue, long maxValue );

        FWWX_API wxObject * Clone() const;

        FWWX_API bool TransferFromWindow();

        FWWX_API bool TransferToWindow();

        FWWX_API bool Validate( wxWindow * parent );

private:

        long & m_value;
        long m_minValue;
        long m_maxValue;

};

} // validator
} // fwWX


#endif /* _FWWX_VALIDATOR_LONGVALIDATOR_HPP_ */
