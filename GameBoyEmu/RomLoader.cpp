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
{}
