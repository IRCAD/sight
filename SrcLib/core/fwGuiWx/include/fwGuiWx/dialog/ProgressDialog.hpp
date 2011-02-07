/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef FWGUIWX_PROGRESSDIALOG
#define FWGUIWX_PROGRESSDIALOG

#include <string>
#include <boost/shared_ptr.hpp>

#include <fwGui/dialog/ProgressDialog.hpp>

#include "fwGuiWx/config.hpp"


class fwProgressDialog;


namespace fwGuiWx
{
namespace dialog
{
/**
 * @brief   This class allows us to select an acquisition in a patient data base.
 * @class   ProgressDialog.
 * @author  IRCAD (Research and Development Team).
 * @date    2009.
 * @note Do not inherit from fwProgressDialog but embed a fwProgressDialog because this lasted is not copiable.
 * @todo ProgressDialog is not commented.
 */
class FWGUIWX_CLASS_API ProgressDialog : public ::fwGui::dialog::IProgressDialog
{
public:

    fwCoreClassDefinitionsWithFactoryMacro( (ProgressDialog)(::fwGui::dialog::IProgressDialog), (()), new ProgressDialog );

    FWGUIWX_API ProgressDialog( const std::string title="Progression", std::string message= std::string(86,' '));

    FWGUIWX_API virtual ~ProgressDialog();

    FWGUIWX_API void operator()(float percent,std::string msg);

    /// override
    FWGUIWX_API void setTitle(const std::string &title);

    /// override
    FWGUIWX_API void setMessage(const std::string &message);

protected :

    ::boost::shared_ptr< fwProgressDialog >  m_pdialog;

};
} // namespace dialog
} // namespace fwGuiWx

#endif /* FWGUIWX_PROGRESSDIALOG */
