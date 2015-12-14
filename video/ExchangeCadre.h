#pragma once

#define SIG_EXCHANGE "ExChAnGe"

/// кадр дл€ обмена
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
    char dirname[80];   // им€ направлени€
	char peregon[80];   // перегон
	long pch;           // номер пч
	char pchname[80];   // им€ пч
	int64_t start_time;	// врем€ поездки

	void SetCoordAnswer(const CoordAnswer & in_answer)
	{
		this->dpp = in_answer.dpp;
		this->has_data = in_answer.has_data;
		this->abs_coord = in_answer.abs_coord;
		this->km = in_answer.km;
		this->direction = in_answer.direction;
		memcpy(this->way, in_answer.way, 8);
		memcpy(this->dirname, in_answer.dirname, 80);
		memcpy(this->peregon, in_answer.peregon, 80);
		this->pch = in_answer.pch;
		this->start_time = in_answer.start_time;
	}
};
#pragma pack(pop)