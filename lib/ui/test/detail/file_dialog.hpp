/************************************************************************
 *
 * Copyright (C) 2026 IRCAD France
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

#include <sight/ui/test/config.hpp>

#include <QFileDialog>
#include <QPointer>
#include <QString>
#include <QStringList>

#include <cstdint>
#include <filesystem>
#include <vector>

namespace sight::ui::test::detail
{

/// One real dialog interaction. Its Qt state is only accessed on the GUI thread.
class SIGHT_UI_TEST_CLASS_API file_dialog
{
public:

    SIGHT_UI_TEST_API file_dialog(std::vector<std::filesystem::path> _paths, QString _filter, bool _folder);

    SIGHT_UI_TEST_API void prepare(QWidget& _modal);
    SIGHT_UI_TEST_API void accept(QWidget& _modal);
    SIGHT_UI_TEST_API void cancel(QWidget& _modal);

    enum class status : std::uint8_t
    {
        open,
        overwrite,
        closed
    };

    [[nodiscard]] SIGHT_UI_TEST_API status observe() const;
    SIGHT_UI_TEST_API void confirm_overwrite(QWidget& _modal);

private:

    QFileDialog& target(QWidget& _modal);

    /// The path as the dialog returns it: forward slashes, no trailing one.
    static QString normalized(const QString& _path);

    /// Whether the dialog would return exactly the requested paths.
    [[nodiscard]] bool selects_requested(const QFileDialog& _dialog) const;

    QPointer<QFileDialog> m_dialog;
    QString m_filter;
    QString m_text;
    QStringList m_paths;
    QString m_directory;
    bool m_folder;
    std::size_t m_path_count;
    bool m_bound {false};
};

} // namespace sight::ui::test::detail
