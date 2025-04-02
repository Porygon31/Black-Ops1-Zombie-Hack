#pragma once
#include <Windows.h>
#include <vector>
#include <TlHelp32.h>
#include <iomanip>

class MemMan
{
public:
	// Destructeur : ferme le handle de processus si ouvert
	~MemMan()
	{
		CloseHandle(Handle);
	}

	// Lecture générique en mémoire à l'adresse donnée
	template <class T>
	static T ReadMem(uintptr_t addr)
	{
		T x;
		ReadProcessMemory(Handle, (LPBYTE*)addr, &x, sizeof(x), NULL); // Lit la mémoire du processus cible
		return x;
	}

	// Écriture générique en mémoire à l'adresse donnée
	template <class T>
	static T WriteMem(uintptr_t addr, T x)
	{
		WriteProcessMemory(Handle, (LPBYTE*)addr, &x, sizeof(x), NULL); // Écrit dans la mémoire du processus cible
		return x;
	}

	// Extrait un type T à partir d'un buffer d'octets, à un certain décalage
	template<class T>
	static T FromBuffer(std::vector<uint8_t> buffer, size_t offset)
	{
		T t_buf{};
		if (buffer.empty())
			return t_buf;

		memcpy(&t_buf, buffer.data() + offset, sizeof(T));
		return t_buf;
	}

	// Lecture brute d'un bloc mémoire
	static std::vector<uint8_t> ReadBytes(DWORD Address, int Size)
	{
		std::vector<uint8_t> arr;
		arr.resize(Size);
		ReadProcessMemory(Handle, (LPVOID)Address, arr.data(), arr.size() * sizeof(uint8_t), 0);
		return arr;
	}

	// Recherche un processus par son nom, retourne son PID et stocke le handle
	static uintptr_t GetProcess(const wchar_t* proc)
	{
		HANDLE hProcessId = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); // Snapshot des processus
		uintptr_t process = 0;
		PROCESSENTRY32 pEntry;
		pEntry.dwSize = sizeof(pEntry);

		do
		{
			if (!_wcsicmp(pEntry.szExeFile, proc)) // Comparaison sans sensibilité à la casse
			{
				process = pEntry.th32ProcessID;
				CloseHandle(hProcessId);
				Handle = OpenProcess(PROCESS_ALL_ACCESS, false, process); // Ouverture du handle avec tous les droits
			}

		} while (Process32Next(hProcessId, &pEntry)); // Parcourt les processus
		return process;
	}

	// Recherche un module (DLL/EXE) dans un processus donné
	static uintptr_t GetModule(uintptr_t procId, const wchar_t* modName)
	{
		HANDLE hModule = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId); // Snapshot des modules
		MODULEENTRY32 mEntry;
		mEntry.dwSize = sizeof(mEntry);

		do
		{
			if (!_wcsicmp(mEntry.szModule, modName)) // Comparaison du nom du module
			{
				CloseHandle(hModule);
				return (uintptr_t)mEntry.hModule; // Retourne l'adresse de base du module
			}
		} while (Module32Next(hModule, &mEntry));
		return 0;
	}

	// Traverse une chaîne de pointeurs avec des offsets (multi-level pointer dereferencing)
	static uintptr_t GetAddress(uintptr_t addr, std::vector<uintptr_t> vect)
	{
		for (int i = 0; i < vect.size(); i++)
		{
			ReadProcessMemory(Handle, (BYTE*)addr, &addr, sizeof(addr), 0); // Lit l'adresse pointée
			addr += vect[i]; // Ajoute l'offset
		}
		return addr;
	}

	// Retourne le handle actuel du processus
	static HANDLE GetHandle() { return Handle; }

public:
	// Handle statique global du processus cible
	static inline HANDLE Handle = NULL;
};
