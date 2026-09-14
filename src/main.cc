#include <Windows.h>

#include <fstream>
#include <iostream>

#include "KCAP.h"
#include "LAC.h"

int wmain(int argc, wchar_t * argv[]) {
  SetConsoleOutputCP(CP_UTF8);

  if (argc == 1) {
    std::cout << "WHITE ALBUM2 UNPACKER\nBasic rundown of file types in this game:\n- Audio files: grouped "
        "using proprietary LAC file format but the entries themself are not compressed (BGM.PAK, SE.PAK, "
        "VOICE.PAK, IC\\BGM.PAK, IC\\SE.PAK, IC\\VOICE.PAK)\n- Image files: grouped using proprietary KCAP "
        "file format and compressed (usually) using a custom LZSS implementation seemingly different from "
        "previous implementations (see KCAP.cc for information) (bak.pak, char.pak, fnt.pak, grp.pak, "
        "script.pak, IC\\bak.pak, IC\\char.pak, IC\\grp.pak, IC\\script.pak)\n- Video files: every file "
        "starting with MV (case-insensitive) is in reality a ASF file";
    return 0;
  }
  else if (argc < 3) {
    std::cout << ".\\WA2-UNPACK < in-path > < out-path > [ log-path ]";
    return 1;
  }

  std::ifstream in(argv[1], std::ios::binary);
  std::ofstream log;

  if (argc == 4) {
    log.open(argv[3]);
    std::cout.rdbuf(log.rdbuf());
  }

  std::filesystem::path out_directory(argv[2]);
  std::filesystem::create_directories(out_directory);

  try {
    mttPAK::KCAP_header header(in);

    std::cout << "KCAP file:\nentry count: " << header.entry_count() << "\nunknown 1: " <<
        header.unknown_1() << " unknown 2: " << header.unknown_2() << '\n';

    auto entries = header.get_entries(in);

    for (std::uint32_t i = 0; i < entries.size(); ++i) {
      auto const& entry = entries[i];

      if (argc < 4) {
        std::cout << "\033[32m";
      }
      std::cout << "extracting entry No. " << i + 1;
      if (argc < 4) {
        std::cout << "\033[0m";
      }
      std::cout << "\nname: " << entry.name() << " flags: " << entry.flags() << " offset: " <<
          entry.offset() << " size: " << entry.size() << "\nunknown 1: " << entry.unknown_1() <<
          " unknown 2: " << entry.unknown_2() << '\n';

      try {
        entry.extract(in, out_directory);
      }
      catch (std::exception const& exception) {
        std::cout << "ERROR: " << exception.what() << '\n';
      }
    }

    return 0;
  }
  catch (std::exception const& exception) {
    std::cout << "ERROR: " << exception.what() << '\n';
  }

  try {
    mttPAK::LAC_header header(in);

    std::cout << "LAC file:\nversion: " << header.version() << " entry count: " << header.entry_count() << '\n';

    auto entries = header.get_entries(in);

    for (std::uint32_t i = 0; i < entries.size(); ++i) {
      auto const& entry = entries[i];

      if (argc < 4) {
        std::cout << "\033[32m";
      }
      std::cout << "extracting entry No. " << i + 1;
      if (argc < 4) {
        std::cout << "\033[0m";
      }
      std::cout << "\nname: " << entry.name() << " offset: " << entry.offset() <<
          " size: " << entry.size() << '\n';

      try {
        entry.extract(in, out_directory);
      }
      catch (std::exception const& exception) {
        std::cout << "ERROR: " << exception.what() << '\n';
      }
    }

    return 0;
  }
  catch (std::exception const& exception) {
    std::cout << "ERROR: " << exception.what() << '\n';
  }

  char ASF_magic[4];

  in.seekg(0);
  in.read(ASF_magic, sizeof(ASF_magic));

  if (in.gcount() != sizeof(ASF_magic) || ASF_magic[0] != '\x30' || ASF_magic[1] != '\x26' ||
      ASF_magic[2] != '\xb2' || ASF_magic[3] != '\x75') {
    std::cout << "ERROR: invalid ASF header";
    return 2;
  }

  std::filesystem::path in_file(argv[1]);
  auto new_in_file = in_file;

  new_in_file.replace_extension("asf");

  std::filesystem::copy(in_file, out_directory / new_in_file.filename());
}