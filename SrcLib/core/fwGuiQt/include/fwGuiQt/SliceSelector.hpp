/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
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

#ifndef __FWGUIQT_SLICESELECTOR_HPP__
#define __FWGUIQT_SLICESELECTOR_HPP__

#include "fwGuiQt/config.hpp"

#include <boost/function.hpp>

#include <QObject>
#include <QPointer>
#include <QWidget>

#include <functional>

// Qt
class QSlider;
class QLineEdit;
class QComboBox;
class QStringList;

namespace fwGuiQt
{

/**
 * @brief A qt panel used to control a VTK 2D Negatoscope view.
 *
 */
class SliceSelector : public QWidget
{

Q_OBJECT

public:
    /**
     * @brief Constructor.
     * @param parent The parent widget.
     */
    FWGUIQT_API SliceSelector( QWidget* const parent = nullptr) noexcept;

    /// @brief Destructor.
    FWGUIQT_API virtual ~SliceSelector() noexcept;

    FWGUIQT_API void setSliceRange( int min, int max );

    FWGUIQT_API void setSliceValue( int index );

    FWGUIQT_API void setTypeSelection( int type );

    FWGUIQT_API void setEnable(bool enable);

    typedef std::function<void (int) > ChangeIndexCallback;
    FWGUIQT_API void setChangeIndexCallback(ChangeIndexCallback fct);

    typedef std::function<void (int) > ChangeTypeCallback;
    FWGUIQT_API void setChangeTypeCallback(ChangeTypeCallback fct);

protected Q_SLOTS:

    /**
     * @brief Event handler for a slice type change.
     * @param index index of the selected type item.
     */
    FWGUIQT_API void onSliceTypeChange( int index );

    /**
     * @brief Event handler for a slice index change.
     * @param value current value of the slice index slider.
     */
    FWGUIQT_API void onSliceIndexChange( int value ) noexcept;

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

} // fwGuiQt

#endif // __FWGUIQT_SLICESELECTOR_HPP__
