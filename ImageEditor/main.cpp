#include "Editor.h"

void imageCollector();
void imageViewer();
bool isDigit(const string& para)
{
    if (para.empty())
    {
        return false;
    }
    for (char c : para)
    {
        if (!isdigit(c))
            return false;
    }
    return true;
}
void imageCollector()
{
    system("cls");
    cout << "\tImage Collection\n";
    string numStr, imgName;
    Mat imageReader;
    int num;
    do
    {
        cout << "How many images do you want to process? ";
        cin >> numStr;
        if (!isDigit(numStr))
            cout << "Please enter a number.\n";
    } while (!isDigit(numStr));
    num = stoi(numStr);
    for (int i = 0; i < num; i++)
    {
        do
        {
            cout << "Please enter image name: ";
            cin.ignore();
            cin >> imgName;
            imgName = resourcePath + imgName;
            imageReader = imread(imgName);
            if (imageReader.empty())
            {
                cout << "Image doesn't exist in the current directory, please enter the image address.\n";
            }
        } while (imageReader.empty());
        image.push_back(imageReader);
        imageName.push_back(imgName);
    }
}
void imageViewer()
{
    if (image.empty())
    {
        cout << "No Images to Show.\n";
        cout << "<Press a key to continue>\n";
        cin.ignore();
        cin.get();
        return;
    }
    string windowName;
    for (int i = 0; i < image.size(); i++)
    {
        windowName = "Window_" + to_string(i);
        //namedWindow(windowName);
        imshow(windowName, image[i]);
    }
    waitKey(0);
    destroyAllWindows();
}

void* imageCompressRunner(void* param)
{
    Mat srcImage = *((Mat*)param);
    pthread_mutex_lock(&imgMutex);
    vector<int> compression_params;
    string imgName;
    Mat imageReader;
    compression_params.push_back(IMWRITE_JPEG_QUALITY);
    compression_params.push_back(50);
    do
    {
        imgName = "Compressed_" + to_string(imgCount) + ".jpg";
        imageReader = imread(imgName);
        if (!imageReader.empty())
        {
            imgCount++;
        }
    } while (!imageReader.empty());
    imgCount = 0;
    imgName = editPath + imgName;
    imwrite(imgName, srcImage, compression_params);
    pthread_mutex_unlock(&imgMutex);
    pthread_exit(0);
    return NULL;
}

void imageCompress()
{
    if (image.empty())
    {
        cout << "No Images to Compress.\n";
        cout << "<Press a key to continue>\n";
        cin.ignore();
        cin.get();
        return;
    }
    system("cls");
    cout << "\tImage Compression\n";
    pthread_mutex_init(&imgMutex, NULL);
    string choice;
    for (int i = 0; i < image.size(); i++)
    {
        cout << "Do you want to compress this image? (Y/N): ";
        string windowName = "Window" + to_string(i);
        namedWindow(windowName);
        imshow(windowName, image[i]);
        waitKey(0);
        destroyAllWindows();
        do
        {
            cin.ignore();
            cin >> choice;
            if (choice != "Y" && choice != "y" && choice != "N" && choice != "n")
            {
                cout << "Invalid choice.\n";
                cout << "Do you want to resize this image? (Y/N): ";
            }
        } while (choice != "Y" && choice != "y" && choice != "N" && choice != "n");
        pthread_t tid;
        if (choice == "Y" || choice == "y")
        {
            pthread_create(&tid, NULL, imageCompressRunner, (void*)&image[i]);
        }
        else
        {
            destroyAllWindows();
            continue;
        }
        threads.push_back(tid);
    }
    for (auto& thread : threads)
    {
        pthread_join(thread, NULL);
    }
    pthread_mutex_destroy(&imgMutex);
}

