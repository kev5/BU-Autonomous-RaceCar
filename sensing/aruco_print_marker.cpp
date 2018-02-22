#include "aruco.h"
#include <iostream>
#include <opencv2/highgui.hpp>
using namespace cv;
using namespace std;

// convinience command line parser
class CmdLineParser
{
    int argc;
    char** argv;
public:
    CmdLineParser(int _argc, char** _argv)
          : argc(_argc)
          , argv(_argv)
    {
    }
    bool operator[](string param)
    {
        int idx = -1;
        for (int i = 0; i < argc && idx == -1; i++)
            if (string(argv[i]) == param)
                idx = i;
        return (idx != -1);
    }
    string operator()(string param, string defvalue = "-1")
    {
        int idx = -1;
        for (int i = 0; i < argc && idx == -1; i++)
            if (string(argv[i]) == param)
                idx = i;
        if (idx == -1)
            return defvalue;
        else
            return (argv[idx + 1]);
    }
};

int main(int argc, char** argv)
{
    try
    {        CmdLineParser cml(argc, argv);

        if (argc < 2)
        {
            // You can also use ids 2000-2007 but it is not safe since there are a lot of false positives.
            cerr << "Usage: <makerid> outfile.(jpg|png|ppm|bmp)  [options] \n\t[-e use enclsing corners]\n\t[-bs <size>:bit size in pixels. 50 by "
                    "default ] \n\t[-d <dictionary>: ARUCO default]"
                 << endl;
            auto dict_names = aruco::Dictionary::getDicTypes();
            cerr << "\t\tDictionaries: ";
            for (auto dict : dict_names)
                cerr << dict << " ";
            cerr << endl;
            cerr << "\t Instead of these, you can directly indicate the path to a file with your own generated "
                    "dictionary"
                 << endl;

            return -1;
        }
        int pixSize = std::stoi(cml("-bs", "75"));  // pixel size each each bit
        bool enclosingCorners = cml["-e"];
        bool waterMark = true;
        // loads the desired dictionary
        aruco::Dictionary dic = aruco::Dictionary::load(cml("-d", "ARUCO"));

        cv::imwrite(argv[2], dic.getMarkerImage_id(stoi(argv[1]), pixSize, waterMark, enclosingCorners));
    }
    catch (std::exception& ex)
    {
        cout << ex.what() << endl;
    }
}
