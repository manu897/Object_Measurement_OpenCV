//
//  Object Measurment.cpp
//  Machine Vision
//
//  Created by Manideep Reddy Tamma on 23/02/22.

// 3 seconds from calibration to measuring the object
// < 1 sec for measuring the objects after calibration
// As this is a continous loop for measurement the accuracy is directly impacted due to the frame rate

#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>


using namespace std;
using namespace cv;

Point x1(25,25), x2(25,40), x3(25,55), x4(25,70), x5(25,85), xB(40,180), xB1(40,320), xB2(40,380); // Declaring the X axis for info display

int thresh1 = 25, thresh2 = 75; //25 ,75 Threshold for Canny
float Fs = 0.4, Ft = 1.95; // font size and thicknes for putText
double dia; // Declaring a dia variable
int minArea = 400; // filter limit for area
int cam = 2; // Camera Route
double cov = 0.1000; // convertion into cm from mm
double number; // Variable to store number of shapes in teh frame
Mat image, imageBlur, gray, imageCanny, imageDilation, imageErod, pic, cot; // MAT is a matrix data type to handle all the images in OpenCV , MAT is a class in OpenCV library
Mat kernelDia = getStructuringElement(MORPH_RECT, Size(3, 3)); // Declaring the kernel size for using in Dilation function
Mat kernelErod = getStructuringElement(MORPH_RECT, Size(3, 3)); // Declaring the kernel size for using in Erod function

double cal , realVal = 28.4000, er = 0.002; // cal = 0.23076923; realVal is in mm of 2 Pound 2011 coin ; er for offset.

bool process = true, calib = true, initial = true; // Declaring process bool for switch
int key;

//////////////////// << Function for Preprocessing The Image >> //////////////////////////////////
void Preprocessing(Mat frame)
{
    cvtColor(frame, gray, COLOR_BGR2GRAY); // Converting Coloured image into Grey
    GaussianBlur(gray, imageBlur, Size(7,7), 3,0); // Applying Gaussian blur to teh image
    Canny(imageBlur, imageCanny, thresh1, thresh2); // Apply Canny function for edge detection
    //Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3)); // Declaring the kernel size for using in Dilation function
    dilate(imageCanny, imageDilation, kernelDia); // Using Dilate function on the canny image
    erode(imageDilation, imageErod, kernelErod); //Using Erode Function on the dilated image

}

///////////////////// <<  Function For Calibration >> ///////////////////////////////////////////
void calibration(Mat input, Mat pic)
{
    cal = 0;
    vector<vector<Point>> contours; // Assignign point in vector of vector to contours variables
    vector<Vec4i> hierarchy; // Assigning 4 int to hierarchy variable which is in vector

    findContours(input, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE); // finding contours using findContours function in OpenCV
    //drawContours(image, contours, -1, Scalar(0,150,255), 2); // drawing contours on the image using drawContours
    number = contours.size(); // Storing the Number of shapes in Double number variable
    
    for(int i = 0; i < contours.size(); i++) // loop for contours
        {
            int area = contourArea(contours[i]); // declaring the area variable and assigning to the area of the contours
            //cout << area << " Area In pixels" << endl; // printing out the area of the contours
    
            vector<vector<Point>> conPoly(contours.size()); // declaring the conPoly variable with vector of vector of point with ristricted to the size of contours for storing Edges of the Contours
            vector<Rect> boundRect(contours.size()); // declaring a boundRect vector variable to store the vaules of the boundingRect function
            vector<Point2f> centers(contours.size()); //
            vector<float> radius(contours.size());
            
            string objectType;
    
            if (area > minArea) // statement for filltering out with respective to area
            {
                float peri = arcLength(contours[i], true); // finding the arc length using contours and assigning it to float variable peri
                approxPolyDP(contours[i], conPoly[i], 0.02 * peri, true); // finding the number of corner points in a given contour
                //cout << conPoly[i] << endl;

                // cout<< conPoly[i].size()<< " Corners" << endl; // Printing out the size of the conPoly
                boundRect[i] = boundingRect(conPoly[i]); // Creating a rectangle around the edges of the contour
                
                
                int objCor = (int)conPoly[i].size(); // Storing the Number of coners in objCor variable
                //drawContours(pic, contours, i, Scalar(0, 255, 0), 2); // drawing contours on the image using drawContours
                
                    if (objCor > 7 && objCor < 9) // Condition For Circle Detection
                    {
                        objectType = "Circle";
                        minEnclosingCircle(conPoly[i], centers[i], radius[i]); //
                        circle(pic, centers[i], radius[i], Scalar(0,255,0),Ft); // Drawing circle using circle function
                        
                        dia = 2*radius[i]; // Declearing the dia on the by the radius
                        cout << "Diameter = " << dia << " in Pixels" << endl; // printing the diameter data out
                        cal = (realVal/dia); // Converting pixels into mm with know values
                        cout << "Calibration ="<< cal << " in cm"  <<  endl; //printing out the calibration value data out
                        double d = (2*radius[i])*cal; // assigning d with the calibrated data
                        d = d*cov; // converting the dia from mm into cm
                        string discp = format("Coin Dia: %.2fcm",d); // to display text on the shape with size
                        putText(pic,discp,{boundRect[i].x - 10,boundRect[i].y}, FONT_ITALIC, Fs, Scalar(0, 0, 255), Ft); // Adding text on the top corner of the shapes
                        
                    }
                
            }
        }
}

