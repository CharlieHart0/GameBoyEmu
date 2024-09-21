#include "RomLoader.h"
#include <format>
#include <iostream>
#include <bitset>

void RomLoader::deleteRom()
{
	rom.clear();
    rom.shrink_to_fit();
}

void RomLoader::loadRomFromFile(const char* filePath)
{
	if (rom.size() > 0) deleteRom();

    

    std::ifstream inputROM(filePath, std::ios::binary);
    inputROM.seekg(0, std::ios::end);
    std::streampos fileSize = inputROM.tellg();

    if (fileSize == -1)
    {
        modalTitle = "Could not open ROM!";
        modalDesc = "No file found at: ";
        modalDesc += filePath;
        showModalWindow = true;
    }

    if (fileSize > GB_ROM_MAX_SIZE_BYTES)
    {
        modalTitle = "ROM load failed!";
        showModalWindow = true;
        modalDesc = std::string("Could not open ROM file! File was too big, max size = ") + std::to_string(GB_ROM_MAX_SIZE_BYTES / 1000) + 
            "KB, file size = "+ 
            std::to_string((int) fileSize / 1000) +
            "KB.";
    }

    inputROM.seekg(0, std::ios::beg);
    rom.resize(fileSize);
    inputROM.read((char*)rom.data(), fileSize);
    if (!ValidateRom())
    {
        //rom validation success
        LoadHeaderInfo();
        LoadRom();
    }
    

}

