// BouyomiReplaceTagGenerator.cpp : �R���\�[�� �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

#include "stdafx.h"

using namespace std;
using namespace std::tr2::sys;

namespace
{
	const wchar_t* const BOUYOMI_EXE_FILE_NAME	= L"BouyomiChan.exe";
	const wchar_t* const SOUND_DIR_NAME = L"Sound";
	const wchar_t* const DIC_FILE_NAME = L"ReplaceTag.dic";
	const wstring SOUND_EXTENSIONS[] = {L".wav", L".mp3"};
}

namespace
{
	/**
	* @brief 
	*/
	class StackRuntimeError
	{
	public:
		StackRuntimeError()
			:m_desc(L"---- StackRuntimeError ---\n")
		{
		}

		StackRuntimeError(const wstring& desc)
			:m_desc(L"---- StackRuntimeError ---\n")
		{
			Push(desc);
		}

		/**
		*/
		void Push(const wstring& desc)
		{
			m_desc.append(L"---- Description ----\n");
			m_desc.append(desc);
		}

		/**
		*/
		const wstring& GetDescription() const
		{
			return m_desc;
		}

	private:
		wstring m_desc;
	};

	/**
	* @brief	�_�ǂ݂���񎫏����R�[�h
	*/
	struct BOUYOMI_RECORD
	{
		int		priority;
		wstring	text;
		path	soundPath;
	};

	//�w��f�B���N�g���ȉ��Ɏw�薼�t�@�C�������݂��邩�`�F�b�N
	void _CheckFile(const path& parent, const wchar_t* const child)
	{
		const path target = parent / child;

		if (!exists(target))
		{
			wostringstream oss;
			oss << L"�t�@�C�������݂��܂���" << endl;
			oss << L"�p�X : " << target << endl;
			throw StackRuntimeError(oss.str());
		}

		if (!is_regular_file(target))
		{
			wostringstream oss;
			oss << L"�t�@�C���ł͂���܂���" << endl;
			oss << L"�p�X : " << target << endl;
			throw StackRuntimeError(oss.str());
		}
	}

	//�w��f�B���N�g���ȉ��Ɏw�薼�f�B���N�g�������݂��邩�`�F�b�N
	void _CheckDir(const path& parent, const wchar_t* const child)
	{
		const path target = parent / child;

		if (!exists(target))
		{
			wostringstream oss;
			oss << L"�f�B���N�g�������݂��܂���" << endl;
			oss << L"�p�X : " << target << endl;
			throw StackRuntimeError(oss.str());
		}

		if (!is_directory(target))
		{
			wostringstream oss;
			oss << L"�f�B���N�g���ł͂���܂���" << endl;
			oss << L"�p�X : " << target << endl;
			throw StackRuntimeError(oss.str());
		}
	}


	// ���s�O�̃`�F�b�N
	void _PreCheck(const path& bouyomiPath)
	{
		try
		{
			//�_�ǂ݂���񂪃J�����g�ɑ��݂��邩�m�F
			_CheckFile(bouyomiPath, BOUYOMI_EXE_FILE_NAME);

			// Sound�f�B���N�g�����J�����g�ȉ��ɑ��݂��邩�m�F
			_CheckDir(bouyomiPath, SOUND_DIR_NAME);

			//�����t�@�C�����J�����g�ȉ��ɑ��݂��邩�m�F
			_CheckFile(bouyomiPath, DIC_FILE_NAME);
		}
		catch (StackRuntimeError& e)
		{
			wostringstream oss;
			oss << L"�Ώۃf�B���N�g���ȉ��ɕK�v�ȃf�B���N�g��/�t�@�C�������݂��܂���" << endl;
			oss << L"�Ώۃf�B���N�g����" << BOUYOMI_EXE_FILE_NAME << L" �̒u���Ă���f�B���N�g�����w�肵�Ă��炳��" << endl;
			e.Push(oss.str());
			throw e;
		}
	}

