/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef FWWX_PROGRESSDIALOG
#define FWWX_PROGRESSDIALOG

#include <string>
#include <boost/shared_ptr.hpp>

#include <fwGui/ProgressDialog.hpp>

#include "fwWX/config.hpp"


class fwProgressDialog;


namespace fwWX
{

/**
 * @brief   This class allows us to select an acquisition in a patient data base.
 * @class   ProgressDialog.
 * @author  IRCAD (Research and Development Team).
 * @date    2009.
 * @note Do not inherit from fwProgressDialog but embed a fwProgressDialog because this lasted is not copiable.
 * @todo ProgressDialog is not commented.
 */
class FWWX_CLASS_API ProgressDialog : public ::fwGui::IProgressDialog
{
public:
    FWWX_API ProgressDialog( const std::string title="Progression", std::string message= std::string(86,' '));

    FWWX_API virtual ~ProgressDialog();

    FWWX_API void operator()(float percent,std::string msg);

    /// override
    FWWX_API void setTitle(const std::string &title);

    /// override
    FWWX_API void setMessage(const std::string &message);

protected :

    ::boost::shared_ptr< fwProgressDialog >  m_pdialog;

};

}

#endif /* FWWX_PROGRESSDIALOG */
