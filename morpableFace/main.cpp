#include <dlib/opencv.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>
#include <dlib/gui_widgets.h>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/mcc.hpp>

#include <iostream>
#include <fstream>

#ifdef _DEBUG
#pragma comment(lib,"opencv_world470d")
#else 
#pragma comment(lib,"opencv_world470")
#endif

using namespace dlib;
using namespace std;
using namespace cv;
using namespace mcc;
using namespace ccm;

void colorCalibration(cv::Mat img, cv::Mat& out_img) {
    cv::Mat imgCopy = img.clone();
    cv::Ptr<CCheckerDetector> detector = CCheckerDetector::create();

    // ��Ʈ Ÿ�� �� Ž��
    TYPECHART chartType = TYPECHART(0); // 0 = Standard chart type
    int nc = 1; // �ִ� ��Ʈ ��
    if (!detector->process(img, chartType, nc))
    {
        cerr << "Error: Chart detection failed." << endl;
    }
    cout << "Chart detection succeeded." << endl;

    std::vector<Ptr<mcc::CChecker>> checkers = detector->getListColorChecker();
    for (Ptr<mcc::CChecker> checker : checkers)
    {
        Ptr<CCheckerDraw> cdraw = CCheckerDraw::create(checker);
        // Ž���� ��Ʈ�� �̹����� �׸���
        // cdraw->draw(imgCopy); 
        //imshow("imgCopy", imgCopy);

        Mat chartsRGB = checker->getChartsRGB();
        // chartsRGB Ȯ��
        if (chartsRGB.empty())
        {
            cerr << "Error: chartsRGB is empty. Chart detection may have failed partially." << endl;
        }

        // src ������ �غ� �� �����
        Mat src = chartsRGB.col(1).clone().reshape(3, chartsRGB.rows / 3);
        src.convertTo(src, CV_64F); // double �������� ��ȯ
        src /= 255.0; // [0, 1] ������ ����ȭ
        // cout << "src matrix: " << src << endl;

        // �� ���� ��(ccm) ���� �� ����
        ColorCorrectionModel model1(src, COLORCHECKER_Macbeth);
        model1.run();

        // CCM �� ���
        // Mat ccm = model1.getCCM();
        // cout << "CCM Matrix: " << ccm << endl;

        // �̹��� Ķ���극�̼�
        Mat img_;
        cvtColor(img, img_, COLOR_BGR2RGB);
        img_.convertTo(img_, CV_64F);
        img_ /= 255.0;

        Mat calibratedimg = model1.infer(img_);

        // ��� ó��
        Mat out_ = calibratedimg * 255.0;
        out_ = min(max(out_, 0), 255.0);
        out_.convertTo(out_, CV_8UC3);
        cvtColor(out_, out_img, COLOR_RGB2BGR);

        // ��� ���
        //imshow("Calibrated img", out_img);
        //waitKey(0);
    }
}
void AnalysisSkinColor(cv::Mat img, point tl, point tr, point bl, point br, cv::Vec3b& minColor, cv::Vec3b& maxColor)
{
    cv::Mat blurImg;
    cv::GaussianBlur(img, blurImg, cv::Size(5, 5), 0); // ������ ���Ÿ� ���� ����þȺ� ����

    // �������� ���� ����/��ο� �ȼ� �÷� ����
    for (int y = tl.y(); y < bl.y(); ++y) {
        for (int x = tl.x(); x < tr.x(); ++x) {
            cv::Vec3b color = blurImg.at<cv::Vec3b>(y, x);
            double brightness = (color[0] + color[1] + color[2]) / 3;

            double minBrightness = (minColor[0] + minColor[1] + minColor[2]) / 3;
            double maxBrightness = (maxColor[0] + maxColor[1] + maxColor[2]) / 3;

            if (brightness < minBrightness) minColor = color;
            if (brightness > maxBrightness) maxColor = color;
        }
    }
}

void SpecifyAnalysisArea(full_object_detection shape, cv_image<bgr_pixel> img, point& tl, point& tr, point& bl, point& br, int dir)
{
    if (dir == 0) { // left
        tl = 0.7 * shape.part(1) + 0.3 * shape.part(28); // ���� ���
        br = 0.5 * shape.part(4) + 0.5 * shape.part(51); // ���� �ϴ�
        tr = point(br.x(), tl.y()); // ���� ���
        bl = point(tl.x(), br.y()); // ���� �ϴ�
    }
    else if (dir == 1) { // right
        tr = 0.7 * shape.part(15) + 0.3 * shape.part(28);
        bl = 0.5 * shape.part(12) + 0.5 * shape.part(51);
        tl = point(bl.x(), tr.y());
        br = point(tr.x(), bl.y());
    }

    // �簢 ���� �׸��� (�׸��� ���� �м��� ������ ���ļ�...)
    //draw_line(img, tl, tr, 0);
    //draw_line(img, tr, br, 0);
    //draw_line(img, br, bl, 0);
    //draw_line(img, bl, tl, 0);
}