	//�����t�@�C���ł��邩
	bool _IsSoundFile(const path& filePath)
	{
		for (const auto& ext : SOUND_EXTENSIONS)
		{
			if (filePath.extension().wstring() == ext)
			{
				return true;
			}
		}
		return false;
	}

	//�����t�@�C����S�ė񋓂���
	vector<path> _EnumerateSound(const path& bouyomiPath)
	{
		const path SOUND_DIR_PATH = bouyomiPath / SOUND_DIR_NAME;

		// target �ȉ����ċA�I�ɒT������
		vector<path> soundPaths;
		const recursive_directory_iterator end;
		for (recursive_directory_iterator iter(SOUND_DIR_PATH); iter != end; ++iter)
		{
			//�����ȊO��e��
			if( !_IsSoundFile(*iter) )
			{
				continue;
			}
			//�����t�@�C�����X�g�ɒǉ�
			soundPaths.push_back(*iter);
		}

		return move(soundPaths);
	}

	//�p�X�� Sound �f�B���N�g������Ƃ��鑊�΃p�X�ɐ��K��
	void _NormalizeSoundPaths(const path& bouyomiPath, vector<path>& paths)
	{
		//�p�X��wstring�𐶐�
		const wstring soundDirPathStr = (bouyomiPath / SOUND_DIR_NAME).wstring();

		//�S�Ẳ����t�@�C���ɂ��ď���
		for (auto& soundFilePath : paths)
		{
			//�����t�@�C���p�X��wstring���Ƃ��Ă���
			const wstring soundFilePathStr = soundFilePath.wstring();
			//<�����t�@�C���p�X>�̐擪��<Sound�f�B���N�g���p�X>���܂܂�邱�Ƃ��m�F
			const size_t foundPos = soundFilePathStr.find(soundDirPathStr);
			if (foundPos == wstring::npos)
			{
				wostringstream oss;
				oss << L"�����t�@�C���p�X���K����" << endl;
				oss << L"<�����t�@�C���p�X>����<Sound�f�B���N�g���p�X>���܂܂�Ă��܂���" << endl;
				oss << L"�قڊԈႢ�Ȃ��v���O�������Ԉ���Ă��܂�" << endl;
				oss << L"<�����t�@�C���p�X> : " << soundFilePathStr << endl;
				oss << L"<Sound�f�B���N�g���p�X> : " << soundDirPathStr << endl;
				throw StackRuntimeError(oss.str());
			}
			if (foundPos != 0)
			{
				wostringstream oss;
				oss << L"�����t�@�C���p�X���K����" << endl;
				oss << L"<Sound�f�B���N�g���p�X>��<�����t�@�C���p�X>�̐擪�ȊO�Ō�����܂���" << endl;
				oss << L"�قڊԈႢ�Ȃ��v���O�������Ԉ���Ă��܂�" << endl;
				oss << L"<�����t�@�C���p�X> : " << soundFilePathStr << endl;
				oss << L"<Sound�f�B���N�g���p�X> : " << soundDirPathStr << endl;
				throw StackRuntimeError(oss.str());
			}
			//<�����t�@�C���p�X>����<�_�ǂ݂����z�u�f�B���N�g���p�X>�����O
			//�擪�� \ ���c���Ă��܂��̂� +1 ����
			soundFilePath = soundFilePathStr.substr(soundDirPathStr.length() + 1);
		}
	}

