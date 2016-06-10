// BouyomiReplaceTagGenerator.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
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
	* @brief	棒読みちゃん辞書レコード
	*/
	struct BOUYOMI_RECORD
	{
		int		priority;
		wstring	text;
		path	soundPath;
	};

	//指定ディレクトリ以下に指定名ファイルが存在するかチェック
	void _CheckFile(const path& parent, const wchar_t* const child)
	{
		const path target = parent / child;

		if (!exists(target))
		{
			wostringstream oss;
			oss << L"ファイルが存在しません" << endl;
			oss << L"パス : " << target << endl;
			throw StackRuntimeError(oss.str());
		}

		if (!is_regular_file(target))
		{
			wostringstream oss;
			oss << L"ファイルではありません" << endl;
			oss << L"パス : " << target << endl;
			throw StackRuntimeError(oss.str());
		}
	}

	//指定ディレクトリ以下に指定名ディレクトリが存在するかチェック
	void _CheckDir(const path& parent, const wchar_t* const child)
	{
		const path target = parent / child;

		if (!exists(target))
		{
			wostringstream oss;
			oss << L"ディレクトリが存在しません" << endl;
			oss << L"パス : " << target << endl;
			throw StackRuntimeError(oss.str());
		}

		if (!is_directory(target))
		{
			wostringstream oss;
			oss << L"ディレクトリではありません" << endl;
			oss << L"パス : " << target << endl;
			throw StackRuntimeError(oss.str());
		}
	}


	// 実行前のチェック
	void _PreCheck(const path& bouyomiPath)
	{
		try
		{
			//棒読みちゃんがカレントに存在するか確認
			_CheckFile(bouyomiPath, BOUYOMI_EXE_FILE_NAME);

			// Soundディレクトリがカレント以下に存在するか確認
			_CheckDir(bouyomiPath, SOUND_DIR_NAME);

			//辞書ファイルがカレント以下に存在するか確認
			_CheckFile(bouyomiPath, DIC_FILE_NAME);
		}
		catch (StackRuntimeError& e)
		{
			wostringstream oss;
			oss << L"対象ディレクトリ以下に必要なディレクトリ/ファイルが存在しません" << endl;
			oss << L"対象ディレクトリに" << BOUYOMI_EXE_FILE_NAME << L" の置いてあるディレクトリを指定してくらさい" << endl;
			e.Push(oss.str());
			throw e;
		}
	}

	//音声ファイルであるか
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

	//音声ファイルを全て列挙する
	vector<path> _EnumerateSound(const path& bouyomiPath)
	{
		const path SOUND_DIR_PATH = bouyomiPath / SOUND_DIR_NAME;

		// target 以下を再帰的に探索する
		vector<path> soundPaths;
		const recursive_directory_iterator end;
		for (recursive_directory_iterator iter(SOUND_DIR_PATH); iter != end; ++iter)
		{
			//音声以外を弾く
			if( !_IsSoundFile(*iter) )
			{
				continue;
			}
			//音声ファイルリストに追加
			soundPaths.push_back(*iter);
		}

		return move(soundPaths);
	}

	//パスを Sound ディレクトリを基準とする相対パスに正規化
	void _NormalizeSoundPaths(const path& bouyomiPath, vector<path>& paths)
	{
		//パスのwstringを生成
		const wstring soundDirPathStr = (bouyomiPath / SOUND_DIR_NAME).wstring();

		//全ての音声ファイルについて処理
		for (auto& soundFilePath : paths)
		{
			//音声ファイルパスのwstringをとっておく
			const wstring soundFilePathStr = soundFilePath.wstring();
			//<音声ファイルパス>の先頭に<Soundディレクトリパス>が含まれることを確認
			const size_t foundPos = soundFilePathStr.find(soundDirPathStr);
			if (foundPos == wstring::npos)
			{
				wostringstream oss;
				oss << L"音声ファイルパス正規化中" << endl;
				oss << L"<音声ファイルパス>中に<Soundディレクトリパス>が含まれていません" << endl;
				oss << L"ほぼ間違いなくプログラムが間違っています" << endl;
				oss << L"<音声ファイルパス> : " << soundFilePathStr << endl;
				oss << L"<Soundディレクトリパス> : " << soundDirPathStr << endl;
				throw StackRuntimeError(oss.str());
			}
			if (foundPos != 0)
			{
				wostringstream oss;
				oss << L"音声ファイルパス正規化中" << endl;
				oss << L"<Soundディレクトリパス>が<音声ファイルパス>の先頭以外で見つかりました" << endl;
				oss << L"ほぼ間違いなくプログラムが間違っています" << endl;
				oss << L"<音声ファイルパス> : " << soundFilePathStr << endl;
				oss << L"<Soundディレクトリパス> : " << soundDirPathStr << endl;
				throw StackRuntimeError(oss.str());
			}
			//<音声ファイルパス>から<棒読みちゃん配置ディレクトリパス>を除外
			//先頭に \ が残ってしまうので +1 する
			soundFilePath = soundFilePathStr.substr(soundDirPathStr.length() + 1);
		}
	}

	//指定ファイルをバックアップ
	void _BackUpFile(const path& targetFilePath)
	{
		//指定ファイルが存在しなければ何もしない
		if (!is_regular_file(targetFilePath))
		{
			return;
		}

		//指定ファイルの親ディレクトリパスを取得
		const path parentDirPath = targetFilePath.parent_path();

		//既存のバックアップファイルの通し番号の最大値を取得
		wregex regularExpression(targetFilePath.filename().wstring() + wstring(L".bak([0-9]+)"));
		wsmatch matchResult;
		int maxLogNumber = 0;
		const directory_iterator end;
		for (directory_iterator iter(parentDirPath); iter != end; ++iter)
		{
			//正規表現で既存バックアップファイルか判別
			const wstring fileNameStr = iter->path().filename().wstring();
			if (!regex_match(fileNameStr, matchResult, regularExpression))
			{
				continue;
			}
			if (matchResult.size() != 2)
			{
				continue;
			}
			//最大バックアップ通し番号をファイル名から取得
			int logNum;
			wistringstream iss(matchResult[1]);
			iss >> logNum;
			//最大バックアップ通し番号を更新
			if ( maxLogNumber < logNum )
			{
				maxLogNumber = logNum;
			}
		}
		
		//バックアップを取る
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
				oss << L"バックアップファイルの作成に失敗しました" << endl;
				oss << L"バックアップファイルの存在するディレクトリの書き込み権限はありますか？" << endl;
				oss << L"バックアップ対象 : " << targetFilePath << endl;
				oss << L"バックアップ先　 : " << backupFilePath << endl;
				throw StackRuntimeError(oss.str());
			}
		}		
	}

	//音声ファイルパス列から辞書を生成
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
		//１レコードづつ
		for (const auto& record : dict)
		{
			fwprintf(fp, L"%d\tN\t%s\t(SoundW %s)\n", record.priority, record.text.c_str(), record.soundPath.c_str());
		}
	}
}

