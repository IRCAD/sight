/************************************************************************
 *
 * Copyright (C) 2022-2023 IRCAD France
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

#include "ui/qt/series/selector_dialog.hpp"

#include "ui/qt/series/selector.hpp"

#include <QDialogButtonBox>
#include <QPointer>
#include <QVBoxLayout>

namespace sight::ui::qt::series
{

/// Private selector implementation
class selector_dialog::selector_dialog_impl
{
public:

    /// Delete default constructors and assignment operators
    selector_dialog_impl(const selector_dialog_impl&)            = delete;
    selector_dialog_impl(selector_dialog_impl&&)                 = delete;
    selector_dialog_impl& operator=(const selector_dialog_impl&) = delete;
    selector_dialog_impl& operator=(selector_dialog_impl&&)      = delete;

    /// Constructor
    inline explicit selector_dialog_impl(selector_dialog* const _selector_dialog) noexcept :
        M_SELECTOR_DIALOG(_selector_dialog)
    {
    }

    /// Default destructor
    inline ~selector_dialog_impl() noexcept = default;

    /// Pointer to the public interface
    selector_dialog* const M_SELECTOR_DIALOG;

    QPointer<selector> m_selector_widget {nullptr};
};

selector_dialog::selector_dialog(
    data::series_set::csptr _series_set,
    const std::string& _displayed_column,
    QWidget* _parent
) :
    QDialog(_parent),
    m_pimpl(std::make_unique<selector_dialog_impl>(this))
{
    // Build the UI
    auto* layout = new QVBoxLayout(this);
    setLayout(layout);

    // Create the selector
    m_pimpl->m_selector_widget = new selector(_displayed_column, this);
    layout->addWidget(m_pimpl->m_selector_widget);

    // Fill the selector
    for(const auto& series : *_series_set)
    {
        m_pimpl->m_selector_widget->addSeries(series);
    }

    // Create the button box
    auto* button_box = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layout->addWidget(button_box);

    connect(button_box, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(button_box, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

// Defining the destructor here, allows us to use PImpl with a unique_ptr
selector_dialog::~selector_dialog() noexcept = default;

//------------------------------------------------------------------------------

data::series_set::sptr selector_dialog::get_selection() const
{
    if(!m_pimpl->m_selector_widget.isNull())
    {
        const auto& selected_series = m_pimpl->m_selector_widget->getSelectedSeries();
        auto series_set             = std::make_shared<data::series_set>();
        series_set->insert(series_set->end(), selected_series.cbegin(), selected_series.cend());

        return series_set;
    }

    return nullptr;
}

} // namespace sight::ui::qt::series
