#include "AssetMap.h"

#include <utility>

namespace {
std::string AssetKey(const std::string &filePath) {
  const size_t slash = filePath.find_last_of("/\\");
  const size_t dot = filePath.find_last_of('.');
  const size_t start = slash == std::string::npos ? 0 : slash + 1;
  const size_t end = dot == std::string::npos || dot < start ? filePath.size() : dot;
  return filePath.substr(start, end - start);
}
}

void AssetMap::LoadMusic(const std::string& filepath) {
    const std::string key = AssetKey(filepath);
    const size_t dot = filepath.find_last_of('.');
    const std::string extension = dot == std::string::npos ? "" : filepath.substr(dot);
   m_Assets[key] = LoadMusicStream(filepath.c_str());
}

void AssetMap::LoadAsset(const std::string &filePath) {
  const std::string key = AssetKey(filePath);
  const size_t dot = filePath.find_last_of('.');
  const std::string extension = dot == std::string::npos ? "" : filePath.substr(dot);

  if (extension == ".wav" || extension == ".ogg" || extension == ".mp3") {
    m_Assets[key] = LoadSound(filePath.c_str());
  } else {
    m_Assets[key] = LoadTexture(filePath.c_str());
  }
}

void AssetMap::AddAsset(const std::string &key, Asset asset) {
  m_Assets[key] = std::move(asset);
}

std::optional<Asset> AssetMap::GetAsset(const std::string &key) const {
  const auto asset = m_Assets.find(key);
  if (asset == m_Assets.end()) {
    return std::nullopt;
  }
  return asset->second;
}
