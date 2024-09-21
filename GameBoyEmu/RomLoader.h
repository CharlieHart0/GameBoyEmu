#pragma once
#include <vector>
#include <fstream>
#include <string>
#include <array>
#include <map>
#include "Vendor/ImGui/imgui.h"
#include "Vendor/magic-enum/magic_enum.hpp"
#include <iostream>

#define GB_ROM_MAX_SIZE_BYTES 4000000

#define NUM_BYTES_IN_MEBIBYTE 1048576
#define NUM_BYTES_IN_KIBIBYTE 1024

#define GB_ROM_HEAD_CHECKSUM_START_ADDR 0x0134
#define GB_ROM_HEAD_CHECKSUM_END_ADDR 0x014C

#define GB_ROM_HEADER_LOGO_ADDR 0x0104
#define GB_ROM_HEADER_TITLE_ADDR 0x0134
#define GB_ROM_HEADER_NEWLICENSEECODE_ADDR 0x0144
#define GB_ROM_HEADER_SGB_FLAG_ADDR 0x0144
#define GB_ROM_HEADER_CART_TYPE_ADDR 0x0147
#define GB_ROM_HEADER_ROM_SIZE_ADDR 0x0148
#define GB_ROM_HEADER_RAM_ADDR 0x0149
#define GB_ROM_HEADER_DEST_ADDR 0x014A
#define GB_ROM_HEADER_OLD_LICENSEE_CODE_ADDR 0x014B
#define GB_ROM_HEADER_ROM_VERSION_ADDR 0x014C
#define GB_ROM_HEADER_HEAD_CHECKSUM_ADDR 0x014D

#define ROMHEADER_SGB_FLAG_VAL 0x03

typedef uint8_t RomLoader_CartInfo;

enum RomLoader_ValidationOutcome
{
	NoIssues = 0,
	MissingCompanyLogo = 0b1,
	CartTypeErr = 0b10,
	RomSize = 0b100,
	RamSize = 0b1000,
	DestinationCode = 0b10000,
	HeaderChecksum = 0b100000
};

enum CartMainType
{
	NO_MAPPER,
	MBC1,
	MBC2,
	MBC3,
	MBC5,
	MBC6,
	MBC7,
	HuC1,
	HuC3,
	MMM01,
	POCKET_CAMERA,
	BANDAI_TAMA5
};

enum CartInfoOtherOptions
{
	ROM = 0b1,
	RAM = 0b10,
	BATTERY = 0b100,
	TIMER = 0b1000,
	RUMBLE = 0b10000,
	SENSOR = 0b100000
};

enum RomHeader_DestinationCode
{
	Japan_And_Possibly_Overseas = 0x00,
	Overseas_Only = 0x01
};

struct RomHeader_CartType
{
	CartMainType mainType;
	RomLoader_CartInfo cartInfo = 0;
};

struct RomHeader
{
	
	std::string title;

	//manufacture code?

	//cgb flag?

	std::string newLicenseeCode;

	std::string newLicenseeName;

	uint8_t sgbFlag;

	RomHeader_CartType cartType;

	unsigned int romSizeBytes;

	//ROM banks = romSizeBytes / 16 KiB
	unsigned int romBanksCount;

	unsigned int cartRamSizeBytes;

	uint8_t cartRamBanksCount = 0;
	
	unsigned int cartRamBanksSizeEach = 0;

	RomHeader_DestinationCode destinationCode;

	uint8_t oldLicenseeCode;

	std::string oldLicenseeName;

	std::string* licenseeName = nullptr;

	uint8_t romVersionNumber;
};

class RomLoader
{
public:
	void deleteRom();
	void loadRomFromFile(const char* filePath);
	void UiUpdate();
	RomLoader();
	std::string cartTypeFeaturesText();

