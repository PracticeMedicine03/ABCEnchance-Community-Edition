#pragma once
#include <metahook.h>
#include <map>
#include <vector>
#include <string>
#include <tier1/strtools.h>
#include "plugins.h"
#include "local.h"
#include "vguilocal.h"
#include "steam_api.h"
#include "player_info.h"
#include "scoreboard.h"
#include <Viewport.h>
#include <exportfuncs.h>

typedef int GLint;
#include "hud.h"
#include "weapon.h"
#include <CCustomHud.h>

CPlayerInfo CPlayerInfo::m_sPlayerInfo[SC_MAX_PLAYERS + 1];

#define UNDEFINEDTEAM_LOCALIZE_TOKEN "Scores_UndifinedTeam"
CPlayerInfo *GetThisPlayerInfo(){
	cl_entity_t* ent = gEngfuncs.GetLocalPlayer();
	if(ent)
		return GetPlayerInfo(ent->index);
	return GetPlayerInfo(0);
}
void CPlayerInfo::InitPlayerInfos() {
	// Set player info IDs
	for (int i = 1; i < SC_MAX_PLAYERS; i++)
		CPlayerInfo::m_sPlayerInfo[i].m_iIndex = i;
}
int CPlayerInfo::GetIndex(){
	return m_iIndex;
}
bool CPlayerInfo::IsConnected(){
	return m_bIsConnected;
}
bool CPlayerInfo::IsValid() {
	return m_bIsConnected && szName[0] != '\0';
}
const char *CPlayerInfo::GetName(){
	Assert(m_bIsConnected);
	return szName;
}
const char* CPlayerInfo::GetRealName() {
	return m_szRealName;
}
int CPlayerInfo::GetPing(){
	Assert(m_bIsConnected);
	return iPing;
}
int CPlayerInfo::GetPacketLoss(){
	Assert(m_bIsConnected);
	return iLoss;
}
bool CPlayerInfo::IsThisPlayer(){
	Assert(m_bIsConnected);
	return m_iIndex == gEngfuncs.GetLocalPlayer()->index;
}
const char *CPlayerInfo::GetModel(){
	Assert(m_bIsConnected);
	return szModel;
}
int CPlayerInfo::GetTopColor(){
	Assert(m_bIsConnected);
	return iTopColor;
}
int CPlayerInfo::GetBottomColor(){
	Assert(m_bIsConnected);
	return iBottomColor;
}
uint64 CPlayerInfo::GetSteamID64(){
	Assert(m_bIsConnected);
	return m_pSteamId.ConvertToUint64();
}
CSteamID* CPlayerInfo::GetSteamID() {
	return &m_pSteamId;
}
const char* CPlayerInfo::GetSteamIDString() {
	return m_pSteamId.Render();
}
const char* CPlayerInfo::GetSteamIDString64() {
	char buffer[64];
	Q_snprintf(buffer, sizeof(buffer), "%llu", m_pSteamId.ConvertToUint64());
	return buffer;
}
int CPlayerInfo::GetFrags(){
	Assert(m_bIsConnected);
	return m_ExtraInfo.frags;
}
int CPlayerInfo::GetDonor() {
	Assert(m_bIsConnected);
	return m_ExtraInfo.donor;
}
int CPlayerInfo::GetAdmin() {
	Assert(m_bIsConnected);
	return m_ExtraInfo.admin;
}
int CPlayerInfo::GetDeaths(){
	Assert(m_bIsConnected);
	return m_ExtraInfo.deaths;
}
int CPlayerInfo::GetHealth(){
	Assert(m_bIsConnected);
	return m_ExtraInfo.health;
}
int CPlayerInfo::GetArmor(){
	Assert(m_bIsConnected);
	return m_ExtraInfo.armor;
}
int CPlayerInfo::GetTeamNumber(){
	Assert(m_bIsConnected);
	return m_ExtraInfo.teamnumber;
}
const char *CPlayerInfo::GetTeamName(){
	Assert(m_bIsConnected);
	return m_ExtraInfo.teamname;
}
bool CPlayerInfo::IsSpectator(){
	return gCustomHud.IsSpectator(m_iIndex);
}
void CPlayerInfo::UpdatePing() {
	player_infosc_t* info = gCustomHud.GetPlayerInfoEx(m_iIndex);
	iPing = info->ping;
	iLoss = info->packet_loss;
}
CPlayerInfo *CPlayerInfo::Update(){
	player_infosc_t* info = gCustomHud.GetPlayerInfoEx(m_iIndex);
	bool bWasConnected = m_bIsConnected;
	bool bIsConnected = info != nullptr;
	m_bIsConnected = bIsConnected;

	if (bIsConnected != bWasConnected){
		// Player connected or disconnected
		memset(szName, 0, sizeof(szName));
		memset(m_szRealName, 0, sizeof(m_szRealName));
		memset(&m_ExtraInfo, 0, sizeof(m_ExtraInfo));
		m_pSteamId.Clear();
		if(g_pViewPort)
			g_pViewPort->GetScoreBoard()->UpdateOnPlayerInfo(GetIndex());
	}
	if (bIsConnected){
		if (!m_pSteamId.IsValid()) {
			// Player has no SteamID, update it
			if (m_iIndex == gEngfuncs.GetLocalPlayer()->index)
				m_pSteamId = SteamUser()->GetSteamID();
			else
				m_pSteamId = CSteamID(info->m_nSteamID);
			//RealNameGet
			if (m_pSteamId.IsValid())
				Q_strcpy(m_szRealName, SteamFriends()->GetFriendPersonaName(m_pSteamId));
		}
		iTopColor = info->topcolor;
		iBottomColor = info->bottomcolor;
		iPing = info->ping;
		iLoss = info->packet_loss;
		Q_strcpy(szName, info->name);
		hud_playerinfo_t* extraInfo = gCustomHud.GetPlayerHUDInfo(m_iIndex);
		m_ExtraInfo.armor = extraInfo->armor;
		m_ExtraInfo.frags = extraInfo->frags;
		m_ExtraInfo.deaths = extraInfo->death;
		m_ExtraInfo.health = extraInfo->health;
		m_ExtraInfo.donor = extraInfo->donors;
		m_ExtraInfo.teamnumber = extraInfo->team;
		m_ExtraInfo.admin = extraInfo->admin;
		Q_strcpy(m_ExtraInfo.teamname, GetTeamInfo(0)->GetNameByIndex(extraInfo->team));
	}
	return this;
}
void CPlayerInfo::UpdateAll() {
	for (int i = 1; i < SC_MAX_PLAYERS; i++)
		CPlayerInfo::m_sPlayerInfo[i].Update();
}
bool CPlayerInfo::HasRealName(){
	return m_szRealName[0] != '\0';
}
void CPlayerInfo::Reset(){
	memset(szName, 0, sizeof(szName));
	memset(m_szRealName, 0, sizeof(m_szRealName));
	memset(&m_ExtraInfo, 0, sizeof(m_ExtraInfo));
	m_bIsConnected = false;
	m_pSteamId.Clear();
}
void CPlayerInfo::ResetAll() {
	for (int i = 1; i < SC_MAX_PLAYERS; i++)
		CPlayerInfo::m_sPlayerInfo[i].Reset();
}

