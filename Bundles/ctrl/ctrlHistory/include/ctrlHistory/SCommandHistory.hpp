/************************************************************************
 *
 * Copyright (C) 2017 IRCAD France
 * Copyright (C) 2017 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

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
 * @section Signals Signals
 * - \b canUndo(bool) : sent when the history is modified, notifies if an undo is still possible.
 * - \b canRedo(bool) : sent when the history is modified, notifies if an redo is still possible.
 *
 * @section Slots Slots
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
    CTRLHISTORY_API virtual void configuring() override;

    /// Notify if undo or redo are possible.
    CTRLHISTORY_API virtual void starting() override;

    /// Notify if undo or redo are possible.
    CTRLHISTORY_API virtual void updating() override;

    /// Clears the history.
    CTRLHISTORY_API virtual void stopping() override;

private:

    typedef ::fwCom::Signal<void (bool)> CanDoSignalType;

    /// SLOT: add a command to the history.
    void enqueue(::fwCommand::ICommand::sptr command);

    /// SLOT: undo the last command.
    void undo();

    /// SLOT: redo the next command.
    void redo();

    /// SLOT: delete all commands from the history.
    void clear();

    /// Send 'canUndo' and 'canRedo' signals.
    void emitModifSig() const;

    CanDoSignalType::sptr m_canUndoSig;

    CanDoSignalType::sptr m_canRedoSig;

    ::fwCommand::UndoRedoManager m_undoRedoManager;
};

} // namespace ctrlHistory

#endif /* __CTRLHISTORY_SCOMMANDHISTORY_HPP__ */
