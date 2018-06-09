#include "stdafx.h"
#include "MConfigFile.h"
#include "stdio.h"
#include "MStringSafeLinux.h"
#ifdef MPLATFORM_WIN32
#include <direct.h>
#endif
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/json_parser.hpp>

MConfigFile::MConfigFile(void)
{
}

MConfigFile::~MConfigFile(void)
{
}

int MConfigFile::SetConfigFile(const char * _filename, const int type, const bool autosave, const bool readcurrentfile)
{
#ifdef MPLATFORM_WIN32
	if ((_filename[0] == '\\') && (_filename[1] == '\\'))
	{
		// unc path
		strcpy_s(filename, 1024, _filename);
	}
	else
	if (_filename[1] != ':')
	{
		// relative path
		_getcwd(filename, 1024);
		int filenamelen = strlen(filename);
		filename[filenamelen] = '\\';
		filenamelen++;
		strcpy_s(filename + filenamelen, 1024 - filenamelen - 1, _filename);
	}
	else
	{
		// absolute path
		strcpy_s(filename, 1024, _filename);
	}
#else
	if (_filename[0] != '/')
	{
		// relative path
		_getcwd(filename, 1024);
		int filenamelen = strlen(filename);
		filename[filenamelen] = '/';
		filenamelen++;
		strcpy_s(filename + filenamelen, 1024 - filenamelen - 1, _filename);
	}
	else
	{
		// absolute path
		strcpy_s(filename, 1024, _filename);
	}
#endif

	this->type = type;
	this->autosave = autosave;

	if (readcurrentfile)
	{
		FILE * f;
		f = fopen(filename, "rb");
		if (f == 0)
		{
			pt.clear();
			return 1;
		}
		fclose(f);

		if (this->type == MConfigFileTypeAutoDetect)
		{
			int filenamen = strlen(filename);
			switch (filename[filenamen - 1])
			{
			default:
			case 'i':
			case 'I':
				this->type = MConfigFileTypeINI;
				break;
			case 'l':
			case 'L':
				this->type = MConfigFileTypeXML;
				break;
			case 'n':
			case 'N':
				this->type = MConfigFileTypeJSON;
				break;
			}
		}

		switch (this->type)
		{
		default:
		case MConfigFileTypeINI:
			boost::property_tree::read_ini(filename, pt);
			break;
		case MConfigFileTypeXML:
			//http://stackoverflow.com/questions/6572550/boostproperty-tree-xml-pretty-printing
			boost::property_tree::read_xml(filename, pt, boost::property_tree::xml_parser::trim_whitespace);
			break;
		case MConfigFileTypeJSON:
			boost::property_tree::read_json(filename, pt);
			break;
		}
	}

	return 0;
}

int MConfigFile::Clear()
{
	unlink(filename);
	pt.clear();
	return 0;
}

int MConfigFile::Save()
{
	switch (type)
	{
	default:
	case MConfigFileTypeINI:
	{
		boost::property_tree::write_ini(filename, pt);
		break;
	}
	case MConfigFileTypeXML:
	{
		// http://stackoverflow.com/questions/6572550/boostproperty-tree-xml-pretty-printing
		boost::property_tree::xml_writer_settings<std::string> settings('\t', 1);
		boost::property_tree::write_xml(filename, pt, std::locale(), settings);
		break;
	}
	case MConfigFileTypeJSON:
	{
		boost::property_tree::write_json(filename, pt);
		break;
	}
	}
	return 0;
}

/*char * MConfigFileA::GetStringA(const char * section, const char * key, const char * def)
{
	char * temp = (char*)this->temp;
	GetPrivateProfileStringA(section,key,def,temp,sizeof(temp),filename);
	//return mini.GetString(section, key);
	return temp;
}*/

/*wchar_t * MConfigFileA::GetString(wchar_t * section, wchar_t * key)
{
	GetPrivateProfileStringW(section,key,L"",temp,sizeof(temp),filename);
	//return mini.GetString(section, key);
	return temp;
}*/

char * MConfigFile::GetString(const char * section, const char * key, const char * def)
{
	//GetPrivateProfileStringA(section,key,def,temp,sizeof(temp) / sizeof(char),filename);
	//return mini.GetString(section, key);

	char path[1024];
	snprintf(path, 1024, "%s.%s", section, key);
	strcpy(temp, pt.get(path, def).c_str());

	return temp;
}

bool MConfigFile::SetString(const char * section, const char * key, const char * value)
{
	char path[1024];
	snprintf(path, 1024, "%s.%s", section, key);
	pt.put(path, value);

#ifdef MPLATFORM_WIN32
	if (autosave && (type == MConfigFileTypeINI))
	{
		// because this saves comments, the boost ini writer doesn't
		return (bool)WritePrivateProfileStringA(section, key, value, filename);
	}
#endif
	
	if (autosave)
	{
		Save();
	}
	return true;
}