//-----------------------------------------------------

CTeamInfo CTeamInfo::m_sTeamInfo[SC_MAX_TEAMS + 1];
enum {
	CLASS_FORCE_NONE = -1,
	CLASS_NONE,
	CLASS_MACHINE,
	CLASS_PLAYER,
	CLASS_HUMAN_PASSIVE,
	CLASS_HUMAN_MILITARY,
	CLASS_ALIEN_MILITARY,
	CLASS_ALIEN_PASSIVE,
	CLASS_ALIEN_MONSTER,
	CLASS_ALIEN_PREY,
	CLASS_ALIEN_PREDATOR,
	CLASS_INSECT,
	CLASS_PLAYER_ALLY,
	CLASS_PLAYER_BIOWEAPON,
	CLASS_ALIEN_BIOWEAPON,
	CLASS_XRACE_PITDRONE,
	CLASS_XRACE_SHOCK,
	CLASS_TEAM1,
	CLASS_TEAM2,
	CLASS_TEAM3,
	CLASS_TEAM4,
	CLASS_BARNACLE = 99
};
#define CLASS_NONE_TOKEN "Team_None"
#define CLASS_PLAYER_TOKEN "Team_Player"
#define CLASS_OTHER_TOKEN "Team_Other"
#define CLASS_RED_TOKEN "Team_Red"
#define CLASS_BLUE_TOKEN "Team_Blue"
#define CLASS_GREEN_TOKEN "Team_Green"
#define CLASS_YELLOW_TOKEN "Team_Yellow"
const char* CTeamInfo::GetNameByIndex(uint i) {
	switch (i) {
		case CLASS_FORCE_NONE:
		case CLASS_NONE: {
			return CLASS_NONE_TOKEN;
			break;
		}
		case CLASS_PLAYER: {
			return CLASS_PLAYER_TOKEN;
			break;
		}
		case CLASS_TEAM1: {
			return CLASS_BLUE_TOKEN;
			break;
		}
		case CLASS_TEAM2: {
			return CLASS_RED_TOKEN;
			break;
		}
		case CLASS_TEAM3: {
			return CLASS_YELLOW_TOKEN;
			break;
		}
		case CLASS_TEAM4: {
			return CLASS_GREEN_TOKEN;
			break;
		}
		default:
		case CLASS_MACHINE:
		case CLASS_HUMAN_PASSIVE:
		case CLASS_HUMAN_MILITARY:
		case CLASS_ALIEN_MILITARY:
		case CLASS_ALIEN_PASSIVE:
		case CLASS_ALIEN_MONSTER:
		case CLASS_ALIEN_PREY:
		case CLASS_ALIEN_PREDATOR:
		case CLASS_INSECT:
		case CLASS_PLAYER_ALLY:
		case CLASS_PLAYER_BIOWEAPON:
		case CLASS_ALIEN_BIOWEAPON:
		case CLASS_XRACE_PITDRONE:
		case CLASS_XRACE_SHOCK:
		case CLASS_BARNACLE: {
			return CLASS_OTHER_TOKEN;
			break;
		}
	}
	return CLASS_OTHER_TOKEN;
}
int CTeamInfo::GetNumber(){
	return m_iNumber;
}
const char *CTeamInfo::GetName(){
	return m_Name;
}
bool CTeamInfo::IsScoreOverriden(){
	return m_bScoreOverriden;
}
int CTeamInfo::GetFrags(){
	Assert(m_bScoreOverriden);
	return m_iFrags;
}
int CTeamInfo::GetDeaths(){
	Assert(m_bScoreOverriden);
	return m_iDeaths;
}
void CTeamInfo::InitTeamInfos() {
	for(size_t i = 0; i < SC_MAX_TEAMS;i++){
		strcpy_s(m_sTeamInfo[i].m_Name, GetNameByIndex(i));
	}
}
void CTeamInfo::Reset(){
	m_bScoreOverriden = false;
	m_iFrags = 0;
	m_iDeaths = 0;
}
void CTeamInfo::ResetAll() {
	for (CTeamInfo t : m_sTeamInfo) {
		t.Reset();
	}
}
void CTeamInfo::UpdateAllTeams(){
	for (int i = 1; i <= SC_MAX_PLAYERS; i++){
		CPlayerInfo *pi = GetPlayerInfo(i);
		if (!pi->IsConnected())
			continue;
		if (pi->GetTeamNumber() < 0 || pi->GetTeamNumber() > SC_MAX_TEAMS)
			continue;
	}
}