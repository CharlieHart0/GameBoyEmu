#pragma once
#include <vector>
#include <fstream>
#include <string>
#include <array>
#include <map>
#include "Vendor/ImGui/imgui.h"
#include "Vendor/magic-enum/magic_enum.hpp"

#define GB_ROM_MAX_SIZE_BYTES 4000000

#define NUM_BYTES_IN_MEBIBYTE 1048576
#define NUM_BYTES_IN_KIBIBYTE 1024

#define GB_ROM_HEAD_CHECKSUM_START_ADDR 0x0134
#define GB_ROM_HEAD_CHECKSUM_END_ADDR 0x014C

#define GB_ROM_HEADER_LOGO_ADDR 0x0104
#define GB_ROM_HEADER_CART_TYPE_ADDR 0x0147
#define GB_ROM_HEADER_ROM_SIZE_ADDR 0x0148
#define GB_ROM_HEADER_RAM_ADDR 0x0149
#define GB_ROM_HEADER_DEST_ADDR 0x014A
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
	CARTMAINTYPE_NONE,
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

	uint8_t romVersionNumber;
};

class RomLoader
{
public:
	void deleteRom();
	void loadRomFromFile(const char* filePath);
	void UiUpdate();

	std::vector<uint8_t> rom;
	RomHeader header;
	uint8_t romValidationOutcome = 0;
	
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
		{0x00,{CARTMAINTYPE_NONE,0b000001}},

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
	std::map<uint8_t, std::string>oldLicenseeCodes
	{
		{0x00,"None"},
		{0x01,"Nintendo"},
		{0x08,"Capcom"},
		{0x09,"HOT-B"},
		{0x0A,"Jaleco"},
		{0x0B,"Coconuts Japan"},
		{0x0C,"Elite Systems"},
		{0x13,"EA (Electronic Arts)"},
		{0x18,"Hudson Soft"},
		{0x19,"ITC Entertainment"},
		{0x1A,"Yanoman"},
		{0x1D,"Japan Clary"},
		{0x1F,"Virgin Games Ltd. (Avalon Interactive Group, Ltd.)"},
		{0x24,"PCM Complete"},
		{0x25,"San-X"},
		{0x28,"Kemco"},
		{0x29,"SETA Corporation"},
		{0x30,"Infogrames (Atari SA)"},
		{0x31,"Nintendo"},
		{0x32,"Bandai"},
		{0x33,"See \"New Licensee Code\"."},
		{0x34,"Konami"},
		{0x35,"HectorSoft"},
		{0x38,"Capcom"},
		{0x39,"Banpresto"},
		{0x3C,".Entertainment i"},
		{0x3E,"Gremlin"},
		{0x41,"Ubi Soft (Ubisoft)"},
		{0x42,"Atlus"},
		{0x44,"Malibu Interactive"},
		{0x46,"Angel"},
		{0x47,"Spectrum Holoby"},
		{0x49,"Irem"},
		{0x4A,"Virgin Games Ltd. (Avalon Interactive Group, Ltd.)"},
		{0x4D,"Malibu Interactive"},
		{0x4F,"U.S. Gold"},
		{0x50,"Absolute"},
		{0x51,"Acclaim Entertainment"},
		{0x52,"Activision"},
		{0x53,"Sammy USA Corporation"},
		{0x54,"GameTek"},
		{0x55,"Park Place"},
		{0x56,"LJN"},
		{0x57,"Matchbox"},
		{0x59,"Milton Bradley Company"},
		{0x5A,"Mindscape"},
		{0x5B,"Romstar"},
		{0x5C,"Naxat Soft (Kaga Create)"},
		{0x5D,"Tradewest"},
		{0x60,"Titus Interactive"},
		{0x61,"Virgin Games Ltd. (Avalon Interactive Group, Ltd.)"},
		{0x67,"Ocean Software"},
		{0x69,"EA (Electronic Arts)"},
		{0x6E,"Elite Systems"},
		{0x6F,"Electro Brain"},
		{0x70,"Infogrames (Atari SA)"},
		{0x71,"Interplay Entertainment"},
		{0x72,"Broderbund"},
		{0x73,"Sculpted Software"},
		{0x75,"The Sales Curve Limited (Eidos)"},
		{0x78,"THQ"},
		{0x79,"Accolade"},
		{0x7A,"Triffix Entertainment"},
		{0x7C,"Microprose"},
		{0x7F,"Kemco"},
		{0x80,"Misawa Entertainment"},
		{0x83,"Lozc"},
		{0x86,"Tokuma Shoten"},
		{0x8B,"Bullet-Proof Software (Blue Planet Software)"},
		{0x8C,"Vic Tokai"},
		{0x8E,"Ape"},
		{0x8F,"I'Max"},
		{0x91,"Chunsoft Co. (Spike Chunsoft Co., Ltd.)"},
		{0x92,"Video System"},
		{0x93,"Tsubaraya Productions"},
		{0x95,"Varie"},
		{0x96,"Yonezawa/S'Pal"},
		{0x97,"Kemco"},
		{0x99,"Arc"},
		{0x9A,"Nihon Bussan"},
		{0x9B,"Temco"},
		{0x9C,"Imagineer"},
		{0x9D,"Banpresto"},
		{0x9F,"Nova"},
		{0xA1,"Hori Electric"},
		{0xA2,"Bandai"},
		{0xA4,"Konami"},
		{0xA6,"Kawada"},
		{0xA7,"Takara"},
		{0xA9,"Technos Japan"},
		{0xAA,"Broderbund"},
		{0xAC,"Toei Animation"},
		{0xAD,"Toho"},
		{0xAF,"Namco"},
		{0xB0,"Acclaim Entertainment"},
		{0xB1,"ASCII Corporation or Nexsoft"},
		{0xB2,"Bandai"},
		{0xB4,"Square Enix"},
		{0xB6,"HAL Laboratory"},
		{0xB7,"SNK"},
		{0xB9,"Pony Canyon"},
		{0xBA,"Culture Brain"},
		{0xBB,"Sunsoft"},
		{0xBD,"Sony Imagesoft"},
		{0xBF,"Sammy Corporation"},
		{0xC0,"Taito"},
		{0xC2,"Kemco"},
		{0xC3,"Square"},
		{0xC4,"Tokuma Shoten"},
		{0xC5,"Data East"},
		{0xC6,"Tonkinhouse"},
		{0xC8,"Koei"},
		{0xC9,"UFL"},
		{0xCA,"Ultra"},
		{0xCB,"Vap"},
		{0xCC,"Use Corporation"},
		{0xCD,"Meldac"},
		{0xCE,"Pony Canyon"},
		{0xCF,"Angel"},
		{0xD0,"Taito"},
		{0xD1,"Sofel"},
		{0xD2,"Quest"},
		{0xD3,"Sigma Enterprises"},
		{0xD4,"ASK Kodansha Co."},
		{0xD6,"Naxat Soft (Kaga Create)"},
		{0xD7,"Copya System"},
		{0xD8,"Banpresto"},
		{0xDA,"Tomy"},
		{0xDB,"LJN"},
		{0xDD,"NCS"},
		{0xDE,"Human"},
		{0xDF,"Altron"},
		{0xE0,"Jaleco"},
		{0xE1,"Towa Chiki"},
		{0xE2,"Yutaka"},
		{0xE3,"Varie"},
		{0xE5,"Epcoh"},
		{0xE7,"Athena"},
		{0xE8,"Asmik Ace Entertainment"},
		{0xE9,"Natsume"},
		{0xEA,"King Records"},
		{0xEB,"Atlus"},
		{0xEC,"Epic/Sony Records"},
		{0xEE,"IGS"},
		{0xF0,"A Wave"},
		{0xF3,"Extreme Entertainment"},
		{0xFF,"LJN"},
	};

	// Only used if old licensee code == 0x33
	std::map<std::string,std::string>newLicenseeCodes
	{

	};

	std::string modalTitle;
	std::string modalDesc;
	bool showModalWindow = false;

protected:
	
	uint8_t ValidateRom();

	void LoadHeaderInfo();

	std::array<uint8_t, 0x30> companyLogo{0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B, 0x03, 0x73, 0x00, 0x83, 0x00, 0x0C, 0x00, 0x0D, 0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E, 0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD, 0xD9, 0x99, 0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC, 0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E};

	

	
	


};