int MConfigFile::GetInt(const char * section, const char * key, const int valuedefault)
{
	int value = valuedefault;
	sscanf(GetString(section,key),"%i", &value);
	return value;
}

bool MConfigFile::SetInt(const char * section, const char * key, const int value)
{
	sprintf(temp, "%i", value);
	return SetString(section, key, temp);
}


unsigned int MConfigFile::GetUInt(const char * section, const char * key, const unsigned int valuedefault)
{
	unsigned int value = valuedefault;
	sscanf(GetString(section,key),"%u",&value);
	return value;
}

bool MConfigFile::SetUInt(const char * section, const char * key, const unsigned int value)
{
	sprintf(temp, "%u", value);
	return SetString(section, key, temp);
}

long long MConfigFile::GetInt64(const char * section, const char * key, const long long valuedefault)
{
	long long value = valuedefault;
	sscanf(GetString(section,key),"%lli",&value);
	return value;
}

bool MConfigFile::SetInt64(const char * section, const char * key, const long long value)
{
	sprintf(temp, "%lli", value);
	return SetString(section, key, temp);
}

unsigned long long MConfigFile::GetUInt64(const char * section, const char * key, const unsigned long long valuedefault)
{
	unsigned long long value = valuedefault;
	sscanf(GetString(section,key),"%llu",&value);
	return value;
}

bool MConfigFile::SetUInt64(const char * section, const char * key, const unsigned long long value)
{
	sprintf(temp, "%llu", value);
	return SetString(section, key, temp);
}

short int MConfigFile::GetSInt(const char * section, const char * key, const short int valuedefault)
{
	int value = (int)valuedefault;
	sscanf(GetString(section,key),"%i", &value);
	return (short int)value;
}

bool MConfigFile::SetSInt(const char * section, const char * key, const short int value)
{
	sprintf(temp, "%i", ((int)value));
	return SetString(section, key, temp);
}

unsigned short int MConfigFile::GetUSInt(const char * section, const char * key, const unsigned short int valuedefault)
{
	unsigned int value = (unsigned int)valuedefault;
	sscanf(GetString(section,key),"%u", &value);
	return (unsigned short int)value;
}

bool MConfigFile::SetUSInt(const char * section, const char * key, const unsigned short int value)
{
	sprintf(temp, "%u", ((unsigned int)value));
	return SetString(section, key, temp);
}


float MConfigFile::GetFloat(const char * section, const char * key, const char * def)
{
	float value = 0;
	sscanf(GetString(section, key, def), "%f", &value);
	return value;
}

float MConfigFile::GetFloat(const char * section, const char * key, const float def)
{
	float value = def;
	sscanf(GetString(section, key), "%f", &value);
	return value;
}

bool MConfigFile::SetFloat(const char * section, const char * key, const float value)
{
	sprintf(temp, "%.8f" ,value);
	return SetString(section, key, temp);
}


double MConfigFile::GetDouble(const char * section, const char * key)
{
	double value = 0;
	sscanf(GetString(section,key), "%lf", &value);
	return (double)value;
}

bool MConfigFile::SetDouble(const char * section, const char * key, const double value)
{
	sprintf(temp, "%.16lf", value);
	return SetString(section, key, temp);
}

bool MConfigFile::GetBool(const char * section, const char * key, const char * def)
{
	int value = 0;
	sscanf(GetString(section, key, def), "%i", &value);
	return (value != 0);
}

bool MConfigFile::GetBool(const char * section, const char * key, const bool def)
{
	int value = def ? 1 : 0;
	sscanf(GetString(section, key), "%i", &value);
	return (value != 0);
}

bool MConfigFile::SetBool(const char * section, const char * key, const bool value)
{
	if (value)
	{
		return SetString(section, key, "1");
	}
	else
	{
		return SetString(section, key, "0");
	}
}

MNetIP MConfigFile::GetIP(const char * section, const char * key)
{
	unsigned int a, b, c, d;
	sscanf(GetString(section,key), "%u.%u.%u.%u", &a, &b, &c, &d);
	return a | (b<<8) | (c<<16) | (d<<24);
}

bool MConfigFile::SetIP(const char * section, const char * key, const MNetIP ip)
{
	char s[64];
	snprintf(s, 64, "%u.%u.%u.%u",
		(ip) & 0xff,
		(ip>>8) & 0xff,
		(ip>>16) & 0xff,
		(ip>>24) & 0xff
		);
	return SetString(section, key, s);
}

MNetPort MConfigFile::GetPort(const char * section, const char * key)
{
	return GetUInt(section, key);
}

#ifdef SetPort
#undef SetPort
#define MUNICODESAVE
#endif
bool MConfigFile::SetPortW(const char * section, const char * key, const MNetPort port)
#ifdef MUNICODESAVE
#undef MUNICODESAVE
//#define SetPort SetPortW
#endif
{
	return SetUInt(section, key, port);
}




// new interface, allowing real trees for xml and json

char * MConfigFile::GString(const char * key, const char * def)
{
	strcpy(temp, pt.get(key, def).c_str());
	return temp;
}

