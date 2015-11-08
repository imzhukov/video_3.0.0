#pragma once

#define SIG_EXCHANGE "ExChAnGe"

/// кадр для обмена
#pragma pack(push, 1)
struct ExchangeCadre
{
	char sign[8];       //сигнатура 'ExChAnGe'
	long km, m;         // координата
	long dir;           // направление
	char way[8];        // путь
	char peregon[80];   // перегон
	double absCoord;    // абс.к-та
	float floatm;       // метры, float
	float kurs;         // курсовой угол
	float vspeed;       // скорость вагона
	float width;        // шк
	float level;        // уровень
	float kren;         // крен
	float tangage;      // тангаж
	unsigned long dpp;  // счетчик тиков
	bool accurate;      // признак аккуратности координаты
};
#pragma pack(pop)

#define SIG_COORD_QUESTION "CXQUESTI"

#pragma pack(push, 1)
struct CoordQuestion
{
    char signature[8];
    unsigned long dpp;
};
#pragma pack(pop)

#define SIG_COORD_ANSWER "CXANSWER"

#pragma pack(push, 1)
struct CoordAnswer
{
    char signature[8];
    unsigned long dpp;
    bool has_data;
    double abs_coord;
    long km;            
    float m;
    long direction;
	char way[8];        // путь
    char dirname[80];   // имя направления
	char peregon[80];   // перегон
	long pch;           // номер пч
	char pchname[80];   // имя пч
};
#pragma pack(pop)