#include "EditorSettings.h"
#include <boost/filesystem/fstream.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>

EditorSettings::EditorSettings(std::wstring fileName) {
	this->fileName = fileName;
	loadFile();
}

bool EditorSettings::saveFile(std::string version) {
	boost::filesystem::ofstream fout(fileName, std::ios_base::binary);
	if (fout) {
		char fileBOM[] = { 0xFF, 0xFE };
		fout.write(fileBOM, sizeof(fileBOM));
		
		long allocSize = version.length() + 1;
		wchar_t *alloc = new wchar_t[allocSize];
		mbstowcs(alloc, version.c_str(), allocSize);
		std::wstring verString = L"//MooseEdit version ";
		verString += alloc;
		verString += L"\r\n";
		fout.write((char *)verString.c_str(), 2*verString.length());
		delete[] alloc;
		
		std::wstring eq = L"=\"";
		std::wstring end = L"\"\r\n";
		for (PropertyMap::iterator it = propertyMap.begin(); it != propertyMap.end(); ++it) {
			std::wstring line = it->first + eq + it->second + end;
			fout.write((char *)line.c_str(), 2*line.length());
		}
		fout.close();
		return true;
	}
	return false;
}

bool EditorSettings::loadFile() {
	boost::filesystem::ifstream fin(fileName, std::ios_base::binary);
	typedef boost::tokenizer<boost::escaped_list_separator<wchar_t>, std::wstring::const_iterator, std::wstring> PropertyTokenizer;
	if (fin) {
		char fileBOM[2];
		fin.read(fileBOM, sizeof(fileBOM));
		for (;;) {
			std::string line = "";
			std::getline(fin, line, '\n');
			char oneByte[1];
			fin.read(oneByte, sizeof(oneByte));
			if (line.length() > 0 && line[line.length() - 1] == '\0') {
				line += '\0';
				line += '\0';
			} else {
				break;
			}
			if (!fin) {
				break;
			}
			std::wstring wline = (wchar_t *)line.c_str();
			if (boost::ends_with(wline, L"\r")) {
				wline.resize(wline.length() - 1);
			}
			PropertyTokenizer tok(wline, boost::escaped_list_separator<wchar_t>(L"", L"=", L"\""));
			bool first = true;
			std::wstring keyToken = L"";
			for (PropertyTokenizer::iterator it = tok.begin(); it != tok.end(); ++it) {
				const std::wstring &token = *it;
				if (first) {
					keyToken = token;
					first = false;
				} else {
					if (!boost::starts_with(keyToken, L"//") && keyToken.length() > 0 && token.length() > 0) {
						this->setProperty(keyToken, token);
					}
					break;
				}
			}
		}
		fin.close();
		return true;
	}
	return false;
}
