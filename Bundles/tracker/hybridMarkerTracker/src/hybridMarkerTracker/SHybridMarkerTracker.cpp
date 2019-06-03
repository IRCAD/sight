#include <cmath>
#include "hybridMarkerTracker/SHybridMarkerTracker.hpp"

SHybridMarkerTracker::SHybridMarkerTracker (std::string filename):
tracker(NULL)
{
    std::cout << "Initializing..." << std::endl;
    fs.open(filename, ::cv::FileStorage::READ);
    // Read settings & configuration
    fs["patternToUse" ] >> patternToUse;
    fs["Circular_BoardSize_Width" ] >> cirboardSize.width;
    fs["Circular_BoardSize_Height"] >> cirboardSize.height;
    fs["Circular_Pattern_Size"] >> cirSize;
    fs["Sym_BoardSize_Width" ] >> symboardSize.width;
    fs["Sym_BoardSize_Height"] >> symboardSize.height;
    fs["Square_Size"]  >> squareSize;
    fs["Asym_Square_Size"]  >> asymSquareSize;
    fs["Sym_Square_Size_X" ] >> symSquareSize.x;
    fs["Sym_Square_Size_Y"] >> symSquareSize.y;
    fs["Radius"]  >> radius;
    fs["Chess_Dist_Center"]  >> Chess_Dist_Center;
    fs["Chess_Interval"]  >> Chess_Interval;

    fs["image_Width" ] >> img_size.width;
    fs["image_Height"] >> img_size.height;
    fs["Camera_Matrix"] >> cameraMatrix;
    fs["Distortion_Coefficients"] >> distCoeffs;

    fs.release();

    // --- Tracker parameters---
    ::cv::Size roi_size(300, 300);
    // Global blob detector setting
    ::cv::SimpleBlobDetector::Params params;
    params.minRepeatability = 2;
    params.minDistBetweenBlobs = 10;
    params.minThreshold = 80;
    params.maxThreshold = 160;
    params.thresholdStep = 20;
    params.filterByArea = true;
    params.minArea = 50;
    params.maxArea = 1000;
    params.filterByConvexity = true;
    params.minConvexity = 0.85f;
    params.maxConvexity = 1.0f;
    params.filterByCircularity = true;
    params.minCircularity = 0.7f;
    params.maxCircularity = 1.0f;
    params.filterByInertia = false;
    params.minInertiaRatio = 0.01;

    // Local blob detector setting
    ::cv::SimpleBlobDetector::Params params_roi;
    params_roi.minRepeatability = 2;
    params_roi.minDistBetweenBlobs = 10;
    params_roi.minThreshold = 50;
    params_roi.maxThreshold = 220;
    params_roi.thresholdStep = 10;
    params_roi.filterByArea = true;
    params_roi.minArea = 50;
    params_roi.maxArea = 2000;
    params.filterByConvexity = true;
    params.minConvexity = 0.85f;
    params.maxConvexity = 1.0f;
    params.filterByCircularity = true;
    params.minCircularity = 0.75;
    params.maxCircularity = 1.0f;


    /**
    *	Calculate model points for marker
    */
    if (patternToUse.compare("HYBRID") == 0)
    {
        /************************************************************************/
        /* Hybrid pattern														*/
        /************************************************************************/
        float arc_inner = symSquareSize.y;
        float arc_outter = arc_inner + 2 * asymSquareSize;
        float chord_inner_2 = radius * sin(arc_inner / (2*radius));	// chord/2
        float chord_outter_2 = radius * sin(arc_outter / (2*radius));
        float sagitta_inner = radius - sqrt(radius * radius - chord_inner_2 * chord_inner_2);
        float sagitta_outter = radius - sqrt(radius * radius - chord_outter_2 * chord_outter_2);
        // MID
        for ( int i = 0; i < symboardSize.height; i++ )
        {
            for ( int j = 0; j < symboardSize.width; j++)
            {
                ::cv::Point3f pt;
                pt.x = i * symSquareSize.x;
                pt.y = chord_inner_2;
                pt.y = (j % 2) == 0 ? pt.y : -pt.y;
                pt.z = sagitta_inner;
                trackMidPatternPoints.push_back(pt);
            }
        }
        // TOP
        int asym_points_num = symboardSize.height + symboardSize.height - 1;
        for ( int i = 0; i < asym_points_num; i++ )
        {
            ::cv::Point3f pt;
            pt.x = (symboardSize.height-1) * symSquareSize.x
                   - i * asymSquareSize;
            pt.y = (i % 2) == 0 ? chord_inner_2 : chord_outter_2;
            pt.z = (i % 2) == 0 ? sagitta_inner : sagitta_outter;
            trackTopPatternPoints.push_back(pt);
        }
        // BOT
        for ( int i = 0; i < asym_points_num; i++ )
        {
            ::cv::Point3f pt;
            pt.x = i * asymSquareSize;
            pt.y = (i % 2) == 0 ? -chord_inner_2 : -chord_outter_2;
            pt.z = (i % 2) == 0 ? sagitta_inner : sagitta_outter;
            trackBotPatternPoints.push_back(pt);
        }

        // Chess pattern points
        // MID
        for (auto i = -1; i < 5; i++)
        {
            ::cv::Point3f pt(0.0f, 0.0f, 0.0f);
            pt.x = Chess_Interval/2 + i * Chess_Interval;
            trackChessMidPatternPoint.push_back(pt);
        }

        float arc_chess = Chess_Dist_Center * 2;
        float chord_chess_2 = radius * sin(arc_chess / (2*radius));	// chord/2
        float sagitta_chess;
        if (arc_chess < CV_PI * radius)
            sagitta_chess = radius - sqrt(radius * radius - chord_chess_2 * chord_chess_2);
        else
            sagitta_chess = radius + sqrt(radius * radius - chord_chess_2 * chord_chess_2);

        // TOP
        for (auto i = 0; i < 5; i++)
        {
            ::cv::Point3f pt;
            pt.x = i * Chess_Interval;
            pt.y = chord_chess_2;
            pt.z = sagitta_chess;

            trackChessTopPatternPoint.push_back(pt);
        }

        // BOT
        for (auto i = 0; i < 5; i++)
        {
            ::cv::Point3f pt;
            pt.x = i * Chess_Interval;
            pt.y = -chord_chess_2;
            pt.z = sagitta_chess;

            trackChessBotPatternPoint.push_back(pt);
        }
        tracker = new TrackerCurvedot(symboardSize, roi_size, params, params_roi);
    }
//    else if (patternToUse.compare("CIRCULAR") == 0)
//    {
//        /************************************************************************/
//        /* Circular-dot pattern													*/
//        /************************************************************************/
//        for( int i = 0; i < cirboardSize.height; i++ )
//        {
//            for( int j = 0; j < cirboardSize.width; j++ )
//            {
//                //trackCirPatternPoint.push_back(::cv::Point3f(float((2*j + i % 2)*cirSize), float(i*cirSize), 0));
//                trackCirPatternPoint.push_back(::cv::Point3f(float(i*cirSize), 0, float((2*j + i % 2)*cirSize)));
//            }
//        }
//
//        tracker = new TrackerKeydot(cirboardSize, ::cv::CALIB_CB_ASYMMETRIC_GRID, roi_size, params, params_roi);
//    }
    else
    {
        std::cerr << "Unknow pattern type" << std::endl;
        exit(0);
    }
}

