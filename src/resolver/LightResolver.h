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

/**
 * @file LightResolver.h
 * @author Schatz Forensic, Ptd Ltd.
 * @version 1.0
 * @date 12-Sep-2017
 * @copyright Copyright Schatz Forensic, Ptd Ltd. 2017. All Rights Reserved. This project is released under the LGPL 3.0+.
 *
 * @brief AFF4 Lightweight Resolver implementation.
 *
 * Provides a lightweight resource resolver for AFF4 containers.
 */
#ifndef SRC_RESOLVER_LIGHTRESOLVER_H_
#define SRC_RESOLVER_LIGHTRESOLVER_H_

#include "aff4config.h"
#include "aff4.h"

#include <atomic>
#include <mutex>
#include <map>
#include <string>

#ifndef _WIN32
#include <unistd.h>
#include <dirent.h>
#endif

#include <sys/types.h>

#include "AFF4Resource.h"
#include "FileUtil.h"

namespace aff4 {
/**
 * @brief AFF4 Resolver implementations.
 */
namespace resolver {

/**
 * @brief A lightweight aff4:Resolver.
 * <p>
 * The resolver interface only supports the query for Volume Resource IDs. (minimum required via IAFF4Resolver
 * interface). Additionally, this will only resolve AFF4 container volumeIDs.
 * <p>
 * This resolver does not track resources that have been opened.
 */
class LightResolver: public AFF4Resource, public IAFF4Resolver {
public:
	/**
	 * Construct a lightweight resolver.
	 * @param resource The resource name of the resolver.
	 * @param path The base path for looking for AFF4 containers.
	 * @param scanSubFolders TRUE to scan the folder and all sub folders for images.
	 */
	LIBAFF4_API LightResolver(const std::string& resource, const std::string& path, bool scanSubFolders = true);
	virtual ~LightResolver();

	/*
	 * From AFF4Resource.
	 */
	LIBAFF4_API std::string getResourceID() const noexcept;

	LIBAFF4_API std::map<aff4::Lexicon, std::vector<aff4::rdf::RDFValue>> getProperties() noexcept;

	LIBAFF4_API std::vector<aff4::rdf::RDFValue> getProperty(aff4::Lexicon resource) noexcept;

	/*
	 * From IAFF4Resolver
	 */
	LIBAFF4_API std::shared_ptr<aff4::IAFF4Resource> open(const std::string& resource) noexcept;

	LIBAFF4_API bool hasResource(const std::string& resource) noexcept;

private:
	/**
	 * Base path for searching for containers.
	 */
	std::string basePath;
	/**
	 * Lock for the map (volumes). (use std::lock_guard to acquire).
	 */
	std::recursive_mutex mapLock;
	/**
	 * Flag to indicate if we are to scan sub folders.
	 */
	const bool scanSubFolders;
	/**
	 * Map of Resource IDs to the container filename.
	 * Key: resource ID.
	 * Value: filename (absolute).
	 */
	std::map<std::string, std::string> volumes;

	/**
	 * Scan for AFF4 volumes in the given path.
	 * @param path The path to look for AFF4 containers.
	 */
	void scanForAFF4Volumes(const std::string& path);

	/**
	 * Does the map contain the following filename within it's values.
	 * @param filename The filename
	 * @return TRUE if we have a resource ID for this filename.
	 */
	bool mapContains(const std::string& filename);

};

} /* namespace resolver */
} /* namespace aff4 */

#endif /* SRC_RESOLVER_LIGHTRESOLVER_H_ */
