/*-
 This file is part of AFF4 CPP.

 AFF4 CPP is free software: you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 AFF4 CPP is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with AFF4 CPP.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * Example application to create a SHA1 digest of all images in the container.
 */

#ifdef _WIN32
#include "stdafx.h"
#endif

#include "aff4.h"
#include "utils.h"

/**
 * Print usage
 * @param argv
 */
void printUsage(char* argv[]) {
	std::cout << "USAGE: " << argv[0] << " <container filename>" << std::endl;
}

/**
 * Main
 * @param argc number of arguments
 * @param argv args
 * @return EXIT_SUCCESS
 */
int main(int argc, char* argv[]) {
	std::cout << "AFF4 Container Information (" << aff4::getVersionString() << ")" << std::endl;
	if (argc < 2) {
		printUsage(argv);
		return 1;
	}
	std::string filename(argv[1]);
	if (filename.empty()) {
		printUsage(argv);
		return 1;
	}

	/*
	 * Create a resolver to allow the container to find any external resources.
	 */

	std::unique_ptr<aff4::IAFF4Resolver> resolver(aff4::container::createResolver(filename));
	if (resolver == nullptr) {
		std::cout << "Unable to construct a resolver?" << std::endl;
	}

	/*
	 * Open the container
	 */

	std::shared_ptr<aff4::IAFF4Container> container = aff4::container::openAFF4Container(filename, resolver.get());
	if (container == nullptr) {
		std::cout << "Unable to open the file \"" << filename << "\"?" << std::endl;
	}

	/*
	 * Print the container's resource values.
	 */

	std::cout << "Container : " << container->getResourceID() << std::endl;
	std::cout << " Images: " << std::endl;

	/*
	 * Get a vector of all aff4:Image instances in this container
	 */

	std::vector<std::shared_ptr<aff4::IAFF4Image>> images = container->getImages();
	for (std::shared_ptr<aff4::IAFF4Image> image : images) {

		/*
		 * Print the image's resource values.
		 */

		std::cout << "  Image : " << image->getResourceID() << std::endl;
		printProperties(image, "    ");

		/*
		 * Get the map instance of the image, and create a digest of the primary stream.
		 */

		std::shared_ptr<aff4::IAFF4Map> map = image->getMap();
		if (map != nullptr) {
			std::shared_ptr<aff4::IAFF4Stream> stream = map->getStream();
			if (stream != nullptr) {
				std::cout << "Progess: ";
				std::cout << "SHA1: " << sha1sum(stream, 1024 * 1024) << std::endl;
			} else {
				std::cout << "Unable to open stream?" << std::endl;
				return 1;
			}
		} else {
			std::cout << "Unable to open map?" << std::endl;
			return 1;
		}

	}
	std::cout << std::endl;
}
