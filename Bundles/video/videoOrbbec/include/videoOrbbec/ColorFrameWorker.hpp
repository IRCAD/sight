/************************************************************************
 *
 * Copyright (C) 2018 IRCAD France
 * Copyright (C) 2018 IHU Strasbourg
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

#include "videoOrbbec/config.hpp"
#include <arData/FrameTL.hpp>

namespace arServices
{
class IGrabber;
} // namespace arServices

#include <QObject>
#include <QVideoFrame>

namespace videoOrbbec
{

class ColorFrameWorker : public QObject
{
Q_OBJECT;
public:
    ColorFrameWorker(::arServices::IGrabber const& parent);
    ~ColorFrameWorker();

    void init(::arData::FrameTL::sptr colorTL);
    void setPause(bool pause);

public Q_SLOTS:
    void presentFrame(QVideoFrame const& frame);

private:
    ::arServices::IGrabber const& m_parent;
    ::arData::FrameTL::sptr m_colorTL;
    bool m_pause;
};

} // namespace videoOrbbec
