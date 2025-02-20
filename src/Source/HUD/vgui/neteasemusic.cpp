#pragma once
#include <metahook.h>
#include <future>
#include <codecvt>
#include <locale>

#include "formatter.h"

#include "Task.h"

#include <vgui/IImage.h>
#include <vgui/ISurface.h>
#include <vgui/ISystem.h>
#include <vgui/ILocalize.h>
#include <vgui_controls/Label.h>
#include "vgui_controls/ImagePanel.h"
#include "vgui_controls/ImageClipPanel.h"
#include "vgui_controls/Button.h"
#include "VGUI2/avatar_image.h"

#include "local.h"
#include "vguilocal.h"

#include "Viewport.h"
#include "BaseUI.h"
#include "neteasemusic.h"
#include "FreeImage/FreeImage.h"
#include "curl.h"
#include "qrcodegen.h"

#pragma comment(lib,"FreeImage/FreeImage.lib")

#define VIEWPORT_NETEASEMUSIC_NAME "NeteasePanel"
#define VIEWPORT_NETEASEMUSICQR_NAME "NeteaseQRPanel"

class CAlbumImage : public vgui::IImage {
public:
	CAlbumImage() {
		m_nX = 0;
		m_nY = 0;
		m_wide = m_tall = 0;
		m_Color = Color(255, 255, 255, 255);
		m_iTextureID = -1;
	}
	// Call to Paint the image
	// Image will draw within the current panel context at the specified position
	virtual void Paint(void) {
		int posX = m_nX + m_offX;
		int posY = m_nY + m_offY;
		if (m_bValid)
		{
			vgui::surface()->DrawSetTexture(m_iTextureID);
			vgui::surface()->DrawSetColor(m_Color);
			vgui::surface()->DrawTexturedRect(posX, posY, posX + m_wide, posY + m_tall);
		}
	}
	// Set the position of the image
	virtual void SetPos(int x, int y)
	{
		m_nX = x;
		m_nY = y;
	}
	virtual void SetOffset(int x, int y)
	{
		m_offX = x;
		m_offY = y;
	}
	// Gets the size of the content
	virtual void GetContentSize(int& wide, int& tall)
	{
		wide = m_wide;
		tall = m_tall;
	}
	// Get the size the image will actually draw in (usually defaults to the content size)
	virtual void GetSize(int& wide, int& tall)
	{
		GetContentSize(wide, tall);
	}
	// Sets the size of the image
	virtual void SetSize(int wide, int tall) {
		m_wide = wide;
		m_tall = tall;
	}
	// Set the draw color
	virtual void SetColor(Color col)
	{
		m_Color = col;
	}
	bool IsValid() { return m_bValid; }
	int GetWide() { return m_wide; }
	int GetTall() { return m_tall; }
	virtual void InitFromRGBA(const byte* rgba, int width, int height) {
		if(m_iTextureID <= 0)
			m_iTextureID = vgui::surface()->CreateNewTextureID(true);
		vgui::surface()->DrawSetTextureRGBA(m_iTextureID, rgba, width, height, true, false);
		m_wide = width;
		m_tall = height;
		m_bValid = true;
	}
protected:
	Color m_Color;
	int m_iTextureID;
	int m_wide, m_tall;
	bool m_bValid;
	int m_nX, m_nY;
	int m_offX = 0, m_offY = 0;
};
static CAlbumImage* s_pAlbumImage;
static std::atomic<netease::CNeteaseMusicAPI*> s_pNeteaseApi;

