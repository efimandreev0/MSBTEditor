// MessageBinaryText.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
#include <cstring>
#include "msgstdbn.h"
#include "umsbt.h"
#include "utils.h"
using namespace std;
void usage() {
    cout << "USAGE:\n";
    cout << "Extract: tool.exe -e file.msbt [delTags: -d]\n";
    cout << "Extract: tool.exe -e file.umsbt\n";
    cout << "Replace: tool.exe -r file.txt\n";
    cout << "Replace: tool.exe -r directoryWithFiles\n";
    cout << "WARNING: to replace text the folder with file.txt needs to contain file.msbt\n";
}

int main(int argc, char* argv[]) {
    //umsbt::write("NPC_Rover_Train");
    // msgstdbn::write("00000000.txt", false);
    if (argc < 2) {
        usage();
        return 1;
    }

    if (strcmp(argv[1], "-e") == 0) {
        if (argc < 3) { // Нужен как минимум путь
            usage();
            return 1;
        }

        bool d_flag = (argc >= 4 && strcmp(argv[3], "-d") == 0);
        string ext = utils::get_extension(argv[2]);
        if (ext == "msbt") {
            msgstdbn::read(argv[2], d_flag);
        }
        else if (ext == "umsbt") {
            umsbt::read(argv[2]);
        }
        return 0;

    }
    else if (strcmp(argv[1], "-r") == 0) {
        if (argc < 3) {
            usage();
            return 1;
        }
        if (utils::is_directory(argv[2])) {
            umsbt::write(argv[2]);
        }
        else {
            msgstdbn::write(argv[2], false);
        }
        return 0;
    }
    else {
        usage();
        return 1;
    }
}
// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
