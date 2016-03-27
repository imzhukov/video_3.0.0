#include "VCapture.h"
#include "VCoordinatesRecord.h"
//#include "VCoordinatesThread.h"

/// ������� � ���� ����������
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
				//8-bit �����������. ����������� � RGB
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
			///��������� ����� � wxMemoryDC
			//������ ���������
			int heightSubtitle = VIDEO_OPTIONS().Value().heightSubtitle;
			wxImage image(GetWidth(), GetHeight(), (unsigned char*) bufData, true);
			wxBitmap bitmap(image);
			///��������� ���� � MemoryDC 
			finallyMemoryDC.SelectObjectAsSource(bitmap);
			//::EnterCriticalSection(&subtitleMemoryDC_access);
			///��������� � finallyMemoryDC subtitleBitmap ��� ��������� ���������
			subtitleMemoryDC->SelectObject(*subtitleBitmap);

			///������ ��������
			int width = subtitleBitmap->GetWidth(), height = subtitleBitmap->GetHeight();
			
			//����� ���� �������� ��������
			int yPos = GetHeight() - 40;

			//����� ���������
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
					LOG_ERROR(L"������������ ������� ������������ ���������");
					break;
			}
			subtitleMemoryDC->DrawRectangle(0, 0, GetWidth(), 2 * heightSubtitle);
			//��������� ������ ���������
			std::wstring subtitle = VIDEO_OPTIONS().Value().tmpltSubtitle;
				if(subtitle.find(L"%����")!=std::string::npos)
				{
					wchar_t dir_str [32] = L"";
					swprintf(dir_str, 31, L"%i", CURRENT_POSITION().Value().direction);
					subtitle.replace(subtitle.find(L"%����"), 5, dir_str);
				}
				if(subtitle.find(L"%����")!=std::string::npos)
					subtitle.replace(subtitle.find(L"%����"), 5, string_to_wstring(CURRENT_POSITION().Value().way).c_str());
				if(subtitle.find(L"%��")!=std::string::npos)
				{
					wchar_t km_str [15] = L"";
					swprintf(km_str, 14, L"%i", CURRENT_POSITION().Value().km);
					subtitle.replace(subtitle.find(L"%��"), 3, km_str);
				}
				if(subtitle.find(L"%�")!=std::string::npos)
				{
					wchar_t m_str [8] = L"";
					swprintf(m_str, 7, L"%.0f", CURRENT_POSITION().Value().m);
					subtitle.replace(subtitle.find(L"%�"), 2, m_str);
				}
				if(subtitle.find(L"%�������")!=std::string::npos)
				{
					if(*(CURRENT_POSITION().Value().peregon))
						subtitle.replace(subtitle.find(L"%�������"), 8, string_to_wstring(CURRENT_POSITION().Value().peregon).c_str());
					else
						subtitle.replace(subtitle.find(L"%�������"), 8, L"");
				}
				if(subtitle.find(L"%�����")!=std::string::npos)
				{
					time_t _now = time(NULL);
					localtime(&_now);
					VTime st(_now);
					subtitle.replace(subtitle.find(L"%�����"), 6, string_to_wstring(st.DateToStringISO()).c_str());
				}

				subtitleMemoryDC->SetFont(wxFont((int)(heightSubtitle * 0.6), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
				subtitleMemoryDC->DrawLabel(subtitle.c_str(), wxRect(0, (int)(-heightSubtitle * 0.1), width, heightSubtitle), wxALIGN_CENTER);

			finallyMemoryDC.Blit(0, 0, GetWidth(), heightSubtitle, subtitleMemoryDC, 0, 0);
			//::LeaveCriticalSection(&subtitleMemoryDC_access);

			///������������ ���������� ����������� ������� � ���������� �����
			
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
		LOG_ERROR(L"������ ��� ������ �����");
	}
}