class CCloudMusicCmdItem {
private:
	std::string name;
	std::list<CCloudMusicCmdItem> children;
	CCloudMusicCmdItem* parent = nullptr;
	bool terminal = false;
	void (*func)(CNeteasePanel* panel, CCloudMusicCmdItem* caller) = nullptr;
	size_t sub = 0;
public:
	std::string GetName() {
		return name;
	}
	CCloudMusicCmdItem* GetParent() {
		return parent;
	}
	size_t Sub() {
		return sub;
	}
	size_t Size() {
		return children.size();
	}
	bool IsTerminal() {
		return terminal;
	}
	void Excute(CNeteasePanel* panel) {
		if(func)
			func(panel, this);
		if (children.size() > 0) {
			for (auto iter = children.begin(); iter != children.end(); iter++) {
				std::string buf;
				CCloudMusicCmdItem* parent = (*iter).GetParent();
				while (parent != nullptr) {
					buf = " " + parent->GetName() + buf;
					parent = parent->GetParent();
				}
				buf += " " + (*iter).GetName() + "\n";
				ConsoleWriteline(const_cast<char*>(buf.c_str()));
			}
		}
	}
	void SetParent(CCloudMusicCmdItem* cmd) {
		parent = cmd;
		sub = cmd->Sub() + 1;
	}
	void SetupParent() {
		for (auto iter = children.begin(); iter != children.end(); iter++) {
			(*iter).SetParent(this);
			if ((*iter).Size() > 0)
				(*iter).SetupParent();
		}
	}
	CCloudMusicCmdItem* GetChild(const char* name) {
		for (auto iter = children.begin(); iter != children.end(); iter++) {
			if (!V_strcmp(iter->name.c_str(), name))
				return &(*iter);
		}
		return nullptr;
	}
	CCloudMusicCmdItem* GetChildOrExcute(const char* name, CNeteasePanel* panel) {
		for (auto iter = children.begin(); iter != children.end(); iter++) {
			if (!V_strcmp(iter->name.c_str(), name))
				return &(*iter);
		}
		Excute(panel);
		return nullptr;
	}
	CCloudMusicCmdItem(const char* n, void(*f)(CNeteasePanel* panel, CCloudMusicCmdItem* caller)) {
		name = n;
		func = f;
		terminal = true;
	}
	CCloudMusicCmdItem(const char* n, std::list<CCloudMusicCmdItem>c) {
		name = n;
		children = c;
		terminal = false;
		func = nullptr;
	}
	//if nothing match, default method
	CCloudMusicCmdItem(const char* n, std::list<CCloudMusicCmdItem>c, void(*f)(CNeteasePanel* panel, CCloudMusicCmdItem* caller)) {
		name = n;
		children = c;
		terminal = false;
		func = f;
	}
};
inline void SearchCaller(CNeteasePanel* panel, CCloudMusicCmdItem* caller, netease::SearchType type) {
	if (gEngfuncs.Cmd_Argc() >= caller->Sub() + 2) {
		char* keyword = gEngfuncs.Cmd_Argv(gEngfuncs.Cmd_Argc() - 1);
		panel->Search(keyword, type);
	}
	else
		CNeteasePanel::PrintF("#Netease_InvalidId", false);
}
static CCloudMusicCmdItem s_CloudMusicRoot = CCloudMusicCmdItem(
	"cloudmusic", {
		CCloudMusicCmdItem("login", {
			CCloudMusicCmdItem("qr", [](CNeteasePanel* panel, CCloudMusicCmdItem* caller) {panel->QRLogin(); }),
			CCloudMusicCmdItem("email", [](CNeteasePanel* panel, CCloudMusicCmdItem* caller) {
				if (gEngfuncs.Cmd_Argc() >= caller->Sub() + 3)
					panel->EmailLogin(gEngfuncs.Cmd_Argv(gEngfuncs.Cmd_Argc() - 2), gEngfuncs.Cmd_Argv(gEngfuncs.Cmd_Argc() - 1));
				else
					CNeteasePanel::PrintF("#Netease_InvalidId", false);
			 }),
			CCloudMusicCmdItem("send_sms", [](CNeteasePanel* panel, CCloudMusicCmdItem* caller) {
				if (gEngfuncs.Cmd_Argc() >= caller->Sub() + 2) {
					if(gEngfuncs.Cmd_Argc() >= caller->Sub() + 3)
						panel->SendSMS(gEngfuncs.Cmd_Argv(gEngfuncs.Cmd_Argc() - 2), std::atoi(gEngfuncs.Cmd_Argv(gEngfuncs.Cmd_Argc() - 1)));
					else
						panel->SendSMS(gEngfuncs.Cmd_Argv(gEngfuncs.Cmd_Argc() - 1), 86);
				}
				else
					CNeteasePanel::PrintF("#Netease_InvalidId", false);
			 }),
			CCloudMusicCmdItem("phone_passwd", [](CNeteasePanel* panel, CCloudMusicCmdItem* caller) {
				if (gEngfuncs.Cmd_Argc() >= caller->Sub() + 3) {
					if (gEngfuncs.Cmd_Argc() >= caller->Sub() + 4)
						panel->PhoneLogin(gEngfuncs.Cmd_Argv(gEngfuncs.Cmd_Argc() - 3), gEngfuncs.Cmd_Argv(gEngfuncs.Cmd_Argc() - 2), std::atoi(gEngfuncs.Cmd_Argv(gEngfuncs.Cmd_Argc() - 1)));
					else
						panel->PhoneLogin(gEngfuncs.Cmd_Argv(gEngfuncs.Cmd_Argc() - 2), gEngfuncs.Cmd_Argv(gEngfuncs.Cmd_Argc() - 1), 86);
				}
				else
					CNeteasePanel::PrintF("#Netease_InvalidId", false);
			 }),
			CCloudMusicCmdItem("phone_sms", [](CNeteasePanel* panel, CCloudMusicCmdItem* caller) {
				if (gEngfuncs.Cmd_Argc() >= caller->Sub() + 3) {
					if (gEngfuncs.Cmd_Argc() >= caller->Sub() + 4)
						panel->SMSLogin(gEngfuncs.Cmd_Argv(gEngfuncs.Cmd_Argc() - 3), gEngfuncs.Cmd_Argv(gEngfuncs.Cmd_Argc() - 2), std::atoi(gEngfuncs.Cmd_Argv(gEngfuncs.Cmd_Argc() - 1)));
					else
						panel->SMSLogin(gEngfuncs.Cmd_Argv(gEngfuncs.Cmd_Argc() - 2), gEngfuncs.Cmd_Argv(gEngfuncs.Cmd_Argc() - 1), 86);
				}
				else
					CNeteasePanel::PrintF("#Netease_InvalidId", false);
			 })
		}),
		CCloudMusicCmdItem("control", {
			CCloudMusicCmdItem("stop", [](CNeteasePanel* panel, CCloudMusicCmdItem* caller) {
				panel->StopMusic();
				panel->ShowPanel(false);
			}),
			CCloudMusicCmdItem("next", [](CNeteasePanel* panel, CCloudMusicCmdItem* caller) { panel->NextMusic(); }),
			CCloudMusicCmdItem("pause", [](CNeteasePanel* panel, CCloudMusicCmdItem* caller) { panel->PauseMusic(); }),
			CCloudMusicCmdItem("info", [](CNeteasePanel* panel, CCloudMusicCmdItem* caller) {
				auto music = panel->GetNowPlaying();
				if (music)
					panel->PrintF("#Netease_NowPlaying", false, std::to_string(music->id).c_str(), 
						music->name.c_str(), music->GetArtists().c_str(), music->al->name.c_str(), 
						music->copyright ? "Yes" : "No");
				else
					panel->PrintF("#Netease_NotPlaying", false);
			})
		}),
		CCloudMusicCmdItem("play", {
			CCloudMusicCmdItem("song", [](CNeteasePanel* panel, CCloudMusicCmdItem* caller) {
				char* end;
				netease::neteaseid_t id = std::strtoull(gEngfuncs.Cmd_Argv(gEngfuncs.Cmd_Argc() - 1), &end, 10);
				if (id == 0)
					CNeteasePanel::PrintF("#Netease_InvalidId", false);
				else
					panel->PlayMusic(id);
			}),
			CCloudMusicCmdItem("playlist", [](CNeteasePanel* panel, CCloudMusicCmdItem* caller) {
				char* end;
				netease::neteaseid_t id = std::strtoull(gEngfuncs.Cmd_Argv(gEngfuncs.Cmd_Argc() - 1), &end, 10);
				if (id == 0)
					CNeteasePanel::PrintF("#Netease_InvalidId", false);
				else
					panel->PlayList(id);
			}),
			CCloudMusicCmdItem("dj", [](CNeteasePanel* panel, CCloudMusicCmdItem* caller) {
				char* end;
				netease::neteaseid_t id = std::strtoull(gEngfuncs.Cmd_Argv(gEngfuncs.Cmd_Argc() - 1), &end, 10);
				if (id == 0)
					CNeteasePanel::PrintF("#Netease_InvalidId", false);
				else
					panel->PlayDj(id);
			}),
			CCloudMusicCmdItem("radio", [](CNeteasePanel* panel, CCloudMusicCmdItem* caller) {
				char* end;
				netease::neteaseid_t id = std::strtoull(gEngfuncs.Cmd_Argv(gEngfuncs.Cmd_Argc() - 1), &end, 10);
				if (id == 0)
					CNeteasePanel::PrintF("#Netease_InvalidId", false);
				else
					panel->PlayRadio(id);
			})
		}),
		CCloudMusicCmdItem("my", {
			CCloudMusicCmdItem("recommend", [](CNeteasePanel* panel, CCloudMusicCmdItem* caller) {panel->PlayRecommendMusic(); }),
			CCloudMusicCmdItem("fm", [](CNeteasePanel* panel, CCloudMusicCmdItem* caller) {panel->PlayFM(); })
		},[](CNeteasePanel* panel, CCloudMusicCmdItem* caller) {
				netease::CMy* my = panel->GetNowUser();
				if (my)
					CNeteasePanel::PrintF("#Netease_MyInfo", false, my->name.c_str(), my->signature.c_str(), my->vip ? "Yes" : "No");
				else
					CNeteasePanel::PrintF("#Netease_NotLogin", false);
		}
		),
		CCloudMusicCmdItem("search", {
			CCloudMusicCmdItem("song", [](CNeteasePanel* panel, CCloudMusicCmdItem* caller) {SearchCaller(panel, caller, netease::ST_SONG);}),
			CCloudMusicCmdItem("album", [](CNeteasePanel* panel, CCloudMusicCmdItem* caller) {SearchCaller(panel, caller, netease::ST_ALBUM);}),
			CCloudMusicCmdItem("artist", [](CNeteasePanel* panel, CCloudMusicCmdItem* caller) {SearchCaller(panel, caller, netease::ST_ARTIST);}),
			CCloudMusicCmdItem("list", [](CNeteasePanel* panel, CCloudMusicCmdItem* caller) {SearchCaller(panel, caller, netease::ST_PLAYLIST);}),
			CCloudMusicCmdItem("user", [](CNeteasePanel* panel, CCloudMusicCmdItem* caller) {SearchCaller(panel, caller, netease::ST_USER);}),
			CCloudMusicCmdItem("radio", [](CNeteasePanel* panel, CCloudMusicCmdItem* caller) {SearchCaller(panel, caller, netease::ST_RADIO);}),
		}),
	}
);

