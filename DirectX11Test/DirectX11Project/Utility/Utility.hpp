#pragma once
#include <shlwapi.h>
#include <filesystem>
namespace Lobelia {
	//C++�p
	template<class T, class Key = std::string> class ResourceBank {
	public:
		template<class U = T, class... Args> static std::shared_ptr<T> Factory(Key key, Args&&... args) {
			try {
				if (IsExist(key)) return resource[key].lock();
				std::shared_ptr<T> ret = std::make_shared<U>(std::forward<Args>(args)...);
				resource[key] = ret;
				return ret;
			}
			catch (...) {
				Error::Message(ErrorCode::DXE90002);
			}
			return nullptr;
		}
		static void Register(Key key, std::shared_ptr<T> p)noexcept { if (!IsExist(key))resource[key] = swap(p); }
		static bool IsExist(Key key)noexcept { return ((resource.find(key) != resource.end()) && (resource[key].use_count() != 0)); }
		static void Clear()noexcept { resource.clear(); }
		static std::shared_ptr<T> Get(Key key)noexcept { return resource[key].lock(); }
		static std::map<Key, std::shared_ptr<T>>& Get()noexcept { return resource; }
		static void Erase(Key key)noexcept { resource.erase(key); }
	private:
		static std::map<Key, std::weak_ptr<T>> resource;
	};
	template<class T, class Key> std::map<Key, std::weak_ptr<T>> ResourceBank<T, Key>::resource;
	class FilePathControl {
	public:
		static std::string GetExtension(const std::string& file_path) {
			//std::tr2::sys::path path(file_path);
			std::experimental::filesystem::path path(file_path);
			if (!path.has_extension())return "";
			return path.extension().string();
		}
		static std::string GetFilename(const std::string& file_path) {
			std::experimental::filesystem::path path(file_path);
			if (!path.has_filename())return "";
			return path.filename().string();
		}
		static std::string GetRootDirectory(const std::string& file_path) {
			std::experimental::filesystem::path path(file_path);
			if (!path.has_root_directory())return "";
			return path.root_directory().string();
		}
		static std::string GetParentDirectory(const std::string& file_path) {
			std::experimental::filesystem::path path(file_path);
			if (!path.has_parent_path())return "";
			return path.parent_path().string();
		}
		static std::string GetRelativeDirectory(const std::string& file_path) {
			std::experimental::filesystem::path path(file_path);
			if (!path.has_relative_path())return "";
			return path.relative_path().string();
		}
		static bool IsFileOrPath(const std::string& path) {
			return std::experimental::filesystem::exists(path.c_str());
		}
		//���ɂ����낢��ǉ��\��
	};

	inline std::wstring ConverteWString(const std::string& str)noexcept {
		wchar_t name[256] = {};
		size_t strSum = {};
		//setlocale(LC_CTYPE, "jpn");
		errno_t err = mbstowcs_s(&strSum, name, 256, str.c_str(), 256);
		std::wstring wstr = name;
		return wstr;
	}
	inline std::string ConverteString(const std::wstring& str)noexcept {
		char name[256] = {};
		size_t strSum = {};
		//setlocale(LC_CTYPE, "jpn");
		errno_t err = wcstombs_s(&strSum, name, 256, str.c_str(), 256);
		std::string wstr = name;
		return wstr;
	}
	template<class First, class ...Arg> inline float Sum(const First& first, Arg ...arg) {
		return first + Sum(arg...);
	}
	template<class T, class ...Arg> inline void SafeNew(T** p, Arg ...arg) {
		SafeDelete(*p);
		*p = new T(arg...);
	}
	template<class T> inline void SafeDelete(T* p) {
		if (p != nullptr) {
			delete p;
			p = nullptr;
		}
	}

	inline float Frand(float min, float max) { return (static_cast<float>(rand()) / RAND_MAX)*(max - min) + min; }
	//CreateContainer�̃w���p�[�֐��Q
	namespace _ {
		//����^�̏ꍇ
		template<template<class, class> class Container, class Type> void PushBack(Container<Type, std::allocator<Type>>& container, const Type& val) {
			//�x���΍�
			container.push_back(s_cast<Type>(val));
		}
		//�z��̏ꍇ
		template<template<class, class> class Container, class Type, size_t _size> void PushBack(Container<Type, std::allocator<Type>>& container, const Type(&val)[_size]) {
			for (int i = 0; i < _size; i++) { PushBack(container, val[i]); }
		}
		//���̑�(�R���e�i�^)�̏ꍇ
		//begin end�����邩�m���߂Ă��ق����x�^�[����
		template<class Container, class Type> void PushBack(Container& container, const Type& val) {
			for each(auto&& it in val) { PushBack(container, it); }
		}
	}
	//�R���e�i���쐬���邽�߂̊֐�
	//�ϐ����z����R���e�i���������ɓ�����Ƃ��̏��Ԓʂ�ɑS����̃R���e�i�ɘA�����ďo�Ă��܂�
	//vector�̏ꍇ��
	template<template<class, class> class Container, class Type, class... Args> void CreateContainer(Container<Type, std::allocator<Type>>& container, Args&&... args) {
		(void)std::initializer_list<int> {
			(void(_::PushBack(container, args)), 0)...
		};
	}

}
