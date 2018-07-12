#pragma once
#include <string>
namespace Lobelia {
	enum class ErrorCode {
		//0 ~ 999 初期化部分エラー
		DXE00000,					//デバイス作成に失敗
		DXE00001,					//スワップチェインの作成に失敗
		DXE00002,					//レンダーターゲットの作成に失敗
		DXE00003,					//テクスチャの作成に失敗
		DXE00004,					//シェーダーリソースビューの作成に失敗
		DXE00005,					//デプスステンシルビューの作成に失敗
		DXE00006,					//バッファの作成に失敗
		DXE00007,					//シェーダーコンパイルに失敗
		DXE00008,					//シェーダーインスタンスの作成に失敗
		DXE00009,					//シェーダーインスタンスの取得に失敗
		DXE00010,					//リフレクターの作成に失敗
		DXE00011,					//頂点シェーダーの作成に失敗
		DXE00012,					//ピクセルシェーダーの作成に失敗
		DXE00013,					//入力レイアウトの作成に失敗
		DXE00014,					//ブレンドステートの作成に失敗
		DXE00015,					//サンプラーステートの作成に失敗
		DXE00016,					//ラスタライザーステートの作成に失敗
		DXE00017,					//深度ステンシルステートの作成に失敗
		DXE00018,					//ノーマルマップの計算に失敗
		//1000 ~ 19999 実行時エラー
		DXE01000 = 1000,		//テクスチャのセットに失敗しました
		DXE01001,					//リソースのマップに失敗
		DXE01002,					//リソースがアンマップされていません
		DXE01003,					//未対応のファイルフォーマット
		DXE01004,					//テクスチャの読み込みに失敗
		DXE01005,					//テクスチャのキャプチャに失敗
		DXE01006,					//テクスチャの保存に失敗
		//90000 ~ 汎用エラー
		DXE90000 = 90000,		//リソースがnullptr
		DXE90001,					//バッファオーバラン
		DXE90002,					//リソースの作成に失敗
		DXE99999 = 99999,		//未定義エラー
	};
	class Error {
	public:
		static void Message(ErrorCode error);
		static void Message(ErrorCode error, const char* mes);
		static void ErrorBox(const char* mes);
		template<class... Args> static void PrintBox(const char* format, Args&&... args) {
			char buffer[256] = {};
			sprintf_s(buffer, format, std::forward<Args>(args)...);
			ErrorBox(buffer);
		}
	private:
		static std::string CreateErrorMessage(ErrorCode error);
	};
}