CNeteasePanel::CNeteasePanel()
	: BaseClass(nullptr, VIEWPORT_NETEASEMUSIC_NAME) {
	SetProportional(true);
	SetKeyBoardInputEnabled(false);
	SetMouseInputEnabled(false);
	SetScheme(g_pViewPort->GetBaseScheme());

	m_pMusicNameLable = new vgui::Label(this, "MusicName", "");
	m_pArtistNameLable = new vgui::Label(this, "ArtistName", "");
	m_pLyricLable = new vgui::Label(this, "Lyric", "");
	m_pTranslatedLyricLable = new vgui::Label(this, "TransLyric", "");
	m_pLyricLableHighlight = new vgui::Label(this, "LyricHighlight", "");
	m_pTranslatedLyricLableHighlight = new vgui::Label(this, "TransLyricHighlight", "");

	m_pTimeLable = new vgui::Label(this, "Timer", "");
	m_pMaxTimeLable = new vgui::Label(this, "MaxTime", "");
	m_pProgressBackgroundPanel = new vgui::ImagePanel(this, "ProgressBackground");
	m_pProgressLable = new vgui::ImageClipPanel(this, "Progress");

	m_pAlbumPanel = new vgui::ImagePanel(this, "Album");
	m_pLoginPanel = new CQRLoginPanel(this->GetParent(), VIEWPORT_NETEASEMUSICQR_NAME);
	s_pAlbumImage = new CAlbumImage();

	m_pQuality = CREATE_CVAR("cl_netease_quality", "0", FCVAR_VALUE, [](cvar_t* cvar) {
		cvar->value = std::clamp<int>(cvar->value, 0, 4);
	});
	m_pVolume = CREATE_CVAR("cl_netease_volume", "1", FCVAR_VALUE, [](cvar_t* cvar) {
		cvar->value = std::clamp<float>(cvar->value, 0.0f, 1.0f);
		g_pViewPort->GetMusicPanel()->SetVolume(cvar->value);
	});
	m_pSearchCount = CREATE_CVAR("cl_netease_searchcount", "5", FCVAR_VALUE, [](cvar_t* cvar) {
		cvar->value = std::max<int>(cvar->value, 1);
	});

	s_pNeteaseApi = new netease::CNeteaseMusicAPI();
	char buf[MAX_PATH];
	vgui::filesystem()->GetLocalPath("abcenchance/", buf, MAX_PATH);
	V_snprintf(buf, "%sneteasecookie", buf);
	netease::CNeteaseMusicAPI::SetCookie(buf);

	LoadControlSettings(VGUI2_ROOT_DIR "NeteasePanel.res");
	SetVisible(false);

	GetMyInfo();

	s_CloudMusicRoot.SetupParent();
	ADD_COMMAND("cloudmusic", []() {
		CCloudMusicCmdItem* pCmd = &s_CloudMusicRoot;
		int argc = gEngfuncs.Cmd_Argc();
		for (int index = 1; index < argc; index++) {
			char* arg = gEngfuncs.Cmd_Argv(index);
			if (pCmd)
				pCmd = pCmd->GetChildOrExcute(arg, g_pViewPort->GetMusicPanel());
			if (!pCmd)
				break;
		}
		if (pCmd)
			pCmd->Excute(g_pViewPort->GetMusicPanel()); 
	});
}
const char* CNeteasePanel::GetName(){
	return VIEWPORT_NETEASEMUSIC_NAME;
}
void CNeteasePanel::Reset(){
	if (IsVisible())
		ShowPanel(false);
}
void CNeteasePanel::ApplySchemeSettings(vgui::IScheme* pScheme){
	BaseClass::ApplySchemeSettings(pScheme);
	SetBgColor(GetSchemeColor("Music.BackGoundColor", GetSchemeColor("Frame.BgColor", pScheme), pScheme));
	m_pMusicNameLable->SetFgColor(GetSchemeColor("Music.MusicFgColor", GetSchemeColor("Lable.FgColor", pScheme), pScheme));
	m_pArtistNameLable->SetFgColor(GetSchemeColor("Music.ArtistFgColor", GetSchemeColor("Lable.FgColor", pScheme), pScheme));
	m_pTimeLable->SetFgColor(GetSchemeColor("Music.NumberFgColor", GetSchemeColor("Lable.FgColor", pScheme), pScheme));
	m_pMaxTimeLable->SetFgColor(GetSchemeColor("Music.NumberFgColor", GetSchemeColor("Lable.FgColor", pScheme), pScheme));
	m_pLyricLable->SetFgColor(GetSchemeColor("Music.LyricFgColor", GetSchemeColor("Lable.FgColor", pScheme), pScheme));
	m_pTranslatedLyricLable->SetFgColor(GetSchemeColor("Music.TransLyricFgColor", GetSchemeColor("Lable.FgColor", pScheme), pScheme));
	m_pLyricLableHighlight->SetFgColor(GetSchemeColor("Music.LyricHighLightFgColor", GetSchemeColor("Lable.FgColor", pScheme), pScheme));
	m_pTranslatedLyricLableHighlight->SetFgColor(GetSchemeColor("Music.TransLyricHighLightFgColor", GetSchemeColor("Lable.FgColor", pScheme), pScheme));
	m_pLoginPanel->SetBgColor(GetSchemeColor("Music.BackGoundColor", GetSchemeColor("Frame.BgColor", pScheme), pScheme));
}
void CNeteasePanel::ShowPanel(bool state){
	if (state == IsVisible())
		return;
	SetKeyBoardInputEnabled(state);
	SetVisible(state);
}
bool CNeteasePanel::IsVisible(){
	return BaseClass::IsVisible();
}
vgui::VPANEL CNeteasePanel::GetVPanel(){
	return BaseClass::GetVPanel();
}
void CNeteasePanel::SetParent(vgui::VPANEL parent){
	BaseClass::SetParent(parent);
}

