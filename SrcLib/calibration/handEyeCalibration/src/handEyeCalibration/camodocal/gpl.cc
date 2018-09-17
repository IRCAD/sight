/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

/* ************************************
 * This work is inspired by :
 * CamOdoCal: Automatic Intrinsic and Extrinsic Calibration of a Rig with
 * Multiple Generic Cameras and Odometry
 * Copyright (c) 2013 Lionel Heng
 ************************************** */

#include "handEyeCalibration/camodocal/gpl.h"

#include <set>
#include <chrono>

const double WGS84_A     = 6378137.0;
const double WGS84_ECCSQ = 0.00669437999013;

namespace camodocal
{

double hypot3(const double x, const double y, const double z)
{
    return sqrt(square(x) + square(y) + square(z));
}

float hypot3f(const float x, const float y, const float z)
{
    return sqrtf(square(x) + square(y) + square(z));
}

double d2r(const double deg)
{
    return deg / 180.0 * M_PI;
}

float d2r(const float deg)
{
    return deg / 180.0f * M_PI;
}

double r2d(const double rad)
{
    return rad / M_PI * 180.0;
}

float r2d(const float rad)
{
    return rad / M_PI * 180.0f;
}

double sinc(const double theta)
{
    return sin(theta) / theta;
}

unsigned long long timeInMicroseconds(void)
{
    const auto now = std::chrono::high_resolution_clock::now();
    const std::chrono::microseconds duration =
            std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch());
    return duration.count();
}

double timeInSeconds(void)
{
    const auto now = std::chrono::high_resolution_clock::now();
    const std::chrono::seconds duration =
            std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch());
    return duration.count();
}

std::vector<::cv::Point2i> bresLine(int x0, int y0, const int x1, const int y1)
{
    // Bresenham's line algorithm
    // Find cells intersected by line between (x0,y0) and (x1,y1)

    std::vector<::cv::Point2i> cells;

    const int dx = std::abs(x1 - x0);
    const int dy = std::abs(y1 - y0);

    const int sx = (x0 < x1) ? 1 : -1;
    const int sy = (y0 < y1) ? 1 : -1;

    int err = dx - dy;

    while (1)
    {
        cells.push_back(::cv::Point2i(x0, y0));

        if (x0 == x1 && y0 == y1)
        {
            break;
        }

        int e2 = 2 * err;
        if (e2 > -dy)
        {
            err -= dy;
            x0  += sx;
        }
        if (e2 < dx)
        {
            err += dx;
            y0  += sy;
        }
    }

    return cells;
}

std::vector< ::cv::Point2i > bresCircle(const int x0, const int y0, const int r)
{
    // Bresenham's circle algorithm
    // Find cells intersected by circle with center (x0,y0) and radius r

    std::vector< std::vector<bool> > mask(2 * r + 1);

    for(int i = 0; i < 2 * r + 1; ++i)
    {
        mask[i].resize(2 * r + 1);
        for (int j = 0; j < 2 * r + 1; ++j)
        {
            mask[i][j] = false;
        }
    }

    int f     = 1 - r;
    int ddF_x = 1;
    int ddF_y = -2 * r;
    int x     = 0;
    int y     = r;

    std::vector< ::cv::Point2i > line;

    line = bresLine(x0, y0 - r, x0, y0 + r);
    for(std::vector<::cv::Point2i>::iterator it = line.begin(); it != line.end(); ++it)
    {
        mask[it->x - x0 + r][it->y - y0 + r] = true;
    }

    line = bresLine(x0 - r, y0, x0 + r, y0);
    for(std::vector<::cv::Point2i>::iterator it = line.begin(); it != line.end(); ++it)
    {
        mask[it->x - x0 + r][it->y - y0 + r] = true;
    }

    while(x < y)
    {
        if (f >= 0)
        {
            y--;
            ddF_y += 2;
            f     += ddF_y;
        }

        x++;
        ddF_x += 2;
        f     += ddF_x;

        line = bresLine(x0 - x, y0 + y, x0 + x, y0 + y);
        for(std::vector<::cv::Point2i>::iterator it = line.begin(); it != line.end(); ++it)
        {
            mask[it->x - x0 + r][it->y - y0 + r] = true;
        }

        line = bresLine(x0 - x, y0 - y, x0 + x, y0 - y);
        for(std::vector<::cv::Point2i>::iterator it = line.begin(); it != line.end(); ++it)
        {
            mask[it->x - x0 + r][it->y - y0 + r] = true;
        }

        line = bresLine(x0 - y, y0 + x, x0 + y, y0 + x);
        for(std::vector<::cv::Point2i>::iterator it = line.begin(); it != line.end(); ++it)
        {
            mask[it->x - x0 + r][it->y - y0 + r] = true;
        }

        line = bresLine(x0 - y, y0 - x, x0 + y, y0 - x);
        for(std::vector<::cv::Point2i>::iterator it = line.begin(); it != line.end(); ++it)
        {
            mask[it->x - x0 + r][it->y - y0 + r] = true;
        }
    }

    std::vector<::cv::Point2i> cells;
    for(int i = 0; i < 2 * r + 1; ++i)
    {
        for(int j = 0; j < 2 * r + 1; ++j)
        {
            if(mask[i][j])
            {
                cells.push_back(::cv::Point2i(i - r + x0, j - r + y0));
            }
        }
    }

    return cells;
}

