//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#include <stdio.h>

#include <vgui/IBorder.h>
#include <vgui/ISurface.h>
#include <vgui/IScheme.h>
#include <vgui/IBorder.h>
#include <KeyValues.h>

#include <vgui2/spr_image.h>
#include <vgui_controls/ImageSprPanel.h>
#include <vgui_controls/Image.h>
#include <vgui_controls/Controls.h>

// memdbgon must be the last include file in a .cpp file!!!
#include <tier0/memdbgon.h>

using namespace vgui;

DECLARE_BUILD_FACTORY(ImageSprPanel);

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
ImageSprPanel::ImageSprPanel(Panel* parent, const char* name) : Panel(parent, name)
{
	m_pImage = NULL;
	m_pszImageName = NULL;
	m_pszFillColorName = NULL;
	m_pszDrawColorName = NULL;	// HPE addition
	m_bCenterImage = false;
	m_bScaleImage = false;
	m_bTileImage = false;
	m_bTileHorizontally = false;
	m_bTileVertically = false;
	m_bPositionImage = true;
	m_fScaleAmount = 0.0f;
	m_FillColor = Color(0, 0, 0, 0);
	m_DrawColor = Color(255, 255, 255, 255);

	SetImage(m_pImage);

	REGISTER_COLOR_AS_OVERRIDABLE(m_FillColor, "fillcolor_override");
	REGISTER_COLOR_AS_OVERRIDABLE(m_DrawColor, "drawcolor_override");
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
ImageSprPanel::~ImageSprPanel()
{
	delete[] m_pszImageName;
	delete[] m_pszFillColorName;
	delete[] m_pszDrawColorName;	// HPE addition
	delete m_pImage;
}

//-----------------------------------------------------------------------------
// Purpose: handles size changing
//-----------------------------------------------------------------------------
void ImageSprPanel::OnSizeChanged(int newWide, int newTall)
{
	BaseClass::OnSizeChanged(newWide, newTall);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void ImageSprPanel::SetImage(CSPRImage* image)
{
	m_pImage = image;
	Repaint();
}

//-----------------------------------------------------------------------------
// Purpose: sets an image by file name
//-----------------------------------------------------------------------------
void ImageSprPanel::SetImage(const char* imageName)
{
	if (imageName && m_pszImageName && V_stricmp(imageName, m_pszImageName) == 0)
		return;

	int len = Q_strlen(imageName) + 1;
	delete[] m_pszImageName;
	m_pszImageName = new char[len];
	Q_strncpy(m_pszImageName, imageName, len);
	InvalidateLayout(false, true); // force applyschemesettings to run
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CSPRImage* ImageSprPanel::GetImage()
{
	return m_pImage;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
Color ImageSprPanel::GetDrawColor(void)
{
	return m_DrawColor;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void ImageSprPanel::SetDrawColor(Color drawColor)
{
	m_DrawColor = drawColor;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void ImageSprPanel::PaintBackground()
{
	if (m_FillColor[3] > 0)
	{
		// draw the specified fill color
		int wide, tall;
		GetSize(wide, tall);
		surface()->DrawSetColor(m_FillColor);
		surface()->DrawFilledRect(0, 0, wide, tall);
	}
	if (m_pImage)
	{
		//=============================================================================
		// HPE_BEGIN:
		// [pfreese] Color should be always set from GetDrawColor(), not just when 
		// scaling is true (see previous code)
		//=============================================================================

		// surface()->DrawSetColor( 255, 255, 255, GetAlpha() );
		Color dcolor = GetDrawColor();
		dcolor.SetColor(dcolor.r(), dcolor.g(), dcolor.b(), GetAlpha());
		m_pImage->SetColor(dcolor);
		int w, h;
		int x, y;
		GetSize(w, h);
		GetPos(x, y);
		m_pImage->SetSize(w, h);
		m_pImage->SetPos(x, y);

		//=============================================================================
		// HPE_END
		//=============================================================================

		if (m_bPositionImage)
		{
			if (m_bCenterImage)
			{
				int wide, tall;
				GetSize(wide, tall);

				int imageWide, imageTall;
				m_pImage->GetSize(imageWide, imageTall);

				if (m_bScaleImage && m_fScaleAmount > 0.0f)
				{
					imageWide = static_cast<int>(static_cast<float>(imageWide) * m_fScaleAmount);
					imageTall = static_cast<int>(static_cast<float>(imageTall) * m_fScaleAmount);
				}

				m_pImage->SetPos((wide - imageWide) / 2, (tall - imageTall) / 2);
			}
			else
			{
				m_pImage->SetPos(0, 0);
			}
		}

		if (m_bScaleImage)
		{
			// Image size is stored in the bitmap, so temporarily set its size
			// to our panel size and then restore after we draw it.

			int imageWide, imageTall;
			m_pImage->GetSize(imageWide, imageTall);

			if (m_fScaleAmount > 0.0f)
			{
				float wide, tall;
				wide = static_cast<float>(imageWide) * m_fScaleAmount;
				tall = static_cast<float>(imageTall) * m_fScaleAmount;
				m_pImage->SetSize(static_cast<int>(wide), static_cast<int>(tall));
			}
			else
			{
				int wide, tall;
				GetSize(wide, tall);
				m_pImage->SetSize(wide, tall);
			}

			m_pImage->Paint();

			m_pImage->SetSize(imageWide, imageTall);
		}
		else if (m_bTileImage || m_bTileHorizontally || m_bTileVertically)
		{
			int wide, tall;
			GetSize(wide, tall);
			int imageWide, imageTall;
			m_pImage->GetSize(imageWide, imageTall);

			int y = 0;
			while (y < tall)
			{
				int x = 0;
				while (x < wide)
				{
					m_pImage->SetPos(x, y);
					m_pImage->Paint();

					x += imageWide;

					if (!m_bTileHorizontally)
						break;
				}

				y += imageTall;

				if (!m_bTileVertically)
					break;
			}

			if (m_bPositionImage)
			{
				m_pImage->SetPos(0, 0);
			}
		}
		else
		{
			m_pImage->SetColor(GetDrawColor());
			m_pImage->Paint();
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Gets control settings for editing
//-----------------------------------------------------------------------------
void ImageSprPanel::GetSettings(KeyValues* outResourceData)
{
	BaseClass::GetSettings(outResourceData);
	if (m_pszImageName)
	{
		outResourceData->SetString("image", m_pszImageName);
	}
	if (m_pszFillColorName)
	{
		outResourceData->SetString("fillcolor", m_pszFillColorName);
	}

	//=============================================================================
	// HPE_BEGIN:
	// [pfreese] Added support for specifying drawcolor
	//=============================================================================
	if (m_pszDrawColorName)
	{
		outResourceData->SetString("drawcolor", m_pszDrawColorName);
	}
	//=============================================================================
	// HPE_END
	//=============================================================================

	if (GetBorder())
	{
		outResourceData->SetString("border", GetBorder()->GetName());
	}

	outResourceData->GetInt("positionImage", m_bPositionImage);
	outResourceData->SetInt("scaleImage", m_bScaleImage);
	outResourceData->SetFloat("scaleAmount", m_fScaleAmount);
	outResourceData->SetInt("tileImage", m_bTileImage);
	outResourceData->SetInt("tileHorizontally", m_bTileHorizontally);
	outResourceData->SetInt("tileVertically", m_bTileVertically);
}

//-----------------------------------------------------------------------------
// Purpose: Applies designer settings from res file
//-----------------------------------------------------------------------------
void ImageSprPanel::ApplySettings(KeyValues* inResourceData)
{
	delete[] m_pszImageName;
	delete[] m_pszFillColorName;
	delete[] m_pszDrawColorName;	// HPE addition
	m_pszImageName = NULL;
	m_pszFillColorName = NULL;
	m_pszDrawColorName = NULL;		// HPE addition

	m_bPositionImage = inResourceData->GetInt("positionImage", 1);
	m_bScaleImage = inResourceData->GetInt("scaleImage", 0);
	m_fScaleAmount = inResourceData->GetFloat("scaleAmount", 0.0f);
	m_bTileImage = inResourceData->GetInt("tileImage", 0);
	m_bTileHorizontally = inResourceData->GetInt("tileHorizontally", m_bTileImage);
	m_bTileVertically = inResourceData->GetInt("tileVertically", m_bTileImage);
	const char* imageName = inResourceData->GetString("image", "");
	if (*imageName)
	{
		SetImage(imageName);
	}

	const char* pszFillColor = inResourceData->GetString("fillcolor", "");
	if (*pszFillColor)
	{
		int r = 0, g = 0, b = 0, a = 255;
		int len = Q_strlen(pszFillColor) + 1;
		m_pszFillColorName = new char[len];
		Q_strncpy(m_pszFillColorName, pszFillColor, len);

		if (sscanf(pszFillColor, "%d %d %d %d", &r, &g, &b, &a) >= 3)
		{
			// it's a direct color
			m_FillColor = Color(r, g, b, a);
		}
		else
		{
			IScheme* pScheme = scheme()->GetIScheme(GetScheme());
			m_FillColor = pScheme->GetColor(pszFillColor, Color(0, 0, 0, 0));
		}
	}

	//=============================================================================
	// HPE_BEGIN:
	// [pfreese] Added support for specifying drawcolor
	//=============================================================================
	const char* pszDrawColor = inResourceData->GetString("drawcolor", "");
	if (*pszDrawColor)
	{
		int r = 255, g = 255, b = 255, a = 255;
		int len = Q_strlen(pszDrawColor) + 1;
		m_pszDrawColorName = new char[len];
		Q_strncpy(m_pszDrawColorName, pszDrawColor, len);

		if (sscanf(pszDrawColor, "%d %d %d %d", &r, &g, &b, &a) >= 3)
		{
			// it's a direct color
			m_DrawColor = Color(r, g, b, a);
		}
		else
		{
			IScheme* pScheme = scheme()->GetIScheme(GetScheme());
			m_DrawColor = pScheme->GetColor(pszDrawColor, Color(255, 255, 255, 255));
		}
	}
	//=============================================================================
	// HPE_END
	//=============================================================================

	const char* pszBorder = inResourceData->GetString("border", "");
	if (*pszBorder)
	{
		IScheme* pScheme = scheme()->GetIScheme(GetScheme());
		SetBorder(pScheme->GetBorder(pszBorder));
	}

	BaseClass::ApplySettings(inResourceData);

	const char* pszRect = inResourceData->GetString("rect", "");
	if (*pszRect){
		int l, r, t, b;
		if (sscanf(pszRect, "%d %d %d %d", &l, &r, &t, &b) >= 3)
			SetRect(l, r, t, b);
	}

	SetFramerate(inResourceData->GetFloat("framerate"));
	SetRenderMode(inResourceData->GetInt("rendermode"));

}

//-----------------------------------------------------------------------------
// Purpose:  load the image, this is done just before this control is displayed
//-----------------------------------------------------------------------------
void ImageSprPanel::ApplySchemeSettings(IScheme* pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);
	if (m_pszImageName && strlen(m_pszImageName) > 0)
	{
		if (m_pImage) {
			delete m_pImage;
			m_pImage = nullptr;
		}
		SetImage(new vgui::CSPRImage(m_pszImageName));
	}
}

//-----------------------------------------------------------------------------
// Purpose: Describes editing details
//-----------------------------------------------------------------------------
const char* ImageSprPanel::GetDescription()
{
	static char buf[1024];
	_snprintf(buf, sizeof(buf), "%s, string image, string border, string fillcolor, bool scaleImage", BaseClass::GetDescription());
	return buf;
}

//-----------------------------------------------------------------------------
// Purpose: sets whether or not the image should scale to fit the size of the ImagePanel (defaults to false)
//-----------------------------------------------------------------------------
void ImageSprPanel::SetShouldScaleImage(bool state)
{
	m_bScaleImage = state;
}

//-----------------------------------------------------------------------------
// Purpose: gets whether or not the image should be scaled to fit the size of the ImagePanel
//-----------------------------------------------------------------------------
bool ImageSprPanel::GetShouldScaleImage()
{
	return m_bScaleImage;
}

//-----------------------------------------------------------------------------
// Purpose: used in conjunction with setting that the image should scale and defines an absolute scale amount
//-----------------------------------------------------------------------------
void ImageSprPanel::SetScaleAmount(float scale)
{
	m_fScaleAmount = scale;
}

float ImageSprPanel::GetScaleAmount(void)
{
	return m_fScaleAmount;
}

//-----------------------------------------------------------------------------
// Purpose: set the color to fill with, if no Image is specified
//-----------------------------------------------------------------------------
void ImageSprPanel::SetFillColor(Color col)
{
	m_FillColor = col;
}

//-----------------------------------------------------------------------------
// Purpose: data accessor
//-----------------------------------------------------------------------------
Color ImageSprPanel::GetFillColor()
{
	return m_FillColor;
}

char* ImageSprPanel::GetImageName()
{
	return m_pszImageName;
}

bool ImageSprPanel::EvictImage()
{
	// GoldSrc: IDK how to fix it, need more research
	Assert(false);
#if 0
	if (!m_pImage)
	{
		// nothing to do
		return false;
	}

	if (!scheme()->DeleteImage(m_pszImageName))
	{
		// no eviction occured, could have an outstanding reference
		return false;
	}

	// clear out our cached concept of it
	// as it may change
	// the next SetImage() will re-establish
	m_pImage = NULL;
	delete[] m_pszImageName;
	m_pszImageName = NULL;
#endif

	return true;
}

void ImageSprPanel::SetFramerate(float fl){
	if(m_pImage)
		m_pImage->SetFramerate(fl);
}

void ImageSprPanel::SetRect(int l, int r, int t, int b){
	if (m_pImage)
		m_pImage->SetRect(l, r, t, b);
}

void ImageSprPanel::SetRenderMode(int mode){
	if (m_pImage)
		m_pImage->SetRenderMode(mode);
}

void ImageSprPanel::Reset(){
	if (m_pImage)
		m_pImage->Reset();
}

void ImageSprPanel::Animate(){
	if (m_pImage)
		m_pImage->Animate();
}

void ImageSprPanel::SetFrame(int frame){
	if (m_pImage)
		m_pImage->SetFrame(frame);
}
