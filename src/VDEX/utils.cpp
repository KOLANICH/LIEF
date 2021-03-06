/* Copyright 2017 R. Thomas
 * Copyright 2017 Quarkslab
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <fstream>
#include <map>

#include "LIEF/VDEX/utils.hpp"
#include "LIEF/VDEX/Structures.hpp"

namespace LIEF {
namespace VDEX {
bool is_vdex(const std::string& file) {
  if (std::ifstream ifs{file, std::ios::in | std::ios::binary}) {

    char magic[sizeof(VDEX::magic)];

    ifs.seekg(0, std::ios::beg);
    ifs.read(magic, sizeof(magic));

    return std::equal(
        std::begin(magic),
        std::end(magic),
        std::begin(VDEX::magic));

  }

  return false;
}

bool is_vdex(const std::vector<uint8_t>& raw) {
  if (raw.size() < sizeof(VDEX::magic)) {
    return false;
  }

  char magic[sizeof(VDEX::magic)];
  std::copy(
    reinterpret_cast<const uint8_t*>(raw.data()),
    reinterpret_cast<const uint8_t*>(raw.data()) + sizeof(VDEX::magic),
    magic);

  return std::equal(std::begin(magic), std::end(magic), std::begin(VDEX::magic));
}

vdex_version_t version(const std::string& file) {
  if (not is_vdex(file)) {
    return 0;
  }

  if (std::ifstream ifs{file, std::ios::in | std::ios::binary}) {

    char version[4];

    ifs.seekg(sizeof(VDEX::magic), std::ios::beg);
    ifs.read(version, sizeof(version) + 1);

    if (std::all_of(std::begin(version), std::end(version) - 1, ::isdigit)) {
      return static_cast<vdex_version_t>(std::stoul(version));
    }
    return 0;

  }
  return 0;

}

vdex_version_t version(const std::vector<uint8_t>& raw) {
  if (raw.size() < 8) {
    return 0;
  }

  char version[4];
  std::copy(
    reinterpret_cast<const uint8_t*>(raw.data()) + sizeof(VDEX::magic),
    reinterpret_cast<const uint8_t*>(raw.data()) + sizeof(VDEX::magic) + sizeof(version) + 1,
    version);


  if (std::all_of(std::begin(version), std::end(version), ::isdigit)) {
    return static_cast<vdex_version_t>(std::stoul(version));
  }

  return 0;
}

LIEF::Android::ANDROID_VERSIONS android_version(vdex_version_t version) {
  static const std::map<vdex_version_t, LIEF::Android::ANDROID_VERSIONS> oat2android {
    { 6,  LIEF::Android::ANDROID_VERSIONS::VERSION_800 },
    { 10, LIEF::Android::ANDROID_VERSIONS::VERSION_810 },

  };
  auto   it  = oat2android.lower_bound(version);
  return it == oat2android.end() ? LIEF::Android::ANDROID_VERSIONS::VERSION_UNKNOWN : it->second;
}


}
}