bool MConfigFile::SString(const char * key, const char * value)
{
	pt.put(key, value);

#ifdef MPLATFORM_WIN32
	if (autosave && (type == MConfigFileTypeINI))
	{
		// because this saves comments, the boost ini writer doesn't
		char section[1024];
		strcpy(section, key);
		char * k = strchr(section, '.');
		if (k != 0)
		{
			*k = 0;
			k++;
			return (bool)WritePrivateProfileStringA(section, key, value, filename);
		}
	}
#endif

	if (autosave)
	{
		Save();
	}
	return true;
}

int MConfigFile::GInt(const char * key, const int valuedefault)
{
	int value = valuedefault;
	sscanf(GString(key), "%i", &value);
	return value;
}

bool MConfigFile::SInt(const char * key, const int value)
{
	sprintf(temp, "%i", value);
	return SString(key, temp);
}

unsigned int MConfigFile::GUInt(const char * key, const unsigned int valuedefault)
{
	unsigned int value = valuedefault;
	sscanf(GString(key), "%u", &value);
	return value;
}

bool MConfigFile::SUInt(const char * key, const unsigned int value)
{
	sprintf(temp, "%u", value);
	return SString(key, temp);
}

long long MConfigFile::GInt64(const char * key, const long long valuedefault)
{
	long long value = valuedefault;
	sscanf(GString(key), "%lli", &value);
	return value;
}

bool MConfigFile::SInt64(const char * key, const long long value)
{
	sprintf(temp, "%lli", value);
	return SString(key, temp);
}

unsigned long long MConfigFile::GUInt64(const char * key, const unsigned long long valuedefault)
{
	unsigned long long value = valuedefault;
	sscanf(GString(key), "%llu", &value);
	return value;
}

bool MConfigFile::SUInt64(const char * key, const unsigned long long value)
{
	sprintf(temp, "%llu", value);
	return SString(key, temp);
}

short int MConfigFile::GSInt(const char * key, const short int valuedefault)
{
	int value = (int)valuedefault;
	sscanf(GString(key), "%i", &value);
	return (short int)value;
}

bool MConfigFile::SSInt(const char * key, const short int value)
{
	sprintf(temp, "%i", ((int)value));
	return SString(key, temp);
}

unsigned short int MConfigFile::GUSInt(const char * key, const unsigned short int valuedefault)
{
	unsigned int value = (unsigned int)valuedefault;
	sscanf(GString(key), "%u", &value);
	return (unsigned short int)value;
}

bool MConfigFile::SUSInt(const char * key, const unsigned short int value)
{
	sprintf(temp, "%u", ((unsigned int)value));
	return SString(key, temp);
}


float MConfigFile::GFloat(const char * key, const char * def)
{
	float value = 0;
	sscanf(GString(key, def), "%f", &value);
	return value;
}

float MConfigFile::GFloat(const char * key, const float def)
{
	float value = def;
	sscanf(GString(key), "%f", &value);
	return value;
}

bool MConfigFile::SFloat(const char * key, const float value)
{
	sprintf(temp, "%.8f", value);
	return SString(key, temp);
}


double MConfigFile::GDouble(const char * key)
{
	double value = 0;
	sscanf(GString(key), "%lf", &value);
	return (double)value;
}

bool MConfigFile::SDouble(const char * key, const double value)
{
	sprintf(temp, "%.16lf", value);
	return SString(key, temp);
}

bool MConfigFile::GBool(const char * key, const char * def)
{
	int value = 0;
	sscanf(GString(key, def), "%i", &value);
	return (value != 0);
}

bool MConfigFile::GBool(const char * key, const bool def)
{
	int value = def ? 1 : 0;
	sscanf(GString(key), "%i", &value);
	return (value != 0);
}

bool MConfigFile::SBool(const char * key, const bool value)
{
	if (value)
	{
		return SString(key, "1");
	}
	else
	{
		return SString(key, "0");
	}
}

MNetIP MConfigFile::GIP(const char * key)
{
	unsigned int a, b, c, d;
	sscanf(GString(key), "%u.%u.%u.%u", &a, &b, &c, &d);
	return a | (b << 8) | (c << 16) | (d << 24);
}

bool MConfigFile::SIP(const char * key, const MNetIP ip)
{
	char s[64];
	snprintf(s, 64, "%u.%u.%u.%u",
		(ip) & 0xff,
		(ip >> 8) & 0xff,
		(ip >> 16) & 0xff,
		(ip >> 24) & 0xff
	);
	return SString(key, s);
}

MNetPort MConfigFile::GPort(const char * key)
{
	return GUInt(key);
}

#ifdef SetPort
#undef SetPort
#define MUNICODESAVE
#endif
bool MConfigFile::SPortW(const char * key, const MNetPort port)
#ifdef MUNICODESAVE
#undef MUNICODESAVE
//#define SetPort SetPortW
#endif
{
	return SUInt(key, port);
}
