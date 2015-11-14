#pragma once

#include <boost/asio.hpp>
#include <wx/wx.h>
#include <vector>

//OpenCV
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "V4fWriter.h"
#include "dbmodule.h"

#include "dblib.h"
#include "VAprinfo.h"
#include "VCameraProperties.h"
#include "V4fFrame.h"
#include "V4fDataSet.h"
#include "VVideoOptions.h"

#define ID_V4F_VIEWER_SLIDER 500
#define ID_V4F_VIEWER_EXPALLIMG 501
#define ID_V4F_VIEWER_EXPCURIMG 502
#define ID_V4F_VIEWER_KMCTRL 503
#define ID_V4F_VIEWER_MCTRL 504
#define ID_V4F_VIEWER_LEFT 505
#define ID_V4F_VIEWER_RIGHT 506
#define ID_V4F_VIEWER_ABSCOORDCTRL 507
#define ID_V4F_VIEWER_KADRCTRL 508
#define ID_V4F_VIEWER_EXPAVI 509

class V4fViewerDialog : public wxDialog
{
	unsigned char * bufData;
	int bufDataLen;
	V4fDataSet * dataSet;
	
	wxMemoryDC *subtitleMemoryDC;
	wxBitmap * subtitleBitmap;

	wxSlider * slider;
	wxPanel * screen;
	//wxButton * exportAllImg;
	wxButton * exportCurrentImg;
	wxButton * exportToAVI;
	wxTextCtrl * kmCtrl, * mCtrl;		// Выставление кадра по км, м
	wxButton * leftCadre, * rightCadre;	// Сдвиг на 1 кадр (влево/вправо)

	VIBDatabasePtr db;
	VAprinfo info;					//Априорная информация

	wxBitmap mainBitmap;
	int currentPosSlider;
	long currentKm;
	float currentm;

public:
	//Конструктор
	V4fViewerDialog(wxWindow * parent, std::string fileName, VRegisteredDatabase * RegisteredDatabase);
	//Деструктор
	~V4fViewerDialog();
	//Обновление интерфейса пользователя
	void OnUpdateUI(wxUpdateUIEvent & event);
	void OnWayCoordUpdate(wxCommandEvent & event);
	void OnLeftButton(wxCommandEvent & event);
	void OnRightButton(wxCommandEvent & event);
	//Экспорт всех файлов в папку
	//void OnExportAllImg(wxCommandEvent & event);
	//Экспорт текущего кадра в файл (.jpg)
	void OnExportCurrentImg(wxCommandEvent & event);
	//Экспорт текущего кадра в .avi
	void OnExportToAvi(wxCommandEvent & event);
	//Рисование кадра
	void OnPaint(wxPaintEvent & event);
	DECLARE_EVENT_TABLE();
};