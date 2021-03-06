// MDisplaySettings.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "MArrayE.h"
#include "MConfigFile.h"


class Display
{
public:
	int idinwindowsdisplaysettings;		// just for the user to correlate the entries in the config file with the display in the windows settings
	DISPLAY_DEVICEA dd;
	DEVMODEA dm;

	inline Display()
	{
		Reset();
	}

	inline void Reset()
	{
		idinwindowsdisplaysettings = -1;

		memset(&dd, 0, sizeof(DISPLAY_DEVICEA));
		dd.cb = sizeof(DISPLAY_DEVICEA);

		memset(&dm, 0, sizeof(DEVMODEA));
	}

	inline const bool GetEnabled() const
	{
		return (dd.StateFlags & DISPLAY_DEVICE_ACTIVE) != 0;
	}
	inline void SetEnabled(const bool v)
	{
		dd.StateFlags |= DISPLAY_DEVICE_ACTIVE;
	}

	inline const bool GetPrimaryDisplay() const
	{
		return (dd.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE) != 0;
	}
	inline void SetPrimaryDisplay(const bool v)
	{
		dd.StateFlags |= DISPLAY_DEVICE_PRIMARY_DEVICE;
	}
};
typedef Display * LPDisplay;

MArrayE < LPDisplay > displays;


void GetDisplays()
{
	int idinwindowsdisplaysettings = 1;
	while (true)
	{
		Display * d = new Display();
		int ret = EnumDisplayDevicesA(NULL, displays.GetN(), &(d->dd), 0);
		if (ret == 0)
		{
			// no more displays
			delete d;
			break;
		}
		else
		{
			if (d->dd.StateFlags == DISPLAY_DEVICE_MIRRORING_DRIVER)
			{
				// ignore
			}
			else
			{
				EnumDisplaySettingsA(d->dd.DeviceName, ENUM_CURRENT_SETTINGS, &d->dm);

				if (d->GetEnabled())
				{
					d->idinwindowsdisplaysettings = idinwindowsdisplaysettings++;
				}

				displays.AddBack(d);
			}
		}
	}
}

void FreeDisplays()
{
	displays.DeleteAll();
}

void PrintDisplays()
{
	unsigned int i;
	for (i = 0; i < displays.GetN(); i++)
	{
		Display * d = displays[i];
		printf("--------------------------------------------------\n");
		printf("%u:\tName:\t%s\n\tID:\t%s\n\tString:\t%s\n\tKey:\t%s\n\tState:\t%u\n", i, d->dd.DeviceName, d->dd.DeviceID, d->dd.DeviceString, d->dd.DeviceKey, d->dd.StateFlags);

		printf("\tPos:\t%i\t%i\t%i\t%i\n", d->dm.dmPosition.x, d->dm.dmPosition.y, d->dm.dmPelsWidth, d->dm.dmPelsHeight);
		printf("\tPrimary\t%i\n", d->GetPrimaryDisplay());
		printf("\tEnabled\t%i\n", d->GetEnabled());
		printf("\tidinwindowsdisplaysettings\t%i\n", d->idinwindowsdisplaysettings);
	}
}

void SetDisplayPositions()
{
	// based on https://www.codeproject.com/Articles/37900/NET-Wrapper-for-ChangeDisplaySettingsEX but modified
	// That code first sets everything without applying it by CDS_NORESET. Afterwards it activates everything with a backward loop.
	// I didn't chekc if that's necessary, I just do it like that as well.

	unsigned int i;
	for (i = 0; i < displays.GetN(); i++)
	{
		Display * d = displays[i];
		
		d->dm.dmFields = DM_POSITION | DM_PELSWIDTH | DM_PELSHEIGHT | (d->GetEnabled() ? DM_DISPLAYFLAGS : 0);
		ChangeDisplaySettingsExA(d->dd.DeviceName, &d->dm, NULL, d->GetPrimaryDisplay() ? CDS_SET_PRIMARY : 0 | CDS_UPDATEREGISTRY | CDS_NORESET, 0);
	}

	for (i = displays.GetN() - 1; i != -1; i--)
	{
		Display * d = displays[i];

		ChangeDisplaySettingsExA(d->dd.DeviceName, &d->dm, NULL, CDS_UPDATEREGISTRY, 0);
	}
}

