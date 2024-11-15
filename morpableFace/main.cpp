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
    // dlib 이미지 윈도우 생성
    image_window win;

    // 얼굴 검출기 및 랜드마크 모델 로드
    frontal_face_detector detector = get_frontal_face_detector();
    shape_predictor pose_model;
    deserialize("shape_predictor_68_face_landmarks.dat") >> pose_model;

    // OpenCV의 Mat를 dlib 이미지 형식으로 변환
    cv_image<bgr_pixel> cimg(img);

    // 얼굴 검출
    std::vector<rectangle> faces = detector(cimg);

    // 얼굴 랜드마크 추출
    std::vector<full_object_detection> shapes;
    for (unsigned long i = 0; i < faces.size(); ++i)
    {
        shapes.push_back(pose_model(cimg, faces[i]));
    }

    // 랜드마크 정보를 텍스트 파일로 저장
    ofstream landmarksFile("landmarks.txt");
    if (landmarksFile.is_open())
    {
        for (const auto& shape : shapes)
        {
            for (int j = 0; j < shape.num_parts(); ++j)
            {
                landmarksFile << shape.part(j).x() << " " << shape.part(j).y() << endl; // x, y 좌표 저장
            }
            landmarksFile << "-----" << endl; // 각 얼굴 랜드마크 구분
        }
        landmarksFile.close();
        cout << "Landmarks saved to landmarks.txt" << endl;
    }
    else
    {
        cerr << "Unable to open landmarks.txt for writing!" << endl;
    }

    // 결과 표시
    win.set_image(cimg);
    win.add_overlay(render_face_detections(shapes));

    // 결과를 보여주고 종료를 기다림
    cout << "Press any key to exit." << endl;
    win.wait_until_closed();
}

int main()
{
    // 입력 이미지 로드
    cv::Mat img = cv::imread("input_image.png");
    if (img.empty())
    {
        cerr << "Unable to load image!" << endl;
        return 1;
    }

    // 랜드마크 검출 및 txt파일 생성
    findLandmark(img);

    return 0;
}