//辞書をストリームに流す
wostream& operator <<(wostream& os, const vector<BOUYOMI_RECORD>& dict)
{
	//１レコードづつ
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

//エントリポイント
int main(int argc, char* argv[])
{
	const locale jaLoc("japanese");
	locale::global(jaLoc);
	cout.imbue(jaLoc);
	wcerr.imbue(jaLoc);
	wcout.imbue(jaLoc);
	wcerr.imbue(jaLoc);
	setlocale(LC_ALL, jaLoc.c_str());

	//実行時のアプリケーションファイル名を取得
	const wstring applicationName = path(argv[0]).filename().wstring();

	wostringstream helpOss;
	helpOss << L"_/ ヘルプ _/" << endl;
	helpOss << L"使い方１ : 引数を渡さずに " << applicationName << L" を起動する" << endl;
	helpOss << L"棒読みちゃん配置ディレクトリがカレントディレクトリだと仮定して処理をします。" << endl;
	helpOss << L"つまり、棒読みちゃんと同じ階層にこのプログラムを配置してダブルクリックしてください。" << endl;
	helpOss << endl;
	helpOss << L"使い方２ : " << applicationName << L" <棒読みちゃん配置ディレクトリパス>" << endl;
	helpOss << L"第１引数で棒読みちゃんが配置されているディレクトリのパスを指定します" << endl;
	helpOss << L"コマンドラインから呼び出してください。" << endl;


	//棒読みちゃん配置ディレクトリパスを引数から決定
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
		oss << L"引数の個数が不正です" << endl;
		oss << L"期待する個数 : 0 から 1 個" << endl;
		oss << L"渡された個数 : " << argc - 1 << endl;
		throw StackRuntimeError(oss.str());
	}

	try
	{
		const path dicFilePath = bouyomiDirPath / DIC_FILE_NAME;


		//実行前チェック
		_PreCheck(bouyomiDirPath);

		//全ての音声ファイルを列挙
		vector<path> soundPaths = _EnumerateSound(bouyomiDirPath);

		//パスを棒読みちゃん配置ディレクトリからの相対パスに修正
		_NormalizeSoundPaths(bouyomiDirPath, soundPaths);

		//パス配列から辞書を生成
		vector<BOUYOMI_RECORD> dictionary = _CreatDictionary(soundPaths);

		// #TODO 自動優先度計算

		// #TODO 文字数/音声秒数によるSoundW自動切り替え

		//辞書ファイルをバックアップ
		_BackUpFile(dicFilePath);

		//辞書書き出し
		FILE* pDicFile;
		_wfopen_s(&pDicFile, dicFilePath.wstring().c_str(), L"w, ccs=UTF-8");
		if (!pDicFile)
		{
			wostringstream oss;
			oss << L"辞書ファイルの書き込みオープンに失敗しました" << endl;
			oss << L"別のアプリケーションで辞書ファイルを開いていませんか？" << endl;
			oss << L"辞書ファイルパス : " << dicFilePath << endl;
			throw StackRuntimeError(oss.str());
		}
		_Print(pDicFile, dictionary);

		wcerr << dictionary;
	}
	catch (const StackRuntimeError& e)
	{
		wcerr << L"_/ 例外が発生しました _/" << endl;
		wcerr << L"説明 : " << endl;
		wcerr << e;
		wcerr << endl;
		wcerr << helpOss.str();
		wcerr << endl;
		wcerr << L"リターンキーを押して終了します…" << endl;
		cin.get();
#ifdef _DEBUG
		throw;
#endif
	}
	catch (const runtime_error& e)
	{
		wcerr << L"_/ 例外が発生しました _/" << endl;
		wcerr << L"説明 : " << endl;
		cerr << e.what();
		wcerr << endl;
		wcerr << helpOss.str();
		wcerr << endl;
		wcerr << L"リターンキーを押して終了します…" << endl;
		cin.get();
#ifdef _DEBUG
		throw;
#endif
	}
	catch (...)
	{
		wcerr << L"ハンドル出来ない例外が発生しました" << endl;
		wcerr << endl;
		wcerr << helpOss.str();
		wcerr << endl;
		wcerr << L"リターンキーを押して終了します…" << endl;
		cin.get();
#ifdef _DEBUG
		throw;
#endif
	}

    return 0;
}