void RomLoader::UiUpdate()
{
    if (showModalWindow)
    {
        ImGui::OpenPopup(modalTitle.c_str());
        if (ImGui::BeginPopupModal(modalTitle.c_str(), &showModalWindow))
        {
            ImGui::Text(modalDesc.c_str());
            if (ImGui::Button("Okay"))
            {
                showModalWindow = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }

}

RomLoader::RomLoader()
{
    newLicenseeCodesNames = {
        {"00","None"},
        {"01","Nintendo Research & Development 1"},
        {"08","Capcom"},
        {"13","EA (Electronic Arts)"},
        {"18","Hudson Soft"},
        {"19","B-AI"},
        {"20","KSS"},
        {"22","Planning Office WADA"},
        {"24","PCM Complete"},
        {"25","San-X"},
        {"28","Kemco"},
        {"29","SETA Corporation"},
        {"30","Viacom"},
        {"31","Nintendo"},
        {"32","Bandai"},
        {"33","Ocean Software/Acclaim Entertainment"},
        {"34","Konami"},
        {"35","HectorSoft"},
        {"37","Taito"},
        {"38","Hudson Soft"},
        {"39","Banpresto"},
        {"41","Ubi Soft (Ubisoft)"},
        {"42","Atlus"},
        {"44","Malibu Interactive"},
        {"46","Angel"},
        {"47","Bullet-Proof Software (Blue Planet Software)"},
        {"49","Irem"},
        {"50","Absolute"},
        {"51","Acclaim Entertainment"},
        {"52","Activision"},
        {"53","Sammy USA Corporation"},
        {"54","Konami"},
        {"55","Hi Tech Expressions"},
        {"56","LJN"},
        {"57","Matchbox"},
        {"58","Mattel"},
        {"59","Milton Bradley Company"},
        {"60","Titus Interactive"},
        {"61","Virgin Games Ltd. (Avalon Interactive Group, Ltd.)"},
        {"64","Lucasfilm Games (LucasArts)"},
        {"67","Ocean Software"},
        {"69","EA (Electronic Arts)"},
        {"70","Infogrames (Atari SA)"},
        {"71","Interplay Entertainment"},
        {"72","Broderbund"},
        {"73","Sculptured Software"},
        {"75","The Sales Curve Limited (Eidos)"},
        {"78","THQ"},
        {"79","Accolade"},
        {"80","Misawa Entertainment"},
        {"83","lozc"},
        {"86","Tokuma Shoten"},
        {"87","Tsukuda Original"},
        {"91","Chunsoft Co. (Spike Chunsoft Co., Ltd.)"},
        {"92","Video System"},
        {"93","Ocean Software/Acclaim Entertainment"},
        {"95","Varie"},
        {"96","Yonezawa/s'pal"},
        {"97","Kaneko"},
        {"99","Pack-In-Video"},
        {"9H","Bottom Up"},
        {"A4","Konami (Yu-Gi-Oh!)"},
        {"BL","MTO"},
        {"DK","Kodansha"}
    };

    oldLicenseeCodesNames = {
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
        {0x73,"Sculptured Software"},
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
        {0xFF,"LJN"}
    };
}

std::string RomLoader::cartTypeFeaturesText()
{
    std::string s;
    if ((header.cartType.cartInfo & RAM) == RAM)  s += "RAM+";
    if ((header.cartType.cartInfo & BATTERY) == BATTERY)  s += "BATTERY+";
    if ((header.cartType.cartInfo & TIMER) == TIMER)  s += "TIMER+";
    if ((header.cartType.cartInfo & RUMBLE) == RUMBLE)  s += "RUMBLE+";
    if ((header.cartType.cartInfo & SENSOR) == SENSOR)  s += "SENSOR+";
    if (s.size() != 0) s.pop_back();

    return s;
}

void RomLoader::LoadRom()
{
    if (header.cartType.mainType == NO_MAPPER)
    {

    }

}



// returns 0 (false) if no issues, combination of bits according to RomLoader_ValidationOutcome (true) if any errors
uint8_t RomLoader::ValidateRom()
{
    uint8_t outcome = RomLoader_ValidationOutcome::NoIssues;

    // check for company logo
    for (auto i = companyLogo.begin(); i != companyLogo.end(); i++)
    {
        if (*i != rom[std::distance(companyLogo.begin(), i) + GB_ROM_HEADER_LOGO_ADDR])
        {
            outcome |= MissingCompanyLogo;
            break;
        }
    }

    // check cart type
    if(!validCartTypes.count(rom[GB_ROM_HEADER_CART_TYPE_ADDR])) outcome |= CartTypeErr;
    
    // check rom size
    if (!validRomSizes.count(rom[GB_ROM_HEADER_ROM_SIZE_ADDR])) outcome |= RomSize;
   
    // check ram on cart size
    if (rom[GB_ROM_HEADER_RAM_ADDR] > 0x05) outcome |= RamSize;
    
    // check destination code
    if (rom[GB_ROM_HEADER_DEST_ADDR] > 0x01) outcome |= DestinationCode;
    
    // check header checksum
    uint8_t checksum = 0;
    for (uint16_t address = GB_ROM_HEAD_CHECKSUM_START_ADDR; address <= GB_ROM_HEAD_CHECKSUM_END_ADDR; address++)
    {
        checksum = checksum - rom[address] - 1;
    }
    if (checksum != rom[GB_ROM_HEADER_HEAD_CHECKSUM_ADDR]) outcome |= HeaderChecksum;
    
    romValidationOutcome = outcome;
    return outcome;
}

void RomLoader::LoadHeaderInfo()
{
    //game title
    header.title = std::string((char*) &(rom[GB_ROM_HEADER_TITLE_ADDR]));
    header.title.resize(header.title.size() > 16 ? 16 : header.title.size());

    //new licensee code
    header.newLicenseeCode = std::string((char*) &(rom[GB_ROM_HEADER_NEWLICENSEECODE_ADDR]), 2);

    //new licensee name
    if (newLicenseeCodesNames.find(header.newLicenseeCode) != newLicenseeCodesNames.end())
    {
        header.newLicenseeName = newLicenseeCodesNames[header.newLicenseeCode];
    }
    else
    {
        header.newLicenseeName = "INVALID";
    }

    //sgb flag
    header.sgbFlag = rom[GB_ROM_HEADER_SGB_FLAG_ADDR];

    //cart type
    header.cartType = validCartTypes[rom[GB_ROM_HEADER_CART_TYPE_ADDR]];

    //rom size (bytes)
    header.romSizeBytes = validRomSizes[rom[GB_ROM_HEADER_ROM_SIZE_ADDR]];

    //rom banks count = romSizeBytes / 16 KiB
    header.romBanksCount = header.romSizeBytes / (16 * NUM_BYTES_IN_KIBIBYTE);

    //cart ram size (bytes)
    header.cartRamSizeBytes = validCartRamSizes[rom[GB_ROM_HEADER_RAM_ADDR]];

    //cart ram banks count
    header.cartRamBanksCount = validCartRamBankCount[rom[GB_ROM_HEADER_RAM_ADDR]];

    //cart ram banks size (each) (bytes)
    header.cartRamBanksSizeEach = validCartRamBankSizeEach[rom[GB_ROM_HEADER_RAM_ADDR]];

    //destination code
    header.destinationCode = (RomHeader_DestinationCode) rom[GB_ROM_HEADER_DEST_ADDR];

    //old licensee code
    header.oldLicenseeCode = rom[GB_ROM_HEADER_OLD_LICENSEE_CODE_ADDR];

    //old licensee name
    if (oldLicenseeCodesNames.find(header.oldLicenseeCode) != oldLicenseeCodesNames.end())
    {
        header.oldLicenseeName = oldLicenseeCodesNames[header.oldLicenseeCode];
    }
    else
    {
        header.oldLicenseeName = "INVALID";
    }

    //correct licensee name ptr (old name if old code != 0x33, else new name)
    header.licenseeName = header.oldLicenseeCode == 0x33 ? &header.newLicenseeName : &header.oldLicenseeName;

    //rom version number
    header.romVersionNumber = rom[GB_ROM_HEADER_OLD_LICENSEE_CODE_ADDR];

}
