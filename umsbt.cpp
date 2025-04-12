#include "umsbt.h"

void umsbt::read(string file) {
	ifstream f(file, ios::binary);
	if (f) {
		vector<uint32_t> offsets;
		vector<uint32_t> sizes;
		offsets.push_back(utils::readuint32(f));
		sizes.push_back(utils::readuint32(f));
		int c = 0;
		while (sizes[c] != 0 && offsets[c] != 0) {
			offsets.push_back(utils::readuint32(f));
			sizes.push_back(utils::readuint32(f));
			c++;
		}
		string path = utils::create_folder_from_file(file);
		for (int i = 0; i < sizes.size() - 1; i++) {
			string newp = path + "\\";
			char buffer[10];
			sprintf(buffer, "%04d.msbt", i);
			newp += buffer;
			f.seekg(offsets[i]);
			ofstream o(newp, ios::binary);
			if (o) {
				char* byts = (char*)malloc(sizes[i] * sizeof(char));
				f.read(byts, sizes[i]);
				o.write(byts, sizes[i]);
			}
			else {
				cout << "Failed to open file.";
			}
			o.close();
		}
	}
	else {
		cout << "Failed to open file.";
	}
	f.close();
}
void umsbt::write(string dir) {
	ofstream f(dir + ".umsbt", ios::binary);
	if (f) {
		vector<string> paths = utils::get_files_by_extension(dir, "msbt");
		f.write(0, (paths.size() * sizeof(uint32_t)) * 2);
		utils::alignPos(f, 0x10);
		vector<uint32_t> offsets(paths.size());
		vector<uint32_t> sizes(paths.size());
		for (int i = 0; i < paths.size(); i++) {
			offsets.push_back(f.tellp());
			uint32_t size = utils::getFileSize(paths[i]);
			sizes.push_back(size);
			ifstream o(paths[i], ios::binary);
			char* bytes = (char*)malloc(size * sizeof(char));
			o.read(bytes, size);
			f.write(bytes, size);
		}
		f.seekp(0);
		for (int i = 0; i < paths.size(); i++) {
			utils::writeuint32(f, offsets[i]);
			utils::writeuint32(f, sizes[i]);
		}
	}
	else {
		cout << "Failed to open file.";
	}
}