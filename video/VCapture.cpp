#include "VCapture.h"
#include "VCoordinatesRecord.h"
//#include "VCoordinatesThread.h"

/// Выводит в кадр координату
void VCapture::ShowPosition(unsigned char * bufDataWithPos, int bufDataWithPosLen)
{
	try
	{
		if (GetWidth() > 0 && GetHeight() > 0)
		{
			if(3 * GetWidth() * GetHeight() > bufDataLen)
			{
				if(bufDataLen)
					delete [] bufData;
				bufData = new unsigned char [3 * GetWidth() * GetHeight()];
				bufDataLen = 3 * GetWidth() * GetHeight();
			}
			GetRawImage(bufData, bufDataLen);
			if(bufDataLen < 0)
				return;
			if(bufDataLen == (GetWidth() * GetHeight()))
			{
				//8-bit изображение. Конвертирую в RGB
				if(3 * GetWidth() * GetHeight() > bufDataForMonoConvertLen)
				{
					if(bufDataForMonoConvertLen)
						delete [] bufDataForMonoConvert;
					bufDataForMonoConvert = new unsigned char [3 * GetWidth() * GetHeight()];
					bufDataForMonoConvertLen = 3 * GetWidth() * GetHeight();
				}
				
				unsigned char * pSrc = bufData;
				unsigned char * pSrcEnd = pSrc + ( GetWidth() * GetHeight() );
				while ( pSrc < pSrcEnd )
				{
					bufDataForMonoConvert[0] = bufDataForMonoConvert[1] = bufDataForMonoConvert[2] = *pSrc++;
					bufDataForMonoConvert += 3;
				}
				bufDataForMonoConvert -= bufDataForMonoConvertLen;
				memcpy((void *) bufData, (void *) bufDataForMonoConvert, bufDataForMonoConvertLen);
				bufDataLen = bufDataForMonoConvertLen;
			}
			///Отрисовка кадра в wxMemoryDC
			//высота субтитров
			int heightSubtitle = VIDEO_OPTIONS().Value().heightSubtitle;
			wxImage image(GetWidth(), GetHeight(), (unsigned char*) bufData, true);
			wxBitmap bitmap(image);
			///Загружаем кадр в MemoryDC 
			finallyMemoryDC.SelectObjectAsSource(bitmap);
			//::EnterCriticalSection(&subtitleMemoryDC_access);
			///Добавляем в finallyMemoryDC subtitleBitmap для отрисовки субтитров
			subtitleMemoryDC->SelectObject(*subtitleBitmap);

			///Рисуем субтитры
			int width = subtitleBitmap->GetWidth(), height = subtitleBitmap->GetHeight();
			
			//узнаём куда рисовать субтитры
			int yPos = GetHeight() - 40;

			//сдвиг субтитров
			int shiftSubtitle = VIDEO_OPTIONS().Value().shiftSubtitle;
			switch(VIDEO_OPTIONS().Value().locationSubtitle)
			{
				case wxTOP:
					yPos = shiftSubtitle;
					break;
				case wxBOTTOM:
					yPos = GetHeight() - shiftSubtitle;
					break;
				default:
					LOG_ERROR(L"Неправильное задание расположения субтитров");
					break;
			}
			subtitleMemoryDC->DrawRectangle(0, 0, GetWidth(), 2 * heightSubtitle);
			//Формируем строку субтитров
			std::wstring subtitle = VIDEO_OPTIONS().Value().tmpltSubtitle;
				if(subtitle.find(L"%НАПР")!=std::string::npos)
				{
					wchar_t dir_str [32] = L"";
					swprintf(dir_str, 31, L"%i", CURRENT_POSITION().Value().direction);
					subtitle.replace(subtitle.find(L"%НАПР"), 5, dir_str);
				}
				if(subtitle.find(L"%ПУТЬ")!=std::string::npos)
					subtitle.replace(subtitle.find(L"%ПУТЬ"), 5, string_to_wstring(CURRENT_POSITION().Value().way).c_str());
				if(subtitle.find(L"%КМ")!=std::string::npos)
				{
					wchar_t km_str [15] = L"";
					swprintf(km_str, 14, L"%i", CURRENT_POSITION().Value().km);
					subtitle.replace(subtitle.find(L"%КМ"), 3, km_str);
				}
				if(subtitle.find(L"%М")!=std::string::npos)
				{
					wchar_t m_str [8] = L"";
					swprintf(m_str, 7, L"%.0f", CURRENT_POSITION().Value().m);
					subtitle.replace(subtitle.find(L"%М"), 2, m_str);
				}
				if(subtitle.find(L"%ПЕРЕГОН")!=std::string::npos)
				{
					if(*(CURRENT_POSITION().Value().peregon))
						subtitle.replace(subtitle.find(L"%ПЕРЕГОН"), 8, string_to_wstring(CURRENT_POSITION().Value().peregon).c_str());
					else
						subtitle.replace(subtitle.find(L"%ПЕРЕГОН"), 8, L"");
				}
				if(subtitle.find(L"%ВРЕМЯ")!=std::string::npos)
				{
					time_t _now = time(NULL);
					localtime(&_now);
					VTime st(_now);
					subtitle.replace(subtitle.find(L"%ВРЕМЯ"), 6, string_to_wstring(st.DateToStringISO()).c_str());
				}

				subtitleMemoryDC->SetFont(wxFont((int)(heightSubtitle * 0.6), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
				subtitleMemoryDC->DrawLabel(subtitle.c_str(), wxRect(0, (int)(-heightSubtitle * 0.1), width, heightSubtitle), wxALIGN_CENTER);

			finallyMemoryDC.Blit(0, 0, GetWidth(), heightSubtitle, subtitleMemoryDC, 0, 0);
			//::LeaveCriticalSection(&subtitleMemoryDC_access);

			///Конвертируем полученное изображение обратно в переменную кадра
			
			bitmap = finallyMemoryDC.GetAsBitmap();
			image = bitmap.ConvertToImage();
			memcpy((void *) bufDataWithPos, (void *) image.GetData(), 3*width*height);
			bufDataWithPosLen = 3*width*height;

			/*image = bitmap.ConvertToImage();
			GetRawImage(bufData, bufDataLen);
			if(bufDataLen <= (3*width*height))
				memcpy((void*) bufData, (void*) image.GetData(), 3*width*height);*/
		}
	}
	catch(VException & excp)
	{
		LOG_ERROR(L"Ошибка при записи кадра");
	}
}