# BouyomiReplaceTagGenerator

棒読みちゃんの ReplaceTag.dic を自動生成するCUIアプリケーション

##入手方法

[ビルド済み実行ファイル][executable]を持って行ってください。

ダウンロードしたら適当な場所に実行ファイルを配置してください。おすすめは棒読みちゃんと同じ階層です。

##動作環境

「Visual Studio 2015 の Visual C++ 再頒布可能パッケージ」を[Microsoftから入手][vs2015redist]してインストールしてください。

パソコンに自信ニキは自分でビルドしてもいいですよ？

## 使い方
* このプログラムに「棒読みちゃんの実行ファイルが配置されているフォルダ」をドラッグ＆ドロップしてください。
* あるいは、このプログラムを棒読みちゃんと同じ階層に配置してダブルクリックしてください。

## 起こること

* Sound ファルダ以下の全ての .wav .mp3 が辞書に登録されます。
* 「読み上げワード」＝「音声ファイル名(拡張子除く)」です。
* 既存の辞書ファイル(ReplaceTag.dic)はバックアップが残ります。

例えば Sound フォルダ以下に ほげほげ.wav が配置してある場合、
このプログラムを実行したあとに棒読みちゃんを起動することで、
「ほげほげ」というテキストが入力された時に「ほげほげ.wav」が再生されます。

## 今後の課題

* ファイル名にタグを付与して Sound と SoundW の切り替え
* 正規表現の検討

## 著作権的なの

すきにして

[executable]: https://github.com/Nu-Pan/BouyomiReplaceTagGenerator/raw/master/BouyomiReplaceTagGenerator/distribution/BouyomiReplaceTagGenerator_x86_Release.exe
[vs2015redist]: https://www.microsoft.com/ja-jp/download/details.aspx?id=48145