void imageCrop()
{
    if (image.empty())
    {
        cout << "No Images to Crop.\n";
        cout << "<Press a key to continue>\n";
        cin.ignore();
        cin.get();
        return;
    }
    system("cls");
    cout << "\tImage Cropping\n";
    string choice;
    for (int i = 0; i < image.size(); i++)
    {
        cout << "Do want to crop this image: ";
        string windowName = "Window" + to_string(i);
        namedWindow(windowName);
        imshow(windowName, image[i]);
        waitKey(0);
        destroyAllWindows();
        do
        {
            cin.ignore();
            cin >> choice;
            if (choice != "Y" && choice != "y" && choice != "N" && choice != "n")
            {
                cout << "Invalid choice.\n";
                cout << "Do want to resize this image: ";
            }
        } while (choice != "Y" && choice != "y" && choice != "N" && choice != "n");
        if (choice == "Y" || choice == "y")
        {
            Rect roi;
            roi = selectROI("Crop Window", image[i]);
            image[i] = image[i](roi);
            destroyAllWindows();
        }
        else
        {
            destroyAllWindows();
            continue;
        }
    }

}
void* imageBlurRunner(void* param)
{
    Mat srcImage = *((Mat*)param);
    pthread_mutex_lock(&imgMutex);
    GaussianBlur(srcImage, srcImage, Size(15, 15), 0);
    *((Mat*)param) = srcImage;
    pthread_mutex_unlock(&imgMutex);
    pthread_exit(0);
    return NULL;
}
void imageBlur()
{
    if (image.empty())
    {
        cout << "No Images to Blur.\n";
        cout << "<Press a key to continue>\n";
        cin.ignore();
        cin.get();
        return;
    }
    system("cls");
    cout << "\tImage Blurring\n";
    pthread_mutex_init(&imgMutex, NULL);
    string choice;
    for (int i = 0; i < image.size(); i++)
    {
        cout << "Do want to blur this image: ";
        string windowName = "Window" + to_string(i);
        namedWindow(windowName);
        imshow(windowName, image[i]);
        waitKey(0);
        destroyAllWindows();
        do
        {
            cin.ignore();
            cin >> choice;
            if (choice != "Y" && choice != "y" && choice != "N" && choice != "n")
            {
                cout << "Invalid choice.\n";
                cout << "Do want to resize this image: ";
            }
        } while (choice != "Y" && choice != "y" && choice != "N" && choice != "n");
        pthread_t tid;
        if (choice == "Y" || choice == "y")
        {
            pthread_create(&tid, NULL, imageBlurRunner, (void*)&image[i]);
        }
        else
        {
            destroyAllWindows();
            continue;
        }
        threads.push_back(tid);
    }
    for (auto& thread : threads)
    {
        pthread_join(thread, NULL);
    }
    pthread_mutex_destroy(&imgMutex);
}

void* imageResizeRunner(void* param)
{
    Mat srcImage = *((Mat*)param);
    pthread_mutex_lock(&imgMutex);
    int width, height;
    cout << "[Width x Height] of the image: " << srcImage.size() << endl;
    cout << "Enter width of resized image: ";
    cin >> width;
    cout << "Enter height of resized image: ";
    cin >> height;
    resize(srcImage, srcImage, Size(width, height), 0, 0);
    *((Mat*)param) = srcImage;
    pthread_mutex_unlock(&imgMutex);
    pthread_exit(0);
    return NULL;
}
void imageResize()
{
    if (image.empty())
    {
        cout << "No Images to Resize.\n";
        cout << "<Press a key to continue>\n";
        cin.ignore();
        cin.get();
        return;
    }
    system("cls");
    cout << "\tImage Resizing\n";
    pthread_mutex_init(&imgMutex, NULL);
    string choice;
    for (int i = 0; i < image.size(); i++)
    {
        cout << "Do want to resize this image: ";
        string windowName = "Window" + to_string(i);
        namedWindow(windowName);
        imshow(windowName, image[i]);
        waitKey(0);
        destroyAllWindows();
        do
        {
            cin.ignore();
            cin >> choice;
            if (choice != "Y" && choice != "y" && choice != "N" && choice != "n")
            {
                cout << "Invalid choice.\n";
                cout << "Do want to resize this image: ";
            }
        } while (choice != "Y" && choice != "y" && choice != "N" && choice != "n");
        pthread_t tid;
        if (choice == "Y" || choice == "y")
        {
            pthread_create(&tid, NULL, imageResizeRunner, (void*)&image[i]);
        }
        else
        {
            destroyAllWindows();
            continue;
        }
        threads.push_back(tid);
    }
    for (auto& thread : threads)
    {
        pthread_join(thread, NULL);
    }
    pthread_mutex_destroy(&imgMutex);
}
void* imageConvertorGRAY(void* param)
{
    Mat srcImage = *((Mat*)param);
    Mat grayImage;
    pthread_mutex_lock(&imgMutex);
    cvtColor(srcImage, grayImage, COLOR_BGR2GRAY);
    pthread_mutex_unlock(&imgMutex);
    *((Mat*)param) = grayImage;
    pthread_exit(0);
    return NULL;
}

