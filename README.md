# Object_measurement_openCV

Basic CPP code

You need a 2 pound Coin for calibration, can be adjusted according to the need, by changing the calibration values.
change the resolution before starting the program and add the openCV headers.

let me know if there is any optimisation can be done.

////////////////////////////////////////////////////////// Pseudo Code ////////////////////////////////////////////////////////

////////////////////////////////// <<< Main Program >>> /////////////////////////////////////

int main() // Main code (head)
{
    // Creating a blank image with white background with a size of 640 / 1080 pixels
   //Printing text "Place a 2 Pound coin in the centre and press 'y' for initiating the calibration" on screen for instructions
    // Displaying the window with text instructions
    // Wait on infinite loop till user gives input from the keyboard
    if (key == 'y' || key == 'Y') // If condition Gate for further execution into calibration Mode
    {
        Destroy opened Window("Welcome");
        do // Do While Loop for user input for recalibration (Head)
        {
            //Capturing image data from webcam/external camera
            im.read(pic); // reading the frame and storing it in pic variable
            // Calling the pre-processing function
            // Calling the Calibration function
            if value of “cal” is equal to zero initiate Coin Detection Loop
            {
                //Print text "Coin not Detected in the Frame..!" on-screen for instructions
                //Print text "Please place a coin" on screen for instructions
                //Print text "& Press' n' on the keyboard" on screen for instructions
            }
            /////// Displaying Text on the Screen/////
            //Print text “Camera resolution = 640 x 480 “on screen for instructions
            //Print text "If the calibration is correct, Please press 'y' on keyboard, to continue on screen for instructions
           // Print text "Press 'n' on keyboard to Recalibrate or 'q' Quit" on screen for instructions 
           // Adding Plus shape on screen at the centre.
            // command to display the Calibration window with the instructions on the image            
            // Wait on infinite loop till user gives input from the keyboard
             if (key == 'y'|| key == 'Y')             // Loop for user input for recalibration
             {
                 // Set Calib variable is equal to false
                 Printing "Initiating the Measurement Program" on the command line
             }
             else if (key == 'n' || key == 'N') // Else statement if the user inputs ‘n’ or ‘N’ on the keyboard
             {
                 // Set Calib variable is equal to false
                 //Printing "Recalibration initiated" on the command line
             }
            else if (key == 'q') // Else statement for exiting the code if the user inputs ‘q’ on the keyboard
            
            {
                //Printing "Exiting the Code Bue byeee.." on the command line
                // Break the execution
            }
            
        } do while Loop For Recalibration // (Tail)
        if(key == 'y') // Get inside if the variable key is equal to ASCI value of ‘y’
        
        {
            // Closing the Opened Windows
            //Capturing image data from webcam/external camera
            
                while (process) // while loop for displaying video frame by frame
                {
                   //cap.read(image); // Reading the video file frame by frame and storing in Variable image
                   // Calling the pre-processing function
                    
                   ///////// Displaying Text on the Screen ///////////
                    //Print text "Camera resolution = 640 x 480", on screen for instructions
                    //Print text "Please place the shapes for the measurement", on screen for instructions
                   // Print text "Press 'n' or 'q' or 'x' on keyboard to exit the code" on screen for instructions
                    
                    // command to display the Original image on a window
                    //command to display the grey image on a window 
                    // command to display the Blur image on a window
                   // command to display the Canny image on a window 
                    // command to display the Dilated image on a window 
                    // command to display the Eroded image on a window 
                    // Calling the get contours function
                    // command to display the Measured image on a window
                   // command to display the Contour image on a window
                   // Wait 100 milli seconds till user gives input from the keyboard
                    if (key == 'n' || key == 'q' || key == 'x') // Switch for killing the program 
                    {
                        //Print out the "Program is Exiting Bue Bye...." instruction on the terminal window
                        // Assign Process is equal to false
                    }
                }
        }   
    }
    Return ‘0’ value to the main
} //Main code Tail

////////////////////////////////// <<< Pre-processing Function >>> /////////////////////////////////////
Input image
{
    // Converting Coloured image into Grey
    // Applying Gaussian blur to the image
    // Apply Canny function for edge detection
    // Declaring the kernel size for using in Dilation and Erode function
    // Using Dilate function on the canny image
    //Using Erode Function on the dilated image
}

