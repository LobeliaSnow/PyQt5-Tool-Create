#include "ErrorCode.hpp"
#include <Windows.h>

namespace Lobelia {
	void Error::Message(ErrorCode error) { ErrorBox(CreateErrorMessage(error).c_str()); }
	void Error::Message(ErrorCode error, const char* mes) {
		std::string errorMes = "DirectX エラー\n\n";
		errorMes += "エラーコード :DXE " + std::to_string(static_cast<int>(error)) + "\n\n";
		errorMes += mes;
		errorMes += "\n\n担当者に連絡ください";
		ErrorBox(errorMes.c_str());
	}
	//エラーメッセージを作成
	std::string Error::CreateErrorMessage(ErrorCode error) {
		std::string errorMes = "DirectX エラー\n\n";
		errorMes += "エラーコード :DXE " + std::to_string(static_cast<int>(error)) + "\n\n";
		switch (error) {
		case ErrorCode::DXE00000:	errorMes += "デバイスの作成に失敗しました";								break;
		case ErrorCode::DXE00001:	errorMes += "スワップチェインの作成に失敗しました";				break;
		case ErrorCode::DXE00002:	errorMes += "レンダーターゲットの作成に失敗しました";				break;
		case ErrorCode::DXE00003:	errorMes += "テクスチャの作成に失敗しました";							break;
		case ErrorCode::DXE00004:	errorMes += "シェーダーリソースビューの作成に失敗しました";		break;
		case ErrorCode::DXE00005:	errorMes += "デプスステンシルビューの作成に失敗しました";		break;
		case ErrorCode::DXE00006:	errorMes += "バッファの作成に失敗しました";							break;
		case ErrorCode::DXE00007:	errorMes += "シェーダーコンパイルに失敗しました";					break;
		case ErrorCode::DXE00008:	errorMes += "シェーダーインスタンスの作成に失敗しました";		break;
		case ErrorCode::DXE00009:	errorMes += "シェーダーインスタンスの取得に失敗しました";		break;
		case ErrorCode::DXE00010:	errorMes += "リフレクターの作成に失敗しました";						break;
		case ErrorCode::DXE00011:	errorMes += "頂点シェーダーの作成に失敗しました";					break;
		case ErrorCode::DXE00012:	errorMes += "ピクセルシェーダーの作成に失敗しました";				break;
		case ErrorCode::DXE00013:	errorMes += "入力レイアウトの作成に失敗しました";					break;
		case ErrorCode::DXE00014:	errorMes += "ブレンドステートの作成に失敗しました";				break;
		case ErrorCode::DXE00015:	errorMes += "サンプラーステートの作成に失敗しました";				break;
		case ErrorCode::DXE00016:	errorMes += "ラスタライザーステートの作成に失敗しました";		break;
		case ErrorCode::DXE00017:	errorMes += "深度ステンシルステートの作成に失敗しました";		break;
		case ErrorCode::DXE00018:	errorMes += "ノーマルマップの計算に失敗しました";					break;
		case ErrorCode::DXE01000:	errorMes += "テクスチャのセットに失敗しました";						break;
		case ErrorCode::DXE01001:	errorMes += "リソースのマップに失敗しました";							break;
		case ErrorCode::DXE01002:	errorMes += "リソースがアンマップされていません	";					break;
		case ErrorCode::DXE01003:	errorMes += "未対応のファイルフォーマットです";						break;
		case ErrorCode::DXE01004:	errorMes += "テクスチャの読み込みに失敗しました	";					break;
		case ErrorCode::DXE01005:	errorMes += "テクスチャのキャプチャに失敗しました	";				break;
		case ErrorCode::DXE01006:	errorMes += "テクスチャの保存に失敗しました	";						break;
		case ErrorCode::DXE90000:	errorMes += "リソースがNULLです";											break;
		case ErrorCode::DXE90001:	errorMes += "バッファオーバーランです";									break;
		case ErrorCode::DXE90002:	errorMes += "リソースの作成に失敗しました";							break;
		default:								errorMes += "未定義のエラーです";											break;
		}
		errorMes += "\n\n担当者に連絡ください";
		return errorMes;
	}
	void Error::ErrorBox(const char* mes) {
		MessageBox(nullptr, mes, "Error", MB_ICONERROR | MB_OK);
	}
}