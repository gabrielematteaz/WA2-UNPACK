#include "LAC.h"

#include <fstream>
#include <memory>
#include <stdexcept>

namespace mttPAK {
  LAC_entry::LAC_entry(LAC_raw_entry raw_entry) :
    m_name(raw_entry.name),
    m_offset(raw_entry.offset),
    m_size(raw_entry.size) {
    auto current = m_name.begin();
    auto last = m_name.end();

    while (current != last) {
      char character = *current;

      if (character == '\xff') {
        break;
      }

      *current = character ^ '\xff';
      ++current;
    }

    m_name.resize(current - m_name.begin());
  }

  std::string const& LAC_entry::name() const noexcept {
    return m_name;
  }

  std::int64_t LAC_entry::offset() const noexcept {
    return m_offset;
  }

  std::int64_t LAC_entry::size() const noexcept {
    return m_size;
  }

  void LAC_entry::extract(std::istream & stream, std::filesystem::path directory) const {
    auto buffer = std::make_unique < char[] > (m_size);

    stream.seekg(m_offset);
    stream.read(buffer.get(), m_size);

    if (stream.gcount() != m_size) {
      throw std::runtime_error("could not read a LAC entry content");
    }

    directory.append(m_name);

    std::ofstream file(directory, std::ios::binary);

    file.write(buffer.get(), m_size);
  }

  LAC_header::LAC_header(std::istream & stream) {
    stream.read((char *)this, sizeof(*this));

    if (stream.gcount() != sizeof(*this)) {
      throw std::runtime_error("could not read the LAC header");
    }

    if (m_magic[0] != 'L' || m_magic[1] != 'A' || m_magic[2] != 'C' || m_version != 0 && m_version != 1) {
      throw std::runtime_error("invalid LAC header");
    }
  }

  std::uint32_t LAC_header::entry_count() const noexcept {
    return m_entry_count;
  }

  std::vector < LAC_entry > LAC_header::get_entries(std::istream & stream) {
    stream.seekg(sizeof(*this));

    std::vector < LAC_entry > entries;

    entries.reserve(m_entry_count);

    for (std::uint32_t i = 0; i < m_entry_count; ++i) {
      LAC_raw_entry raw_entry;

      stream.read((char *)&raw_entry, sizeof(raw_entry));

      if (stream.gcount() != sizeof(raw_entry)) {
        throw std::runtime_error("could not read a LAC entry information");
      }

      entries.emplace_back(raw_entry);
    }

    return entries;
  }
}