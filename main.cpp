#include <opencv2/opencv.hpp>

constexpr int kWIDTH = 320;
constexpr int kHEIGHT = 240;

int main() {
    // 检测部分变量
    cv::VideoCapture capture("media\\eye_0.mp4");
    cv::Mat frame;
    cv::Mat filt;
    cv::Mat gray;
    cv::Mat bina;
    cv::Mat edge;
    std::vector<cv::Vec4i> hierarchy;
    std::vector<std::vector<cv::Point>> contours;
    std::vector<std::vector<cv::Point>>::const_iterator itContours;
    cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(15, 15));
    int eye_x = 0;
    int eye_y = 0;

    // 检测结果
    double eye_xf = 0.0f;
    double eye_yf = 0.0f;
    bool eyeon = false;

    // 绘制部分变量
    cv::Mat out(kHEIGHT, kWIDTH, CV_8UC3);

    // 检测
    while (capture.read(frame)) {
        cv::bilateralFilter(frame, filt, 0, 50, 10, 4);
        cv::cvtColor(filt, gray, cv::COLOR_BGR2GRAY);
        cv::threshold(gray, bina, 65, 255, cv::THRESH_BINARY);
        cv::Canny(bina, edge, 150, 100);
        cv::findContours(edge, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
        itContours = contours.begin();
        int i = 0;
        for (; itContours != contours.end(); ++itContours) {
            cv::drawContours(edge, contours, i, cv::Scalar(255, 255, 255), cv::FILLED);
            i++;
        }
        cv::erode(edge, edge, element);
        if (contours.size() > 0) {
            int x = 0;
            int y = 0;
            int c = 0;
            int pixcount = edge.rows * edge.cols;
            for (int i = 0; i < edge.rows; i++) {
                for (int j = 0; j < edge.cols; j++) {
                    if (edge.at<uchar>(i, j) != 0) {
                        y += i;
                        x += j;
                        c++;
                    }
                }
            }
            if (c != 0) {
                x /= c;
                y /= c;
            }
            if (eye_x != 0 || eye_y != 0) {
                int img = (int)sqrt(edge.rows * edge.rows + edge.cols * edge.cols);
                int dx = abs(eye_x - x);
                int dy = abs(eye_y - y);
                int dis = sqrt(dx * dx + dy * dy);
                if (dis < img * 0.2) {
                    eye_x = x;
                    eye_y = y;
                    eyeon = c > pixcount * 0.005;
                }
            }
            else {
                eye_x = x;
                eye_y = y;
                eyeon = c > pixcount * 0.005;
            }
            eye_xf = (double)eye_x / edge.cols;
            eye_yf = (double)eye_y / edge.rows;
        }

        // 绘制
        if (eyeon) {
            cv::rectangle(out, cv::Rect(0, 0, kWIDTH, kHEIGHT), cv::Scalar(255, 255, 255), -1);
            int bgx = eye_xf * kWIDTH / 10 + kWIDTH / 2;
            int bgy = eye_yf * kHEIGHT / 10 + kHEIGHT / 2;
            // 底色
            cv::circle(out, cv::Point(bgx, bgy), kHEIGHT * 0.45, cv::Scalar(221, 187, 0), -1);
            // 高光
            cv::circle(out, cv::Point(bgx, bgy * 1.3), kHEIGHT * 0.35, cv::Scalar(255, 221, 0), -1);
            // 描边
            cv::circle(out, cv::Point(bgx, bgy), kHEIGHT * 0.45, cv::Scalar(0, 0, 0), kHEIGHT * 0.01);
            // 瞳孔
            cv::circle(out, cv::Point(eye_xf * kWIDTH * 1.05, eye_yf * kHEIGHT * 1.05), kHEIGHT * 0.15, cv::Scalar(119, 51, 0), -1);
            // 外圈
            cv::circle(out, cv::Point(bgx, bgy), kHEIGHT * 0.51, cv::Scalar(255, 255, 255), kHEIGHT * 0.1);
            // 整体高光
            cv::circle(out, cv::Point(bgx * 0.75, bgy * 0.75), kHEIGHT * 0.1, cv::Scalar(255, 255, 255), -1);
            // 瞳孔高光
            cv::circle(out, cv::Point(eye_xf * kWIDTH * 1.15, eye_yf * kHEIGHT * 0.95), kHEIGHT * 0.03, cv::Scalar(255, 255, 255), -1);

            cv::circle(frame, cv::Point(eye_x, eye_y), 30, cv::Scalar(0, 0, 255));
        }
        else {
            cv::rectangle(out, cv::Rect(0, 0, kWIDTH, kHEIGHT), cv::Scalar(170, 204, 238), -1);
            // 眼睑
            cv::circle(out, cv::Point(kWIDTH / 2, kHEIGHT * 3), kHEIGHT * 2.5, cv::Scalar(0, 0, 0), kHEIGHT * 0.03);
        }
        cv::imshow("Pupilla", out);
        cv::imshow("Original", frame);

        if (cv::waitKey(1) == 27) break;
    }

    return 0;
}
