#pragma once
#include <string>
namespace Lobelia {
	enum class ErrorCode {
		//0 ~ 999 �����������G���[
		DXE00000,					//�f�o�C�X�쐬�Ɏ��s
		DXE00001,					//�X���b�v�`�F�C���̍쐬�Ɏ��s
		DXE00002,					//�����_�[�^�[�Q�b�g�̍쐬�Ɏ��s
		DXE00003,					//�e�N�X�`���̍쐬�Ɏ��s
		DXE00004,					//�V�F�[�_�[���\�[�X�r���[�̍쐬�Ɏ��s
		DXE00005,					//�f�v�X�X�e���V���r���[�̍쐬�Ɏ��s
		DXE00006,					//�o�b�t�@�̍쐬�Ɏ��s
		DXE00007,					//�V�F�[�_�[�R���p�C���Ɏ��s
		DXE00008,					//�V�F�[�_�[�C���X�^���X�̍쐬�Ɏ��s
		DXE00009,					//�V�F�[�_�[�C���X�^���X�̎擾�Ɏ��s
		DXE00010,					//���t���N�^�[�̍쐬�Ɏ��s
		DXE00011,					//���_�V�F�[�_�[�̍쐬�Ɏ��s
		DXE00012,					//�s�N�Z���V�F�[�_�[�̍쐬�Ɏ��s
		DXE00013,					//���̓��C�A�E�g�̍쐬�Ɏ��s
		DXE00014,					//�u�����h�X�e�[�g�̍쐬�Ɏ��s
		DXE00015,					//�T���v���[�X�e�[�g�̍쐬�Ɏ��s
		DXE00016,					//���X�^���C�U�[�X�e�[�g�̍쐬�Ɏ��s
		DXE00017,					//�[�x�X�e���V���X�e�[�g�̍쐬�Ɏ��s
		DXE00018,					//�m�[�}���}�b�v�̌v�Z�Ɏ��s
		//1000 ~ 19999 ���s���G���[
		DXE01000 = 1000,		//�e�N�X�`���̃Z�b�g�Ɏ��s���܂���
		DXE01001,					//���\�[�X�̃}�b�v�Ɏ��s
		DXE01002,					//���\�[�X���A���}�b�v����Ă��܂���
		DXE01003,					//���Ή��̃t�@�C���t�H�[�}�b�g
		DXE01004,					//�e�N�X�`���̓ǂݍ��݂Ɏ��s
		DXE01005,					//�e�N�X�`���̃L���v�`���Ɏ��s
		DXE01006,					//�e�N�X�`���̕ۑ��Ɏ��s
		//90000 ~ �ėp�G���[
		DXE90000 = 90000,		//���\�[�X��nullptr
		DXE90001,					//�o�b�t�@�I�[�o����
		DXE90002,					//���\�[�X�̍쐬�Ɏ��s
		DXE99999 = 99999,		//����`�G���[
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