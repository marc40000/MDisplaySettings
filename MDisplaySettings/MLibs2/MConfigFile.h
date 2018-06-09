#pragma once

#include <boost/property_tree/ptree.hpp>

#ifndef MNetIP
typedef unsigned int MNetIP;
#endif

#ifndef MNetPort
typedef unsigned short int MNetPort;
#endif

const int MConfigFileTypeAutoDetect = -1;
const int MConfigFileTypeINI = 0;
const int MConfigFileTypeXML = 1;
const int MConfigFileTypeJSON = 2;

class MConfigFile
{
public:
	MConfigFile(void);
	~MConfigFile(void);
	char filename[1024];
	int type;
	bool autosave;
	boost::property_tree::ptree pt;
private:
	char temp[16*1024];
	//MIni mini;
public:
	int SetConfigFile(const char * _filename, const int type = MConfigFileTypeINI, const bool autosave = true, const bool readcurrentfile = true);
	int Clear();
	int Save();
	
	//char * GetStringA(const char * section, const char * key, const char * def = "");
	char * GetString(const char * section, const char * key, const char * def = "");
	//char * MConfigFile::GetString(char * section, char * key);
	//wchar_t * MConfigFile::GetString(wchar_t * section, wchar_t * key);
	bool SetString(const char * section, const char * key, const char * value);
	
	int GetInt(const char * section, const char * key, const int valuedefault = 0);
	bool SetInt(const char * section, const char * key, const int value);

	unsigned int GetUInt(const char * section, const char * key, const unsigned int valuedefault = 0);
	bool SetUInt(const char * section, const char * key, const unsigned int value);

	long long GetInt64(const char * section, const char * key, const long long valuedefault = 0);
	bool SetInt64(const char * section, const char * key, const long long value);

	unsigned long long GetUInt64(const char * section, const char * key, const unsigned long long valuedefault = 0);
	bool SetUInt64(const char * section, const char * key, const unsigned long long value);

	short int GetSInt(const char * section, const char * key, const short int valuedefault = 0);
	bool SetSInt(const char * section, const char * key, const short int value);

	unsigned short int GetUSInt(const char * section, const char * key, const unsigned short int valuedefault = 0);
	bool SetUSInt(const char * section, const char * key, const unsigned short int value);

	float GetFloat(const char * section, const char * key, const char * def = "0");
	float GetFloat(const char * section, const char * key, const float def);
	bool SetFloat(const char * section, const char * key, const float value);

	double GetDouble(const char * section, const char * key);
	bool SetDouble(const char * section, const char * key, const double value);

	bool GetBool(const char * section, const char * key, const char * def = "0");
	bool GetBool(const char * section, const char * key, const bool def);
	bool SetBool(const char * section, const char * key, const bool value);

	MNetIP GetIP(const char * section, const char * key);
	bool SetIP(const char * section, const char * key, const MNetIP ip);

	MNetPort GetPort(const char * section, const char * key);
	bool SetPortW(const char * section, const char * key, const MNetPort port);		// << the W is very strange


	// new interface, allowing real trees for xml and json
	// key can be any point in a tree, like "root.node.node.value"
	char * GString(const char * key, const char * def = "");
	bool SString(const char * key, const char * value);

	int GInt(const char * key, const int valuedefault = 0);
	bool SInt(const char * key, const int value);

	unsigned int GUInt(const char * key, const unsigned int valuedefault = 0);
	bool SUInt(const char * key, const unsigned int value);

	long long GInt64(const char * key, const long long valuedefault = 0);
	bool SInt64(const char * key, const long long value);

	unsigned long long GUInt64(const char * key, const unsigned long long valuedefault = 0);
	bool SUInt64(const char * key, const unsigned long long value);

	short int GSInt(const char * key, const short int valuedefault = 0);
	bool SSInt(const char * key, const short int value);

	unsigned short int GUSInt(const char * key, const unsigned short int valuedefault = 0);
	bool SUSInt(const char * key, const unsigned short int value);

	float GFloat(const char * key, const char * def = "0");
	float GFloat(const char * key, const float def);
	bool SFloat(const char * key, const float value);

	double GDouble(const char * key);
	bool SDouble(const char * key, const double value);

	bool GBool(const char * key, const char * def = "0");
	bool GBool(const char * key, const bool def);
	bool SBool(const char * key, const bool value);

	MNetIP GIP(const char * key);
	bool SIP(const char * key, const MNetIP ip);

	MNetPort GPort(const char * key);
	bool SPortW(const char * key, const MNetPort port);		// << the W is very strange
};
