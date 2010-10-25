/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWWX_VALIDATOR_DOUBLEVALIDATOR_HPP_
#define _FWWX_VALIDATOR_DOUBLEVALIDATOR_HPP_

#include <wx/wx.h>

#include "fwWX/config.hpp"

namespace fwWX
{
/**
 * @brief This namespace provides a list of validator based on WxWidgets for several datas.
 * @namespace   fwWX::validator
 *
 * @author IRCAD (Research and Development Team).
 * @date    2009-2010.
 */

namespace validator
{

/**
 * @brief   This class allows us to double value in wx widgets.
 * @class   DoubleValidator.
 * @author  IRCAD (Research and Development Team).
 * @date    2009.
 * @note This class is not commented, because it is not used in the open framework tutorial.
 * @todo DoubleValidator is not commented.
 */
class FWWX_CLASS_API DoubleValidator : public wxTextValidator
{

public:

    FWWX_API DoubleValidator( const DoubleValidator & validator );

    FWWX_API DoubleValidator( double & value );

    FWWX_API DoubleValidator( double & value, double minValue, double maxValue );

    FWWX_API wxObject * Clone() const;

    FWWX_API bool TransferFromWindow();

    FWWX_API bool TransferToWindow();

    FWWX_API bool Validate( wxWindow * parent );

private:

    double & m_value;

    double m_minValue;

    double m_maxValue;

};

} // validator

} // fwWX


#endif /* _FWWX_VALIDATOR_DOUBLEVALIDATOR_HPP_ */
