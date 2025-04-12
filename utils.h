#pragma once
#include <string>
#include <fstream>
#include <icucommon.h>
#include <vector>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <locale>
#include <algorithm>
#include <cctype>
#include <codecvt>
#ifdef _WIN32
#include <windows.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#endif

using namespace std;
class utils
{

public:
    static bool directory_exists(const string& path) {
#ifdef _WIN32
        DWORD attrib = GetFileAttributesA(path.c_str());
        return (attrib != INVALID_FILE_ATTRIBUTES &&
            (attrib & FILE_ATTRIBUTE_DIRECTORY));
#else
        struct stat info;
        return (stat(path.c_str(), &info) == 0 && S_ISDIR(info.st_mode));
#endif
    }

    static bool create_directory_recursive(const string& path) {
        string current;
        vector<string> parts;

        string normalized = path;
        replace(normalized.begin(), normalized.end(), '\\', '/');

        size_t pos = 0;
        while ((pos = normalized.find('/', pos)) != string::npos) {
            parts.push_back(normalized.substr(0, pos));
            pos++;
        }
        parts.push_back(normalized);

        for (const auto& part : parts) {
            current = part.empty() ? "" : part;
            if (current.empty() || directory_exists(current)) continue;

#ifdef _WIN32
            if (!CreateDirectoryA(current.c_str(), NULL)) {
                if (GetLastError() != ERROR_ALREADY_EXISTS) return false;
            }
#else
            if (mkdir(current.c_str(), 0777) != 0) {
                if (errno != EEXIST) return false;
            }
#endif
        }
        return true;
    }