void CNeteasePanel::Think() {
	if (m_pPlaying != nullptr && m_pChannel.Valid()) {
		ShowPanel(true);
		if (m_bPaused)
			return;
		size_t pos;
		m_pChannel.GetPosition(&pos, FMOD_TIMEUNIT_MS);
		if (pos < m_pPlaying->duration) {
			char buffer[MAX_PATH];
			V_snprintf(buffer, "%02d:%02d", pos / 60000, pos % 60000 / 1000);
			m_pTimeLable->SetText(buffer);
			float flRatio = static_cast<float>(pos) / static_cast<float>(m_pPlaying->duration);
			m_pProgressLable->SetWide(static_cast<float>(m_pProgressBackgroundPanel->GetWide()) * flRatio);
			//lyric
			if (m_pLyric != nullptr && m_pLyric->Size() > 0) {
				auto lrc = m_pLyric->LyricAt(pos);
				std::wstring szLrc = lrc.lyric;
				m_pLyricLable->SetText(szLrc.c_str());
				szLrc.resize(szLrc.size() * std::clamp(1.0f - (static_cast<float>(lrc.end_time - pos) / (lrc.end_time - lrc.start_time)), 0.0f, 1.0f));
				m_pLyricLableHighlight->SetText(szLrc.c_str());
			}
			else {
				m_pLyricLable->SetText("");
				m_pLyricLableHighlight->SetText("");
			}
			if (m_pTransLyric != nullptr && m_pTransLyric->Size() > 0) {
				auto lrc = m_pTransLyric->LyricAt(pos);
				std::wstring szLrc = lrc.lyric;
				m_pTranslatedLyricLable->SetText(szLrc.c_str());
				szLrc.resize(szLrc.size() * std::clamp(1.0f - (static_cast<float>(lrc.end_time - pos) / (lrc.end_time - lrc.start_time)), 0.0f, 1.0f));
				m_pTranslatedLyricLableHighlight->SetText(szLrc.c_str());
			}
			else {
				m_pTranslatedLyricLable->SetText("");
				m_pTranslatedLyricLableHighlight->SetText("");
			}
		}
		else
			NextMusic();
	}
}
size_t append(void* ptr, size_t size, size_t nmemb, void* user) {
	std::vector<byte>* p = (std::vector<byte>*)user;
	auto cs = p->size();
	p->resize(cs + size * nmemb);
	memcpy(p->data() + cs, ptr, size * nmemb);
	return size * nmemb;
}
std::vector<byte> DownLoad(const std::string& url) {
	std::vector<byte> retdata;
	void* curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_HEADER, 0);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_READFUNCTION, nullptr);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &append);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &retdata);
		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
		curl_easy_setopt(curl, CURLOPT_COOKIEFILE, netease::CNeteaseMusicAPI::CokkieInPath().c_str());
		curl_easy_setopt(curl, CURLOPT_COOKIEJAR, netease::CNeteaseMusicAPI::CookieOutPath().c_str());
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 6); // set transport and time out time  
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 6);
		curl_easy_perform(curl);
	}
	curl_easy_cleanup(curl);
	return retdata;
}
const char* g_aryMusicQuality[] = {
	"standard",
	"higher",
	"exhigh",
	"lossless",
	"hires"
};
void CNeteasePanel::Search(const char* keyword, netease::SearchType type) {
	GetTaskManager()->Add(std::async([](const char* keyword, netease::SearchType type, int limit) -> std::any {
		return s_pNeteaseApi.load()->Search(keyword, type, limit, 0);
		}, keyword, type, m_pSearchCount->value))->ContinueWith([](const std::any& anyresult) {
			if (anyresult.type() == typeid(std::vector<std::shared_ptr<netease::CSearchResult>>)) {
				auto result = std::any_cast<std::vector<std::shared_ptr<netease::CSearchResult>>>(anyresult);
				char buffer[512];
				std::string buf;
				for (auto iter = result.begin(); iter != result.end(); iter++) {
					netease::CSearchResult* ret = (*iter).get();
					if (ret->extra.size() > 0)
						V_snprintf(buffer, "#[%llu] %s - %s : %s\n", ret->id, ret->name.c_str(), ret->extra.c_str(), ret->copyright ? "[VIP]" : "[Free]");
					else
						V_snprintf(buffer, "#[%llu] %s : %s\n", ret->id, ret->name.c_str(), ret->copyright ? "[VIP]" : "[Free]");
					buf += buffer;
				}
				PrintF("#Netease_SearchResult", false, buf.c_str());
			}
			})->Start();
}
void CNeteasePanel::PlayMusic(netease::neteaseid_t  id){
	SetPlayerState(PLAYSTATE::NORMAL);
	m_aryPlayList.clear();
	AddToList(id);
	PlayListMusic();
}
void CNeteasePanel::PlayDj(netease::neteaseid_t  id) {
	SetPlayerState(PLAYSTATE::NORMAL);
	m_aryPlayList.clear();
	AddToList(id, CNeteasePanel::DJ);
	PlayListMusic();
}
void CNeteasePanel::PlayRadio(netease::neteaseid_t id){
	SetPlayerState(PLAYSTATE::NORMAL);
	m_aryPlayList.clear();
	GetTaskManager()->Add(std::async([](netease::neteaseid_t id)->std::any {
		return s_pNeteaseApi.load()->GetRadioSongs(id);
	}, id))->ContinueWith([panel = this](std::any anyList) {
			if (anyList.type() == typeid(std::vector<std::shared_ptr<netease::CDjMusic>>)) {
				auto list = std::any_cast<std::vector<std::shared_ptr<netease::CDjMusic>>>(anyList);
				for (auto iter = list.begin(); iter != list.end(); iter++) {
					panel->AddToList((*iter)->id, CNeteasePanel::DJ);
				}
				panel->PlayListMusic();
			}
	})->Start();
}
void CNeteasePanel::PlayList(netease::neteaseid_t id){
	SetPlayerState(PLAYSTATE::NORMAL);
	m_aryPlayList.clear();
	GetTaskManager()->Add(std::async([](netease::neteaseid_t id)->std::any{
		return s_pNeteaseApi.load()->GetPlayList(id);
	}, id))->ContinueWith([panel = this](std::any anyList) {
		if (anyList.type() == typeid(std::shared_ptr<netease::CPlayList>)) {
			auto list = std::any_cast<std::shared_ptr<netease::CPlayList>>(anyList);
			for (auto iter = list->mucics.begin(); iter != list->mucics.end(); iter++) {
				panel->AddToList(*iter);
			}
			panel->PlayListMusic();
		}
	})->Start();
}
void CNeteasePanel::PlayRecommendMusic(){
	if (m_pLogined == nullptr) {
		PrintF("#Netease_NotLogin", false);
		return;
	}
	SetPlayerState(PLAYSTATE::NORMAL);
	StopMusic();
	m_aryPlayList.clear();
	std::vector<netease::neteaseid_t> list = m_pLogined->GetDailyRecommend();
	for (auto iter = list.begin(); iter != list.end(); iter++) {
		AddToList(*iter);
	}
	PlayListMusic();
	PrintF("#Netease_PlayingRecommend", false, m_pLogined->name.c_str());
}
void CNeteasePanel::PlayFM(){
	if (m_pLogined == nullptr) {
		PrintF("#Netease_NotLogin", false);
		return;
	}
	SetPlayerState(PLAYSTATE::FM);
	StopMusic();
	m_aryPlayList.clear();
	RenewFM(true);
	PrintF("#Netease_PlayingFM", false, m_pLogined->name.c_str());
}
void CNeteasePanel::RenewFM(bool play){
	m_bRenewingFM = true;
	GetTaskManager()->Add(std::async([](std::shared_ptr<netease::CMy> my) -> std::any{
		return my->GetFM();
	}, m_pLogined))->ContinueWith([panel = this, play](std::any anyIds) {
		if (anyIds.type() == typeid(std::vector<netease::neteaseid_t>)) {
			auto list = std::any_cast<std::vector<netease::neteaseid_t>>(anyIds);
			for (auto iter = list.begin(); iter != list.end(); iter++) {
				panel->AddToList(*iter);
			}
			if(play)
				panel->PlayListMusic();
			panel->m_bRenewingFM = false;
		}
	})->Start();
}