/////////////////////////////////// <<  Function For Calibration >> ////////////////////////////////////////////
Input Two Images for processing
{
    // Setting Cal value to 0
    // Assigning point in vector of vector to contours variables
    // Assigning 4 int to hierarchy variable which is in vector
    // finding contours using findContours function in OpenCV
    // drawing contours on the image using drawContours
    // For loop for contours
        {
            // Declaring the area variable and assigning to the area of the contours
            //Printing out the area of the contours
    
           // Declaring the conPoly variable with a vector of vector of point with restricted to the size of contours for storing Edges of the Contours
            // Declaring a boundRect vector variable to store the values of the boundingRect function
            //Declaring a Poin2F vector variable to store the values of the Centers of the contours
            // Declaring a float vector variable to store the values of the radius of the contours
            //Declaring objectType Variable into String type
            // if statement for filtering out shapes with respect to the area 
            {
                // finding the arc length using contours and assigning it to float variable peri
                // finding the number of corner points in a given contours
                // Creating a rectangle around the edges of the contour
                // Storing the Number of corners in objCor variable
                // drawing contours on the image using drawContours
                 // if Condition For Circle Detection
                    {
                        // Declaring the object type as Circle
                        // Usign minEnclosingCircle function to determine the centres and radius of the circle
                        // Drawing circle using circle function
                        // Declaring the dia on the by the radius
                        // printing the diameter data out
                        // Converting pixels into mm with known values
                        //printing out the calibration value data out
                        // assigning d with the calibrated data
                        converting the dia from mm into cm
                        // to display text on the shape with size
                        //Adding text on the top corner of the shapes
                    } // end of IF condition for circle detection
            }// end of if condition for shapes filters based on area
        }// End of for loop for contours
} // End of Calibration function

//////////////////////////////////// << Function For Measuring Operation  >> //////////////////////////////////////////////

The input of two images (Header for the measurement function)
{
    // Assigning point in a vector of vector to contours variables
    //Assigning 4 int to hierarchy variable which is in vector

    // finding contours using findContours function in OpenCV
    // drawing contours on the image using drawContours
    // loop for contours
        {
           // declaring the area variable and assigning to the area of the contours
          //Printing out the area of the contours
            // declaring the conPoly variable with a vector of vector of point with restricted to the size of contours for storing Edges of the Contours
            // Declaring a boundRect vector variable to store the values of the boundingRect function
            // Declaring a roRect vector variable to store values of the RotatedRect function
            // Declaring a centres vector variable to store values in the Point2f type
            // Declaring a radius vector variable to store values in float type
            // Declaring objectType
            // if statement for filtering out with respect to the area
            {
                 // finding the arc length using contours and assigning it to float variable peri
                // finding the number of corner points in a given contour
                // Creating a rectangle around the edges of the contour
                // Declaring a rectangle for rotating rect
                // Declaring a variable to store the point data of the edges
                // Getting the edge points for rotated rectangle from rect_points and storing in roRect [i]
                // Storing the Number of corners in objCor variable
                // Loop for detecting the shape of the object //
                if (objCor == 3) // if condition for triangle
                {
                    // Assigning the object type
                    // Declaring the definition of the rectangle
                    // drawing contours on the image using drawContours
                    //Adding text on the top corner of the shapes
                }
                if (objCor == 4) // if condition for square and rectangle
                {
                    // Calculating the width and height of a rectangle
                    // Calculating the Width and height of the rectangle using the distance between two points
                    // calculating the aspect ration for the square and rectangle difference
                    for ( int j = 0; j < 4; j++ ) // loop for drawing line from one point to another for Rotating Recatangle
                            {
                                //Line function to draw a line in between points
                                // Drawing circles on the corners
                            } //end of for loop for drawing lines
                    // Logic for separating the square and rectangle
                    {
                        //Declaring objectType as Square
                        // calculating value side using the area and side formula
                        // adding the conversion for displaying in cm
                        // to display text on the shape with size
                        // Adding text on the top corner of the shapes
                    }
                    Else // else condition for rectangle
                    {
                        //Declaring objectType as Rectangle
                        // conversion and calibration of the width and height
                        // Measuring the distance between two points
          // Declaring Point a and adding b and c variables in it
         // to display text on the shape with size
                        // Adding text on the top corner of the shapes
                        // Adding text on the top corner of the shapes
                        // Adding text on the top corner of the shapes
                    }
                }
                if (objCor == 5) // IF condition for Pentagon
                {
                    //Declaring objectType as Pentagon
                    // Declaring the definition of the rectangle
                    // drawing contours on the image using drawContours
                    // Adding text on the top corner of the shapes
                }
                if (objCor == 6) //if Condition for Hexagon
                {
                    //Declaring objectType as Hexagon
                    // Declaring the definition of the rectangle
                    // drawing contours on the image using drawContours
                    // Adding text on the top corner of the shapes
                }
                if (objCor > 7) // if condition for circle
                {
                    //Declaring objectType as Circle
                    // Using minEnclosingCircle function to determine the centres and radius of the circle
                    // Drawing circle using circle function
                    //Calculating the Dia from radius and cal value for conversion to know the value
                    // to display text on the shape with size
                    //Adding text on the top corner of the shapes
                }

        }//end of if condition for shapes filters based on area

    }//end of for loop for contours

}  // End of Measuring Function 
