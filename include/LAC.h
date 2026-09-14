#ifndef MTTPAK_INCLUDE_LAC_H_
#define MTTPAK_INCLUDE_LAC_H_

#include <cstdint>
#include <filesystem>
#include <istream>
#include <vector>

namespace mttPAK {
#pragma pack(push, 1)
  struct LAC_raw_entry {
    char name[32];
    std::uint32_t size;
    std::uint32_t offset;
  };

#pragma pack(pop)
  class LAC_entry {
    std::string m_name;
    std::wstring m_name_2;
    std::int64_t m_offset;
    std::int64_t m_size;

  public:
    LAC_entry(LAC_raw_entry raw_entry);
    std::string const& name() const noexcept;
    std::wstring const& name_2() const noexcept;
    std::int64_t offset() const noexcept;
    std::int64_t size() const noexcept;
    void extract(std::istream & stream, std::filesystem::path directory) const;
  };

#pragma pack(push, 1)
  class LAC_header {
    char m_magic[3];
    std::uint8_t m_version;
    std::uint32_t m_entry_count;

  public:
    LAC_header(std::istream & stream);
    int version() const noexcept;
    std::uint32_t entry_count() const noexcept;
    std::vector < LAC_entry > get_entries(std::istream & stream);
  };
#pragma pack(pop)
}

#endif