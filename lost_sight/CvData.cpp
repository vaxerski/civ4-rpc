#include "CvData.h"
#include <Windows.h>

CvData* CvData::instance = 0;

CvData* CvData::getInstance()
{
    if (instance == 0)
    {
        instance = new CvData();
    }

    return instance;
}

CvData::CvData()
{

}

RPCDATA* CvData::getRpc() {
    return &rpc;
}

gameinfo* CvData::getGameinfo() {
    return &gamedata;
}

void CvData::updateTime() {
    rpc.startTime = time(0);
    return;
}

void CvData::updateRPC() {

    DiscordRichPresence dp;

	memset(&dp, 0, sizeof(dp));
    std::string state, details;

	if (rpc.turn == 0) { /* Quick check for turn 0, meaning prolly main menu */
		state = "In Main Menu";
        dp.state = CSTR(state);

        details = "In-Game";
        dp.details = CSTR(details);
    }
    else {
		state = std::to_string(rpc.playersAlive) + " of " + std::to_string(rpc.playersTotal) + " alive";
        dp.state = CSTR(state);

        details = "Turn " + std::to_string(rpc.turn) + "";
        dp.details = CSTR(details);
    }

    dp.startTimestamp = rpc.startTime;

    dp.endTimestamp = 0;

    dp.largeImageKey = CSTR(rpc.iconKey);
    dp.largeImageText = "";

    Discord_UpdatePresence(&dp);
}

void CvData::initRPC() {
    DiscordEventHandlers handlers;
    memset(&handlers, 0, sizeof(handlers));
    Discord_Initialize(rpc.clientID.c_str(), &handlers, 1, NULL);
}