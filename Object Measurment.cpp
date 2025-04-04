//
//  Object Measurement.cpp
//  Machine Vision
//
//  Created by Manideep Reddy Tamma on 23/02/22.

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace std;
using namespace cv;

// Global Variables
Point x1(25, 25), x2(25, 40), x3(25, 55), x4(25, 70), x5(25, 85), xB(40, 180), xB1(40, 320), xB2(40, 380);
int thresh1 = 25, thresh2 = 75;
float Fs = 0.4, Ft = 1.95;
double dia;
int minArea = 400;
int cam = 2;
double cov = 0.1000;
double number;
Mat image, imageBlur, gray, imageCanny, imageDilation, imageErod, pic, cot;
Mat kernelDia = getStructuringElement(MORPH_RECT, Size(3, 3));
Mat kernelErod = getStructuringElement(MORPH_RECT, Size(3, 3));
double cal, realVal = 28.4000, er = 0.002;
bool process = true, calib = true, initial = true;
int key;

/**
 * @brief Preprocesses the input frame by converting it to grayscale, applying Gaussian blur, 
 *        and performing edge detection using Canny.
 * 
 * @param frame Input frame to preprocess.
 */
void Preprocessing(Mat frame) {
    cvtColor(frame, gray, COLOR_BGR2GRAY);
    GaussianBlur(gray, imageBlur, Size(7, 7), 3, 0);
    Canny(imageBlur, imageCanny, thresh1, thresh2);
    dilate(imageCanny, imageDilation, kernelDia);
    erode(imageDilation, imageErod, kernelErod);
}

/**
 * @brief Calibrates the system using a reference object (e.g., a coin) to determine the pixel-to-mm ratio.
 * 
 * @param input Preprocessed image for contour detection.
 * @param pic Original image for displaying results.
 */
