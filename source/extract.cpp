#include "extract.hpp"
#include <iostream>



std::vector<std::string> getInstalledTitles(){
    std::vector<std::string> titles;
    NcmContentMetaDatabase metadb;
    Result rc = ncmOpenContentMetaDatabase(&metadb, NcmStorageId_SdCard);
    if(R_SUCCEEDED(rc))
        {
            NcmApplicationContentMetaKey *recs = new NcmApplicationContentMetaKey[MaxTitleCount]();
            s32 wrt = 0;
            s32 total = 0;
            rc = ncmContentMetaDatabaseListApplication(&metadb, &total, &wrt, recs, MaxTitleCount, NcmContentMetaType_Application);
            if((R_SUCCEEDED(rc)) && (wrt > 0))
            {
                titles.reserve(wrt);
                for(s32 i = 0; i < wrt; i++)
                {
                    titles.push_back(formatApplicationId(recs[i].application_id));
                }
            }
            delete[] recs;
            serviceClose(&metadb.s);
        }
    return titles;
}

std::vector<title> getTitlesInfo(){
    std::vector<title> res;
    NcmContentMetaDatabase metadb;
    Result rc = ncmOpenContentMetaDatabase(&metadb, NcmStorageId_SdCard);
    if(R_SUCCEEDED(rc)){
        NcmContentMetaKey *recs = new NcmContentMetaKey[MaxTitleCount]();
        s32 wrt = 0;
        s32 total = 0;
        rc = ncmContentMetaDatabaseList(&metadb, &total, &wrt, recs, MaxTitleCount, NcmContentMetaType_Application, 0, 0, U64_MAX, NcmContentInstallType_Full);
        if((R_SUCCEEDED(rc)) && (wrt > 0)){
            res.reserve(wrt);
            for(s32 i = 0; i < wrt; i++)
            {
                title t = {};
                t.id = formatApplicationId(recs[i].id);
                //t.id = "0";
                t.version = formatVersion(recs[i].version);
                std::cout << t.version << " " << recs[i].version << std::endl;
                //t.version = recs[i].version;
                res.push_back(t);
            }
        }
        delete[] recs;
        serviceClose(&metadb.s);
    }
    return res;
}

std::string formatApplicationId(u64 ApplicationId){
    std::stringstream strm;
    strm << std::uppercase << std::setfill('0') << std::setw(16) << std::hex << ApplicationId;
    return strm.str();
}

std::string formatVersion(u32 version){
    std::stringstream strm;
    strm << std::uppercase << std::setfill('0') << std::setw(16) << std::hex << version;
    return strm.str();
}

struct caselessCompare { 
    bool operator() (const std::string& a, const std::string& b) const {
        return strcasecmp(a.substr(7, 16).c_str(), b.c_str()) < 0;
    }
};

int extractCheats(std::string zipPath, std::vector<std::string> titles, bool sxos, bool credits){
    zipper::Unzipper unzipper(zipPath);
    std::vector<zipper::ZipEntry> entries = unzipper.entries();

    int offset;
    if(sxos){
        offset = std::string(TITLES_PATH).length();
        chdir(SXOS_PATH);
    }
    else{
        offset = std::string(CONTENTS_PATH).length();
        chdir(AMS_PATH);
    }

    std::vector<std::string> entriesNames;
    std::vector<int> parentIndexes;
    for (size_t i = 1; i < entries.size(); i++){
        entriesNames.push_back(entries[i].name);
    }

    std::sort(entriesNames.begin(), entriesNames.end());
    std::sort(titles.begin(), titles.end());

    std::vector<std::string> parents;
    std::vector<std::vector<std::string>> children;
    std::vector<std::string> tempChildren;

    size_t k = 0;
    while(k < entriesNames.size()){
        if(entriesNames[k].length() == (size_t) (offset + 17)){
            parents.push_back(entriesNames[k]);
            k++;
            while(entriesNames[k].length() != (size_t) (offset + 17) && k < entriesNames.size()){
                if(credits == false){
                    if(strcasecmp(entriesNames[k].substr(offset + 16, 7).c_str(), "/cheats") == 0){
                        tempChildren.push_back(entriesNames[k]);
                    }
                }
                else{
                    tempChildren.push_back(entriesNames[k]);
                }
                k++;
            }
            children.push_back(tempChildren);
            tempChildren.clear (); 
        }
        else{
            k++;
        }
    }

    std::cout << "\033[4;31m"<< "\n*** Extracting cheats (this may take a while) ***" << "\033[0m" <<std::endl;

    int count = 0;
    size_t lastL = 0;
    for(size_t j = 0; j < titles.size(); j++){
        for(size_t l = lastL; l < parents.size(); l++){
            if(strcasecmp(titles[j].c_str(), parents[l].substr(offset, 16).c_str()) == 0){
                unzipper.extractEntry(parents[l]);
                for(auto& e : children[l]){
                    unzipper.extractEntry(e);
                    std::cout << std::setfill(' ') << std::setw(80) << '\r';
                    std::cout << e.substr(0, 79) << "\r";
                    count++;
                    consoleUpdate(NULL);
                }
                lastL = l;
                break;
            }
        }
    }
    std::cout << std::endl;

    unzipper.close();
    return count;
}

