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

#include "ui/qt/series/SelectorDialog.hpp"

#include "ui/qt/series/Selector.hpp"

#include <QDialogButtonBox>
#include <QPointer>
#include <QVBoxLayout>

namespace sight::ui::qt::series
{

/// Private SelectorDialog implementation
class SelectorDialog::SelectorDialogImpl
{
public:

    /// Delete default constructors and assignment operators
    SelectorDialogImpl(const SelectorDialogImpl&)            = delete;
    SelectorDialogImpl(SelectorDialogImpl&&)                 = delete;
    SelectorDialogImpl& operator=(const SelectorDialogImpl&) = delete;
    SelectorDialogImpl& operator=(SelectorDialogImpl&&)      = delete;

    /// Constructor
    inline explicit SelectorDialogImpl(SelectorDialog* const selector_dialog) noexcept :
        m_selector_dialog(selector_dialog)
    {
    }

    /// Default destructor
    inline ~SelectorDialogImpl() noexcept = default;

    /// Pointer to the public interface
    SelectorDialog* const m_selector_dialog;

    QPointer<Selector> m_selector_widget {nullptr};
};

SelectorDialog::SelectorDialog(data::SeriesSet::csptr series_set, const std::string& displayedColumn, QWidget* parent) :
    QDialog(parent),
    m_pimpl(std::make_unique<SelectorDialogImpl>(this))
{
    // Build the UI
    auto* layout = new QVBoxLayout(this);
    setLayout(layout);

    // Create the selector
    m_pimpl->m_selector_widget = new Selector(displayedColumn, this);
    layout->addWidget(m_pimpl->m_selector_widget);

    // Fill the selector
    for(const auto& series : *series_set)
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
SelectorDialog::~SelectorDialog() noexcept = default;

//------------------------------------------------------------------------------

data::SeriesSet::sptr SelectorDialog::get_selection() const
{
    if(!m_pimpl->m_selector_widget.isNull())
    {
        const auto& selected_series = m_pimpl->m_selector_widget->getSelectedSeries();
        auto series_set             = data::SeriesSet::New();
        series_set->insert(series_set->end(), selected_series.cbegin(), selected_series.cend());

        return series_set;
    }

    return nullptr;
}

} // namespace sight::ui::qt::series