void calibration(Mat input, Mat pic) {
    cal = 0;
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;

    findContours(input, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    number = contours.size();

    for (int i = 0; i < contours.size(); i++) {
        int area = contourArea(contours[i]);
        if (area > minArea) {
            vector<vector<Point>> conPoly(contours.size());
            vector<Point2f> centers(contours.size());
            vector<float> radius(contours.size());

            float peri = arcLength(contours[i], true);
            approxPolyDP(contours[i], conPoly[i], 0.02 * peri, true);

            if (conPoly[i].size() > 7) { // Circle detection
                minEnclosingCircle(conPoly[i], centers[i], radius[i]);
                circle(pic, centers[i], radius[i], Scalar(0, 255, 0), Ft);

                dia = 2 * radius[i];
                cal = realVal / dia;
                double d = dia * cal * cov;
                string discp = format("Coin Dia: %.2fcm", d);
                putText(pic, discp, {conPoly[i][0].x, conPoly[i][0].y}, FONT_ITALIC, Fs, Scalar(0, 0, 255), Ft);
            }
        }
    }
}

/**
 * @brief Measures the dimensions of detected objects in the frame and displays the results.
 * 
 * @param input Preprocessed image for contour detection.
 * @param image Original image for displaying results.
 */
void getMeasurements(Mat input, Mat image) {
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;

    findContours(input, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    for (int i = 0; i < contours.size(); i++) {
        int area = contourArea(contours[i]);
        if (area > minArea) {
            vector<vector<Point>> conPoly(contours.size());
            vector<Rect> boundRect(contours.size());
            vector<RotatedRect> roRect(contours.size());
            vector<Point2f> centers(contours.size());
            vector<float> radius(contours.size());

            float peri = arcLength(contours[i], true);
            approxPolyDP(contours[i], conPoly[i], 0.02 * peri, true);

            int objCor = (int)conPoly[i].size();
            if (objCor == 4) { // Rectangle or Square
                roRect[i] = minAreaRect(contours[i]);
                Point2f rect_points[4];
                roRect[i].points(rect_points);

                double w = norm(rect_points[0] - rect_points[1]);
                double h = norm(rect_points[1] - rect_points[2]);
                float aspRatio = (float)w / (float)h;

                if (aspRatio > 0.95 && aspRatio < 1.05) { // Square
                    double side = sqrt(area) * (cal + er) * cov;
                    string discp = format("Square Side: %.2fcm", side);
                    putText(image, discp, rect_points[0], FONT_ITALIC, Fs, Scalar(0, 0, 255), Ft);
                } else { // Rectangle
                    w = w * (cal + er) * cov;
                    h = h * (cal + er) * cov;
                    string discp = format("Rectangle W: %.2fcm H: %.2fcm", w, h);
                    putText(image, discp, rect_points[0], FONT_ITALIC, Fs, Scalar(0, 0, 255), Ft);
                }
            } else if (objCor > 7) { // Circle
                minEnclosingCircle(conPoly[i], centers[i], radius[i]);
                circle(image, centers[i], radius[i], Scalar(0, 255, 0), Ft);
                double dia = (2 * radius[i]) * (cal + er) * cov;
                string discp = format("Circle Dia: %.2fcm", dia);
                putText(image, discp, {conPoly[i][0].x, conPoly[i][0].y}, FONT_ITALIC, Fs, Scalar(0, 0, 255), Ft);
            }
        }
    }
}

/**
 * @brief Main function to handle calibration and measurement processes.
 */
int main() {
    VideoCapture it(cam);

    // Initial display loop
    do {
        it.read(cot);
        putText(cot, "Place a 2 Pound coin in the center", x1, FONT_ITALIC, 0.8, Scalar(0, 255, 0), 1.9);
        putText(cot, "& Press 'y' for initiating the calibration", x3, FONT_ITALIC, 0.8, Scalar(0, 255, 0), 1.9);
        line(cot, Point(300, 240), Point(340, 240), Scalar(0, 255, 0), Ft);
        line(cot, Point(320, 220), Point(320, 260), Scalar(0, 255, 0), Ft);
        imshow("Welcome", cot);
        key = waitKey(60);

        if (key == 'y' || key == 'Y') {
            initial = false;
            cout << "Initiating the Calibration Program" << endl;
        }
    } while (initial);

    if (key == 'y' || key == 'Y') {
        destroyWindow("Welcome");

        // Calibration loop
        do {
            VideoCapture im(cam);
            im.read(pic);
            Preprocessing(pic);
            calibration(imageDilation, pic);

            if (cal == 0 || cal < 0) {
                putText(pic, "Coin not Detected in the Frame..!", xB, FONT_ITALIC, 1, Scalar(0, 0, 255), 2);
                putText(pic, "Please place a coin", xB1, FONT_ITALIC, 1, Scalar(0, 0, 255), 2);
                putText(pic, "& Press 'n' on the keyboard", xB2, FONT_ITALIC, 1, Scalar(0, 0, 255), 2);
            }

            string disp = format("Camera resolution = 640 x 480 True value of the coin is %.2fcm", (realVal * 0.1));
            putText(pic, disp, x1, FONT_ITALIC, Fs, Scalar(0, 255, 0), Ft);
            putText(pic, "If the calibration is correct, Please press 'y' on keyboard, to continue", x2, FONT_ITALIC, Fs, Scalar(0, 255, 0), Ft);
            putText(pic, "Press 'n' on keyboard to Recalibrate or 'q' Quit", x3, FONT_ITALIC, Fs, Scalar(0, 255, 0), Ft);
            string pixels = format("Distance between two pixels is %.2fmm & Number of shapes detected: %.1f", cal, number);
            putText(pic, pixels, x4, FONT_ITALIC, Fs, Scalar(0, 255, 0), Ft);

            line(pic, Point(300, 240), Point(340, 240), Scalar(0, 255, 0), Ft);
            line(pic, Point(320, 220), Point(320, 260), Scalar(0, 255, 0), Ft);

            imshow("Calibration Mode", pic);
            key = waitKey(0);

            if (key == 'y' || key == 'Y') {
                calib = false;
                cout << "Initiating the Measurement Program" << endl;
            } else if (key == 'n' || key == 'N') {
                calib = true;
                cout << "Recalibration initiated" << endl;
            } else if (key == 'q') {
                cout << "Exiting the Code. Bye!" << endl;
                break;
            }
        } while (calib);

        if (key == 'y') {
            destroyAllWindows();
            VideoCapture cap(cam);

            while (process) {
                cap.read(image);
                createTrackbar("Threshold1", "Canny", &thresh1, 255);
                createTrackbar("Threshold2", "Canny", &thresh2, 255);

                Preprocessing(image);

                string thr = format("Camera resolution = 640 x 480 and the Canny Threshold T1: %d T2: %d", thresh1, thresh2);
                putText(image, thr, x1, FONT_ITALIC, Fs, Scalar(0, 255, 0), Ft);
                string tex = format("Please place the shapes for the measurement & Number of shapes detected: %.1f", number);
                putText(image, tex, x2, FONT_ITALIC, Fs, Scalar(0, 255, 0), Ft);
                putText(image, "Press 'n' or 'q' or 'x' on keyboard to exit the code", x3, FONT_ITALIC, Fs, Scalar(0, 255, 0), Ft);
                putText(image, "Adjust the Threshold Until you see the edges of the shapes clearly in the Canny Window", x4, FONT_ITALIC, Fs, Scalar(0, 255, 0), Ft);
                putText(image, "Optimal is around 25 and 75 respectively", x5, FONT_ITALIC, Fs, Scalar(0, 255, 0), Ft);

                imshow("Canny", imageCanny);
                getMeasurements(imageErod, image);
                imshow("Measurements", image);

                key = waitKey(100);
                if (key == 'n' || key == 'q' || key == 'x') {
                    cout << "Program is Exiting. Bye!" << endl;
                    process = false;
                }
            }
        }
    }

    return 0;
}