int extractCheatsWithVersion(std::string zipPath, std::vector<title> titles, bool sxos, bool credits){
    zipper::Unzipper unzipper(zipPath);
    std::vector<zipper::ZipEntry> entries = unzipper.entries();

    int offset;
    if(sxos){
        offset = std::string(TITLES_PATH).length();
        chdir(SXOS_PATH);
    }
    else{
        offset = std::string(CONTENTS_PATH).length();
        chdir(AMS_PATH);
    }

    std::vector<std::string> entriesNames;
    std::vector<int> parentIndexes;
    for (size_t i = 1; i < entries.size(); i++){
        entriesNames.push_back(entries[i].name);
    }

    std::sort(entriesNames.begin(), entriesNames.end());
    std::sort(titles.begin(), titles.end());

    std::vector<std::string> parents;
    std::vector<std::vector<std::string>> children;
    std::vector<std::string> tempChildren;

    size_t k = 0;
    while(k < entriesNames.size()){
        if(entriesNames[k].length() == (size_t) (offset + 17)){
            parents.push_back(entriesNames[k]);
            k++;
            while(entriesNames[k].length() != (size_t) (offset + 17) && k < entriesNames.size()){
                if(credits == false){
                    if(strcasecmp(entriesNames[k].substr(offset + 16, 7).c_str(), "/cheats") == 0){
                        tempChildren.push_back(entriesNames[k]);
                    }
                }
                else{
                    tempChildren.push_back(entriesNames[k]);
                }
                k++;
            }
            children.push_back(tempChildren);
            tempChildren.clear (); 
        }
        else{
            k++;
        }
    }

    std::cout << "\033[4;31m"<< "\n*** Extracting cheats (this may take a while) ***" << "\033[0m" <<std::endl;

    //std::cout << "title " << titles[0].id << std::endl;
    //std::cout << "parent " << parents[0].substr(offset, 16).c_str() << std::endl;
    std::string lol = "0";
    std::string lol2 = "0";
    int count = 0;
    size_t lastL = 0;
    for(size_t j = 0; j < titles.size(); j++){
        for(size_t l = lastL; l < parents.size(); l++){
            if(strcasecmp(titles[j].id.c_str(), parents[l].substr(offset, 16).c_str()) == 0){
                //unzipper.extractEntry(parents[l]);
                for(auto& e : children[l]){
                    lol = e.substr(offset + 16 + 7, 17).c_str();
                    lol2 = ("/" + titles[j].version).c_str();
                    if(strcasecmp(("/" + titles[j].version).c_str(), e.substr(offset + 16 + 7, 17).c_str())){
                        //unzipper.extractEntry(e);
                        std::cout << std::setfill(' ') << std::setw(80) << '\r';
                        std::cout << e.substr(0, 79) << "\r";
                        count++;
                        consoleUpdate(NULL);
                    }
                }
                lastL = l;
                break;
            }
        }
    }
    std::cout << std::endl;
        std::cout << "lol1" <<lol << std::endl;
        std::cout << "lol2" <<lol2 << std::endl;


    unzipper.close();
    return count;
}














int removeCheats(bool sxos){
    std::string path;
    if(sxos){
        path = std::string(SXOS_PATH) + std::string(TITLES_PATH);
        std::filesystem::remove("./titles.zip");
    }
    else{
        path = std::string(AMS_PATH) + std::string(CONTENTS_PATH);
        std::filesystem::remove("./contents.zip");
    }
    int c = 0;
    for (const auto & entry : std::filesystem::directory_iterator(path)){
        std::string cheatsPath =  entry.path().string() + "/cheats";
        if(std::filesystem::exists(cheatsPath)){
            for (const auto & cheat : std::filesystem::directory_iterator(cheatsPath)){
                std::filesystem::remove(cheat);
                c++;
            }
            std::filesystem::remove(cheatsPath);
        }
    }
    return c;
}