SHybridMarkerTracker::~SHybridMarkerTracker()
{
    if(!tracker)
        delete tracker;
}

void SHybridMarkerTracker::process(const ::cv::Mat &img, ::cv::Mat &out_img)
{
    m_img_track = img;

    bool use_ippe = true;
    // Tracking
    if(patternToUse.compare("HYBRID") == 0)
    {
        if (static_cast<TrackerCurvedot*>(tracker)->track(m_img_track))
        {
            std::vector<::cv::Point2f> imgPoints = static_cast<TrackerCurvedot*>(tracker)->getP_img();

            // Determine which pattern is detected
            std::vector<::cv::Point3f> visiblePatternPoints;
            int curr_detect_state = static_cast<TrackerCurvedot*>(tracker)->CurrDetectState();
            if ((curr_detect_state & TrackerCurvedot::TOP_CIR) && !(curr_detect_state & TrackerCurvedot::MID_CIR))
                visiblePatternPoints = trackTopPatternPoints;
            else if ((curr_detect_state & TrackerCurvedot::BOT_CIR) && !(curr_detect_state & TrackerCurvedot::MID_CIR))
                visiblePatternPoints = trackBotPatternPoints;
            else if ((curr_detect_state & TrackerCurvedot::TOP_CIR) && (curr_detect_state & TrackerCurvedot::MID_CIR))
            {
                int num_sym_dot = trackMidPatternPoints.size();
                visiblePatternPoints.reserve(trackTopPatternPoints.size() + num_sym_dot/2);
                visiblePatternPoints.insert(visiblePatternPoints.end(), trackTopPatternPoints.begin(), trackTopPatternPoints.end());
                for (int i = 0; i < num_sym_dot; i++)
                {
                    if (i%2 != 0)
                        visiblePatternPoints.push_back(trackMidPatternPoints[i]);
                }
                use_ippe = false;	// IPPE is only for planar model
            }
            else if ((curr_detect_state & TrackerCurvedot::BOT_CIR) && (curr_detect_state & TrackerCurvedot::MID_CIR))
            {
                int num_sym_dot = trackMidPatternPoints.size();
                visiblePatternPoints.reserve(trackBotPatternPoints.size() + num_sym_dot/2);
                visiblePatternPoints.insert(visiblePatternPoints.end(), trackBotPatternPoints.begin(), trackBotPatternPoints.end());
                for (int i = 0; i < num_sym_dot; i++)
                {
                    if (i%2 == 0)
                        visiblePatternPoints.push_back(trackMidPatternPoints[i]);
                }
                use_ippe = false;
            }
            else if (curr_detect_state & TrackerCurvedot::MID_CIR)
                visiblePatternPoints = trackMidPatternPoints;

            // Calculate pattern pose in camera coordinate
            ::cv::Mat rvec, tvec, cRp;
            ::cv::Mat rvec1, tvec1;	// 1st solution
            ::cv::Mat rvec2, tvec2;	// 2nd solution
            float error1, error2;

            current_cHp = ::cv::Mat::eye(4, 4, CV_64F);

            if (use_ippe)
            {
                std::vector<::cv::Point3f> pts_3d;
                bool is_chess_detect = true;

                if (curr_detect_state & TrackerCurvedot::TOP_CHESS)
                    pts_3d = trackChessTopPatternPoint;
                else if (curr_detect_state & TrackerCurvedot::MID_CHESS)
                    pts_3d = trackChessMidPatternPoint;
                else if (curr_detect_state & TrackerCurvedot::BOT_CHESS)
                    pts_3d = trackChessBotPatternPoint;
                else
                    is_chess_detect = false;	// Chess line not found

                ippe_solver.solveGeneric(visiblePatternPoints, imgPoints,
                                         cameraMatrix, distCoeffs, rvec1, tvec1, error1, rvec2, tvec2, error2);

                // Use chessboard features to disambigulate if two solutions are similar
                if (is_chess_detect && abs(error1 - error2) < 0.1 && error1 < 0.2 && error2 < 0.2)
                {
                    calculate_correct_pose(rvec1, tvec1, rvec2, tvec2,
                                           pts_3d, rvec, tvec);
                }
                else
                {
                    rvec = rvec1;
                    tvec = tvec1;
                }

                ::cv::Mat cHp_1 = ::cv::Mat::eye(4, 4, CV_64F);
                ::cv::Rodrigues(rvec1, cRp);
                ::cv::Mat aux = cHp_1.colRange(0,3).rowRange(0,3);
                cRp.copyTo(aux);
                aux = cHp_1.colRange(3,4).rowRange(0,3);
                tvec1.copyTo(aux);

                ::cv::Mat cHp_2 = ::cv::Mat::eye(4, 4, CV_64F);
                ::cv::Rodrigues(rvec2, cRp);
                aux = cHp_2.colRange(0,3).rowRange(0,3);
                cRp.copyTo(aux);
                aux = cHp_2.colRange(3,4).rowRange(0,3);
                tvec2.copyTo(aux);

                draw_rect(cHp_1, m_img_track, ::cv::Scalar(255, 0, 0));
                draw_rect(cHp_2, m_img_track, ::cv::Scalar(0, 0, 255));
            }
            else
            {
                ::cv::solvePnP(visiblePatternPoints, imgPoints, cameraMatrix, distCoeffs, rvec, tvec);
                ::cv::Rodrigues(rvec, cRp);
                ::cv::Mat aux = current_cHp.colRange(0,3).rowRange(0,3);
                cRp.copyTo(aux);
                aux = current_cHp.colRange(3,4).rowRange(0,3);
                tvec.copyTo(aux);
                draw_rect(current_cHp, m_img_track);
            }

            static_cast<TrackerCurvedot*>(tracker)->drawKeydots(m_img_track);
        }

    }
//    else if (patternToUse.compare("CIRCULAR") == 0)
//    {
//        if (static_cast<TrackerKeydot*>(tracker)->track(m_img_track))
//        {
//            std::vector<::cv::Point2f> imgPoints = static_cast<TrackerKeydot*>(tracker)->getP_img();
//            ;
//
//            // Calculate pattern pose in camera coordinate
//            ::cv::Mat rvec, tvec, cRp;
//            ::cv::Mat rvec1, tvec1;	// 1st solution
//            ::cv::Mat rvec2, tvec2;	// 2nd solution
//            float error1, error2;
//
//            if (use_ippe)
//            {
//                ippe_solver.solveGeneric(trackCirPatternPoint, imgPoints,
//                                         cameraMatrix, distCoeffs, rvec1, tvec1, error1, rvec2, tvec2, error2);
//
//                ::cv::Mat cHp_1 = ::cv::Mat::eye(4, 4, CV_64F);
//                ::cv::Rodrigues(rvec1, cRp);
//                ::cv::Mat aux = cHp_1.colRange(0,3).rowRange(0,3);
//                cRp.copyTo(aux);
//                aux = cHp_1.colRange(3,4).rowRange(0,3);
//                tvec1.copyTo(aux);
//
//                ::cv::Mat cHp_2 = ::cv::Mat::eye(4, 4, CV_64F);
//                ::cv::Rodrigues(rvec2, cRp);
//                aux = cHp_2.colRange(0,3).rowRange(0,3);
//                cRp.copyTo(aux);
//                aux = cHp_2.colRange(3,4).rowRange(0,3);
//                tvec2.copyTo(aux);
//
//                draw_rect(cHp_2, m_img_track, ::cv::Scalar(0, 0, 255));
//                draw_rect(cHp_1, m_img_track, ::cv::Scalar(255, 0, 0));
//            }
//            else
//            {
//                ::cv::solvePnP(trackCirPatternPoint, imgPoints, cameraMatrix, distCoeffs, rvec, tvec);
//                ::cv::Rodrigues(rvec, cRp);
//                ::cv::Mat aux = current_cHp.colRange(0,3).rowRange(0,3);
//                cRp.copyTo(aux);
//                aux = current_cHp.colRange(3,4).rowRange(0,3);
//                tvec.copyTo(aux);
//                draw_rect(current_cHp, m_img_track);
//            }
//            static_cast<TrackerKeydot*>(tracker)->drawKeydots(m_img_track);
//        }
//    }

    std::string str_1 = "Blue rectangle shows current estimated pose";
    std::string str_2 = "Red rectangle shows the ambiguous pose provided by IPPE";
    std::string str_3 = "Green shows detection of pattern";
    std::string str_4 = "Yellow shows tracking of pattern";
    ::cv::putText(m_img_track, str_1, ::cv::Point(10,20), ::cv::FONT_HERSHEY_COMPLEX, 0.5, ::cv::Scalar(255,0,0), 1);
    ::cv::putText(m_img_track, str_2, ::cv::Point(10,40), ::cv::FONT_HERSHEY_COMPLEX, 0.5, ::cv::Scalar(0,0,255), 1);
    ::cv::putText(m_img_track, str_3, ::cv::Point(10,60), ::cv::FONT_HERSHEY_COMPLEX, 0.5, ::cv::Scalar(0,255,0), 1);
    ::cv::putText(m_img_track, str_4, ::cv::Point(10,80), ::cv::FONT_HERSHEY_COMPLEX, 0.5, ::cv::Scalar(0,255,255), 1);

    m_img_track.copyTo(out_img);
}

