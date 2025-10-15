#pragma once

#include <filesystem>

class masAssetSearch
{
private:
	std::string mPath;
	std::string mFileName;
	std::string mExtension;
	std::string mDirectory;

private:
	void ReplaceDirectorySlashes(std::string& Str)
	{
		for (int32_t i = 0; i < Str.size(); ++i)
			if (Str[i] == '\\')
				Str[i] = '/';
	}

public:
	masAssetSearch(const std::string& FolderName, const std::string& FileName)
	{
		std::string AssetDir = std::filesystem::current_path().string();
		AssetDir.append("\\Assets\\");

		for (const auto& Entry : std::filesystem::recursive_directory_iterator(AssetDir))
		{
			if (Entry.is_directory() && Entry.path().filename().string() == FolderName)
			{
				mDirectory = Entry.path().string();
				mDirectory.append("/");
				ReplaceDirectorySlashes(mDirectory);
				break;
			}
		}

		if (!mDirectory.empty())
		{
			for (const auto& Entry : std::filesystem::recursive_directory_iterator(mDirectory))
			{
				if (Entry.is_regular_file() && Entry.path().filename() == FileName)
				{
					mPath      = Entry.path().string();
					mFileName  = Entry.path().filename().string();
					mExtension = Entry.path().extension().string();
					ReplaceDirectorySlashes(mPath);
					break;
				}
			}
		}

		if (mFileName.empty())
			mDirectory.clear();
	}

	const std::string& Directory() const { return mDirectory; }
	const std::string& FileName()  const { return mFileName;  }
	const std::string& Path()      const { return mPath;      }
	const std::string& Extension() const { return mExtension; }

	bool IsValid() { return !mDirectory.empty(); }
};