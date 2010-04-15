/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef FWWX_PROGRESSTOWX
#define FWWX_PROGRESSTOWX

#include <string>


#include <boost/shared_ptr.hpp>
#include <boost/signals/trackable.hpp>
#include "fwWX/config.hpp"


class fwProgressDialog;


namespace fwWX
{

/**
 * @brief   This class allows us to select an acquisition in a patient data base.
 * @class   ProgressTowx.
 * @author  IRCAD (Research and Development Team).
 * @date    2009.
 * @note Do not inherit from fwProgressDialog but embedded a fwProgressDialog because this lasted is not copiable.
 * @todo ProgressTowx is not commented.
 */
class FWWX_CLASS_API ProgressTowx : public ::boost::signals::trackable // to autoDisconnect if handler is destroyed before the notifier
{
public:
    FWWX_API ProgressTowx( const std::string title, std::string message= std::string(86,' '));

    FWWX_API virtual ~ProgressTowx();

    FWWX_API void operator()(float percent,std::string msg);

protected :

    ::boost::shared_ptr< fwProgressDialog >  m_pdialog;

};

}

#endif /* FWWX_PROGRESSTOWX */
