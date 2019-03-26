/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "fwGuiQml/config.hpp"

#include <boost/function.hpp>

#include <QObject>
#include <QPointer>
#include <QWidget>

#include <functional>

// Qml
class QSlider;
class QLineEdit;
class QComboBox;

class QStringList;

namespace fwGuiQml
{

/**
 * @brief A qml panel used to control a VTK 2D Negatoscope view.
 *
 */
class SliceSelector : public QWidget //QWidget
{

Q_OBJECT

public:
    /**
     * @brief Constructor.
     * @param parent The parent widget.
     */
    FWGUIQML_API SliceSelector( QWidget* const parent = nullptr) noexcept;

    /// @brief Destructor.
    FWGUIQML_API virtual ~SliceSelector() noexcept;

    FWGUIQML_API void setSliceRange( int min, int max );

    FWGUIQML_API void setSliceValue( int index );

    FWGUIQML_API void setTypeSelection( int type );

    FWGUIQML_API void setEnable(bool enable);

    typedef std::function<void (int) > ChangeIndexCallback;
    FWGUIQML_API void setChangeIndexCallback(ChangeIndexCallback fct);

    typedef std::function<void (int) > ChangeTypeCallback;
    FWGUIQML_API void setChangeTypeCallback(ChangeTypeCallback fct);

protected Q_SLOTS:

    /**
     * @brief Event handler for a slice type change.
     * @param index index of the selected type item.
     */
    FWGUIQML_API void onSliceTypeChange( int index );

    /**
     * @brief Event handler for a slice index change.
     * @param value current value of the slice index slider.
     */
    FWGUIQML_API void onSliceIndexChange( int value ) noexcept;

private:

    QPointer< QComboBox > m_sliceType;

    /// @brief The slice index slider widget.
    QPointer< QSlider > m_sliceIndex;
    QPointer< QLineEdit > m_pSliceIndexText;

    void printIndex(int index);
    void printType(int type);

    ChangeIndexCallback m_fctChangeIndexCallback;
    ChangeTypeCallback m_fctChangeTypeCallback;
};

} // fwGuiQml
