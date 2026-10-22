#pragma once

#include <raylib.h>

#include <optional>
#include <string>
#include <unordered_map>
#include <variant>

using Asset = std::variant<Texture2D, Sound, Music>;

class AssetMap {
public:
	void LoadMusic(const std::string &filepath);
  void LoadAsset(const std::string &filePath);
  void AddAsset(const std::string &key, Asset asset);

  std::optional<Asset> GetAsset(const std::string &key) const;

private:
  std::unordered_map<std::string, Asset> m_Assets;
};