//////////////////////////////////// << Creating a Function For Measuring Operation  >> /////////////////////////////////////////////////
void getMeasurements (Mat input,Mat image)
{
    vector<vector<Point>> contours; // Assignign point in vector of vector to contours variables
    vector<Vec4i> hierarchy; // Assigning 4 int to hierarchy variable which is in vector

    findContours(input, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE); // finding contours using findContours function in OpenCV
    //drawContours(image, contours, -1, Scalar(0,150,255), 2); // drawing contours on the image using drawContours
    //cout <<" Size of the Contours"<< contours.size()<< endl; // Printing the number of shapes detected
    for(int i = 0; i < contours.size(); i++) // loop for contours
        {
            int area = contourArea(contours[i]); // declaring the area variable and assigning to the area of the contours
            //cout << area << " Area In mm" << endl; // printing out the area of the contours
    
            vector<vector<Point>> conPoly(contours.size()); // declaring the conPoly variable with vector of vector of point with ristricted to the size of contours for storing Edges of the Contours
            vector<Rect> boundRect(contours.size()); // Declaring a boundRect vector variable to store the vaules of the boundingRect function
            vector<RotatedRect> roRect(contours.size()); // Declaring a roRect vector variable to store vaules of the RotatedRect function
            vector<Point2f> centers(contours.size()); // Declaring a centers vector variable to store vaules in the Point2f type
            vector<float> radius(contours.size()); // Declaring a radius vector variable to store vaules in float type
            
            string objectType; // Declaring objectType
    
            if (area > minArea) // statement for filltering out with respective to area
            {
                float peri = arcLength(contours[i], true); // finding the arc length using contours and assigning it to float variable peri
                approxPolyDP(contours[i], conPoly[i], 0.02 * peri, true); // finding the number of corner points in a given contour
                //cout << conPoly[i] << endl;
    
               // cout<< conPoly[i].size()<< " Corners" << endl; // Printing out the size of the conPoly
                boundRect[i] = boundingRect(conPoly[i]); // Creating a rectangle around the edges of the contour
                roRect[i] = minAreaRect(contours[i]); // Declaring a rectangle for rotating rect
                Point2f rect_points[4]; // Declaring a varible to store the point data of the edges
                roRect[i].points(rect_points);
                
                int objCor = (int)conPoly[i].size(); // Storing the Number of coners in objCor variable
                
                number = contours.size(); // Storing the Number of shapes in Double number variable
                
                // Loop for detecting the shape of the object //
                if (objCor == 3)
                {
                    objectType = "Triangle: Coming Soon.....";
                    rectangle(image, boundRect[i].tl(), boundRect[i].br(), Scalar(0,255,0), Ft); // Declaring the definition of the rectangle
                    drawContours(image, conPoly, i, Scalar(255, 0, 0), Ft); // drawing contours on the image using drawContours
                    putText(image,objectType,{boundRect[i].x,boundRect[i].y}, FONT_ITALIC, Fs, Scalar(0, 0, 255), Ft); // Adding text on the top corner of the shapes
                    
                }
                if (objCor == 4)
                {
                    
                    // Calculating the width and height of a rectangle
                    double w = norm((rect_points[i]) - (rect_points[(i+1)%4])); // Calculating the Width and height of the rectangle using the distance between two points
                    double h = norm((rect_points[i+1]) - (rect_points[(i+2)%4]));
                    
                    float aspRatio = (float)w / (float)h; // calculating the aspect ration for the square and rectangle differenciation
                    
                    for ( int j = 0; j < 4; j++ ) // loop for drawing line from one point to another for Rotating Recatangle
                            {
                                line( image, rect_points[j], rect_points[(j+1)%4], Scalar(0,255,0), Ft);
                                //circle(image, rect_points[j], 5, Scalar(0, 0 , 255), FILLED); // Drawing circles on the corners
                            } //*/
                    
                    if (aspRatio > 0.95 && aspRatio < 1.05) // Logic for separating the square and rectangle
                    {
                        objectType = "Square";
                        double side = (sqrt(area))*(cal+er); // calculating value side using the area and side formula
                        side = side*cov; // adding the conversion for displaying in cm
                        string discp = format("Square Side: %.2fcm" , side); // to display text on the shape with size
                        putText(image,discp,rect_points[i], FONT_ITALIC, Fs, Scalar(0, 0, 255), Ft); // Adding text on the top corner of the shapes
                        
                    }
                    else
                    {
                        objectType = "Rectangle";
                        
                        // conversion and calibration of the width and height
                        w = (w*(cal+er))*cov;
                        h = (h*(cal+er))*cov;
                       
                        // Measuring the distance between two points
                        int b = (norm((rect_points[i]) - (rect_points[(i+1)%4])))/2; // Width
                        int c = (norm((rect_points[i+1]) - (rect_points[(i+2)%4])))/2; // Height
                        Point a(b,c); // Declaring Point a and adding b and c variables in it
                        //Point d(rect_points[i]);
                        //Point e(rect_points[(i+1)%4]);
                        string discp = format("Rectangle W: %.2fcm H: %.2fcm" ,w,h); // to display text on the shape with size
                        //putText(image, discp, a, FONT_ITALIC, 0.5, Scalar(0, 0, 255), 2); // Adding text on the top corner of the shapes
                        putText(image, discp,rect_points[i], FONT_ITALIC, Fs, Scalar(0, 0, 255), Ft); // Adding text on the top corner of the shapes
                        //putText(image, discp, e, FONT_ITALIC, 0.5, Scalar(0, 0, 255), 2); // Adding text on the top corner of the shapes
                        
                    }
                    
                }
                if (objCor == 5)
                {
                    objectType = "Pentagon: Coming Soon.....";
                    rectangle(image, boundRect[i].tl(), boundRect[i].br(), Scalar(0,255,0), 2); // Declaring the definition of the rectangle
                    drawContours(image, conPoly, i, Scalar(255, 0, 0), 2); // drawing contours on the image using drawContours
                    //putText(image,objectType,{boundRect[i].x,boundRect[i].y}, FONT_ITALIC, Fs, Scalar(0, 0, 255), Ft); // Adding text on the top corner of the shapes
                }
                if (objCor == 6)
                {
                    objectType = "Hexagon: Coming Soon.....";
                    rectangle(image, boundRect[i].tl(), boundRect[i].br(), Scalar(0,255,0), 2); // Declaring the definition of the rectangle
                    drawContours(image, conPoly, i, Scalar(255, 0, 0), 2); // drawing contours on the image using drawContours
                    putText(image,objectType,{boundRect[i].x,boundRect[i].y}, FONT_ITALIC, Fs, Scalar(0, 0, 255), Ft); // Adding text on the top corner of the shapes
                }
                if (objCor > 7)
                {
                    objectType = "Circle";
                    minEnclosingCircle(conPoly[i], centers[i], radius[i]); //
                    circle(image, centers[i], radius[i], Scalar(0,255,0), Ft); // Drawing circle using circle function
                    double dia = (2*radius[i])*(cal+er);
                    dia = dia*cov;
                    string discp = format("Circle Dia: %.2fcm",dia); // to display text on the shape with size
                    putText(image,discp,{boundRect[i].x,boundRect[i].y}, FONT_ITALIC, Fs, Scalar(0, 0, 255), Ft); // Adding text on the top corner of the shapes
                }
              
                //drawContours(image, conPoly, i, Scalar(0, 0, 0), 2); // drawing contours on the image using drawContours
                //rectangle(image, boundRect[i].tl(), boundRect[i].br(), Scalar(0,255,0), 2); // Declaring the definition of the rectangle
                //putText(image,to_string(area),{boundRect[i].x,boundRect[i].y - 5}, FONT_ITALIC, 0.5, Scalar(0, 0, 255), 1.8); // Adding text on the top corner of the shapes
                //putText(image,to_string(boundRect[i].width),{boundRect[i].x,boundRect[i].y}, FONT_ITALIC, 0.5, Scalar(0, 0, 255), 2); // Adding text on the top corner of the shapes
                //putText(image, to_string(boundRect[i].height),{boundRect[i].x+10,boundRect[i].y}, FONT_ITALIC, 0.5, Scalar(0, 0, 255), 1.8); // Adding text on the top corner of the shapes
                
        }
            
    }
    
}

