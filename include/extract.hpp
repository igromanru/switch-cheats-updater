#ifndef EXTRACT_HPP
#define EXTRACT_HPP

#include <string>
#include <vector>
#include <switch.h>
#include <sstream>
#include <iomanip>
#include <iostream> 
#include <algorithm>
#include <iterator>
#include <string.h>
#include <stdio.h>
#include <filesystem>

#include <unzipper.h>

#define CONTENTS_PATH "contents/"
#define TITLES_PATH "contents/"
#define AMS_PATH "/atmosphere/"
#define SXOS_PATH "/sxos/"

struct title{
    std::string id;
    std::string version;
    friend bool operator<(const title& a, const title& b)
    {
        return a.id.compare(b.id) < 0;
        //return a.id.length() < b.id.length();
    }
};

std::vector<std::string> getInstalledTitles();

std::vector<title> getTitlesInfo();

std::string formatApplicationId(u64 ApplicationId);

std::string formatVersion(u32 version);

int extractCheats(std::string zipPath, std::vector<std::string> titles, bool sxos, bool credits);

int extractCheatsWithVersion(std::string zipPath, std::vector<title> titles, bool sxos, bool credits);

int removeCheats(bool sxos);


struct caselessCompare;

static constexpr u32 MaxTitleCount = 64000;

#endif