cv::Vec2f SHybridMarkerTracker::error_dist_points(const std::vector<cv::Point2f> &pts_d,
                                         const std::vector<cv::Point2f> &pts_1,
                                         const std::vector<cv::Point2f> &pts_2,
                                         const double max_dist_sq)
{
    cv::Vec2f sum_error(0,0);
    auto dist_sq_1 = 0.0, dist_sq_2 = 0.0;
    cv::Point2f pts_diff;
    for (auto i = 0; i < pts_d.size(); i++)
    {
        int valid_ind = -1;
        for (auto j = 0; j < pts_1.size(); j++)
        {
            pts_diff = pts_d[i] - pts_1[j];
            dist_sq_1 = pts_diff.x * pts_diff.x + pts_diff.y * pts_diff.y;
            if (dist_sq_1 > max_dist_sq) continue;
            else
            {
                valid_ind = j;
                break;
            }
        }

        for (auto j = 0; j < pts_2.size(); j++)
        {
            pts_diff = pts_d[i] - pts_2[j];
            dist_sq_2 = pts_diff.x * pts_diff.x + pts_diff.y * pts_diff.y;
            if (dist_sq_2 > max_dist_sq) continue;

            if (valid_ind == j)	// assume pts_1 and pts_2 have same order
            {
                sum_error[0] += dist_sq_1;
                sum_error[1] += dist_sq_2;

                break;
            }
        }
        dist_sq_1 = dist_sq_2 = 0.0;
    }

    // recursive in case threshold max_dist_sq is too small
    if (sum_error[0] == 0.0)
    {
        sum_error = error_dist_points(pts_d, pts_1, pts_2, max_dist_sq * 2);
    }

    return sum_error;
}