	//�w��t�@�C�����o�b�N�A�b�v
	void _BackUpFile(const path& targetFilePath)
	{
		//�w��t�@�C�������݂��Ȃ���Ή������Ȃ�
		if (!is_regular_file(targetFilePath))
		{
			return;
		}

		//�w��t�@�C���̐e�f�B���N�g���p�X���擾
		const path parentDirPath = targetFilePath.parent_path();

		//�����̃o�b�N�A�b�v�t�@�C���̒ʂ��ԍ��̍ő�l���擾
		wregex regularExpression(targetFilePath.filename().wstring() + wstring(L".bak([0-9]+)"));
		wsmatch matchResult;
		int maxLogNumber = 0;
		const directory_iterator end;
		for (directory_iterator iter(parentDirPath); iter != end; ++iter)
		{
			//���K�\���Ŋ����o�b�N�A�b�v�t�@�C��������
			const wstring fileNameStr = iter->path().filename().wstring();
			if (!regex_match(fileNameStr, matchResult, regularExpression))
			{
				continue;
			}
			if (matchResult.size() != 2)
			{
				continue;
			}
			//�ő�o�b�N�A�b�v�ʂ��ԍ����t�@�C��������擾
			int logNum;
			wistringstream iss(matchResult[1]);
			iss >> logNum;
			//�ő�o�b�N�A�b�v�ʂ��ԍ����X�V
			if ( maxLogNumber < logNum )
			{
				maxLogNumber = logNum;
			}
		}
		
		//�o�b�N�A�b�v�����
		{
			wostringstream oss;
			oss << targetFilePath.filename() << L".bak" << setw(3) << setfill(L'0') << maxLogNumber + 1;
			const path backupFilePath = targetFilePath.parent_path() / oss.str();
			try
			{
				copy_file(targetFilePath, backupFilePath);
			}
			catch (...)
			{
				wostringstream oss;
				oss << L"�o�b�N�A�b�v�t�@�C���̍쐬�Ɏ��s���܂���" << endl;
				oss << L"�o�b�N�A�b�v�t�@�C���̑��݂���f�B���N�g���̏������݌����͂���܂����H" << endl;
				oss << L"�o�b�N�A�b�v�Ώ� : " << targetFilePath << endl;
				oss << L"�o�b�N�A�b�v��@ : " << backupFilePath << endl;
				throw StackRuntimeError(oss.str());
			}
		}		
	}

	//�����t�@�C���p�X�񂩂玫���𐶐�
	vector<BOUYOMI_RECORD> _CreatDictionary(const vector<path>& soundPaths)
	{
		vector<BOUYOMI_RECORD> dict;
		BOUYOMI_RECORD record;
		record.priority = 99;
		dict.reserve(soundPaths.size());
		for(const auto& soundFilePath : soundPaths)
		{
			record.soundPath = soundFilePath;
			record.text = soundFilePath.stem().wstring();
			dict.push_back(record);
		}
		return dict;
	}

	/**
	*/
	void _Print(FILE* fp, const vector<BOUYOMI_RECORD>& dict)
	{
		//�P���R�[�h�Â�
		for (const auto& record : dict)
		{
			fwprintf(fp, L"%d\tN\t%s\t(SoundW %s)\n", record.priority, record.text.c_str(), record.soundPath.c_str());
		}
	}
}

//�������X�g���[���ɗ���
wostream& operator <<(wostream& os, const vector<BOUYOMI_RECORD>& dict)
{
	//�P���R�[�h�Â�
	for (const auto& record : dict)
	{
		os << record.priority << L"\t";
		os << L"N" << L"\t";
		os << record.text << L"\t";
		os << L"(SoundW " << record.soundPath << L")" << L"\n";
	}
	return os;
}

/**
*/
wostream& operator <<(wostream& os, const StackRuntimeError& e)
{
	os << e.GetDescription();
	return os;
}