    static string create_folder_from_file(const string& file_path) {
        size_t last_slash = file_path.find_last_of("/\\");
        size_t last_dot = file_path.find_last_of('.');

        string new_path = file_path;
        if (last_dot != string::npos &&
            (last_slash == string::npos || last_dot > last_slash)) {
            new_path = file_path.substr(0, last_dot);
        }

        if (create_directory_recursive(new_path)) {
            return new_path;
        }
        return "";
    }
    static vector<string> get_files_by_extension(const string& directory,
        const string& extension) {
        vector<string> result;
        string dot_extension = "." + extension;

#ifdef _WIN32
        // Windows реализация
        WIN32_FIND_DATAA find_data;
        HANDLE hFind = FindFirstFileA((directory + "\\*" + dot_extension).c_str(), &find_data);

        if (hFind == INVALID_HANDLE_VALUE) return result;

        do {
            if (!(find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                result.push_back(find_data.cFileName);
            }
        } while (FindNextFileA(hFind, &find_data));

        FindClose(hFind);
#else
        // Linux/macOS реализация
        DIR* dir;
        struct dirent* ent;

        if ((dir = opendir(directory.c_str())) != NULL) {
            while ((ent = readdir(dir)) != NULL) {
                string filename(ent->d_name);

                if (filename.size() > dot_extension.size() &&
                    filename.substr(filename.size() - dot_extension.size()) == dot_extension) {
                    result.push_back(filename);
                }
            }
            closedir(dir);
        }
#endif

        // Сортируем результат
        sort(result.begin(), result.end());
        return result;
    }
    static uint32_t getFileSize(string file) {
        ifstream f(file, ios::binary);
        f.seekg(0, ios::end);
        uint32_t tm = f.tellg();
        f.close();
        return tm;
    }
    static uint32_t getFileSize(ifstream& f) {
        uint32_t pos = f.tellg();
        f.seekg(0, ios::end);
        uint32_t end = f.tellg();
        f.seekg(pos);
        return end;
    }
    static vector<uint8_t> gettag(const string& input);
	static string replace_extension(const string& filename, const string& new_ext);
    static wstring getstring(ifstream& f, bool ignoreTags);
    static bool contains(const vector<string>& vec, const string& target) {
        string targetLower = target;
        transform(targetLower.begin(), targetLower.end(), targetLower.begin(), ::tolower);

        for (const auto& str : vec) {
            string strLower = str;
            transform(strLower.begin(), strLower.end(), strLower.begin(), ::tolower);
            if (strLower == targetLower) {
                return true;
            }
        }
        return false;
    }
    static uint16_t readuint16(ifstream& f) {
        uint16_t value;
        f.read(reinterpret_cast<char*>(&value), sizeof(value));
        return value;
    }
    static vector<uint8_t> get_utf16_bytes(const u16string& wstr, bool add_bom = false) {
        vector<uint8_t> bytes;

        // Добавить BOM (0xFFFE для UTF-16 LE)
        if (add_bom) {
            bytes.push_back(0xFF);
            bytes.push_back(0xFE);
        }

        // Преобразование каждого символа
        for (wchar_t c : wstr) {
            // Little-endian порядок
            bytes.push_back(static_cast<uint8_t>(c & 0xFF));       // Младший байт
            bytes.push_back(static_cast<uint8_t>((c >> 8) & 0xFF)); // Старший байт
        }

        return bytes;
    }

    static int16_t readint16(ifstream& f) {
        int16_t value;
        f.read(reinterpret_cast<char*>(&value), sizeof(value));
        return value;
    }
    
    static int32_t readint32(ifstream& f) {
        int32_t value;
        f.read(reinterpret_cast<char*>(&value), sizeof(value));
        return value;
    }
    
    static uint32_t readuint32(ifstream& f) {
        uint32_t value;
        f.read(reinterpret_cast<char*>(&value), sizeof(value));
        return value;
    }
    template <typename T>
    static bool readStruct(ifstream& file, T& obj) {
        static_assert(is_trivially_copyable<T>::value,
            "Struct type need to be POD");
        if (!file.read(reinterpret_cast<char*>(&obj), sizeof(T))) {
            return false;
        }
        return true;
    }
    static void alignPos(ofstream& file, int align) {
        uint32_t pos = file.tellp();
        while (pos % align != 0) {
            file.seekp(1, ios::cur);
            pos = file.tellp();
        }
    }
    static void alignPos(ifstream& file, int align) {
        uint32_t pos = file.tellg();
        while (pos % align != 0) {
            file.seekg(1, ios::cur);
            pos = file.tellg();
        }
    }
    static void alignPos(fstream& file, int align, char alignByte) {
        uint32_t pos = file.tellg();
        while (pos % align != 0) {
            file.write(reinterpret_cast<char*>(alignByte), sizeof(alignByte));
            file.seekg(1, ios::cur);
            pos = file.tellg();
        }
    }
    static void skipBlock(ifstream& f);
    static bool WriteLines(const string& filename, const vector<string>& strings) {
        ofstream file(filename);
        if (!file.is_open())
            return false;

        for (const auto& str : strings)
            file << str << "\n";
        return true; 
    }
    //static bool WriteLines(const string& filename, const vector<wstring>& strings) {
    //    wofstream file(filename);
    //    if (!file.is_open())
    //        return false;

    //    for (const auto& str : strings)
    //        file << str << L"\n";
    //    return true;
    //}
    /*static bool WriteLines(const string& filename, const vector<wstring>& strings) {
        // Открываем файл в бинарном режиме
        wofstream file(filename, ios::binary);
        if (!file.is_open()) {
            return false;
        }

        // Устанавливаем локаль для UTF-16 (Windows)
        file.imbue(locale(file.getloc(),
            new codecvt_utf16<wchar_t, 0x10FFFF, little_endian>));

        // Записываем BOM для UTF-16 Little-Endian
        file << L'\xFEFF';

        for (const auto& str : strings) {
            file << str << L'\n';  // Используем \n как разделитель

            // Проверяем состояние потока после каждой записи
            if (!file.good()) {
                return false;  // Ошибка записи
            }
        }

        return true;
    }*/
    static bool WriteLines(const string& filename,
        const vector<wstring>& strings) {
        ofstream file(filename, ios::binary);
        if (!file.is_open()) {
            return false;
        }

        // Добавляем BOM для UTF-8 (опционально)
        //file << "\xEF\xBB\xBF";

        wstring_convert<codecvt_utf8<wchar_t>> converter;
        for (const auto& wstr : strings) {
            string utf8_str = converter.to_bytes(wstr + L'\n');
            file << utf8_str;
            if (!file.good()) {
                return false;
            }
        }

        return true;
    }
    static vector<string> readLines(const string& filename) {
        vector<string> lines;
        ifstream file(filename);

        if (file.is_open()) {
            string line;
            while (getline(file, line)) {
                lines.push_back(line);
            }
            file.close();
        }

        return lines;
    }
    static vector<uint16_t> utf8_to_utf16(const string& utf8) {
        vector<uint16_t> utf16;
        size_t i = 0;
        const size_t len = utf8.size();

        while (i < len) {
            uint32_t codepoint = 0;
            uint8_t first = utf8[i++];

            // Однобайтовый символ (0xxxxxxx)
            if (first < 0x80) {
                codepoint = first | 0;
            }
            // Двухбайтовый символ (110xxxxx 10xxxxxx)
            else if ((first & 0xE0) == 0xC0) {
                if (i >= len) throw runtime_error("Invalid UTF-8");
                codepoint = ((first & 0x1F) << 6) | (utf8[i++] & 0x3F);
            }
            // Трёхбайтовый символ (1110xxxx 10xxxxxx 10xxxxxx)
            else if ((first & 0xF0) == 0xE0) {
                if (i + 1 >= len) throw runtime_error("Invalid UTF-8");
                codepoint = ((first & 0x0F) << 12) |
                    ((utf8[i++] & 0x3F) << 6) |
                    (utf8[i++] & 0x3F);
            }
            // Четырёхбайтовый символ (11110xxx 10xxxxxx 10xxxxxx 10xxxxxx)
            else if ((first & 0xF8) == 0xF0) {
                if (i + 2 >= len) throw runtime_error("Invalid UTF-8");
                codepoint = ((first & 0x07) << 18) |
                    ((utf8[i++] & 0x3F) << 12) |
                    ((utf8[i++] & 0x3F) << 6) |
                    (utf8[i++] & 0x3F);
            }
            else {
                throw runtime_error("Invalid UTF-8");
            }

            // Преобразование в UTF-16
            if (codepoint <= 0xFFFF) {
                utf16.push_back(static_cast<uint16_t>(codepoint));
            }
            else {
                // Суррогатная пара для символов > U+FFFF
                codepoint -= 0x10000;
                utf16.push_back(static_cast<uint16_t>(0xD800 | (codepoint >> 10)));
                utf16.push_back(static_cast<uint16_t>(0xDC00 | (codepoint & 0x3FF)));
            }
        }

        return utf16;
    }
    static void writebytes(fstream& f, string s);

    static void writeuint16(fstream& f, uint16_t value) {
        f.write(reinterpret_cast<const char*>(&value), sizeof(value));
    }

    static void writeint16(fstream& f, int16_t value) {
        f.write(reinterpret_cast<const char*>(&value), sizeof(value));
    }
    static void writeint32(fstream& f, int32_t value) {
        f.write(reinterpret_cast<const char*>(&value), sizeof(value));
    }
    static void writeuint32(fstream& f, uint32_t value) {
        f.write(reinterpret_cast<const char*>(&value), sizeof(value));
    }
    static void writeuint16(ofstream& f, uint16_t value) {
        f.write(reinterpret_cast<const char*>(&value), sizeof(value));
    }

    static void writeint16(ofstream& f, int16_t value) {
        f.write(reinterpret_cast<const char*>(&value), sizeof(value));
    }
    static void writeint32(ofstream& f, int32_t value) {
        f.write(reinterpret_cast<const char*>(&value), sizeof(value));
    }
    static void writeuint32(ofstream& f, uint32_t value) {
        f.write(reinterpret_cast<const char*>(&value), sizeof(value));
    }
    static std::string get_extension(const std::string& filepath) {
        size_t last_slash = filepath.find_last_of("/\\");
        size_t last_dot = filepath.find_last_of('.');
        if (last_dot != std::string::npos &&
            (last_slash == std::string::npos || last_dot > last_slash) &&
            last_dot != filepath.length() - 1) {
            return filepath.substr(last_dot + 1);
        }
        return "";
    }
    static bool is_directory(const std::string& path) {
#ifdef _WIN32
        DWORD attrib = GetFileAttributesA(path.c_str());
        return (attrib != INVALID_FILE_ATTRIBUTES) &&
            (attrib & FILE_ATTRIBUTE_DIRECTORY);
#else
        struct stat info;
        if (stat(path.c_str(), &info) != 0) return false;
        return S_ISDIR(info.st_mode);
#endif
    }

    bool is_file(const std::string& path) {
#ifdef _WIN32
        DWORD attrib = GetFileAttributesA(path.c_str());
        return (attrib != INVALID_FILE_ATTRIBUTES) &&
            !(attrib & FILE_ATTRIBUTE_DIRECTORY);
#else
        struct stat info;
        if (stat(path.c_str(), &info) != 0) return false;
        return S_ISREG(info.st_mode);
#endif
    }
};

