#include <Windows.h>
#include <iostream>
#include <sstream>
#include <filesystem>

void CreateEncryptedFile(const std::string& filepath, const std::string& directoryPath)
{
	FILE* src;

	int size;
	char* buf;

	fopen_s(&src, filepath.c_str(), "rb");

	fseek(src, 0, SEEK_END);
	size = ftell(src);
	fseek(src, 0, SEEK_SET);

	buf = new char[size];

	fread_s(buf, size, 1, size, src);

	fclose(src);

	for (int i = 0; i < size; i++)
	{
		buf[i] = ~buf[i];
	}

	size_t idx = filepath.rfind(".");
	size_t idx2 = filepath.rfind("\\");

	std::string path;
	if (idx != std::string::npos)
	{
		path = filepath.substr(idx2 + 1, idx - (idx2 + 1));
	}
	else
	{
		path = filepath.substr(idx2 + 1);
	}
	std::string newfilepath = ".\\EncryptedResources\\" + directoryPath + path + ".rescrypt";

	FILE* dst;

	auto&& err = fopen_s(&dst, newfilepath.c_str(), "wb");
	if (err != 0)
	{
		return;
	}

	fwrite(buf, 1, size, dst);

	fclose(dst);

	printf(newfilepath.c_str());
	printf("\n");
}

int main(int argc, char* argv[])
{
	if (argc >= 2)
	{
		std::filesystem::create_directory("./EncryptedResources");
	}

    for (int i = 1; i < argc; i++)
    {
        std::string filepath = argv[i];

		size_t idx = filepath.rfind(".");
		size_t idx2 = filepath.rfind("\\");

		std::filesystem::file_status st = std::filesystem::status(filepath);

		if (st.type() == std::filesystem::file_type::directory)
		{
			std::string direc = filepath.substr(idx2);
			bool r = std::filesystem::create_directory(std::string(".\\EncryptedResources") + direc);

			for (auto&& dir : std::filesystem::recursive_directory_iterator(filepath))
			{
				std::string fp = dir.path().string();
				std::filesystem::file_status stats = std::filesystem::status(fp);
				if (stats.type() == std::filesystem::file_type::directory)
				{
					std::string direc = fp.substr(idx2);
					bool r = std::filesystem::create_directory(std::string(".\\EncryptedResources") + direc);
				}
			}

			for (auto&& dir : std::filesystem::recursive_directory_iterator(filepath))
			{
				std::string fp = dir.path().string();
				std::filesystem::file_status stats = std::filesystem::status(fp);
				if (stats.type() == std::filesystem::file_type::directory)
				{
					continue;
				}
				size_t idx3 = fp.rfind("\\");

				if (stats.type() == std::filesystem::file_type::regular)
				{
					CreateEncryptedFile(fp, fp.substr(idx2 + 1, idx3 - idx2));
				}
			}

		}
		else
		{
			std::filesystem::file_status stats = std::filesystem::status(filepath);
			if (stats.type() == std::filesystem::file_type::regular)
			{
				CreateEncryptedFile(filepath, "");
			}
		}
    }
	MessageBoxA(nullptr, "Complete", "ResourceEncryptor", MB_OK | MB_TOPMOST);

    return 0;
}
