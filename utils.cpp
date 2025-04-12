#include "utils.h"

string utils::replace_extension(const string& filename,
    const string& new_ext) {
    // Находим последнюю точку и последний слэш
    size_t last_dot = filename.find_last_of('.');
    size_t last_slash = filename.find_last_of("/\\");

    // Проверяем, что точка находится после последнего слэша (если есть)
    if (last_dot != string::npos &&
        (last_slash == std::string::npos || last_dot > last_slash)) {
        // Обрезаем до точки
        return filename.substr(0, last_dot) + (new_ext.empty() ? "" : "." + new_ext);
    }
    // Если расширения нет - добавляем новое
    return filename + (new_ext.empty() ? "" : "." + new_ext);
}
void utils::skipBlock(ifstream& f) {
    f.seekg(-4, ios::cur);
    char bytes[5];
    f.read(bytes, 4);
    bytes[4] = '\0';
    string magic = bytes;

    uint32_t size = utils::readuint32(f) + 8;
    f.seekg(size, ios::cur);
    utils::alignPos(f, 0x10);
    std::cout << "Skipped Block " << magic << "\n";
}
wstring utils::getstring(ifstream& f, bool ignoreTags) {
    vector<string> delTags = utils::readLines("SkipTags.txt");
    wstring s = L"";
    while (true) {
        uint16_t val = readuint16(f);
        int pos = f.tellg();
        switch (val)
        {
        case 0xe:
            if (!ignoreTags) {
                uint16_t group = readuint16(f);
                uint16_t num = readuint16(f);
                if (group == 0 && num == 4) {
                    readuint16(f);
                    s += L"<p>";
                    break;
                }
                else if (utils::contains(delTags, (to_string(group)))) {
                    val = readuint16(f);
                    for (int i = 0; i < val / 2; i++) {
                        (readuint16(f));
                    }
                    break;
                }
                s += L"<" + to_wstring(group) + L"." + to_wstring(num);
                val = readuint16(f);
                for (int i = 0; i < val / 2; i++) {
                    s += L":" + to_wstring(readuint16(f));
                }
                s += L">";
            }
            else {
                uint16_t group = readuint16(f);
                uint16_t num = readuint16(f);
                val = readuint16(f);
                if (group == 0 && num == 4) {
                    s += L"<p>";
                    break;
                }
                for (int i = 0; i < val / 2; i++) {
                    (readuint16(f));
                }
            }
            break;
        case 0xf:
            if (!ignoreTags) {
                uint16_t group = readuint16(f);
                uint16_t num = readuint16(f);
                if (group == 0 && num == 4) {
                    s += L"<p>";
                    break;
                }
                if (utils::contains(delTags, (to_string(group))))
                    break;
                s += L"<" + to_wstring(group) + L"." + to_wstring(num) + L">";
            }
            else {
                uint16_t group = readuint16(f);
                uint16_t num = readuint16(f);
                if (group == 0 && num == 4) {
                    s += L"<p>";
                    break;
                }
            }
            break;
        case 0xa:
            s += L"<lf>";
            break;
        case 0x0:
            return s;
            break;
        default:
            s += val;
            break;
        }
    }
}
std::vector<uint8_t> utils::gettag(const std::string& input) {
    std::vector<uint8_t> result;

    // Удаляем угловые скобки
    std::string content = input.substr(1, input.size() - 2);

    // Разбиваем строку на части
    std::vector<std::string> parts;
    std::stringstream ss(content);
    std::string item;

    while (getline(ss, item, ':')) {
        parts.push_back(item);
    }

    // Обрабатываем первое значение (формат X.Y)
    size_t dot_pos = parts[0].find('.');
    int16_t first = static_cast<int16_t>(std::stoi(parts[0].substr(0, dot_pos)));
    int16_t second = static_cast<int16_t>(std::stoi(parts[0].substr(dot_pos + 1)));

    // Добавляем первые два значения в little-endian
    result.push_back(first & 0xFF);
    result.push_back((first >> 8) & 0xFF);
    result.push_back(second & 0xFF);
    result.push_back((second >> 8) & 0xFF);

    // Добавляем количество элементов * 2
    int16_t count = static_cast<int16_t>((parts.size() - 1) * 2);
    result.push_back(count & 0xFF);
    result.push_back((count >> 8) & 0xFF);

    // Добавляем остальные значения
    for (size_t i = 1; i < parts.size(); ++i) {
        int16_t num = static_cast<int16_t>(std::stoi(parts[i]));
        result.push_back(num & 0xFF);
        result.push_back((num >> 8) & 0xFF);
    }

    return result;
}
void utils::writebytes(fstream& f, string s) {
    std::vector<uint8_t> result;
    for (size_t i = 0; i < s.size(); i++) {
        if (s[i] == '<') {
            std::string tag;
            size_t start = i;
            while (i < s.size() && s[i] != '>') {
                tag += s[i];
                i++;
            }
            if (i < s.size()) tag += s[i]; 
            if (tag == "<lf>") {
                result.push_back(0xa);
                result.push_back(0x0);
                continue;
            }
            else if (tag == "<p>") {
                result.push_back(0xe);
                result.push_back(0x0);
                result.push_back(0x0);
                result.push_back(0x0);
                result.push_back(0x4);
                result.push_back(0x0);
                result.push_back(0x0);
                result.push_back(0x0);
                continue;
            }
            auto tagb = gettag(tag);
            result.push_back(0xe);
            result.push_back(0x0);
            result.insert(result.end(), tagb.begin(), tagb.end());
        }
        else {
            string n2 = "";
            while (s[i] != '<') {
                n2 += s[i];
                i++;
                if (i >= s.size() - 1) {
                    break;
                }
            }
            i--;
            //while (s[i] != '<') {
            //    if (s[i] < 0) {
            //        vector<char> n2;
            //
            //        n.push_back(s[i]);
            //        n.push_back(s[i + 1]);
            //        i++;
            //    }
            //    else {
            //        n.push_back(s[i]);
            //        n.push_back(0);
            //    }
            //    i++;
            //    if (i == s.size() - 1) {
            //        if (s[i] < 0) {
            //            n.push_back(s[i]);
            //            n.push_back(s[i + 1]);
            //        }
            //        else {
            //            n.push_back(s[i]);
            //            n.push_back(0);
            //        }
            //        break;
            //    }
            //}
            std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> conv16;
            std::u16string cvt = conv16.from_bytes(n2);
            for (auto& c2 : cvt) {
                result.push_back(c2 & 0xFF);
                result.push_back((c2 >> 8) & 0xFF);
            }
            cout << "";
        }
    }
    result.push_back(0);
    result.push_back(0);
    f.write(reinterpret_cast<const char*>(result.data()), result.size());
}