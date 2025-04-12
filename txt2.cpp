#include "txt2.h"
#pragma pack(push, 1)  
struct txt2::TXT2 {
    char magic[4];
    uint32_t size;
    uint64_t padding;
    uint32_t count;
};
#pragma pack(pop)  

vector<wstring> txt2::read(ifstream& f, bool ignoreTags, bool debug) {
    f.seekg(-4, ios::cur);
    uint32_t pos = 0x10 + f.tellg();
    TXT2 t;
    utils::readStruct<TXT2>(f, t);
    f.seekg(pos + 4);
    vector<wstring> strings(t.count);
    vector<uint32_t> pointers(t.count);
    for (int i = 0; i < t.count; i++)
        pointers[i] = utils::readuint32(f);
    for (int i = 0; i < t.count; i++) {
        f.seekg(pointers[i] + pos);
        strings[i] = utils::getstring(f, ignoreTags);
        if (strings[i] == L"")
            strings[i] = L"<EMPTY>";
        wcout << strings[i];
    }
    return strings;
}
void txt2::write(fstream& f, vector<string> strings, bool debug) {
    int pos = 0x10 + f.tellp();
    f.seekp(0x14 + strings.size() * 4, ios::cur);
    vector<uint32_t> pointers(strings.size());
    for (int i = 0; i < strings.size(); i++) {
        pointers[i] = (-pos) + f.tellp();
        if (strings[i] != "<EMPTY>")
            utils::writebytes(f, strings[i]);
        else
            utils::writeuint16(f, 0);
    }
    f.seekp(pos + 4, ios::beg);
    for (int i = 0; i < strings.size(); i++)
        utils::writeuint32(f, pointers[i]);
    utils::alignPos(f, 0x10, 0xab);
    uint32_t end = f.tellg();
    f.seekg(pos - 12);
    utils::writeuint32(f, (uint32_t)(end - pos));
}