void* imageConvertorYUV(void* param)
{
    Mat srcImage = *((Mat*)param);
    Mat yuvImage;
    pthread_mutex_lock(&imgMutex);
    cvtColor(srcImage, yuvImage, COLOR_BGR2YUV);
    pthread_mutex_unlock(&imgMutex);
    *((Mat*)param) = yuvImage;
    pthread_exit(0);
    return NULL;
}

void* imageConvertorHLS(void* param)
{
    Mat srcImage = *((Mat*)param);
    Mat hlsImage;
    pthread_mutex_lock(&imgMutex);
    cvtColor(srcImage, hlsImage, COLOR_BGR2HLS);
    pthread_mutex_unlock(&imgMutex);
    *((Mat*)param) = hlsImage;
    pthread_exit(0);
    return NULL;
}

void imageConvertor()
{
    if (image.empty())
    {
        cout << "No Images to Process.\n";
        cout << "<Press a key to continue>\n";
        cin.ignore();
        cin.get();
        return;
    }
    string choice;
    int num = -1; // Initialize num to an invalid value
    pthread_mutex_init(&imgMutex, NULL);
    system("cls");
    cout << "Color Model Conversion Menu\n";
    cout << "1- RGB to Grayscale.\n";
    cout << "2- RGB to YUV.\n";
    cout << "3- RGB to HLS.\n";
    cout << "0- No Conversion.\n";

    for (int i = 0; i < image.size(); i++)
    {
        cout << "Enter your choice: ";
        string windowName = "Window" + to_string(i);
        imshow(windowName, image[i]);
        waitKey(0);
        destroyAllWindows();
        do
        {
            cin >> choice;
            if (isdigit(choice[0]))
            {
                num = stoi(choice);
                if (num != 1 && num != 2 && num != 3 && num != 0)
                {
                    cout << "Invalid Entry. Please try again.\n";
                    continue;
                }
            }
            else
            {
                cout << "Invalid Entry. Please enter a number.\n";
                continue;
            }
        } while (num != 1 && num != 2 && num != 3 && num != 0);
        pthread_t tid;
        switch (num)
        {
        case 1:
            pthread_create(&tid, NULL, imageConvertorGRAY, (void*)&image[i]);
            break;
        case 2:
            pthread_create(&tid, NULL, imageConvertorYUV, (void*)&image[i]);
            break;
        case 3:
            pthread_create(&tid, NULL, imageConvertorHLS, (void*)&image[i]);
            break;
        case 0:
            continue;
        }
        threads.push_back(tid);
    }
    for (auto& thread : threads)
    {
        pthread_join(thread, NULL);
    }
    pthread_mutex_destroy(&imgMutex);
}


void* imageResizeRunner2(void* param)
{
    imageResizeStruct* runnerObject = (imageResizeStruct*)param;
    Mat& srcImage = runnerObject->image;
    int width = runnerObject->minWidth;
    int height = runnerObject->minHeight;
    pthread_mutex_lock(&imgMutex);
    resize(srcImage, srcImage, Size(width, height), 0, 0);
    pthread_mutex_unlock(&imgMutex);

    pthread_exit(0);
    return NULL;
}

