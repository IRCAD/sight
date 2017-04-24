/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __CTRLHISTORY_SCOMMANDHISTORY_HPP__
#define __CTRLHISTORY_SCOMMANDHISTORY_HPP__

#include "ctrlHistory/config.hpp"

#include <fwCommand/ICommand.hpp>
#include <fwCommand/UndoRedoManager.hpp>

#include <fwCore/base.hpp>

#include <fwServices/IService.hpp>
#include <fwServices/macros.hpp>

namespace ctrlHistory
{

/**
 * @brief This service manages a command history.
 * The history is modified when receiving "undo", "redo", "enqueue" or "clear" signal.
 *
 *  * @section Slots Slots
 * - \b enqueue(::fwCommand::ICommand::sptr) : add a command to the history.
 * - \b undo() : undo the last command.
 * - \b redo() : redo the next command.
 * - \b clear() : clear the history (delete all commands).
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service uid="..." type="::ctrlHistory::SControlHistory" >
            <maxCommands>10</maxCommands>
            <maxMemory>100000000</maxMemory>
        </service>
   @endcode
 *
 * @subsection Configuration Configuration
 * - \b maxCommands (optional) : The maximum number of commands stored in the history. Unlimited by default.
 * - \b maxMemory (optional) : The maximum amount of memory (in bytes) used available to store commands.
 * Unlimited by default.
 */
class CTRLHISTORY_CLASS_API SCommandHistory : public ::fwServices::IService
{
public:

    fwCoreServiceClassDefinitionsMacro( (SCommandHistory)(::fwServices::IService) );

    /// Constructor.
    CTRLHISTORY_API SCommandHistory();

    /// Destructor.
    CTRLHISTORY_API virtual ~SCommandHistory();

protected:

    /// Set memory and command boundaries.
    CTRLHISTORY_API virtual void configuring() throw( ::fwTools::Failed );

    /// Does nothing.
    CTRLHISTORY_API virtual void starting() throw( ::fwTools::Failed );

    /// Does nothing.
    CTRLHISTORY_API virtual void updating() throw( ::fwTools::Failed );

    /// Clears the history.
    CTRLHISTORY_API virtual void stopping() throw( ::fwTools::Failed );

private:

    /// SLOT: add a command to the history.
    void enqueue(::fwCommand::ICommand::sptr command);

    /// SLOT: undo the last command.
    void undo();

    /// SLOT: redo the next command.
    void redo();

    /// SLOT: delete all commands from the history.
    void clear();

    ::fwCommand::UndoRedoManager m_undoRedoManager;
};

} // namespace ctrlHistory

#endif /* __CTRLHISTORY_SCOMMANDHISTORY_HPP__ */
