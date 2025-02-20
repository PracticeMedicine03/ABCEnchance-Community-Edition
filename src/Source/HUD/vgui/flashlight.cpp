#pragma once
#include <metahook.h>

#include <vgui/ISurface.h>
#include <vgui/ISystem.h>
#include <vgui/ILocalize.h>
#include <vgui_controls/Label.h>
#include <vgui_controls/ImageClipPanel.h>
#include <vgui_controls/AnimationController.h>

#include "local.h"
#include "vguilocal.h"

#include "flashlight.h"
#include "Viewport.h"

#define VIEWPORT_FLASHLIGHT_NAME "FlashLightPanel"
CFlashLightPanel::CFlashLightPanel()
	: BaseClass(nullptr, VIEWPORT_FLASHLIGHT_NAME) {
	SetProportional(true);
	SetKeyBoardInputEnabled(false);
	SetMouseInputEnabled(false);
	SetScheme(g_pViewPort->GetBaseScheme());
	// Header labels
	m_pMessage = new vgui::Label(this, "Message", "");
	m_pOffImage = new vgui::ImagePanel(this, "OffImage");
	m_pOnImage = new vgui::ImagePanel(this, "OnImage");
	m_pPowerImage = new vgui::ImageClipPanel(this, "PowerImage");

	LoadControlSettings(VGUI2_ROOT_DIR "FlashLightPanel.res");
}
const char* CFlashLightPanel::GetName() {
	return VIEWPORT_FLASHLIGHT_NAME;
}
void CFlashLightPanel::Reset() {
	m_iBattery = 100;
	ShowPanel(true);
}
void CFlashLightPanel::ApplySchemeSettings(vgui::IScheme* pScheme) {
	BaseClass::ApplySchemeSettings(pScheme);

	SetBgColor(GetSchemeColor("FlashLight.BgColor", GetSchemeColor("Panel.BgColor", pScheme), pScheme));
	m_pMessage->SetBgColor(GetSchemeColor("FlashLight.TextBgColor", GetSchemeColor("Label.BgColor", pScheme), pScheme));
	m_pMessage->SetFgColor(GetSchemeColor("FlashLight.TextFgColor", GetSchemeColor("Label.FgColor", pScheme), pScheme));
	m_cIconColor = GetSchemeColor("FlashLight.IconFgColor", GetSchemeColor("Panel.FgColor", pScheme), pScheme);
}
void CFlashLightPanel::ApplySettings(KeyValues* inResourceData) {
	BaseClass::ApplySettings(inResourceData);
	m_flFadeAnimateTime = inResourceData->GetFloat("fade_time");
}
void CFlashLightPanel::ShowPanel(bool state) {
	if (state == IsVisible())
		return;
	SetVisible(state);
}
bool CFlashLightPanel::IsVisible() {
	return BaseClass::IsVisible();
}
vgui::VPANEL CFlashLightPanel::GetVPanel() {
	return BaseClass::GetVPanel();
}
void CFlashLightPanel::SetParent(vgui::VPANEL parent) {
	BaseClass::SetParent(parent);
}

void CFlashLightPanel::SetFlashLight(bool on, int battery){
	if (on) {
		m_pOnImage->SetAlpha(1);
		vgui::GetAnimationController()->RunAnimationCommand(m_pOnImage, "alpha", 255, 0.0f, m_flFadeAnimateTime, vgui::AnimationController::INTERPOLATOR_LINEAR);
	}
	else {
		m_pOffImage->SetAlpha(1);
		vgui::GetAnimationController()->RunAnimationCommand(m_pOffImage, "alpha", 255, 0.0f, m_flFadeAnimateTime, vgui::AnimationController::INTERPOLATOR_LINEAR);

	}
	m_pOnImage->SetVisible(on);
	m_pOffImage->SetVisible(!on);
	SetFlashBattery(battery);
}
void CFlashLightPanel::SetFlashBattery(int battery){
	m_iBattery = battery;
	float flRatio = static_cast<float>(m_iBattery) / 100.0f;
	m_pPowerImage->SetWide(flRatio * m_pOffImage->GetWide());
	//120 ~ 0 Green To Red
	Vector vecColor = { (float)m_cIconColor.r() / 255.0f, (float)m_cIconColor.g() / 255.0f, (float)m_cIconColor.b() / 255.0f };
	RGBtoHSV(vecColor, vecColor);
	vecColor.x *= flRatio;
	HSVtoRGB(vecColor, vecColor);
	vecColor *= 255;
	m_pPowerImage->SetDrawColor(Color(vecColor.x, vecColor.y, vecColor.z, m_cIconColor.a()));
	char temp[32];
	Q_snprintf(temp, sizeof(temp), "%d%%", m_iBattery);
	m_pMessage->SetText(temp);
	ShowPanel(true);
}