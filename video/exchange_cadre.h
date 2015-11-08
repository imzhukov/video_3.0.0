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
};
#pragma pack(pop)