	std::vector<uint8_t> rom;
	RomHeader header;
	uint8_t romValidationOutcome = 0;
	 

#pragma region valid header info maps
	std::map<uint8_t, unsigned int> validRomSizes = {
		{0x0,32 * NUM_BYTES_IN_KIBIBYTE},
		{0x1,64 * NUM_BYTES_IN_KIBIBYTE},
		{0x2,128 * NUM_BYTES_IN_KIBIBYTE},
		{0x3,256 * NUM_BYTES_IN_KIBIBYTE},
		{0x4,512 * NUM_BYTES_IN_KIBIBYTE},
		{0x5,NUM_BYTES_IN_MEBIBYTE},
		{0x6,2 * NUM_BYTES_IN_KIBIBYTE},
		{0x7,4 * NUM_BYTES_IN_KIBIBYTE},
		{0x8,8 * NUM_BYTES_IN_KIBIBYTE}
	};

	// https://gbdev.io/pandocs/The_Cartridge_Header.html#0147--cartridge-type
	// Sensor, rumble, timer, battery, ram, rom
	std::map<uint8_t, RomHeader_CartType> validCartTypes = {
		{0x00,{NO_MAPPER,0b000001}},

		{0x01,{MBC1,0b000000}},
		{0x02,{MBC1,0b000010}},
		{0x03,{MBC1,0b000110}},

		{0x05,{MBC2,0b000000}},
		{0x06,{MBC2,0b000100}},

		{0x0B,{MMM01,0b000000}},
		{0x0C,{MMM01,0b000010}},
		{0x0D,{MMM01,0b000110}},

		{0x0F,{MBC3,0b001100}},
		{0x10,{MBC3,0b001110}},
		{0x11,{MBC3,0b000000}},
		{0x12,{MBC3,0b000010}},
		{0x13,{MBC3,0b000110}},

		{0x19,{MBC5,0b000000}},
		{0x1A,{MBC5,0b000010}},
		{0x1B,{MBC5,0b000110}},
		{0x1C,{MBC5,0b010000}},
		{0x1D,{MBC5,0b010010}},
		{0x1E,{MBC5,0b010110}},

		{0x20,{MBC6,0b000000}},

		{0x22,{MBC7,0b110110}},
		{0xFC,{POCKET_CAMERA,0b000000}},
		{0xFD,{BANDAI_TAMA5,0b000000}},
		{0xFE,{HuC3,0b000000}},
		{0xFF,{HuC1,0b000110}}
	};

	//https://gbdev.io/pandocs/The_Cartridge_Header.html#014b--old-licensee-code
	std::map<uint8_t, std::string>oldLicenseeCodesNames;

	std::array<unsigned long, 6> validCartRamSizes = {
		0,
		2 * NUM_BYTES_IN_KIBIBYTE,
		8 * NUM_BYTES_IN_KIBIBYTE,
		32 * NUM_BYTES_IN_KIBIBYTE,
		128 * NUM_BYTES_IN_KIBIBYTE,
		64 * NUM_BYTES_IN_KIBIBYTE
	};

	std::array<unsigned long, 6> validCartRamBankCount = {
		0,
		1,
		1,
		4,
		16,
		8
	};

	std::array<unsigned long, 6> validCartRamBankSizeEach = {
		0,
		2 * NUM_BYTES_IN_KIBIBYTE,
		8 * NUM_BYTES_IN_KIBIBYTE,
		8 * NUM_BYTES_IN_KIBIBYTE,
		8 * NUM_BYTES_IN_KIBIBYTE,
		8 * NUM_BYTES_IN_KIBIBYTE
	};

	//https://gbdev.io/pandocs/The_Cartridge_Header.html#01440145--new-licensee-code
	// Only used if old licensee code == 0x33
	std::map<std::string, std::string>newLicenseeCodesNames; 

#pragma endregion

	std::string modalTitle;
	std::string modalDesc;
	bool showModalWindow = false;

protected:
	
	uint8_t ValidateRom();

	void LoadHeaderInfo();

	std::array<uint8_t, 0x30> companyLogo{0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B, 0x03, 0x73, 0x00, 0x83, 0x00, 0x0C, 0x00, 0x0D, 0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E, 0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD, 0xD9, 0x99, 0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC, 0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E};

	void LoadRom();
	
	


};