void imageSmallestSize()
{
    long long int minArea = 40000000;
    int minHeight = 0;  // Initialize minHeight
    int minWidth = 0;   // Initialize minWidth

    for (int i = 0; i < image.size(); i++)
    {
        // Cast operands to long long to prevent overflow
        long long int area = static_cast<long long int>(image[i].cols) * static_cast<long long int>(image[i].rows);
        if (area < minArea)
        {
            minArea = area;
            minHeight = image[i].rows; //minimum area image's height
            minWidth = image[i].cols; //minimum area image's width
        }
    }

    // Check if minHeight and minWidth are still zero
    if (minHeight == 0 || minWidth == 0)
    {
        cout << "No valid images to process.\n";
        return;
    }

    minHeight /= 2;
    minWidth /= 2;

    threads.clear();
    pthread_mutex_init(&imgMutex, NULL);
    collageImages = image;
    for (int i = 0; i < image.size(); i++)
    {
        //collageImages[i]=image[i];
        imageResizeStruct* structObject = new imageResizeStruct{ collageImages[i], minWidth, minHeight };
        rStructObjects.push_back(structObject);
        pthread_t tid;
        pthread_create(&tid, NULL, imageResizeRunner2, (void*)structObject);
        threads.push_back(tid);
    }

    // Wait for all threads to finish
    for (auto& thread : threads)
    {
        pthread_join(thread, NULL);
    }
    pthread_mutex_destroy(&imgMutex);
    rStructObjects.clear();
}


int xPos = 0;
int yPos = 0;
void* collageMakerRunner(void* param)
{
    collageMakerStruct* runnerObject = (collageMakerStruct*)param;
    Mat& destImage = runnerObject->collageBackground; // Get a reference to the original image
    Mat srcImage = runnerObject->srcImage;
    pthread_mutex_lock(&imgMutex);
    if (xPos + srcImage.cols > destImage.cols)// || yPos + srcImage.rows > destImage.rows) 
    {
        xPos = 0;
        yPos += srcImage.rows;
    }
    Mat roi = destImage(Rect(xPos, yPos, srcImage.cols, srcImage.rows));
    srcImage.copyTo(roi);
    xPos += srcImage.cols;
    pthread_mutex_unlock(&imgMutex);
    pthread_exit(0);
    return NULL;
}
void collageMaker()
{
    if (image.empty())
    {
        cout << "No Images to Uploaded to generate a collage.\n";
        cout << "<Press a key to continue>\n";
        cin.ignore();
        cin.get();
        return;
    }
    system("cls");
    cout << "\tCollage Generation\n";
    imageSmallestSize();
    string imgName;
    Mat imageReader;
    int numImages = static_cast<int>(image.size());
    int numRows = 0, numCols = 0;
    if (numImages == 1)
        numCols = 1;
    else if (numImages == 2 || numImages == 3 || numImages == 4)
        numCols = 2;
    else if (numImages == 5 || numImages == 6)
        numCols = 3;
    else if (numImages >= 7)
        numCols = 4;
    if (numImages / numCols != static_cast<float>(numImages) / numCols)
        numRows += (numImages / numCols) + 1;
    else
        numRows = numImages / numCols;
    threads.clear();
    Mat collageBackground(collageImages[0].rows * numRows, collageImages[0].cols * numCols, CV_8UC3, Scalar(0, 0, 0));
    pthread_mutex_init(&imgMutex, NULL);

    for (int i = 0; i < image.size(); i++)
    {
        collageMakerStruct* structObject = new collageMakerStruct{ collageBackground, collageImages[i] };
        cStructObjects.push_back(structObject);
        pthread_t tid;
        pthread_create(&tid, NULL, collageMakerRunner, (void*)structObject);
        threads.push_back(tid);
    }
    for (auto& thread : threads)
    {
        pthread_join(thread, NULL);
    }
    pthread_mutex_destroy(&imgMutex);
    cStructObjects.clear();
    do
    {
        imgName = collageName + to_string(imgCount) + ".jpg";
        imageReader = imread(imgName);
        if (!imageReader.empty())
        {
            imgCount++;
        }
    } while (!imageReader.empty());
    imgCount = 0;
    imgName = editPath + imgName;
    imwrite(imgName, collageBackground);
    char choice;
    do
    {
        cout << "Do you want to view the collage? (Y/n)";
        cin >> choice;
        if (choice != 'y' && choice != 'n' && choice != 'Y' && choice != 'N')
        {
            cout << "Invalid Choice. Please re-enter.\n";
        }
        if (choice == 'y' || choice == 'Y')
        {
            imshow("Window", collageBackground);
            waitKey(0);
            destroyAllWindows();
            break;
        }
        else if (choice == 'n' || choice == 'N')
        {
            break;
        }
    } while (choice != 'y' && choice != 'n');
}


