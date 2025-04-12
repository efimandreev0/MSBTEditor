#include "msgstdbn.h"
#pragma pack(push, 1)  
struct msgstdbn::MsgStdBn{
    char magic[8];
    uint32_t IsBE;
    char Encoding;
    char BlockCount;
};
#pragma pack(pop)  
void msgstdbn::read(string file, bool ignoreTags, bool debug) {
    ifstream f(file, ios::binary);
    if (f) {
        MsgStdBn h;
        utils::readStruct(f, h);
        f.seekg(0x20);
        for (int i = 0; i < h.BlockCount; i++) {
            char bytes[5];
            f.read(bytes, 4);
            bytes[4] = '\0';
            string magic = bytes;
            if (magic == "TXT2") {
                utils::WriteLines(utils::replace_extension(file, "txt"), txt2::read(f, ignoreTags));
                break;
            }
            else
                utils::skipBlock(f);
        }
    }
    f.close();
}
void msgstdbn::write(string file, bool debug) {
    string name = utils::replace_extension(file, "msbt");
    int pos = -1;
    ifstream f(name, ios::binary);
    if (f) {
        MsgStdBn h;
        utils::readStruct(f, h);
        f.seekg(0x20);
        for (int i = 0; i < h.BlockCount; i++) {
            char bytes[5];
            f.read(bytes, 4);
            bytes[4] = '\0';
            string magic = bytes;
            if (magic == "TXT2") {
                pos = (-4) + f.tellg();
            }
            else
                utils::skipBlock(f);
        }
        f.close();
        if (pos > -1) {
            fstream f(name, ios::binary | ios::in | ios::out);
            if (f) {
                f.seekp(pos);
                txt2::write(f, utils::readLines(file));
            }
        }
        else
            std::cout << "TXT2-Block not finded";
    }
}