#ifndef FTP_UTILS_STREAMS_H
#define FTP_UTILS_STREAMS_H

#include <fstream>
#include <functional>
#include <cmath>

namespace FTP { namespace Utils { namespace Streams {

std::uint64_t GetLength(std::ifstream & stream) {
	std::streampos original_offset = stream.tellg();

	stream.seekg(0, stream.end);
	auto length = static_cast<std::uint64_t>(stream.tellg());
	stream.seekg(original_offset);

	return length;
}

void ChunkRead(std::ifstream & stream, std::size_t chunk_size, std::function<void(const std::string &)> callback) {
	std::uint64_t chunks = ceil(static_cast<double>(GetLength(stream)) / chunk_size);
	char buffer[chunk_size + 1];

	for (int i = 0; i < chunks; ++i) {
		stream.read(buffer, chunk_size);

		buffer[chunk_size] = '\0'; // Properly end the string.

		callback(std::string(buffer));
	}
}

}
}
}

#endif