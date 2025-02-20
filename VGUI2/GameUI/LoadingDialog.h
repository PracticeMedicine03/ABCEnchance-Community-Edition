﻿#ifndef LOADINGDIALOG_H
#define LOADINGDIALOG_H

#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/Frame.h>
#include <vgui_controls/HTML.h>

namespace vgui {
	class CBitmapImagePanel;
}

class CLoadingDialog : public vgui::Frame
{
	DECLARE_CLASS_SIMPLE(CLoadingDialog, vgui::Frame);

public:
	CLoadingDialog(vgui::Panel* parent);
	~CLoadingDialog(void);

public:
	void Open(bool bShowBackground = false);
	bool SetProgressPoint(int progressPoint);
	void SetProgressRange(int min, int max);
	void SetStatusText(const char* statusText);
	void SetSecondaryProgress(float progress);
	void SetSecondaryProgressText(const char* statusText);
	void DisplayGenericError(const char* failureReason, const char* extendedReason = NULL);
	void SetBackgroundImage(const char* imageName);
	void Activate(void);

protected:
	virtual void OnCommand(const char* command);
	virtual void PerformLayout(void);
	virtual void OnThink(void);
	virtual void OnClose(void);
	virtual void OnKeyCodePressed(vgui::KeyCode code);
	virtual void PaintBackground(void);

private:
	void SetupControlSettings(bool bForceShowProgressText);
	void SetupControlSettingsForErrorDisplay(const char* settingsFile);

public:
	vgui::Panel* m_pParent;
	vgui::ProgressBar* m_pProgress;
	vgui::ProgressBar* m_pProgress2;
	vgui::Label* m_pInfoLabel;
	vgui::Label* m_pTimeRemainingLabel;
	vgui::Button* m_pCancelButton;
	static vgui::CBitmapImagePanel* m_pLoadingBackground;
	int m_iRangeMin, m_iRangeMax;
	bool m_bShowingSecondaryProgress;
	float m_flSecondaryProgress;
	float m_flLastSecondaryProgressUpdateTime;
	float m_flSecondaryProgressStartTime;
	bool m_bCenter;
	float m_flProgressFraction;
	char m_szBackgroundImage[MAX_PATH];
	bool m_bShowBackground;
	vgui::VPANEL m_RestrictPanel;

public:
	CPanelAnimationVar(int, m_iAdditionalIndentX, "AdditionalIndentX", "0");
	CPanelAnimationVar(int, m_iAdditionalIndentY, "AdditionalIndentY", "0");
};

CLoadingDialog* LoadingDialog(void);
#endif