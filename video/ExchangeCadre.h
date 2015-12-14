#pragma once

#define SIG_EXCHANGE "ExChAnGe"

/// ���� ��� ������
#pragma pack(push, 1)
struct ExchangeCadre
{
	char sign[8];       //��������� 'ExChAnGe'
	long km, m;         // ����������
	long dir;           // �����������
	char way[8];        // ����
	char peregon[80];   // �������
	double absCoord;    // ���.�-��
	float floatm;       // �����, float
	float kurs;         // �������� ����
	float vspeed;       // �������� ������
	float width;        // ��
	float level;        // �������
	float kren;         // ����
	float tangage;      // ������
	unsigned long dpp;  // ������� �����
	bool accurate;      // ������� ������������ ����������
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
	char way[8];        // ����
    char dirname[80];   // ��� �����������
	char peregon[80];   // �������
	long pch;           // ����� ��
	char pchname[80];   // ��� ��
	int64_t start_time;	// ����� �������

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