void CNeteasePanel::StopMusic(){
	if (m_pChannel.Valid())
		m_pChannel.Stop();
	if (m_pSound.Valid())
		m_pSound.Release();
	m_pPlaying = nullptr;
}
void CNeteasePanel::SetVolume(float vol) {
	if (m_pChannel.Valid())
		m_pChannel.SetVolume(vol);
}

template<typename... Args>
void CNeteasePanel::PrintF(const char* str, bool dev, const Args&& ...args){
	const static auto wchartoutf = [](const std::wstring& in_wStr) {
		int nNeedChars = WideCharToMultiByte(CP_UTF8, 0, in_wStr.c_str(), -1, 0, 0, 0, 0);
		if (nNeedChars > 0) {
			std::vector<char> temp(nNeedChars);
			::WideCharToMultiByte(CP_UTF8, 0, in_wStr.c_str(), -1, &temp[0], nNeedChars, 0, 0);
			return std::string(&temp[0]);
		}
		return std::string();
	};
	std::string format;
	if (str[0] == '#')
		format = wchartoutf(vgui::localize()->Find(str)).c_str();
	else
		format = str;
	
	format = formatter::format(format, args...);
	format = "[NeteaseApi] " + format + "\n";
	if(dev)
		gEngfuncs.Con_DPrintf(const_cast<char*>(format.c_str()));
	else
		gEngfuncs.Con_Printf(const_cast<char*>(format.c_str()));
}
netease::CMusic* CNeteasePanel::GetNowPlaying(){
	return m_pPlaying.get();
}
netease::CMy* CNeteasePanel::GetNowUser(){
	return m_pLogined.get();
}
void CNeteasePanel::SetPlayerState(PLAYSTATE state){
	m_pNowState = state;
}