void fitCircle(const std::vector<::cv::Point2d>& points, double& centerX, double& centerY, double& radius)
{
    // D. Umbach, and K. Jones, A Few Methods for Fitting Circles to Data,
    // IEEE Transactions on Instrumentation and Measurement, 2000
    // We use the modified least squares method.
    double sum_x   = 0.0;
    double sum_y   = 0.0;
    double sum_xx  = 0.0;
    double sum_xy  = 0.0;
    double sum_yy  = 0.0;
    double sum_xxx = 0.0;
    double sum_xxy = 0.0;
    double sum_xyy = 0.0;
    double sum_yyy = 0.0;

    const int n = points.size();
    for (int i = 0; i < n; ++i)
    {
        const double x = points.at(i).x;
        const double y = points.at(i).y;

        sum_x   += x;
        sum_y   += y;
        sum_xx  += x * x;
        sum_xy  += x * y;
        sum_yy  += y * y;
        sum_xxx += x * x * x;
        sum_xxy += x * x * y;
        sum_xyy += x * y * y;
        sum_yyy += y * y * y;
    }

    const double A = n * sum_xx - square(sum_x);
    const double B = n * sum_xy - sum_x * sum_y;
    const double C = n * sum_yy - square(sum_y);
    const double D = 0.5 * (n * sum_xyy - sum_x * sum_yy + n * sum_xxx - sum_x * sum_xx);
    const double E = 0.5 * (n * sum_xxy - sum_y * sum_xx + n * sum_yyy - sum_y * sum_yy);

    centerX = (D * C - B * E) / (A * C - square(B));
    centerY = (A * E - B * D) / (A * C - square(B));

    double sum_r = 0.0;
    for (int i = 0; i < n; ++i)
    {
        const double x = points.at(i).x;
        const double y = points.at(i).y;

        sum_r += hypot(x - centerX, y - centerY);
    }

    radius = sum_r / n;
}

std::vector< ::cv::Point2d > intersectCircles(const double x1, const double y1, const double r1, const double x2, const double y2, const double r2)
{
    std::vector<::cv::Point2d> ipts;

    const double d = hypot(x1 - x2, y1 - y2);
    if (d > r1 + r2)
    {
        // circles are separate
        return ipts;
    }
    if (d < fabs(r1 - r2))
    {
        // one circle is contained within the other
        return ipts;
    }

    const double a = (square(r1) - square(r2) + square(d)) / (2.0 * d);
    const double h = sqrt(square(r1) - square(a));

    const double x3 = x1 + a * (x2 - x1) / d;
    const double y3 = y1 + a * (y2 - y1) / d;

    if (h < 1e-10)
    {
        // two circles touch at one point
        ipts.push_back(::cv::Point2d(x3, y3));
        return ipts;
    }

    ipts.push_back(::cv::Point2d(x3 + h * (y2 - y1) / d,
                               y3 - h * (x2 - x1) / d));
    ipts.push_back(::cv::Point2d(x3 - h * (y2 - y1) / d,
                               y3 + h * (x2 - x1) / d));
    return ipts;
}

long int timestampDiff(const uint64_t t1, const uint64_t t2)
{
    if (t2 > t1)
    {
        const uint64_t d = t2 - t1;

        if (d > std::numeric_limits<long int>::max())
        {
            return std::numeric_limits<long int>::max();
        }
        else
        {
            return d;
        }
    }
    else
    {
        const uint64_t d = t1 - t2;

        if (d > std::numeric_limits<long int>::max())
        {
            return std::numeric_limits<long int>::min();
        }
        else
        {
            return -static_cast<long int>(d);
        }
    }
}

}
