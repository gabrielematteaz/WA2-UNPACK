#include <fstream>
#include <iostream>

#include "KCAP.h"

int main(int argc, char * argv[]) {
  if (argc < 3) {
    std::cout << "argc < 3";
    return 1;
  }

  std::ofstream result;
  std::ifstream file(argv[1], std::ios::binary);

  if (argc == 4) {
    result.open(argv[3]);
    std::cout.rdbuf(result.rdbuf());
  }

  std::filesystem::create_directories(argv[2]);

  mttPAK::KCAP_header header(file);
  auto entries = header.get_entries(file);

  std::cout << "unknown_1(): " << header.unknown_1() << " unknown_2(): " << header.unknown_2() <<
      " entry_count(): " << header.entry_count() << '\n';

  for (auto const& entry : entries) {
    std::cout << "name(): " << entry.name() << " flag(): " << entry.flag() << " unknown_1(): " <<
        entry.unknown_1() << " unknown_2(): " << entry.unknown_2() << " offset: " << entry.offset() <<
        " size(): " << entry.size() << '\n';
    entry.extract(file, argv[2]);
  }
}