full_object_detection FindLandmark(cv_image<bgr_pixel> cimg)
{
    // �� ����� �� ���帶ũ �� �ε�
    frontal_face_detector detector = get_frontal_face_detector();
    shape_predictor pose_model;
    deserialize("shape_predictor_68_face_landmarks.dat") >> pose_model;

    // �� ����
    std::vector<dlib::rectangle> faces = detector(cimg);

    // �� ���帶ũ ����
    std::vector<full_object_detection> shapes;
    for (unsigned long i = 0; i < faces.size(); ++i)
    {
        shapes.push_back(pose_model(cimg, faces[i]));
    }

    // ���帶ũ ������ �ؽ�Ʈ ���Ϸ� ����
    ofstream landmarksFile("landmarks.txt");
    if (landmarksFile.is_open())
    {
        for (const auto& shape : shapes)
        {
            for (int j = 0; j < shape.num_parts(); ++j)
            {
                landmarksFile << shape.part(j).x() << " " << shape.part(j).y() << endl; // x, y ��ǥ ����
            }
            landmarksFile << "-----" << endl; // �� �� ���帶ũ ����
        }
        landmarksFile.close();
        cout << "Landmarks saved to landmarks.txt" << endl;
    }
    else
    {
        cerr << "Unable to open landmarks.txt for writing!" << endl;
    }

    return shapes[0];
}

int main()
{
    // �Է� �̹��� �ε�
    cv::Mat img = cv::imread("je7.jpg");
    if (img.empty())
    {
        cerr << "Unable to load image!" << endl;
        return 1;
    }
    // �Է� �̹��� ���
    cv::Mat resizedImg;
    resize(img, resizedImg, Size(), 0.2, 0.2, INTER_LINEAR);

    // 0. �� ����
    cv::Mat calibrated;
    colorCalibration(resizedImg, calibrated);
    imshow("src image", resizedImg);
    imshow("calibrated image", calibrated);
    waitKey(0);
    std::cout << "Press any key to skip next level." << endl;

    // dlib �̹��� ������ ����
    image_window win;

    // OpenCV�� Mat�� dlib �̹��� �������� ��ȯ
    cv_image<bgr_pixel> cimg(calibrated);

    // 1. ���帶ũ ���� �� txt���� ����
    full_object_detection shape = FindLandmark(cimg);

    // ���� �м��� ���� ���帶ũ (1~17��)
    std::vector<point> faceShapePoints;
    cout << "Landmarks for face shape (1~17)" << endl;
    for (int i = 0; i < 17; i++) {
        faceShapePoints.push_back(shape.part(i));
        cout << shape.part(i) << endl;
    }

    // 2. �÷� �м� ���� ����
    point tl_left, tr_left, bl_left, br_left;
    point tl_right, tr_right, bl_right, br_right;

    SpecifyAnalysisArea(shape, cimg, tl_left, tr_left, bl_left, br_left, 0);
    SpecifyAnalysisArea(shape, cimg, tl_right, tr_right, bl_right, br_right, 1);

    //cout << "left rectangle points" << "\ntop left: " << tl_left << "\ntop right: " << tr_left << "\nbottom left: " << bl_left << "\nbottom right: " << br_left << endl;
    //cout << "left rectangle points" << "\ntop left: " << tl_right << "\ntop right: " << tr_right << "\nbottom left: " << bl_right << "\nbottom right: " << br_right << endl;

    // 3. �Ǻ� �÷� �м�
    cv::Vec3b minColor_left = cv::Vec3b(255, 255, 255);
    cv::Vec3b maxColor_left = cv::Vec3b(0, 0, 0);
    cv::Vec3b minColor_right = cv::Vec3b(255, 255, 255);
    cv::Vec3b maxColor_right = cv::Vec3b(0, 0, 0);

    AnalysisSkinColor(calibrated, tl_left, tr_left, bl_left, br_left, minColor_left, maxColor_left);
    AnalysisSkinColor(calibrated, tl_right, tr_right, bl_right, br_right, minColor_right, maxColor_right);

    // �� ���� �÷� ���
    cv::Vec3b average_minColor = (minColor_left / 2 + minColor_right / 2);
    cv::Vec3b average_maxColor = (maxColor_left / 2 + maxColor_right / 2);
    cout << "average lightest color(BGR): " << average_maxColor << endl;
    cout << "average darkest color(BGR): " << average_minColor << endl;

    // ��� ǥ��
    win.set_image(cimg);
    win.add_overlay(render_face_detections(shape));

    // ����� �����ְ� ���Ḧ ��ٸ�
    cout << "Press any key to exit." << endl;
    win.wait_until_closed();

    return 0;
}
