#include "VCaptureJAI.h"

VCaptureJAI::VCaptureJAI(VCameraBase * in_camera_props, bool & isConnected) : VCapture()
{
	camera_props = in_camera_props;
	m_hFactory = NULL;
    m_hCam = NULL;
	m_CnvImageInfo.pImageBuffer = NULL;
	m_hThread = NULL;
	if(OpenFactoryAndCamera())
	{
		isConnected = true;
		subtitleBitmap = new wxBitmap(width, height);
	}
	else
	{
		isConnected = false;
	}
	middleValue = -1;
	countGetImage = 0;
}

bool VCaptureJAI::OpenFactoryAndCamera()
{
    J_STATUS_TYPE   retval;
    uint32_t        iSize;
    uint32_t        iNumDev;
    bool8_t         bHasChange;

    // Open factory
    retval = J_Factory_Open((const int8_t*)"" , &m_hFactory);
    if (retval != J_ST_SUCCESS)
    {
        LOG_ERROR(L"Не удалось открыть сервис JAI камер!");
        return FALSE;
    }
    LOG_INFO(L"Успешное открытие сервиса JAI камер");

    // Update camera list
    retval = J_Factory_UpdateCameraList(m_hFactory, &bHasChange);
    if (retval != J_ST_SUCCESS)
    {
        LOG_ERROR(L"Не удалось обновить список камер!");
        return FALSE;
    }
    LOG_INFO(L"Успешное обновление списка камер");

    // Get the number of Cameras
    retval = J_Factory_GetNumOfCameras(m_hFactory, &iNumDev);
    if (retval != J_ST_SUCCESS)
    {
        LOG_ERROR(L"Невозможно узнать количество камер!");
        return FALSE;
    }
    if (iNumDev == 0)
    {
        LOG_ERROR(L"Камеры не обнаружены");
        return FALSE;
    }

	int	iValidCamera = 0;
	for(int idx_camera = 0; idx_camera < iNumDev; idx_camera++)
	{
		// Get camera ID
		iSize = J_CAMERA_ID_SIZE;
		m_sCameraId[iValidCamera][0] = 0;
		retval = J_Factory_GetCameraIDByIndex(m_hFactory, idx_camera, m_sCameraId[iValidCamera], &iSize);
		if (retval != J_ST_SUCCESS)
		{
			LOG_ERROR(L"Невозможно получить ID камер!");
			iValidCamera++;
			continue;
		}

		char msg [512] = "";
		_snprintf(msg, 511, "Camera ID [%i]: %s\0", idx_camera, (const char *) m_sCameraId[iValidCamera]);
		LOG_INFO(string_to_wstring(msg).c_str());

		// Open camera
		// Ошибка -1005
		retval = J_Camera_Open(m_hFactory, m_sCameraId[iValidCamera], &m_hCam);
		if (retval != J_ST_SUCCESS)
		{
			char msg [512] = "";
			_snprintf(msg, 511, "Невозможно открыть камеру: %i", retval);
			LOG_ERROR(string_to_wstring(msg));
			continue;
		}
		LOG_INFO(L"Камера успешно открыта");

		int64_t int64Val;
		// Get Width from the camera
		retval = J_Camera_GetValueInt64(m_hCam, (int8_t*) NODE_NAME_WIDTH, &int64Val);
		if (retval != J_ST_SUCCESS)
		{
			LOG_ERROR(L"Невозможно получить ширину кадра!");
			iValidCamera++;
			continue;
		}
		else
			width = int64Val;

		// Get Height from the camera
		retval = J_Camera_GetValueInt64(m_hCam, (int8_t*) NODE_NAME_HEIGHT, &int64Val);
		if (retval != J_ST_SUCCESS)
		{
			LOG_ERROR(L"Невозможно получить высоту кадра");
			continue;
		}
		else
			height = int64Val;
    
		SIZE	ViewSize;
		ViewSize.cx = width;
		ViewSize.cy = height;
		// Get pixelformat from the camera
		int64_t pixelFormat;
		retval = J_Camera_GetValueInt64(m_hCam, (int8_t*) NODE_NAME_PIXELFORMAT, &pixelFormat);
		if (retval != J_ST_SUCCESS)
		{
			LOG_ERROR(L"Невозможно получить значение формата пиксела");
			continue;
		}

		// Calculate number of bits (not bytes) per pixel using macro
		int bpp = J_BitsPerPixel(pixelFormat);

		// Open stream
		retval = J_Image_OpenStream(m_hCam, 0, reinterpret_cast<J_IMG_CALLBACK_OBJECT>(this), reinterpret_cast<J_IMG_CALLBACK_FUNCTION>(&VCaptureJAI::StreamCBFunc), &m_hThread, (ViewSize.cx*ViewSize.cy*bpp)/8);
		if (retval != J_ST_SUCCESS)
		{
			LOG_ERROR(L"Невозможно открыть поток");
			continue;
		}
		LOG_INFO(L"Успешное открытие потока");

		// Start Acquision
		retval = J_Camera_ExecuteCommand(m_hCam, (int8_t*) NODE_NAME_ACQSTART);
		if (retval != J_ST_SUCCESS)
		{
			LOG_ERROR(L"Невозможно начать обработку кадра");
			continue;
		}
		return TRUE;
	}

    return FALSE;
}

