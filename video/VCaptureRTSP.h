#include "VCapture.h"
#include "VCameraProperties.h"

//OpenCV
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

/// Определять частоту кадров автоматически
#define FPS_AUTO -1

/// Кодек auto
#define CODEC_AUTO -1
/// Кодек RAW
#define CODEC_RAW 0
/// MPEG-1 codec
#define CODEC_PIM1 CV_FOURCC('P','I','M','1')
/// motion-jpeg codec
#define CODEC_MJPG CV_FOURCC('M','J','P','G')
/// MPEG-4.2 codec
#define CODEC_MP42 CV_FOURCC('M', 'P', '4', '2')
/// MPEG-4.3 codec
#define CODEC_DIV3 CV_FOURCC('D', 'I', 'V', '3')
/// MPEG-4 codec
#define CODEC_DIVX CV_FOURCC('D', 'I', 'V', 'X')
/// H263 codec
#define CODEC_H263 CV_FOURCC('U', '2', '6', '3')
/// H263I codec
#define CODEC_H263I CV_FOURCC('I', '2', '6', '3')
/// FLV1 codec
#define CODEC_FLV1 CV_FOURCC('F', 'L', 'V', '1') 
/// H264 codec
#define CODEC_H264 CV_FOURCC('x', '2', '6', '4')

class VCaptureRTSP : public VCapture
{
	/// Устройство захвата
	VideoCapture camera;
	/// Высота изображения
	mutable int height;
	/// Ширина изображения
	mutable int width;
	/// Текущий отображаемый кадр в формате IplImage (без субтитра)
	mutable Mat frame;
	mutable IplImage img;
	/// Кодек
	int codec;
	/// Частота кадров
	int frame_rate;
public:
	/// Конструктор
	VCaptureRTSP(VCameraBase * in_camera_props, bool & isConnected);
	/// Деструктор
	~VCaptureRTSP();
	/// true, если удалось успешно подлючиться
	bool IsConnected() const;
	/// Ширина кадра
	int GetWidth() const;
	/// Высота кадра
	int GetHeight() const;
	/// Обработка кадра с камеры и сохранение в отдельный буфер
	void ProcessFrame();
	/// Возвращает сырые данные
	void GetRawImage(unsigned char * image, int & imageLen);
	/// Возвращает cv кадр
	cv::Mat GetCvFrame();
};