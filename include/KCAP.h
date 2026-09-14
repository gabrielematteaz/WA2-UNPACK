#ifndef MTTPAK_INCLUDE_LAC_H_
#define MTTPAK_INCLUDE_LAC_H_

#include <cstdint>
#include <filesystem>
#include <istream>
#include <vector>

// https://github.com/vn-tools/arc_unpacker/blob/master/src/dec/leaf/common/custom_lzss.cc

namespace mttPAK {
#pragma pack(push, 1)
  struct KCAP_raw_entry {
    std::uint32_t flag;
    char name[24];
    std::uint32_t unknown_1;
    std::uint32_t unknown_2;
    std::uint32_t offset;
    std::uint32_t size;
  };

  struct KCAP_raw_entry_info {
    std::uint32_t size;
    std::uint32_t final_size;
  };

  class KCAP_entry {
    std::string m_name;
    std::wstring m_name_2;
    std::uint32_t m_flag;
    std::uint32_t m_unknown_1;
    std::uint32_t m_unknown_2;
    std::int64_t m_offset;
    std::int64_t m_size;

  public:
    KCAP_entry(KCAP_raw_entry raw_entry);
    std::string const& name() const noexcept;
    std::wstring const& name_2() const noexcept;
    std::uint32_t flag() const noexcept;
    std::uint32_t unknown_1() const noexcept;
    std::uint32_t unknown_2() const noexcept;
    std::int64_t offset() const noexcept;
    std::int64_t size() const noexcept;
    void extract(std::istream & stream, std::filesystem::path directory) const;
  };

  class KCAP_header {
    char m_magic[4];
    std::uint32_t m_unknown_1;
    std::uint32_t m_unknown_2;
    std::uint32_t m_entry_count;

  public:
    KCAP_header(std::istream & stream);
    std::uint32_t unknown_1() const noexcept;
    std::uint32_t unknown_2() const noexcept;
    std::uint32_t entry_count() const noexcept;
    std::vector < KCAP_entry > get_entries(std::istream & stream);
  };
#pragma pack(pop)
}

#endif