void SHybridMarkerTracker::calculate_correct_pose(cv::InputArray rvec1, cv::InputArray tvec1,
                                         cv::InputArray rvec2, cv::InputArray tvec2,
                                         const std::vector<cv::Point3f> &pts_3d,
                                         cv::OutputArray rvec, cv::OutputArray tvec)
{
    std::vector<cv::Point2f> projPoints_1, projPoints_2;

    cv::projectPoints(pts_3d, rvec1, tvec1,
                      cameraMatrix, distCoeffs, projPoints_1);
    cv::projectPoints(pts_3d, rvec2, tvec2,
                      cameraMatrix, distCoeffs, projPoints_2);

    std::vector<cv::Point2f> detect_pts = static_cast<TrackerCurvedot*>(tracker)->get_chess_pts();

    // Calculate a threshold to determine correspondence
    cv::Point2f diff_temp = (projPoints_1[0] - projPoints_1[1]) * 0.7;
    double max_dist_sq = diff_temp.x*diff_temp.x + diff_temp.y*diff_temp.y;
    diff_temp = (projPoints_2[0] - projPoints_2[1]) * 0.7;
    max_dist_sq = (max_dist_sq + diff_temp.x*diff_temp.x + diff_temp.y*diff_temp.y)/2;

    cv::Vec2f errors = error_dist_points(detect_pts, projPoints_1, projPoints_2, max_dist_sq);

    rvec.create(3,1,CV_64FC1);
    tvec.create(3,1,CV_64FC1);
    if ( errors[0] < errors[1] )
    {
        rvec1.getMat().copyTo(rvec);
        tvec1.getMat().copyTo(tvec);
    }
    else
    {
        rvec2.getMat().copyTo(rvec);
        tvec2.getMat().copyTo(tvec);
    }
}

