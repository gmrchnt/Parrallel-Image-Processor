#define HAVE_STRUCT_TIMESPEC
#include<opencv2/opencv.hpp> 
#include<opencv2/highgui.hpp>
#include<stdio.h> 
#include<iostream>
#include<pthread.h>
#include<cctype>
#include<string>
#include<vector>
#include<stdlib.h>
//#include <sys/wait.h> 
#include <fcntl.h> 

using namespace cv;
using namespace std;

struct imageResizeStruct {
    Mat& image;
    int minWidth;
    int minHeight;
};

struct collageMakerStruct {
    Mat& collageBackground;
    Mat srcImage;
};



vector<Mat>image;
vector<Mat>collageImages;
vector<string>imageName;
vector<pthread_t> threads;
vector<imageResizeStruct*> rStructObjects;
vector<collageMakerStruct*> cStructObjects;
pthread_mutex_t imgMutex;
int imgCount = 0;

string collageName = "Collage_";

string resourcePath="D:/ImageEditor/ImageEditor/Resources/";
string editPath="D:/ImageEditor/ImageEditor/Resources/Edits/";