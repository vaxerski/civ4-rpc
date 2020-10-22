#pragma once
#include <string>
#include "Discord/win32-static/include/discord_rpc.h"
#include <ctime>

#define CSTR(s) s.c_str()
#define CIV CvData::getInstance()

struct gameinfo {
	int m_iElapsedGameTurns;
	int m_iStartTurn;
	int m_iStartYear;
	int m_iEstimateEndTurn;
	int m_iTurnSlice;
	int m_iCutoffSlice;
	int m_iNumGameTurnActive;
	int m_iNumCities;
	int m_iTotalPopulation;
	int m_iTradeRoutes;
	int m_iFreeTradeCount;
	int m_iNoNukesCount;
	int m_iNukesExploded;
	int m_iMaxPopulation;
	int m_iMaxLand;
	int m_iMaxTech;
	int m_iMaxWonders;
	int m_iInitPopulation;
	int m_iInitLand;
	int m_iInitTech;
	int m_iInitWonders;
	int m_iAIAutoPlay;

	unsigned int m_uiInitialTime;

	bool m_bScoreDirty;
	bool m_bCircumnavigated;
	bool m_bDebugMode;
	bool m_bDebugModeCache;
	bool m_bFinalInitialized;
	bool m_bPbemTurnSent;
	bool m_bHotPbemBetweenTurns;
	bool m_bPlayerOptionsSent;
	bool m_bNukesValid;
};

struct RPCDATA {
	int turn;
	int points;
	int playersAlive;
	int playersTotal;
	std::string leader;
	std::string igDate;
	time_t startTime;
	std::string clientID = "518379213645021185";
	std::string iconKey = "icon";
};

class CvData
{
private:
    static CvData* instance;

    CvData();

	gameinfo gamedata;
	RPCDATA rpc;

public:
    static CvData* getInstance();
	
	RPCDATA* getRpc();
	gameinfo* getGameinfo();

	void updateTime();
	void updateRPC();
	void initRPC();
};