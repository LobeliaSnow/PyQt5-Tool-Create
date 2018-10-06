#include "ErrorCode.hpp"
#include <Windows.h>

namespace Lobelia {
	void Error::Message(ErrorCode error) { ErrorBox(CreateErrorMessage(error).c_str()); }
	void Error::Message(ErrorCode error, const char* mes) {
		std::string errorMes = "DirectX �G���[\n\n";
		errorMes += "�G���[�R�[�h :DXE " + std::to_string(static_cast<int>(error)) + "\n\n";
		errorMes += mes;
		errorMes += "\n\n�S���҂ɘA����������";
		ErrorBox(errorMes.c_str());
	}
	//�G���[���b�Z�[�W���쐬
	std::string Error::CreateErrorMessage(ErrorCode error) {
		std::string errorMes = "DirectX �G���[\n\n";
		errorMes += "�G���[�R�[�h :DXE " + std::to_string(static_cast<int>(error)) + "\n\n";
		switch (error) {
		//�`��֘A
		case ErrorCode::DXE00000:	errorMes += "�f�o�C�X�̍쐬�Ɏ��s���܂���";								break;
		case ErrorCode::DXE00001:	errorMes += "�X���b�v�`�F�C���̍쐬�Ɏ��s���܂���";				break;
		case ErrorCode::DXE00002:	errorMes += "�����_�[�^�[�Q�b�g�̍쐬�Ɏ��s���܂���";				break;
		case ErrorCode::DXE00003:	errorMes += "�e�N�X�`���̍쐬�Ɏ��s���܂���";							break;
		case ErrorCode::DXE00004:	errorMes += "�V�F�[�_�[���\�[�X�r���[�̍쐬�Ɏ��s���܂���";		break;
		case ErrorCode::DXE00005:	errorMes += "�f�v�X�X�e���V���r���[�̍쐬�Ɏ��s���܂���";		break;
		case ErrorCode::DXE00006:	errorMes += "�o�b�t�@�̍쐬�Ɏ��s���܂���";							break;
		case ErrorCode::DXE00007:	errorMes += "�V�F�[�_�[�R���p�C���Ɏ��s���܂���";					break;
		case ErrorCode::DXE00008:	errorMes += "�V�F�[�_�[�C���X�^���X�̍쐬�Ɏ��s���܂���";		break;
		case ErrorCode::DXE00009:	errorMes += "�V�F�[�_�[�C���X�^���X�̎擾�Ɏ��s���܂���";		break;
		case ErrorCode::DXE00010:	errorMes += "���t���N�^�[�̍쐬�Ɏ��s���܂���";						break;
		case ErrorCode::DXE00011:	errorMes += "���_�V�F�[�_�[�̍쐬�Ɏ��s���܂���";					break;
		case ErrorCode::DXE00012:	errorMes += "�s�N�Z���V�F�[�_�[�̍쐬�Ɏ��s���܂���";				break;
		case ErrorCode::DXE00013:	errorMes += "���̓��C�A�E�g�̍쐬�Ɏ��s���܂���";					break;
		case ErrorCode::DXE00014:	errorMes += "�u�����h�X�e�[�g�̍쐬�Ɏ��s���܂���";				break;
		case ErrorCode::DXE00015:	errorMes += "�T���v���[�X�e�[�g�̍쐬�Ɏ��s���܂���";				break;
		case ErrorCode::DXE00016:	errorMes += "���X�^���C�U�[�X�e�[�g�̍쐬�Ɏ��s���܂���";		break;
		case ErrorCode::DXE00017:	errorMes += "�[�x�X�e���V���X�e�[�g�̍쐬�Ɏ��s���܂���";		break;
		case ErrorCode::DXE00018:	errorMes += "�m�[�}���}�b�v�̌v�Z�Ɏ��s���܂���";					break;
		case ErrorCode::DXE00019:	errorMes += "�R���s���[�g�V�F�[�_�[�̍쐬�Ɏ��s";					break;
		case ErrorCode::DXE00020:	errorMes += "�W�I���g���V�F�[�_�[�̍쐬�Ɏ��s";						break;
		//���͊֘A
		case ErrorCode::DXE00400:	errorMes += "Direct Input�f�o�C�X�̍쐬�Ɏ��s";						break;
		case ErrorCode::DXE00401:	errorMes += "���̓f�o�C�X�̍쐬�Ɏ��s";									break;
		case ErrorCode::DXE00402:	errorMes += "���̓f�o�C�X�̃f�[�^�ݒ�Ɏ��s";							break;
		case ErrorCode::DXE00403:	errorMes += "�������x���̐ݒ�Ɏ��s";										break;
		case ErrorCode::DXE00404:	errorMes += "�v���p�e�B�̐ݒ�Ɏ��s";										break;
		case ErrorCode::DXE00405:	errorMes += "���̓f�o�C�X�̏��擾�Ɏ��s";								break;
		case ErrorCode::DXE00406:	errorMes += "�͈͊O�̃L�[�R�[�h���ݒ肳��܂���";					break;
		//���s���G���[
		case ErrorCode::DXE01000:	errorMes += "�e�N�X�`���̃Z�b�g�Ɏ��s���܂���";						break;
		case ErrorCode::DXE01001:	errorMes += "���\�[�X�̃}�b�v�Ɏ��s���܂���";							break;
		case ErrorCode::DXE01002:	errorMes += "���\�[�X���A���}�b�v����Ă��܂���	";					break;
		case ErrorCode::DXE01003:	errorMes += "���Ή��̃t�@�C���t�H�[�}�b�g�ł�";						break;
		case ErrorCode::DXE01004:	errorMes += "�e�N�X�`���̓ǂݍ��݂Ɏ��s���܂���	";					break;
		case ErrorCode::DXE01005:	errorMes += "�e�N�X�`���̃L���v�`���Ɏ��s���܂���	";				break;
		case ErrorCode::DXE01006:	errorMes += "�e�N�X�`���̕ۑ��Ɏ��s���܂���	";						break;
		//�ėp�G���[
		case ErrorCode::DXE90000:	errorMes += "���\�[�X��NULL�ł�";											break;
		case ErrorCode::DXE90001:	errorMes += "�o�b�t�@�I�[�o�[�����ł�";									break;
		case ErrorCode::DXE90002:	errorMes += "���\�[�X�̍쐬�Ɏ��s���܂���";							break;
		case ErrorCode::DXE90003:	errorMes += "�X�^�b�N�I�[�o�[�t���[�ł�";								break;
		default:								errorMes += "����`�̃G���[�ł�";											break;
		}
		errorMes += "\n\n�S���҂ɘA����������";
		return errorMes;
	}
	void Error::ErrorBox(const char* mes) {
		MessageBox(nullptr, mes, "Error", MB_ICONERROR | MB_OK);
	}
}