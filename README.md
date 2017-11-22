# P3 SEMESTER PROJECT
by
Sofia Villar
Jeppe Køhlert
Miicha Valiima
Simon Koed Pedersen
David Sebastian Eriksen
Mikkel Brogaard Vittrup

Main contact: mvittr14@student.aau.dk

# Purpose
The purpose of this project is to calculate a person's running speed based on video footage. The project is done in collaboration with Polaric Semiconductor and AAU supervisor George Palamas.
The software is written in C++ with the OpenCV computer-vision library.

To process a video, instantiate a RunningSpeedCalculator and call the _process_ method:

    RunningSpeedCalulator video("somevideo.mp4");
    double speed = video.process();

The double _speed_ now holds the speed in centimeters/second.

# Installation 
If you just want to try the software, skip the C++ section.

## C++ Visual Studio project
Use Git to clone repository:
    git clone https://github.com/MikkelBV/P3.

Alternatively download the ZIP file.

Install OpenCV 3.3.0 and place it in your C-drive
    http://opencv.org/

OpenCV does not have to be in the C-drive, but we have set the Visual Studio compiler to look in that directory. If you wish to have OpenCV in a different directory, you will have to configure the settings of the Visual Studio solution.

and add /bin to environment variables

Open 
    P3/src/P3.sln
in Microsoft Visual Studio 2017


## HTML user interface

The /P3Webapp contains all the files you need to try the system, including a compiled .exe of the C++ project.
Have your .mp4 video file stored locally on your device

Get the software from
    https://nodejs.org/en/

Navigate to /P3Webapp and run the following command (only do this once)
    npm install

When you want to run the ser, run the following command in /P3Webapp
    node app.js

Open Chrome and go to http://localhost:3000
    Voila! 

# Bugs
Submit bugs at https://github.com/MikkelBV/P3/issues 