void SHybridMarkerTracker::draw_rect(const cv::Mat &cHp, cv::Mat & img, cv::Scalar color) {
    cv::Mat rect_corners = cv::Mat::ones(4, 4, CV_64F);
    rect_corners.at<double>(0, 0) = 0.0;
    rect_corners.at<double>(1, 0) = 0.0;
    rect_corners.at<double>(2, 0) = 0.0;
    rect_corners.at<double>(0, 1) = 20.0;
    rect_corners.at<double>(1, 1) = 0.0;
    rect_corners.at<double>(2, 1) = 0.0;
    rect_corners.at<double>(0, 2) = 20.0;
    rect_corners.at<double>(1, 2) = 0.0;
    rect_corners.at<double>(2, 2) = 30.0;
    rect_corners.at<double>(0, 3) = 0.0;
    rect_corners.at<double>(1, 3) = 0.0;
    rect_corners.at<double>(2, 3) = 30.0;

    cv::Mat corners_hm = cHp * rect_corners;

    std::vector <cv::Point3f> corners(4);    // 0-3: US image endpoints, 4-7: shadow pts

    for (int i = 0; i < corners.size(); i++) {
        corners[i].x = corners_hm.at<double>(0, i) / corners_hm.at<double>(3, i);
        corners[i].y = corners_hm.at<double>(1, i) / corners_hm.at<double>(3, i);
        corners[i].z = corners_hm.at<double>(2, i) / corners_hm.at<double>(3, i);
    }

    cv::Mat rVec, tVec, distCoeffs;
    rVec = cv::Mat::zeros(3, 1, CV_32FC1);
    tVec = cv::Mat::zeros(3, 1, CV_32FC1);
    std::vector <cv::Point2f> corners_2d;

    cv::projectPoints(corners, rVec, tVec, cameraMatrix,
                      distCoeffs, corners_2d);

    cv::Rodrigues(cHp(cv::Rect(0, 0, 3, 3)), rVec);
    cHp.colRange(3, 4).rowRange(0, 3).copyTo(tVec);

    cv::line(img, corners_2d[0], corners_2d[1], color, 2);
    cv::line(img, corners_2d[1], corners_2d[2], color, 2);
    cv::line(img, corners_2d[2], corners_2d[3], color, 2);
    cv::line(img, corners_2d[0], corners_2d[3], color, 2);

    cv::circle(img, corners_2d[0], 4, color, -1);


    // Draw coord axes
    //std::vector<cv::Point3f> coord_pts;
    //std::vector<cv::Point2f> proj_coord;
    //coord_pts.push_back(cv::Point3f(0, 0, 0));
    //coord_pts.push_back(cv::Point3f(6, 0, 0));
    //coord_pts.push_back(cv::Point3f(0, 6, 0));
    //coord_pts.push_back(cv::Point3f(0, 0, 6));
    //cv::projectPoints(coord_pts, rVec, tVec, cameraMatrix, distCoeffs, proj_coord);
    //cv::line(img, proj_coord[0], proj_coord[1], cv::Scalar(0, 0, 255),4);
    //cv::line(img, proj_coord[0], proj_coord[2], cv::Scalar(0, 255, 0),4);
    //cv::line(img, proj_coord[0], proj_coord[3], cv::Scalar(255, 0, 0),4);
}