struct music_obj {
	std::shared_ptr <netease::CMusic> music;
	std::shared_ptr<netease::CLyric> lyric;
	int id;
	std::vector<byte> musicData;
	byte* album;
	size_t album_h;
	size_t album_w;
};
void CNeteasePanel::PlayListMusic(){
	if (m_bPendingMusic)
		return;
	m_bPendingMusic = true;
	//Clear lyric
	m_pLyricLable->SetText("");
	m_pLyricLableHighlight->SetText("");
	m_pTranslatedLyricLable->SetText("");
	m_pTranslatedLyricLableHighlight->SetText("");

	if (m_aryPlayList.size() == 0)
		return;
	if (m_pPlaying != nullptr)
		StopMusic();
	auto item = *m_aryPlayList.begin();
	m_aryPlayList.pop_front();
	GetTaskManager()->Add(std::async([](std::shared_ptr<PlayItem> playitem, size_t quality)->std::any {
		static music_obj obj;
		//Load Music
		std::shared_ptr <netease::CMusic> music;
		switch (playitem->type){
			case CNeteasePanel::PLAYTYPE::DJ: {
				music = s_pNeteaseApi.load()->GetDjSongDetail(playitem->id); 
				break;
			}
			case CNeteasePanel::PLAYTYPE::MUSIC:
			default: {
				music = s_pNeteaseApi.load()->GetSongDetail(playitem->id);
				break;
			}
		}
		if (music == nullptr)
			return nullptr;
		obj.musicData = DownLoad(music->GetPlayUrl(g_aryMusicQuality[quality], "flac"));
		//Load Album
		std::vector<byte> imageData = DownLoad(music->al->picUrl + "?param=130y130");
		FIMEMORY* mem = FreeImage_OpenMemory(imageData.data(), imageData.size());
		FREE_IMAGE_FORMAT format = FreeImage_GetFileTypeFromMemory(mem);
		FIBITMAP* bitmap = FreeImage_LoadFromMemory(format, mem);
		byte* pixels = (byte*)FreeImage_GetBits(bitmap);
		int width = FreeImage_GetWidth(bitmap);
		int height = FreeImage_GetHeight(bitmap);
		int pitch = FreeImage_GetPitch(bitmap);
		int bpp = FreeImage_GetBPP(bitmap);
		static size_t s_iArea;
		static byte* s_pBuf;
		if (s_iArea < width * height) {
			s_iArea = width * height;
			delete[] s_pBuf;
			s_pBuf = new byte[s_iArea * 4];
		}
		size_t c = 0;
		pixels += pitch * (height - 1);
		for (int y = 0; y < height; y++) {
			BYTE* pixel = (BYTE*)pixels;
			for (int x = 0; x < width; x++) {
				BYTE r = pixel[FI_RGBA_RED];
				BYTE g = pixel[FI_RGBA_GREEN];
				BYTE b = pixel[FI_RGBA_BLUE];
				s_pBuf[c * 4 + 0] = r;
				s_pBuf[c * 4 + 1] = g;
				s_pBuf[c * 4 + 2] = b;
				//Jpeg with alpha? weird
				if (bpp == 32) {
					BYTE a = pixel[FI_RGBA_ALPHA];
					s_pBuf[c * 4 + 3] = a;
					pixel += 4;
				}
				else {
					s_pBuf[c * 4 + 3] = 255;
					pixel += 3;
				}
				c++;
			}
			pixels -= pitch;
		}
		FreeImage_Unload(bitmap);
		FreeImage_CloseMemory(mem);
		if (playitem->type == CNeteasePanel::PLAYTYPE::MUSIC)
			obj.lyric = s_pNeteaseApi.load()->GetLyric(playitem->id);
		else
			obj.lyric = nullptr;
		obj.music = music;
		obj.album = s_pBuf;
		obj.album_h = height;
		obj.album_w = width;
		return &obj;
		}, item, static_cast<size_t>(m_pQuality->value)))->ContinueWith([panel = this](std::any& anyMusic) {
		if (anyMusic.type() == typeid(music_obj*)) {
			auto obj = std::any_cast<music_obj*>(anyMusic);
			if (obj == nullptr) {
				PrintF("#Netease_CannotPlay", false);
				return;
			}
			if (obj->music->copyright < 2 || (panel->m_pLogined != nullptr && panel->m_pLogined->vip > 0)) {
					//Music
					FModEngine::CFModSystem* soundSystem = FModEngine::GetSystem();
					FMOD_CREATESOUNDEXINFO extrainfo = {};
					extrainfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
					extrainfo.length = obj->musicData.size();
					soundSystem->CreateStream(reinterpret_cast<const char*>(obj->musicData.data()), FMOD_HARDWARE | FMOD_OPENMEMORY, &extrainfo, panel->m_pSound);
					soundSystem->PlaySound(FMOD_CHANNEL_FREE, panel->m_pSound, 0, panel->m_pChannel);
					panel->m_pChannel.SetVolume(panel->m_pVolume->value);
					//Album
					s_pAlbumImage->InitFromRGBA(obj->album, obj->album_w, obj->album_h);
					panel->m_pAlbumPanel->SetImage(s_pAlbumImage);
					//Set Lyric
					if (obj->lyric != nullptr) {
						lrc::LrcParser parser;
						panel->m_pLyric = parser.ParseString(std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(obj->lyric->lyric));
						panel->m_pTransLyric = parser.ParseString(std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(obj->lyric->tlyric));
					}
					//Text
					panel->m_pMusicNameLable->SetText(obj->music->name.c_str());
					std::string buf = obj->music->ar[0]->name.c_str();
					buf += " - ";
					buf += obj->music->al->name;
					panel->m_pArtistNameLable->SetText(buf.c_str());

					size_t len = obj->music->duration / 1000;
					static char buffer[256];
					V_snprintf(buffer, "%02d:%02d", len / 60, len % 60);
					panel->m_pMaxTimeLable->SetText(buffer);
					panel->m_pPlaying = obj->music;
					panel->ShowPanel(true);
				}
			else
				PrintF("#Netease_VIPCannotPlay", false);
		}
		panel->m_bPendingMusic = false;
	})->Start();
}
void CNeteasePanel::AddToList(netease::neteaseid_t id, CNeteasePanel::PLAYTYPE type){
	auto playitem = std::make_shared<CNeteasePanel::PlayItem>();
	playitem->id = id;
	playitem->type = type;
	m_aryPlayList.push_back(playitem);
}
void CNeteasePanel::NextMusic(){
	switch (m_pNowState) {
	case PLAYSTATE::FM: {
		if (m_bRenewingFM)
			return;
		if (m_aryPlayList.size() == 2)
			RenewFM(false);
		PlayListMusic();
		break;
	}
	case PLAYSTATE::NORMAL:
	default: {
		if (m_aryPlayList.size() == 0) {
			StopMusic();
			ShowPanel(false);
		}
		else
			PlayListMusic();
		break;
	}
	}
	
}
void CNeteasePanel::PauseMusic(){
	if (!m_pChannel.Valid()) {
		m_bPaused = false;
		return;
	}
	m_bPaused = !m_bPaused;
	m_pChannel.SetPaused(m_bPaused);
}
bool CNeteasePanel::IsPaused(){
	return m_bPaused;
}
void CNeteasePanel::QRLogin() {
	m_pLoginPanel->ResetText();
	m_pLoginPanel->Login();
}
void CNeteasePanel::EmailLogin(const char* mail, const char* passwd){
	GetTaskManager()->Add(std::async([](std::string& mail, std::string& passwd)-> std::any {
		return s_pNeteaseApi.load()->GetUser()->EMail(mail, passwd);
	}, std::string(mail), std::string(passwd)))->ContinueWith([panel = this](std::any anyCode) {
		if (anyCode.type() == typeid(netease::neteasecode_t)) {
			netease::neteasecode_t code = std::any_cast<netease::neteasecode_t>(anyCode);
			if (code == 200)
				panel->GetMyInfo(false);
			else
				PrintF("#Netease_NotLogin", false);
		}
	})->Start();
}
void CNeteasePanel::SendSMS(const char* phone, int country){
	char* end;
	GetTaskManager()->Add(std::async([](netease::neteaseid_t phone, int country)-> std::any {
		s_pNeteaseApi.load()->GetUser()->SendCaptcha(phone, country);
		return 0;
	}, std::strtoull(phone, &end, 10), country))->ContinueWith([](std::any anyCode) {
		PrintF("#Netease_SentSMS", false);
	})->Start();
}
void CNeteasePanel::SMSLogin(const char* phone, const char* captcha, int country){
	char* end;
	GetTaskManager()->Add(std::async([](netease::neteaseid_t phone, int captcha, int country)-> std::any {
		return s_pNeteaseApi.load()->GetUser()->CellPhone(phone, captcha, country);
	}, std::strtoull(phone, &end, 10), std::strtoull(captcha, &end, 10), country))->ContinueWith([panel = this](std::any anyCode) {
		if (anyCode.type() == typeid(netease::neteasecode_t)) {
			netease::neteasecode_t code = std::any_cast<netease::neteasecode_t>(anyCode);
			if (code == 200)
				panel->GetMyInfo(false);
			else
				PrintF("#Netease_NotLogin", false);
		}
	})->Start();
}
void CNeteasePanel::PhoneLogin(const char* phone, const char* passwd, int country){
	char* end;
	GetTaskManager()->Add(std::async([](netease::neteaseid_t phone, std::string& captcha, int country)-> std::any {
		return s_pNeteaseApi.load()->GetUser()->CellPhone(phone, captcha, country);
	}, std::strtoull(phone, &end, 10), std::string(passwd), country))->ContinueWith([panel = this](std::any anyCode) {
		if (anyCode.type() == typeid(netease::neteasecode_t)) {
			netease::neteasecode_t code = std::any_cast<netease::neteasecode_t>(anyCode);
			if (code == 200)
				panel->GetMyInfo(false);
			else
				PrintF("#Netease_NotLogin", false);
		}
	})->Start();
}
void CNeteasePanel::GetMyInfo(bool silence){
	GetTaskManager()->Add(std::async([]() -> std::any {
		return s_pNeteaseApi.load()->GetMyself();
		}))->ContinueWith([silence](std::any& anyMy){
			if (anyMy.type() == typeid(m_pLogined)) {
				auto info = std::any_cast<std::shared_ptr<netease::CMy>>(anyMy);
				char buffer[512];
				if (info != nullptr)
					PrintF("#Netease_MyInfo", silence, info->name.c_str(), info->signature.c_str(), info->vip ? "Yes" : "No");
				else
					PrintF("#Netease_NotLogin", silence);
				g_pViewPort->GetMusicPanel()->SetMyInfo(info);
			}
		})->Start();
}
void CNeteasePanel::SetMyInfo(std::shared_ptr<netease::CMy> info){
	m_pLogined = std::move(info);
}

