/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GUI_EDITOR_SJOBBAR_HPP__
#define __GUI_EDITOR_SJOBBAR_HPP__

#include "gui/config.hpp"
#include "gui/editor/IDialogEditor.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Slots.hpp>

#include <fwGui/IActionSrv.hpp>

#include <fwJobs/IJob.hpp>

#include <fwServices/IService.hpp>

#include <set>

namespace fwGui
{
namespace dialog
{
class ProgressDialog;
}
}

namespace gui
{
namespace editor
{

/**
 * @brief   Service diplaying a progress bar.
 */

class GUI_CLASS_API SJobBar : public ::gui::editor::IDialogEditor
{

public:

    fwCoreServiceClassDefinitionsMacro ( (SJobBar)(::gui::editor::IDialogEditor) );

    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;

    /// Type of showJob slot
    typedef ::fwCom::Slot< void ( ::fwJobs::IJob::sptr ) >  ShowJobSlot;

    /**
     * @brief Constructor. Do nothing.
     */
    GUI_API SJobBar() noexcept;

    /**
     * @brief Destructor. Do nothing.
     */
    GUI_API virtual ~SJobBar() noexcept;

protected:

    /**
     * @brief This method gives information about the class. Do nothing.
     */
    GUI_API virtual void info(std::ostream &_sstream );

    /**
     * @brief This method emit a signal.
     */
    GUI_API void updating();

    /**
     * @brief This method is used to configure the service.
     */
    GUI_API void configuring();

    GUI_API virtual void starting();

    GUI_API virtual void stopping();

    /**
     * @brief showJob slot's method
     */
    GUI_API virtual void showJob( ::fwJobs::IJob::sptr iJob );

    typedef std::set< SPTR(::fwGui::dialog::ProgressDialog) > ProgressDialogs;
    ProgressDialogs m_progressDialogs;

    typedef ::fwCom::Signal<void () > StartedSignalType;
    typedef ::fwCom::Signal<void () > EndedSignalType;

    SPTR(StartedSignalType) m_sigStarted;
    SPTR(EndedSignalType) m_sigEnded;

};

} // namespace editor
} // namespace gui


#endif /*__GUI_EDITOR_SJOBBAR_HPP__*/
