#include <kromakit/graphics/svg/SvgResourceCache.h>
#include <kromakit/platform/PlatformResourceLoader.h>

std::shared_ptr<SvgDocument> SvgResourceCache::Load(
  const std::string& resourcePath) {
  if (auto it = cache.find(resourcePath); it != cache.end()) {
    if (auto existing = it->second.lock())
      return existing;
  }

  auto resolvedPath = PlatformResourceLoader::ResolveResourcePath(resourcePath);
  auto loaded = SvgLoader::LoadFromFile(resolvedPath);
  if (!loaded)
    return nullptr;

  cache[resourcePath] = loaded;
  return loaded;
}

std::unordered_map<std::string, std::weak_ptr<SvgDocument>>
  SvgResourceCache::cache;