/*
	QRLogin Panel
*/
class CQRImage : public CAlbumImage {
public:
	virtual void InitFromRGBA(const byte* rgba, int width, int height) override {
		if (m_iTextureID <= 0)
			m_iTextureID = vgui::surface()->CreateNewTextureID(true);
		vgui::surface()->DrawSetTextureRGBA(m_iTextureID, rgba, width, height, false, false);
		m_wide = width;
		m_tall = height;
		m_bValid = true;
	}
};
static CQRImage* s_pQRCodeImage;
CQRLoginPanel::CQRLoginPanel(vgui::Panel* parent, char* name) 
	: BaseClass(parent, name) {
	SetProportional(true);
	SetKeyBoardInputEnabled(false);
	SetMouseInputEnabled(false);
	SetTitleBarVisible(false);
	SetCloseButtonVisible(false);
	SetSizeable(false);
	SetMoveable(false);
	SetScheme(g_pViewPort->GetBaseScheme());

	m_pNotice = new vgui::Label(this, "QRNotice", "#Netease_QRNoticeText");
	m_pNotice->SetMouseInputEnabled(true);
	m_pQRImagePanel = new vgui::ImagePanel(this, "QRImage");
	m_pQRImagePanel->SetMouseInputEnabled(true);
	s_pQRCodeImage = new CQRImage();

	LoadControlSettings(VGUI2_ROOT_DIR "NeteaseQRPanel.res");
}
struct loginshare_obj {
	byte* qrimagebyte;
	size_t size;
	std::string qrkey;
};
void QRCheck(CQRLoginPanel* panel, std::string& qrkey) {
	GetTaskManager()->Add(std::async([](std::string unikey) -> std::any {
		return s_pNeteaseApi.load()->GetUser()->QRCheck(unikey);
	}, qrkey))->ContinueWith([panel, &qrkey](std::any anyRes) {
		if (anyRes.type() == typeid(netease::CLocalUser::QRStatue)) {
			auto result = std::any_cast<netease::CLocalUser::QRStatue>(anyRes);
			switch (result) {
			default:
			case netease::CLocalUser::QRStatue::INVALID: {
				CNeteasePanel::PrintF("#Netease_CannotLogin", false);
				panel->SetVisible(false);
				break;
			}
			case netease::CLocalUser::QRStatue::WAITINGSCAN:
			case netease::CLocalUser::QRStatue::AUTHORIZING: {
				QRCheck(panel, qrkey);
				break;
			}
			case netease::CLocalUser::QRStatue::OK: {
				panel->SetVisible(false);
				panel->SendMyInfo();
				break;
			}
			}
		}
	})->Start();
}
void CQRLoginPanel::Login(){
	GetTaskManager()->Add(std::async([]() -> std::any {
		static loginshare_obj obj;
		obj.qrkey = s_pNeteaseApi.load()->GetUser()->RequestQRKey();
		std::string url = "https://music.163.com/login?codekey=" + obj.qrkey;
		qrcodegen::QrCode qrcode = qrcodegen::QrCode::encodeText(url.c_str(), qrcodegen::QrCode::Ecc::HIGH);
		static byte* s_qrbyte;
		static int s_size;

		int qrsize = qrcode.getSize();
		if (qrsize > s_size) {
			s_size = qrsize;
			delete[] s_qrbyte;
			s_qrbyte = new byte[s_size * s_size * 4];
		}
		size_t c = 0;
		for (int x = 0; x < qrsize; x++) {
			for (int y = 0; y < qrsize; y++) {
				bool p = qrcode.getModule(x, y);
				s_qrbyte[c * 4 + 0] = p ? 0 : 255;
				s_qrbyte[c * 4 + 1] = p ? 0 : 255;
				s_qrbyte[c * 4 + 2] = p ? 0 : 255;
				s_qrbyte[c * 4 + 3] = 255;
				c++;
			}
		}
		obj.qrimagebyte = s_qrbyte;
		obj.size = qrsize;
		return &obj;
	}))->ContinueWith([panel = this](std::any anyobj) {
		if (anyobj.type() == typeid(loginshare_obj*)) {
			loginshare_obj* obj = std::any_cast<loginshare_obj*>(anyobj);
			s_pQRCodeImage->InitFromRGBA(obj->qrimagebyte, obj->size, obj->size);
			panel->m_pQRImagePanel->SetImage(s_pQRCodeImage);
			panel->SetVisible(true);
			QRCheck(panel, obj->qrkey);
		}
	})->Start();
}
void CQRLoginPanel::ResetText(){
	m_pNotice->SetText("#Netease_QRNoticeText");
}
void CQRLoginPanel::SendMyInfo(){
	m_pMusicPanel->GetMyInfo();
}