#ifndef FILE_PATH_H
#define FILE_PATH_H

#include <string>
#include <iostream>
#include <filesystem>
#include <unordered_map>
using namespace std;

std::filesystem::path getExtensionPath(const std::string& filePath){
    static const std::unordered_map<std::string, std::filesystem::path> extensionMap = {
        {".obj", "Models"},
        {".off", "Models"},
        {".tga", "Textures"},
        {".png", "Textures"},
        {".jpg", "Textures"},
        {".jpeg", "Textures"},
        {".bmp", "Textures"},
        {".gif", "Textures"},
        {".hdr", "Textures"},
        {".dds", "Textures"},
    };

    std::string extension = std::filesystem::path(filePath).extension().string();

    auto it = extensionMap.find(extension);
    if(it != extensionMap.end()){
        return "static" / it->second;
    }
    return "";
}

std::string getFilePath(const std::string& fileName){
    std::filesystem::path currentFilePath = std::filesystem::absolute(std::filesystem::path(__FILE__));
    std::filesystem::path baseDir = currentFilePath.parent_path().parent_path();

    std::string extensionPath = getExtensionPath(fileName);
    if(extensionPath.empty()){
        cout << "Error :: File Type Unsupported" << endl;
        return "";
    }

    return baseDir / extensionPath / fileName;
}

#endif