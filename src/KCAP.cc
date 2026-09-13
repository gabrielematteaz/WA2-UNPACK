#include "KCAP.h"

#include <Windows.h>

#include <fstream>
#include <iterator>
#include <memory>
#include <stdexcept>

namespace mttPAK {
  KCAP_entry::KCAP_entry(KCAP_raw_entry raw_entry) :
    m_flag(raw_entry.flag),
    m_unknown_1(raw_entry.unknown_1),
    m_unknown_2(raw_entry.unknown_2),
    m_offset(raw_entry.offset),
    m_size(raw_entry.size) {
    auto first = std::begin(raw_entry.name);
    auto current = first;

    for (auto last = std::end(raw_entry.name); current != last; ++current) {
      if (*current == '\0') {
        break;
      }
    }

    int raw_size = static_cast < int > (current - first);
    int size_2 = MultiByteToWideChar(932, 0, raw_entry.name, raw_size, NULL, 0);

    m_name_2.resize(size_2);
    MultiByteToWideChar(932, 0, raw_entry.name, raw_size, m_name_2.data(), size_2);

    int size = WideCharToMultiByte(CP_UTF8, 0, m_name_2.data(), size_2, NULL, 0, NULL, NULL);

    m_name.resize(size);
    WideCharToMultiByte(CP_UTF8, 0, m_name_2.data(), size_2, m_name.data(), size, NULL, NULL);
  }

  std::string const& KCAP_entry::name() const noexcept {
    return m_name;
  }

  std::wstring const& KCAP_entry::name_2() const noexcept {
    return m_name_2;
  }

  std::uint32_t KCAP_entry::flag() const noexcept {
    return m_flag;
  }

  std::uint32_t KCAP_entry::unknown_1() const noexcept {
    return m_unknown_1;
  }

  std::uint32_t KCAP_entry::unknown_2() const noexcept {
    return m_unknown_2;
  }

  std::int64_t KCAP_entry::offset() const noexcept {
    return m_offset;
  }

  std::int64_t KCAP_entry::size() const noexcept {
    return m_size;
  }

  void KCAP_entry::extract(std::istream & stream, std::filesystem::path directory) const {
    if (m_flag != 1) {
      return;
    }

    auto buffer = std::make_unique < char[] > (m_size);

    stream.seekg(m_offset);
    stream.read(buffer.get(), m_size);

    if (stream.gcount() != m_size) {
      throw std::runtime_error("could not read a KCAP entry content");
    }

    directory.append(m_name_2);

    std::ofstream file(directory, std::ios::binary);

    file.write(buffer.get(), m_size);
  }

  KCAP_header::KCAP_header(std::istream & stream) {
    stream.read((char *)this, sizeof(*this));

    if (stream.gcount() != sizeof(*this)) {
      throw std::runtime_error("could not read the KCAP header");
    }

    if (m_magic[0] != 'K' || m_magic[1] != 'C' || m_magic[2] != 'A' || m_magic[3] != 'P') {
      throw std::runtime_error("invalid KCAP header");
    }
  }

  std::uint32_t KCAP_header::unknown_1() const noexcept {
    return m_unknown_1;
  }

  std::uint32_t KCAP_header::unknown_2() const noexcept {
    return m_unknown_2;
  }

  std::uint32_t KCAP_header::entry_count() const noexcept {
    return m_entry_count;
  }

  std::vector < KCAP_entry > KCAP_header::get_entries(std::istream & stream) {
    stream.seekg(sizeof(*this));

    std::vector < KCAP_entry > entries;

    entries.reserve(m_entry_count);

    for (std::uint32_t i = 0; i < m_entry_count; ++i) {
      KCAP_raw_entry raw_entry;

      stream.read((char *)&raw_entry, sizeof(raw_entry));

      if (stream.gcount() != sizeof(raw_entry)) {
        throw std::runtime_error("could not read a KCAP entry information");
      }

      entries.emplace_back(raw_entry);
    }

    return entries;
  }
}