void createCopies()
{
    if (image.empty())
    {
        cout << "No Images to Uploaded to create copies.\n";
        cout << "<Press a key to continue>\n";
        cin.ignore();
        cin.get();
        return;
    }
    system("cls");
    cout << "\tImage Copier\n";
    string imgName;
    Mat imageReader;
    for (int i = 0; i < image.size(); i++)
    {
        do
        {
            imgName = "imageCopy_" + to_string(imgCount) + ".jpg";
            imageReader = imread(imgName);
            imgCount += 1;
        } while (!imageReader.empty());
        imgName = editPath + imgName;
        imwrite(imgName, image[i]);
    }
    imgCount = 0;
    cout << "Copies of Image created.\n";
    cout << "<Press a key to continue>\n";
    cin.ignore();
    cin.get();

}
void imageProcessing()
{
    string choice;
    int num = -1;
    system("cls");
    cout << "Image Processing Menu\n";
    cout << "1- Convert Color Model of Image(s).\n";
    cout << "2- Resize Image(s).\n";
    cout << "3- Blur Image(s).\n";
    cout << "4- Crop Image(s).\n";
    cout << "5- Compress Image(s).\n";
    cout << "0- Exit\n";
    while (1)
    {
        do
        {
            cout << "Enter your choice: ";
            cin >> choice;
            if (isDigit(choice))
            {
                num = stoi(choice);
                if (num != 1 && num != 2 && num != 3 && num != 4 && num != 5 && num != 0)
                {
                    cout << "Invalid Entry. Please try again.\n";
                    continue;
                }
            }
            else
            {
                cout << "Invalid Entry. Please enter a number.\n";
                continue;
            }
        } while (num != 1 && num != 2 && num != 3 && num != 4 && num != 5 && num != 0);
        switch (num)
        {
        case 1:
            imageConvertor();
            break;
        case 2:
            imageResize();
            break;
        case 3:
            imageBlur();
            break;
        case 4:
            imageCrop();
            break;
        case 5:
            imageCompress();
            break;
        case 0:
            return;
        }
        system("cls");
        cout << "Image Processing Menu\n";
        cout << "1- Convert Color Model of Image(s).\n";
        cout << "2- Resize Image(s).\n";
        cout << "3- Blur Image(s).\n";
        cout << "4- Crop Image(s).\n";
        cout << "5- Compress Image(s).\n";
        cout << "0- Exit\n";
    }
}
void mainMenu()
{
    string choice;
    int num = -1;
    system("cls");
    cout << "Main Menu\n";
    cout << "1- Enter Images.\n";
    cout << "2- View Images.\n";
    cout << "3- Apply Image Processing.\n";
    cout << "4- Generate Collage\n";
    cout << "5- Create Copies\n";
    cout << "0- Exit\n";

    while (1)
    {
        do
        {
            cout << "Enter your choice: ";
            cin >> choice;
            if (isDigit(choice))
            {
                num = stoi(choice);
                if (num != 1 && num != 2 && num != 3 && num != 4 && num != 5 && num != 0)
                {
                    cout << "Invalid Entry. Please try again.\n";
                    continue;
                }
            }
            else
            {
                cout << "Invalid Entry. Please enter a number.\n";
                continue;
            }
        } while (num != 1 && num != 2 && num != 3 && num != 4 && num != 5 && num != 0);
        switch (num)
        {
        case 1:
            imageCollector();
            break;
        case 2:
            imageViewer();
            break;
        case 3:
            imageProcessing();
            break;
        case 4:
            collageMaker();
            break;
        case 5:
            createCopies();
            break;
        case 0:
            return;
        }
        system("cls");
        cout << "Main Menu\n";
        cout << "1- Enter Images.\n";
        cout << "2- View Images.\n";
        cout << "3- Apply Image Processing.\n";
        cout << "4- Generate Collage\n";
        cout << "5- Create Copies\n";
        cout << "0- Exit\n";
    }
}
int main()
{
    mainMenu();
    return 0;
}
