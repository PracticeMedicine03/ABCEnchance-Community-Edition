#pragma once
#include <metahook.h>

#include <vgui/IImage.h>
#include <vgui/ISurface.h>
#include <vgui/ISystem.h>
#include <vgui/ILocalize.h>
#include <vgui_controls/Label.h>

#include "local.h"
#include "vguilocal.h"

#include <keydefs.h>
#include "vgui_controls/ImagePanel.h"
#include "vote.h"
#include "Viewport.h"
#include "BaseUI.h"

#include "plugins.h"
#include <regex>


#define VIEWPORT_VOTE_NAME "VotePanel"
#define VOTE_TITLE_LOCALIZE_TOKEN "#Vote_DefaultMessage"
#define VOTE_YES_LOCALIZE_TOKEN "#Vote_DefaultYes"
#define VOTE_NO_LOCALIZE_TOKEN "#Vote_DefaultNo"
#define VOTE_KILL_LOCALIZE_TOKEN "Vote_VoteKill"
#define VOTE_KICK_LOCALIZE_TOKEN "Vote_VoteKick"
#define VOTE_BAN_LOCALIZE_TOKEN "Vote_VoteBan"
#define VOTE_MAP_LOCALIZE_TOKEN "Vote_VoteMap"

CVotePanel::CVotePanel()
	: BaseClass(nullptr, VIEWPORT_VOTE_NAME){
	SetProportional(true);
	SetKeyBoardInputEnabled(false);
	SetMouseInputEnabled(false);
	SetScheme(g_pViewPort->GetBaseScheme());

	// Header labels
	m_pContentPanel = new vgui::Panel(this, "VoteContentPanel");
	m_pTitleLable = new vgui::Label(this, "VoteTitle", VOTE_TITLE_LOCALIZE_TOKEN);
	m_pContentLable = new vgui::Label(this, "VoteContent", "(None)");
	m_pYesLable = new vgui::Label(this, "VoteYesText", VOTE_YES_LOCALIZE_TOKEN);
	m_pNoLable = new vgui::Label(this, "VoteNoText", VOTE_NO_LOCALIZE_TOKEN);

	m_pYesIconImagePanel = new vgui::ImagePanel(this, "VoteYesIcon");
	m_pNoImagePanel = new vgui::ImagePanel(this, "VoteNoIcon");

	LoadControlSettings(VGUI2_ROOT_DIR "VotePanel.res");
	SetVisible(false);

	m_pHudVote = CREATE_CVAR("cl_hud_vote", "1", FCVAR_VALUE, nullptr);
	m_pHudVoteKeyYes = CREATE_CVAR("cl_hud_votekey_yes", "F1", FCVAR_VALUE, [](cvar_t* cvar) {
		g_pViewPort->GetVotePanel()->m_iYes = gameuifuncs->Key_KeyStringToKeyNum(cvar->string);
	});
	m_pHudVoteKeyNo = CREATE_CVAR("cl_hud_votekey_no", "F2", FCVAR_VALUE, [](cvar_t* cvar) {
		g_pViewPort->GetVotePanel()->m_iNo = gameuifuncs->Key_KeyStringToKeyNum(cvar->string);
	});
}
const char* CVotePanel::GetName(){
	return VIEWPORT_VOTE_NAME;
}
void CVotePanel::Reset(){
	if (IsVisible())
		ShowPanel(false);
	m_iYes = gameuifuncs->Key_KeyStringToKeyNum(m_pHudVoteKeyYes->string);
	m_iNo = gameuifuncs->Key_KeyStringToKeyNum(m_pHudVoteKeyNo->string);
}
void CVotePanel::ApplySchemeSettings(vgui::IScheme* pScheme){
	BaseClass::ApplySchemeSettings(pScheme);
	SetBgColor(GetSchemeColor("Vote.BackGoundColor", GetSchemeColor("Panel.BgColor", pScheme), pScheme));
	m_pContentPanel->SetBgColor(GetSchemeColor("Vote.ContentBgColor", GetSchemeColor("Panel.BgColor", pScheme), pScheme));
	m_pTitleLable->SetBgColor(GetSchemeColor("Vote.TitleBgColor", GetSchemeColor("Label.BgColor", pScheme), pScheme));
	m_pTitleLable->SetFgColor(GetSchemeColor("Vote.TitleFgColor", GetSchemeColor("Label.FgColor", pScheme), pScheme));
	m_pContentLable->SetFgColor(GetSchemeColor("Vote.ContentFgColor", GetSchemeColor("Label.FgColor", pScheme), pScheme));
	m_pYesLable->SetFgColor(GetSchemeColor("Vote.YesFgColor", GetSchemeColor("Label.FgColor", pScheme), pScheme));
	m_pNoLable->SetFgColor(GetSchemeColor("Vote.NoFgColor", GetSchemeColor("Label.FgColor", pScheme), pScheme));
}
void CVotePanel::ShowPanel(bool state){
	if (state == IsVisible())
		return;
	SetKeyBoardInputEnabled(state);
	SetVisible(state);
}
bool CVotePanel::IsVisible(){
	return BaseClass::IsVisible();
}
vgui::VPANEL CVotePanel::GetVPanel(){
	return BaseClass::GetVPanel();
}
void CVotePanel::SetParent(vgui::VPANEL parent){
	BaseClass::SetParent(parent);
}
bool CVotePanel::KeyCodeTyped(int code){
	if (IsVisible()) {
		if (code == m_iYes) {
			ServerCmd("voteyes");
			ShowPanel(false);
			return false;
		}
		else if (code == m_iNo) {
			ServerCmd("voteno");
			ShowPanel(false);
			return false;
		}
	}
	return true;
}
void CVotePanel::StartVote(char* szContent, char* szYes, char* szNo, int iVoteType){
	auto VotePattern = [&](std::string & content, std::string & szpattern, std::string & localtoken) {
		std::regex pattern(szpattern);
		std::smatch result;
		if (std::regex_match(content, result, pattern) && result.size() >= 2) {
			std::string playername = result[1];
			wchar_t wPlayerName[32] = { 0 };
			wchar_t wszContent[1024] = { 0 };
			vgui::localize()->ConvertANSIToUnicode(playername.c_str(), wPlayerName, sizeof(wPlayerName));
			vgui::localize()->ConstructString(wszContent, sizeof(wszContent), vgui::localize()->Find(localtoken.c_str()), 1, wPlayerName);
			m_pContentLable->SetText(wszContent);
		}
	};
	std::string strContent = szContent;
	std::string szPatten;
	std::string szToken;

#define BUFSIZE 256
	wchar_t buf[BUFSIZE];
	Q_UTF8ToUnicode(gameuifuncs->Key_NameForKey(m_iYes), buf, BUFSIZE);
	std::wstring wszYes = buf;
	wszYes += L" ";
	Q_UTF8ToUnicode(gameuifuncs->Key_NameForKey(m_iNo), buf, BUFSIZE);
	std::wstring wszNo = buf;
	wszNo += L" ";
	switch (iVoteType){
		case KILL: {
			szPatten = "Would you like to kill \"(.*)\"\\?";
			szToken = VOTE_KILL_LOCALIZE_TOKEN;
			break;
		}
		case KICK: {
			szPatten = "Would you like to kick \"(.*)\" from the server\\?";
			szToken = VOTE_KICK_LOCALIZE_TOKEN;
			break;
		}
		case BAN: {
			szPatten = "Do you wish to ban \"(.*)\" from the server\\?";
			szToken = VOTE_BAN_LOCALIZE_TOKEN;
			break;
		}
		case MAP: {
			szPatten = "Would you like to change the map to \"(.*)\"\\?";
			szToken = VOTE_MAP_LOCALIZE_TOKEN;
			break;
		}
		case SURVIVAL:
		case DONKNOW:{
			m_pContentLable->SetText(szContent);
			wszYes += vgui::localize()->Find(VOTE_YES_LOCALIZE_TOKEN);
			m_pYesLable->SetText(wszYes.c_str());
			wszNo += vgui::localize()->Find(VOTE_NO_LOCALIZE_TOKEN);
			m_pNoLable->SetText(wszNo.c_str());
			break;
		}
		case CUSTOM:
		default: {
			m_pContentLable->SetText(szContent);
			Q_UTF8ToUnicode(szYes, buf, BUFSIZE);
			wszYes += buf;
			m_pYesLable->SetText(wszYes.c_str());
			Q_UTF8ToUnicode(szNo, buf, BUFSIZE);
			wszNo += buf;
			m_pNoLable->SetText(wszNo.c_str());
			break;
		}
	}
	if (iVoteType >= KILL && iVoteType <= MAP) {
		VotePattern(strContent, szPatten, szToken);
		wszYes += vgui::localize()->Find(VOTE_YES_LOCALIZE_TOKEN);
		m_pYesLable->SetText(wszYes.c_str());
		wszNo += vgui::localize()->Find(VOTE_NO_LOCALIZE_TOKEN);
		m_pNoLable->SetText(wszNo.c_str());
	}
	ShowPanel(true);
}
void CVotePanel::EndVote(){
	ShowPanel(false);
}
bool CVotePanel::IsVoteEnable(){
	return m_pHudVote->value > 0;
}