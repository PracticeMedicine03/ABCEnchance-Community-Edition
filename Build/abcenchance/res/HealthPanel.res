"abcenchance/res/HealthPanel.res"
{
	"HealthPanel"
	{
		"ControlName"		"CHealthPanel"
		"fieldName"		"HealthPanel"
		"xpos"		"0"
		"ypos"		"rs1.0"
		"wide"		"300"
		"tall"		"30"

		"autoResize"		"1"
		"visible"		"1"
		"enabled"		"1"

		"icon_poison"	"abcenchance/tga/icon_poison"
		"icon_acid"	"abcenchance/tga/icon_acid"
		"icon_freeze"	"abcenchance/tga/icon_freeze"
		"icon_drown"	"abcenchance/tga/icon_drown"
		"icon_burn"	"abcenchance/tga/icon_burn"
		"icon_gas"	"abcenchance/tga/icon_gas"
		"icon_radiation"	"abcenchance/tga/icon_radiation"
		"icon_shock"	"abcenchance/tga/icon_shock"
	}

	"Background"
	{
		"ControlName"		"ImagePanel"
		"fieldName"		"Background"
		"xpos"		"0"
		"ypos"		"rs1.0"
		"wide"		"f0"
		"tall"		"f0"
		"visible"		"1"
		"scaleImage"		"1"
		"proportionalToParent" "1"
		"image"			"abcenchance/tga/healthpanel_background"
		"drawColor"		"255 255 255 125"
	}

	"HealthIcon"
	{
		"ControlName"		"ImagePanel"
		"fieldName"		"HealthIcon"
		"xpos"		"6"
		"ypos"		"cs-0.5"
		"wide"		"15"
		"tall"		"15"
		"visible"		"1"
		"proportionalToParent" "1"
		"image"			"abcenchance/tga/healthbar_icon"
		"scaleImage"		"1"
		//生命值图标颜色
		"drawColor"		"0 255 0 255"
	}

	"Health"
	{
		"ControlName"		"Label"
		"fieldName"		"Health"
		"xpos"		"14"
		"ypos"		"cs-0.5"
		"wide"		"48"
		"tall"		"30"
		"visible"		"1"
		"enabled"		"1"
		"labelText"		"0"
		"proportionalToParent" "1"
		"textAlignment"		"center"
		"brighttext"		"1"
		"font"		"HealthBar"
		"wrap"		"0"
	}
	"HealthBar"
	{
		"ControlName"		"ImageClipPanel"
		"fieldName"		"HealthBar"
		"xpos"		"55"
		"ypos"		"cs-0.5"
		"wide"		"56"
		"tall"		"11"
		"visible"		"1"
		"proportionalToParent" "1"
		"image"			"abcenchance/tga/healthbar"
		"scaleImage"		"1"
		//血条颜色
		"drawColor"		"0 255 0 200"
		"zpos"		"999"
	}
	"HealthBackground"
	{
		"ControlName"		"ImagePanel"
		"fieldName"		"HealthBackground"
		"xpos"		"55"
		"ypos"		"cs-0.5"
		"wide"		"56"
		"tall"		"11"
		"visible"		"1"
		"proportionalToParent" "1"
		"image"			"abcenchance/tga/healthbar_bg"
		"scaleImage"		"1"
		//血条背景颜色
		"drawColor"		"0 50 0 50"
		"zpos"		"0"
	}

	"ArmorIcon"
	{
		"ControlName"		"ImagePanel"
		"fieldName"		"ArmorIcon"
		"xpos"		"120"
		"ypos"		"cs-0.5"
		"wide"		"15"
		"tall"		"15"
		"visible"		"1"
		"proportionalToParent" "1"
		"image"			"abcenchance/tga/armorbar_icon"
		"scaleImage"		"1"
		//护甲图标颜色
		"drawColor"		"0 255 0 255"
	}
	"Armor"
	{
		"ControlName"		"Label"
		"fieldName"		"Armor"
		"xpos"		"128"
		"ypos"		"cs-0.5"
		"wide"		"48"
		"tall"		"30"
		"visible"		"1"
		"enabled"		"1"
		"labelText"		"0"
		"proportionalToParent" "1"
		"textAlignment"		"center"
		"brighttext"		"1"
		"font"		"HealthBar"
		"wrap"		"0"
	}
	"ArmorBar"
	{
		"ControlName"		"ImageClipPanel"
		"fieldName"		"ArmorBar"
		"xpos"		"169"
		"ypos"		"cs-0.5"
		"wide"		"56"
		"tall"		"11"
		"visible"		"1"
		"proportionalToParent" "1"
		"image"			"abcenchance/tga/armorbar"
		"scaleImage"		"1"
		//护甲条颜色
		"drawColor"		"0 255 0 200"
		"zpos"		"999"
	}
	"ArmorBackground"
	{
		"ControlName"		"ImagePanel"
		"fieldName"		"ArmorBackground"
		"xpos"		"169"
		"ypos"		"cs-0.5"
		"wide"		"56"
		"tall"		"11"
		"visible"		"1"
		"proportionalToParent" "1"
		"image"			"abcenchance/tga/armorbar_bg"
		"scaleImage"		"1"
		//护甲条背景颜色
		"drawColor"		"0 50 0 50"
		"zpos"		"0"
	}

	"Longjump"
	{
		"ControlName"		"ImagePanel"
		"fieldName"		"Longjump"
		"xpos"		"248"
		"ypos"		"cs-0.5"
		"wide"		"20"
		"tall"		"20"
		"visible"		"1"
		"proportionalToParent" "1"
		"scaleImage"		"1"
		"image"			"abcenchance/tga/longjump"
		//长跳图标颜色
		"drawColor"		"0 255 0 255"
	}
}
