#include <dlib/opencv.h>
#include <opencv2/highgui/highgui.hpp>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>
#include <dlib/gui_widgets.h>
#include <fstream>

using namespace dlib;
using namespace std;

void findLandmark(cv::Mat img) {
    // dlib �̹��� ������ ����
    image_window win;

    // �� ����� �� ���帶ũ �� �ε�
    frontal_face_detector detector = get_frontal_face_detector();
    shape_predictor pose_model;
    deserialize("shape_predictor_68_face_landmarks.dat") >> pose_model;

    // OpenCV�� Mat�� dlib �̹��� �������� ��ȯ
    cv_image<bgr_pixel> cimg(img);

    // �� ����
    std::vector<rectangle> faces = detector(cimg);

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

    // ��� ǥ��
    win.set_image(cimg);
    win.add_overlay(render_face_detections(shapes));

    // ����� �����ְ� ���Ḧ ��ٸ�
    cout << "Press any key to exit." << endl;
    win.wait_until_closed();
}

int main()
{
    // �Է� �̹��� �ε�
    cv::Mat img = cv::imread("input_image.png");
    if (img.empty())
    {
        cerr << "Unable to load image!" << endl;
        return 1;
    }

    // ���帶ũ ���� �� txt���� ����
    findLandmark(img);

    return 0;
}