/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#pragma once

#include "ui/history/command.hpp"
#include "ui/history/config.hpp"

#include <deque>
#include <limits>

namespace sight::ui::history
{

/**
 * @brief Keep track of commands, undo/redo them.
 */
class UI_HISTORY_CLASS_API UndoRedoManager
{
public:

    /**
     * @brief Default constructor.
     *
     * @param[in] maxMemory the maximum amount of memory used by the manager (commands are dumped beyond this point)
     * @param[in] maxUndo the maximum number of commands stored by the manager (commands are dumped beyond this point)
     */
    UI_HISTORY_API UndoRedoManager(
        std::size_t _max_memory   = std::numeric_limits<std::size_t>::max(),
        std::size_t _max_commands = std::numeric_limits<std::size_t>::max()
    );

    /**
     * @brief Push a command to the history.
     *
     * @param[in] cmd the command
     *
     * @return false if the enqueue failed.
     */
    UI_HISTORY_API bool enqueue(command::sptr _cmd);

    /**
     * @brief Execute the next command if any.
     *
     * @return true if a command was redone
     */
    UI_HISTORY_API bool redo();

    /**
     * @brief Execute the previous command if any.
     *
     * @return true if a command was undone.
     */
    UI_HISTORY_API bool undo();

    /// Return true if we can undo.
    [[nodiscard]] UI_HISTORY_API bool canUndo() const;

    /// Return true if we can redo.
    [[nodiscard]] UI_HISTORY_API bool canRedo() const;

    /// Remove all commands in history.
    UI_HISTORY_API void clear();

    /// Get the number of enqueued commands.
    [[nodiscard]] UI_HISTORY_API std::size_t getCommandCount() const;

    /// Set the maximum number of enqueued commands.
    UI_HISTORY_API void setCommandCount(std::size_t _cmd_count);

    /// Returns the amount of memory used by the history.
    [[nodiscard]] UI_HISTORY_API std::size_t getHistorySize() const;

    /// Set the maximum amount of memory used by the history.
    UI_HISTORY_API void setHistorySize(std::size_t _hist_size);

private:

    typedef std::deque<command::sptr> command_history_t;

    /// Maximum amount of memory (in bytes) that can be used by the manager.
    std::size_t m_maxMemory;

    /// Maximum number of commands stored in the history.
    std::size_t m_maxCommands;

    /// Amount of memory currently in use by the command history.
    std::size_t m_usedMemory {0};

    /// Double-ended queue of commands.
    command_history_t m_commandQueue;

    /// Pointer to the last command.
    std::int64_t m_commandIndex {-1};

    /// Removes the oldest command from the history.
    void popFront();
};

} // namespace sight::ui::history