void VCaptureJAI::StreamCBFunc(J_tIMAGE_INFO * pAqImageInfo)
{
        // Here we want to read a certain pixel value from the image:
        // In order to do so we need to convert the image from Raw to Image (to interpolate Bayer
        // if needed and unpack the Packed pixel formats as well)
 
        // Allocate the buffer to hold converted the image. (We only want to do this once for performance reasons)
        if (m_CnvImageInfo.pImageBuffer == NULL)
        {
            if (J_Image_Malloc(pAqImageInfo, &m_CnvImageInfo) != J_ST_SUCCESS)
                return;
        }

        if (m_CnvImageInfo.pImageBuffer != NULL)
        {
            // Convert the raw image to "normalized" image format
            if (J_Image_FromRawToImage(pAqImageInfo, &m_CnvImageInfo) == J_ST_SUCCESS)
            {
                // Now we can read the pixel value
                POINT pt;
                //EnterCriticalSection(&m_CriticalSection);
                pt.x = 10;
                pt.y = 10;
                //LeaveCriticalSection(&m_CriticalSection);

                PIXELVALUE pixelValue;

                if (J_Image_GetPixel(&m_CnvImageInfo, &pt, &pixelValue) == J_ST_SUCCESS)
                {
                    // Copy the pixel type and value so we can display it in the UI thread context
                    //EnterCriticalSection(&m_CriticalSection);
                    m_PixelType = m_CnvImageInfo.iPixelType;
                    m_PixelValue = pixelValue;
                    //LeaveCriticalSection(&m_CriticalSection);
                }
                else
                {
                    // Copy the pixel type and value so we can display it in the UI thread context
                    //EnterCriticalSection(&m_CriticalSection);
                    m_PixelType = m_CnvImageInfo.iPixelType;
                    pixelValue.PixelValueUnion.BGR48Type.RValue = 0;
                    pixelValue.PixelValueUnion.BGR48Type.GValue = 0;
                    pixelValue.PixelValueUnion.BGR48Type.BValue = 0;
                    m_PixelValue = pixelValue;
                    //LeaveCriticalSection(&m_CriticalSection);
                }
				int qqq = m_PixelValue.PixelValueUnion.Mono8Type.Value;
				qqq++;
            }
        }
 }

/// Деструктор
VCaptureJAI::~VCaptureJAI()
{
    J_STATUS_TYPE   retval;
    if (m_hCam)
    {
        // Close camera
        retval = J_Camera_Close(m_hCam);
        if (retval != J_ST_SUCCESS)
            LOG_ERROR(L"Невозможно завершить работу с камерой");
        m_hCam = NULL;
        LOG_INFO(L"Завершилась работа с камерой JAI");
    }
    if (m_hFactory)
    {
        // Close factory
        retval = J_Factory_Close(m_hFactory);
        if (retval != J_ST_SUCCESS)
            LOG_ERROR(L"Невозможно закрыть сервис JAI камер");
        m_hFactory = NULL;
		LOG_INFO(L"Успешно завершена работа с сервисом JAI камер");
    }
}

bool VCaptureJAI::IsConnected() const
{
	if(m_hCam)
		return true;
	else
		return false;
}

int VCaptureJAI::GetWidth() const
{
	if(m_hCam)
		return width;
	else
		return 0;
}

int VCaptureJAI::GetHeight() const
{
	if(m_hCam)
		return height;
	else
		return 0;
}

void VCaptureJAI::ProcessFrame()
{}

void VCaptureJAI::GetRawImage(unsigned char * image, int & imageLen)
{
	try
	{
		if (image != NULL && m_CnvImageInfo.iImageSize > 0 && m_CnvImageInfo.pImageBuffer != NULL)
		{
			memcpy((void*) image, (void*) m_CnvImageInfo.pImageBuffer, m_CnvImageInfo.iImageSize);
			imageLen = m_CnvImageInfo.iImageSize;
		}
	}
	catch (std::exception)
	{
		LOG_ERROR(L"Ошибка при попытке получить сырое изображение с JAI камеры");
	}
}

short VCaptureJAI::GetPixelValue(unsigned char * image, int x, int y)
{
	if(width > 0 && height > 0 && image)
		return (short) image[y * width + x];
	else
		return -1;
}

bool VCaptureJAI::FindZazor(unsigned char * image)
{
	bool findZazor = false;
	if(width > 0 && height > 0 && image)
	{
		#pragma omp parallel for
		for(int q = 0; q < width; q++)
		{
			float quadr = 0.0;
			float sum = 0.0;
			for(int j = 0; j < height; j++)
			{
				short pxlValue = GetPixelValue(image, q, j);
				quadr += pxlValue * pxlValue;
				sum += pxlValue;
			}
			float dispersio = (quadr - sum * sum/height)/(height - 1);
			float sko = sqrt(dispersio);
			if(sko < SKO_SEARCHING)
				middleValue = middleValue > (sum / height) ? middleValue : (sum / height);
		}

		if(middleValue > 0)
		{
			for(int j = int (width - WIDTH_SEARCHING)/2; j < int (width + WIDTH_SEARCHING)/2; j++)
			{
				for(int k = 0; k < height; k++)
				{
					short pxlValue = GetPixelValue(image, j, k);
					if(pxlValue < ((RANGE_PROCENTAGE/100) * SKO_SEARCHING))
					{
						findZazor = true;
						//Ищем в окрестности отклонение
						for(int l = int (width - WIDTH_SEARCHING)/2; l < int (width + WIDTH_SEARCHING)/2 && findZazor; l++)
						{
							if(l == j)
								continue;
							for(int m = k - 5; m < k + 5; m++)
							{
								if(m < 0)
									continue;
								pxlValue = GetPixelValue(image, l, m);
								if(pxlValue < ((RANGE_PROCENTAGE/100) * SKO_SEARCHING))
									findZazor = true;
								else
									findZazor = false;
							}
						}
					}
				}
			}
		}
		return findZazor;
	}
	else
		return false;
}