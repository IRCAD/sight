/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