///////////////////////////////////////// <<< Main Program >>> /////////////////////////////////////
int main()
{
    
    VideoCapture it(cam); // Capturing Image from webcam
    
    
    do // Do while loop for initial display
    {
        
    it.read(cot); // Reading the fames and storing it in the cot variable
    
    putText(cot, "Place a 2 Pound coin in the center " , x1, FONT_ITALIC, 0.8, Scalar(0, 255, 0), 1.9); // Adding Text on the window
    putText(cot, "& Press 'y' for initiating the calibration" , x3, FONT_ITALIC, 0.8, Scalar(0, 255, 0), 1.9); // Adding Text on the window
    
    // Adding PLus on screen
    line(cot, Point(300,240), Point(340,240), Scalar(0, 255, 0), Ft);
    line(cot, Point(320,220), Point(320,260), Scalar(0, 255, 0), Ft);
    
   imshow("Welcome",cot); // Displaying the window with instructions
    
    key = waitKey(60); // Waitkey 60 = Display one frame 60 milli seconds
        
        if (key == 'y'|| key == 'Y') //Gate for further execution into calibration Mode
        {
            initial = false;
            cout << "Initiating the Calibration Program" << endl ;
        }
        
    }while(initial); // (Tail) do Do while loop for initial display
    
    if (key == 'y' || key == 'Y') //Gate for further execution into calibration Mode
    {
        destroyWindow("Welcome");
        
        do // Loop for user input for recalibration
        {
        
            VideoCapture im(cam);
            
            im.read(pic); // reading the frame and storing it in pic variable
            
            Preprocessing(pic); // Calling the preprocessing function
            
            calibration(imageDilation,pic); // Calling the Calibration function
            
            if (cal == 0 || cal < 0) // Coin Detection Loop start
            {
                putText(pic,"Coin not Detected in the Frame..!",xB, FONT_ITALIC, 1, Scalar(0, 0, 255), 2); // Adding text on the top corner of the shapes
                putText(pic,"Please place a coin",xB1, FONT_ITALIC, 1, Scalar(0, 0, 255), 2); // Adding text on the top corner of the shapes
                putText(pic,"& Press 'n' on the keyboard",xB2, FONT_ITALIC, 1, Scalar(0, 0, 255), 2); // Adding text on the top corner of the shapes
            } // Coin Detection Loop end
            // Displaying Text on the Screen
            string disp = format("Camera resolution = 640 x 480 True value of the coin is %.2fcm", (realVal*0.1));
            putText(pic, disp, x1, FONT_ITALIC, Fs, Scalar(0, 255, 0), Ft);
            putText(pic, "If the calibration is correct, Please press 'y' on keyboard, to continue", x2, FONT_ITALIC, Fs, Scalar(0, 255, 0), Ft);
            putText(pic, "Press 'n' on keyboard to Recalibrate or 'q' Quit", x3, FONT_ITALIC, Fs, Scalar(0, 255, 0), Ft);
            string pixels= format("Distance between two pixels is %2fmm & Number of shapes detected: %.1f",cal,number);
            putText(pic,pixels, x4, FONT_ITALIC, Fs, Scalar(0, 255, 0), Ft);
            
            // Adding PLus on screen
            line(pic, Point(300,240), Point(340,240), Scalar(0, 255, 0), Ft);
            line(pic, Point(320,220), Point(320,260), Scalar(0, 255, 0), Ft);
            
            imshow("Calibration Mode",pic); // command to display the Calibration window with the instructions on the image
            //imshow("Erod", imageErod);
            //imshow("Dilation", imageDilation); // command to display the image in a window (it is a string type)
         
            key=waitKey(0);
            // Loop for user input for recalibration
             if (key == 'y'|| key == 'Y')
             {
                 calib = false;
                 cout << "Initiating the Measurment Program" << endl ;
             }
             else if (key == 'n' || key == 'N')
             {
                 calib = true;
                 cout << "Recalibration inititated" << endl ;
             }
            else if (key == 'q') // exit the code
            {
                cout << "Exiting the Code Bue byeee.." << endl;
                break;
            }
        }while(calib); // do while Loop For Recalibration
        
        if(key == 'y')
            
        {
            // Closing the Opened Windows
            destroyWindow("Erod");
            destroyWindow("Dilation");
            destroyWindow("Calibration Mode");
            
            VideoCapture cap(cam); // streaming video data from webcame/external camera
            
                while (process) // while loop for displaying video frame by frame
                {
                    
                    cap.read(image); // reading the video file frame by frame
                    
                    // Creating a Canny Threshold tab for user input (for Thresold calibration
                    createTrackbar("Threshold1", "Canny", 0, 255);
                    createTrackbar("Threshold2", "Canny", 0, 255);
                   
                    Preprocessing(image); // calling the preprossing function
                    
                    // Getting the track bar possition values from Window Canny
                    thresh1 = getTrackbarPos("Threshold1", "Canny");
                    thresh2 = getTrackbarPos("Threshold2", "Canny");
                    
                    // Displaying Text on the Screen
                    string thr = format("Camera resolution = 640 x 480 and the Canny Threshold T1: %d T2: %d", thresh1,thresh2);
                    putText(image, thr, x1, FONT_ITALIC, Fs, Scalar(0, 255, 0), Ft);
                    string tex = format("Please place the shapes for the measurement & Number of shapes detected: %.1f",number);
                    putText(image, tex, x2, FONT_ITALIC, Fs, Scalar(0, 255, 0), Ft);
                    putText(image, "Press 'n' or 'q' or 'x' on keyboard to exit the code", x3, FONT_ITALIC, Fs, Scalar(0, 255, 0), Ft);
                    putText(image, "Adjust the Threshold Untill u see the edges of the shapes clearly in the Canny Window", x4, FONT_ITALIC, Fs, Scalar(0, 255, 0), Ft);
                    putText(image, "Optimal is around 25 and 75 respectively", x5, FONT_ITALIC, Fs, Scalar(0, 255, 0), Ft);
                    
                    //imshow("Original Video", image); // command to display the image in a window
                    //imshow("gray", gray); // command to display the image in a window
                    //imshow("Blur", imageBlur); // command to display the image in a window
                    imshow("Canny", imageCanny); // command to display the Canny image in a window
                    //imshow("Dilation", imageDilation); // command to display the image in a window
                    //imshow("Erod", imageErod); // command to display the image in a window
            
                    
                    
                    getMeasurements(imageErod, image); // Calling the get contours function
                    
                    imshow("Measurements", image); // command to display the Contour image
                    
                    key = waitKey(100); // Delay (20 = 20 milli seconds)
                    
                    if (key == 'n' || key == 'q' || key == 'x') // Switch for killing the program
                    {
                        cout << "Program is Exiting Bue Bye...." << endl;
                        process = false;
                        
                    }
                }
        }
        
    }
    
    return (0); //Return True to the main program
} // Main Program (Tail)