void Save(MArrayE < LPDisplay > &displays, char * filename)
{
	MConfigFile cf;
	cf.SetConfigFile(filename, MConfigFileTypeINI, false, false);

	cf.SetUInt("main", "displaysn", displays.GetN());

	unsigned int i;
	for (i = 0; i < displays.GetN(); i++)
	{
		Display * d = displays[i];

		char section[1024];
		sprintf(section, "display%u", i);
		
		cf.SetInt(section, "idinwindowsdisplaysettings", d->idinwindowsdisplaysettings);

		cf.SetString(section, "DeviceName", d->dd.DeviceName);
		cf.SetString(section, "DeviceString", d->dd.DeviceString);
		cf.SetUInt(section, "StateFlags", d->dd.StateFlags);
		cf.SetBool(section, "enabled", d->GetEnabled());
		cf.SetBool(section, "primarydisplay", d->GetPrimaryDisplay());
		cf.SetString(section, "DeviceID", d->dd.DeviceID);
		cf.SetString(section, "DeviceKey", d->dd.DeviceKey);

		cf.SetInt(section, "dmPosition_x", d->dm.dmPosition.x);
		cf.SetInt(section, "dmPosition_y", d->dm.dmPosition.y);
		cf.SetInt(section, "dmPelsWidth", d->dm.dmPelsWidth);
		cf.SetInt(section, "dmPelsHeight", d->dm.dmPelsHeight);
	}

	cf.Save();
}

int Load(MArrayE < LPDisplay > &displays, char * filename)
{
	MConfigFile cf;
	cf.SetConfigFile(filename, MConfigFileTypeINI, false, true);

	unsigned int displaysn = cf.GetUInt("main", "displaysn");
	if (displaysn != displays.GetN())
	{
		printf("Displays mismathc: Number of displays in this system: %u, Number of displays in the file: %u\n", displays.GetN(), displaysn);
		printf("Display Settings not changed.\n");
		return 1;
	}

	// check diplays matching rl vs. file
	bool allok = true;
	unsigned int i;
	for (i = 0; i < displays.GetN(); i++)
	{
		Display * d = displays[i];

		char section[1024];
		sprintf(section, "display%u", i);

		Display dcur;
		
		strcpy(dcur.dd.DeviceName, cf.GetString(section, "DeviceName"));
		strcpy(dcur.dd.DeviceString, cf.GetString(section, "DeviceString"));
		strcpy(dcur.dd.DeviceID, cf.GetString(section, "DeviceID"));
		strcpy(dcur.dd.DeviceKey, cf.GetString(section, "DeviceKey"));

		if ((strcmp(dcur.dd.DeviceName, d->dd.DeviceName) == 0) &&
			(strcmp(dcur.dd.DeviceString, d->dd.DeviceString) == 0) &&
			(strcmp(dcur.dd.DeviceID, d->dd.DeviceID) == 0) &&
			(strcmp(dcur.dd.DeviceKey, d->dd.DeviceKey) == 0))
		{

		}
		else
		{
			printf("Display %u does not match\n", i);
			allok = false;
		}
	}
	if (!allok)
	{
		return 2;
	}

	// Merge
	for (i = 0; i < displays.GetN(); i++)
	{
		Display * d = displays[i];

		char section[1024];
		sprintf(section, "display%u", i);

		d->dm.dmPosition.x = cf.GetInt(section, "dmPosition_x");
		d->dm.dmPosition.y = cf.GetInt(section, "dmPosition_y");
		d->dm.dmPelsWidth = cf.GetInt(section, "dmPelsWidth");
		d->dm.dmPelsHeight = cf.GetInt(section, "dmPelsHeight");

		d->dd.StateFlags = cf.GetUInt(section, "StateFlags");
		bool enabled = cf.GetBool(section, "enabled");
		d->SetEnabled(enabled);
		bool primarydisplay = cf.GetBool(section, "primarydisplay");
		d->SetPrimaryDisplay(primarydisplay);
	}

	return 0;
}

void PrintHelp()
{
	printf("usage:\n");
	printf("\tMDisplaySettings print\n");
	printf("\t\tPrints all displays.\n");
	printf("\tMDisplaySettings save [filename]\n");
	printf("\t\tSaves the current configuration in the specified file. You can modify that file and load it.\n");
	printf("\tMDisplaySettings load [filename]\n");
	printf("\t\tLoads a settings file and applies it.\n");
}

int main(int argc, char ** argv)
{
	printf("MDisplaySettings    v0 (c) 2018 M. Rochel\n");

	if (argc <= 1)
	{
		PrintHelp();
	}
	else
	{
		char * cmd = argv[1];
		if (strcmp(cmd, "print") == 0)
		{
			GetDisplays();
			PrintDisplays();
			FreeDisplays();
		}
		else
		if (strcmp(cmd, "save") == 0)
		{
			GetDisplays();
			Save(displays, argv[2]);
			FreeDisplays();
		}
		else
		if (strcmp(cmd, "load") == 0)
		{
			GetDisplays();
			int ret = Load(displays, argv[2]);
			if (ret == 0)
			{
				SetDisplayPositions();
			}
			FreeDisplays();
		}
		else
		{
			PrintHelp();
		}
		//displays[4]->dm.dmPosition.x = 3360;
		//displays[4]->dm.dmPosition.y = 1050 - 768;
	}

    return 0;
}

