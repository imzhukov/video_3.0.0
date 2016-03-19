#include "VCapture.h"
#include "VCameraProperties.h"

//#define JAI_SDK_DYNAMIC_LOAD
#include <Jai_Factory.h>

#define NODE_NAME_WIDTH     "Width"
#define NODE_NAME_HEIGHT    "Height"
#define NODE_NAME_PIXELFORMAT   "PixelFormat"
#define NODE_NAME_ACQSTART  "AcquisitionStart"

const int WIDTH_SEARCHING = 20;
const float SKO_SEARCHING = 5.0;
const float RANGE_PROCENTAGE = 1.5;
const long MAX_COUNT_GET_IMAGE = 50;

class VCaptureJAI : public VCapture
{
	/// Свойства камеры
    FACTORY_HANDLE  m_hFactory;     // Factory Handle
    CAM_HANDLE      m_hCam;         // Camera Handle
	THRD_HANDLE     m_hThread;
	J_tIMAGE_INFO   m_CnvImageInfo;    // Image info structure
    PIXELVALUE        m_PixelValue;
    uint32_t        m_PixelType;
	CRITICAL_SECTION    m_CriticalSection;
    int8_t          m_sCameraId[4][J_CAMERA_ID_SIZE];    // Camera ID
	/// Высота изображения
	mutable int height;
	/// Ширина изображения
	mutable int width;
	/// Открытие камеры
	bool OpenFactoryAndCamera();
	// Функция получения и анализа изображения
	void StreamCBFunc(J_tIMAGE_INFO * pAqImageInfo);
	// Функция получения значения пиксела
	short GetPixelValue(unsigned char * image, int x, int y);
	// Функция обнаружения зазора
	bool FindZazor(unsigned char * image);
	// Среднее значение пиксела на рельсе (в отсутствие зазора)
	short middleValue;
	// Счётчик попыток получить кадр
	long countGetImage;
public:
	/// Конструктор
	VCaptureJAI(VCameraBase * in_camera_props, bool & isConnected);
	/// Деструктор
	~VCaptureJAI();
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
};