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
 * @file AFF4ZipContainer.h
 * @author Schatz Forensic, Ptd Ltd.
 * @version 1.0
 * @date 12-Sep-2017
 * @copyright Copyright Schatz Forensic, Ptd Ltd. 2017. All Rights Reserved. This project is released under the LGPL 3.0+.
 *
 * @brief AFF4 Zip based Container implementation.
 *
 * This class an interface with interacting with AFF4 Containers that are Zip files.
 */
#ifndef SRC_CONTAINER_AFF4ZIPCONTAINER_H_
#define SRC_CONTAINER_AFF4ZIPCONTAINER_H_

#include "aff4config.h"
#include "aff4.h"

#include <atomic>
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <memory>

#include "AFF4Resource.h"
#include "Zip.h"
#include "StringUtil.h"
#include "Model.h"
#include "AFF4Image.h"
#include "ImageStreamFactory.h"
#include "ImageStream.h"
#include "AFF4Map.h"

namespace aff4 {
/**
 * @brief Base AFF4 Container implementations.
 */
namespace container {

/**
 * @brief Base AFF4 Zip Container.
 */
class AFF4ZipContainer: public AFF4Resource, public IAFF4Container {
public:

	/**
	 * Open an existing AFF4 Zip based Container
	 * @param resource The AFF4 Resource ID.
	 * @param parent The parent Zip file.
	 */
	LIBAFF4_API_LOCAL AFF4ZipContainer(const std::string& resource, std::unique_ptr<aff4::zip::Zip> parent);
	virtual ~AFF4ZipContainer();

	/*
	 * From AFF4Resource.
	 */
	std::string getResourceID() const noexcept;
	aff4::Lexicon getBaseType() noexcept;
	std::map<aff4::Lexicon, std::vector<aff4::rdf::RDFValue>> getProperties() noexcept;
	std::vector<aff4::rdf::RDFValue> getProperty(aff4::Lexicon resource) noexcept;

	/*
	 * From IAFF4Resolver
	 */
	std::shared_ptr<aff4::IAFF4Resource> open(const std::string& resource) noexcept;
	bool hasResource(const std::string& resource) noexcept;

	/*
	 * From IAFF4Container
	 */
	std::vector<std::shared_ptr<IAFF4Image>> getImages() noexcept;
	void setResolver(IAFF4Resolver* newResolver) noexcept;
	IAFF4Resolver* getResolver() noexcept;
	void close() noexcept;

	/**
	 * Get the RDF Model for this container.
	 * @return The RDF model for this container.
	 */
	LIBAFF4_API std::shared_ptr<aff4::rdf::Model> getRDFModel() noexcept;

	/**
	 * Get the Zip Entry details for the given segment name
	 * @param segmentName The name of the segment.
	 * @return The Zip Entry for the given segment name.
	 */
	LIBAFF4_API std::shared_ptr<aff4::zip::ZipEntry> getSegmentEntry(const std::string& segmentName) noexcept;

	/**
	 * Create a readable stream for the given segment name.
	 *
	 * @param segmentName The name of the segment to open
	 * @return A Stream, or NULL if segment doesn't exist or is unreadable.
	 */
	LIBAFF4_API std::shared_ptr<IAFF4Stream> getSegment(const std::string& segmentName) noexcept;

	/**
	 * Create a readable image stream for the given aff4:ImageStream resource name.
	 *
	 * @param resource The name of the aff4:ImageStream resource to open
	 * @return A Stream, or NULL if segment doesn't exist or is unreadable.
	 */
	LIBAFF4_API std::shared_ptr<IAFF4Stream> getImageStream(const std::string& resource) noexcept;

	/**
	 * Create an image object of the given resource.
	 *
	 * @param resource The name of the aff4:Image resource to open
	 * @return An aff4:Image, or NULL if the resource doesn't exist or is unreadable.
	 */
	LIBAFF4_API std::shared_ptr<IAFF4Image> getImage(const std::string& resource) noexcept;

	/**
	 * Create an map object of the given resource.
	 *
	 * @param resource The name of the aff4:Map resource to open
	 * @return An aff4:Map, or NULL if the resource doesn't exist or is unreadable.
	 */
	LIBAFF4_API std::shared_ptr<IAFF4Map> getMap(const std::string& resource) noexcept;

	/**
	 * Read a number of bytes from the underlying stream starting at offset
	 * @param buf A pointer to the buffer to read to.
	 * @param count The number of bytes to read
	 * @param offset The offset from the start of the stream.
	 * @return The number of bytes read. (0 indicates nothing read, or -1 indicates error.
	 */
	LIBAFF4_API int64_t fileRead(void *buf, uint64_t count, uint64_t offset) noexcept;
private:
	/**
	 * The parent zip container
	 */
	std::unique_ptr<aff4::zip::Zip> parent;
	/**
	 * An external resolver.
	 */
	IAFF4Resolver* externalResolver;
	/**
	 * Collection of available aff4:Image.
	 */
	std::vector<std::shared_ptr<IAFF4Image>> images;
	/**
	 * The RDF model.
	 */
	std::shared_ptr<aff4::rdf::Model> model;
	/**
	 * The collection of base properties for this container.
	 */
	void setBasicProperties() noexcept;

	/**
	 * Load the version.txt file and add to the containers properties.
	 */
	void loadVersionInformation() noexcept;

	/**
	 * Load the RDF model.
	 */
	void loadModel() noexcept;

	/**
	 * Attempt to sanitise the given resource string
	 *
	 * @param resource The resource string to sanitise
	 * @return The sanitised resource string.
	 */
	std::string sanitizeResource(const std::string& resource) noexcept;
};

} /* namespace container */
} /* namespace aff4 */

#endif /* SRC_CONTAINER_AFF4ZIPCONTAINER_H_ */