//�G���g���|�C���g
int main(int argc, char* argv[])
{
	const locale jaLoc("japanese");
	locale::global(jaLoc);
	cout.imbue(jaLoc);
	wcerr.imbue(jaLoc);
	wcout.imbue(jaLoc);
	wcerr.imbue(jaLoc);
	setlocale(LC_ALL, jaLoc.c_str());

	//���s���̃A�v���P�[�V�����t�@�C�������擾
	const wstring applicationName = path(argv[0]).filename().wstring();

	wostringstream helpOss;
	helpOss << L"_/ �w���v _/" << endl;
	helpOss << L"�g�����P : ������n������ " << applicationName << L" ���N������" << endl;
	helpOss << L"�_�ǂ݂����z�u�f�B���N�g�����J�����g�f�B���N�g�����Ɖ��肵�ď��������܂��B" << endl;
	helpOss << L"�܂�A�_�ǂ݂����Ɠ����K�w�ɂ��̃v���O������z�u���ă_�u���N���b�N���Ă��������B" << endl;
	helpOss << endl;
	helpOss << L"�g�����Q : " << applicationName << L" <�_�ǂ݂����z�u�f�B���N�g���p�X>" << endl;
	helpOss << L"��P�����Ŗ_�ǂ݂���񂪔z�u����Ă���f�B���N�g���̃p�X���w�肵�܂�" << endl;
	helpOss << L"�R�}���h���C������Ăяo���Ă��������B" << endl;


	//�_�ǂ݂����z�u�f�B���N�g���p�X���������猈��
	path bouyomiDirPath;
	switch (argc)
	{
	case 1:
		bouyomiDirPath = current_path();
		break;
	case 2:
		bouyomiDirPath = argv[1];
		break;
	default:
		wostringstream oss;
		oss << L"�����̌����s���ł�" << endl;
		oss << L"���҂���� : 0 ���� 1 ��" << endl;
		oss << L"�n���ꂽ�� : " << argc - 1 << endl;
		throw StackRuntimeError(oss.str());
	}

	try
	{
		const path dicFilePath = bouyomiDirPath / DIC_FILE_NAME;


		//���s�O�`�F�b�N
		_PreCheck(bouyomiDirPath);

		//�S�Ẳ����t�@�C�����
		vector<path> soundPaths = _EnumerateSound(bouyomiDirPath);

		//�p�X��_�ǂ݂����z�u�f�B���N�g������̑��΃p�X�ɏC��
		_NormalizeSoundPaths(bouyomiDirPath, soundPaths);

		//�p�X�z�񂩂玫���𐶐�
		vector<BOUYOMI_RECORD> dictionary = _CreatDictionary(soundPaths);

		// #TODO �����D��x�v�Z

		// #TODO ������/�����b���ɂ��SoundW�����؂�ւ�

		//�����t�@�C�����o�b�N�A�b�v
		_BackUpFile(dicFilePath);

		//���������o��
		FILE* pDicFile;
		_wfopen_s(&pDicFile, dicFilePath.wstring().c_str(), L"w, ccs=UTF-8");
		if (!pDicFile)
		{
			wostringstream oss;
			oss << L"�����t�@�C���̏������݃I�[�v���Ɏ��s���܂���" << endl;
			oss << L"�ʂ̃A�v���P�[�V�����Ŏ����t�@�C�����J���Ă��܂��񂩁H" << endl;
			oss << L"�����t�@�C���p�X : " << dicFilePath << endl;
			throw StackRuntimeError(oss.str());
		}
		_Print(pDicFile, dictionary);

		wcerr << dictionary;
	}
	catch (const StackRuntimeError& e)
	{
		wcerr << L"_/ ��O���������܂��� _/" << endl;
		wcerr << L"���� : " << endl;
		wcerr << e;
		wcerr << endl;
		wcerr << helpOss.str();
		wcerr << endl;
		wcerr << L"���^�[���L�[�������ďI�����܂��c" << endl;
		cin.get();
#ifdef _DEBUG
		throw;
#endif
	}
	catch (const runtime_error& e)
	{
		wcerr << L"_/ ��O���������܂��� _/" << endl;
		wcerr << L"���� : " << endl;
		cerr << e.what();
		wcerr << endl;
		wcerr << helpOss.str();
		wcerr << endl;
		wcerr << L"���^�[���L�[�������ďI�����܂��c" << endl;
		cin.get();
#ifdef _DEBUG
		throw;
#endif
	}
	catch (...)
	{
		wcerr << L"�n���h���o���Ȃ���O���������܂���" << endl;
		wcerr << endl;
		wcerr << helpOss.str();
		wcerr << endl;
		wcerr << L"���^�[���L�[�������ďI�����܂��c" << endl;
		cin.get();
#ifdef _DEBUG
		throw;
#endif
	}

    return 0;
}

