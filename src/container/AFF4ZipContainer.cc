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

#include "AFF4ZipContainer.h"

namespace aff4 {
namespace container {

AFF4ZipContainer::AFF4ZipContainer(const std::string& resource, std::unique_ptr<aff4::zip::Zip> parent) :
		AFF4Resource(resource), parent(std::move(parent)) {
#if DEBUG
	fprintf( aff4::getDebugOutput(), "%s[%d] : New AFF4 Zip Container: %s : %s \n", __FILE__, __LINE__,
			this->parent->getFilename().c_str(), resource.c_str());
#endif
	// Set base properties
	setBasicProperties();
	// Load version.txt
	loadVersionInformation();
	// Load the RDF model.
	loadModel();
	// Add information about THIS object to the object properties.
	std::map<aff4::Lexicon, std::vector<aff4::rdf::RDFValue>> elements = model->getObjectInformation(resource);
	if (!elements.empty()) {
		for (auto it = elements.begin(); it != elements.end(); it++) {
			addProperty(it->first, it->second);
		}
	}
}

AFF4ZipContainer::~AFF4ZipContainer() {
	parent->close();
}

void AFF4ZipContainer::setBasicProperties() noexcept {
	addProperty(aff4::Lexicon::AFF4_TYPE, aff4::rdf::RDFValue(aff4::Lexicon::AFF4_ZIP_TYPE));
	addProperty(aff4::Lexicon::AFF4_STORED, aff4::rdf::RDFValue(parent->getFilename()));
}

void AFF4ZipContainer::loadVersionInformation() noexcept {
	std::string version(AFF4_VERSIONDESCRIPTIONFILE);
	std::shared_ptr<IAFF4Stream> stream = parent->getStream(version);
	if (stream != nullptr) {
		std::string versionTXT;
		std::unique_ptr<char[]> buffer(new char[stream->size()]);
		int64_t res = stream->read(buffer.get(), stream->size(), 0);
		if (res > 0) {
			versionTXT = std::string(buffer.get(), res);
		}
		// Decode the version.txt file. Examples expected contents is below.
		// Note: the AFF4 Spec does not note that the major/minor values are int/float/string, so we store as String.
		/*
		 * major=1
		 * minor=0
		 * tool=Evimetry 3.0.0
		 */
		// Iterate over each line, looking for our elements.
		const std::string MAJOR(AFF4_VERSION_MAJOR);
		const std::string MINOR(AFF4_VERSION_MINOR);
		const std::string TOOL(AFF4_VERSION_TOOL);

		std::stringstream data(versionTXT);
		std::string line;

		while (std::getline(data, line, '\n')) {
			if (!line.empty()) {
				if (aff4::util::hasPrefix(line, MAJOR)) {
					addProperty(aff4::Lexicon::AFF4_MAJOR_VERSION, aff4::rdf::RDFValue(line.substr(MAJOR.size())));
#if DEBUG
					fprintf( aff4::getDebugOutput(), "%s[%d] : Major Version : %s\n", __FILE__, __LINE__,
							line.substr(MAJOR.size()).c_str());
#endif
				} else if (aff4::util::hasPrefix(line, MINOR)) {
					addProperty(aff4::Lexicon::AFF4_MINOR_VERSION, aff4::rdf::RDFValue(line.substr(MINOR.size())));
#if DEBUG
					fprintf( aff4::getDebugOutput(), "%s[%d] : Minor Version : %s\n", __FILE__, __LINE__,
							line.substr(MINOR.size()).c_str());
#endif
				} else if (aff4::util::hasPrefix(line, TOOL)) {
					addProperty(aff4::Lexicon::AFF4_TOOL, aff4::rdf::RDFValue(line.substr(TOOL.size())));
#if DEBUG
					fprintf( aff4::getDebugOutput(), "%s[%d] : Tool Version : %s\n", __FILE__, __LINE__,
							line.substr(TOOL.size()).c_str());
#endif
				}

			}
		}
	} else {
#if DEBUG
		fprintf( aff4::getDebugOutput(), "%s[%d] : version.txt file is missing from container? : %s\n", __FILE__, __LINE__,
				parent->getFilename().c_str());
#endif
	}
}

void AFF4ZipContainer::loadModel() noexcept {
	std::string version(AFF4_INFORMATIONTURTLE);
	std::shared_ptr<IAFF4Stream> stream = parent->getStream(version);
	if (stream != nullptr) {
		std::unique_ptr<uint8_t[]> buffer(new uint8_t[stream->size()]);
		int64_t res = stream->read(buffer.get(), stream->size(), 0);
		if (res > 0) {
			// Attempt to construct a RDF model from the buffer.
			model = std::make_shared<aff4::rdf::Model>();
			model->parse(buffer.get(), res);
		}
	}
}

std::shared_ptr<aff4::rdf::Model> AFF4ZipContainer::getRDFModel() noexcept {
	return model;
}

std::shared_ptr<IAFF4Stream> AFF4ZipContainer::getSegment(const std::string& segmentName) noexcept {
	std::string res = sanitizeResource(segmentName);
	std::shared_ptr<IAFF4Stream> stream = parent->getStream(res);
	return stream;
}

std::shared_ptr<aff4::zip::ZipEntry> AFF4ZipContainer::getSegmentEntry(const std::string& segmentName) noexcept {
	std::string res = sanitizeResource(segmentName);
	std::vector<std::shared_ptr<aff4::zip::ZipEntry>> entries = parent->getEntries();
	for (std::shared_ptr<aff4::zip::ZipEntry> e : entries) {
		if (!e->getSegmentName().compare(res)) {
			return e;
		}
	}
	return nullptr;
}

int64_t AFF4ZipContainer::fileRead(void *buf, uint64_t count, uint64_t offset) noexcept {
	return parent->fileRead(buf, count, offset);
}

std::shared_ptr<IAFF4Stream> AFF4ZipContainer::getImageStream(const std::string& resource) noexcept {
#if DEBUG
	fprintf( aff4::getDebugOutput(), "%s[%d] : aff4:ImageStream : %s \n", __FILE__, __LINE__, resource.c_str());
#endif
	// No stream resource
	if (resource.empty()) {
		return nullptr;
	}
	/*
	 * Check for symbolic stream types.
	 */
	if (resource.compare(aff4::lexicon::getLexiconString(aff4::Lexicon::AFF4_IMAGESTREAM_ZERO)) == 0) {
		return aff4::stream::createZeroStream();
	}
	if (resource.compare(aff4::lexicon::getLexiconString(aff4::Lexicon::AFF4_IMAGESTREAM_UNKNOWN)) == 0) {
		return aff4::stream::createUnknownStream();
	}
	if (resource.compare(aff4::lexicon::getLexiconString(aff4::Lexicon::AFF4_IMAGESTREAM_UNREADABLE)) == 0) {
		return aff4::stream::createUnreadableStream();
	}
	if (aff4::util::hasPrefix(resource, lexicon::getLexiconString(aff4::Lexicon::AFF4_IMAGESTREAM_SYMBOLIC_PREFIX))) {
		return aff4::stream::createSymbolicStream(resource);
	}
	/*
	 * Check for aff4:ImageStream contained in us.
	 */
	std::vector<std::string> availableStreams = model->getResourcesOfType(aff4::Lexicon::AFF4_IMAGESTREAM_TYPE);
	for (std::string streamID : availableStreams) {
		if (streamID == resource) {
			// We have this stream in our model.
			std::map<aff4::Lexicon, std::vector<aff4::rdf::RDFValue>> properties = model->getObjectInformation(
					resource);
			auto entry = properties.find(aff4::Lexicon::AFF4_STORED);
			if (entry != properties.end()) {
				for (aff4::rdf::RDFValue v : entry->second) {
					if (v.getXSDType() == aff4::rdf::XSDType::Resource) {
						std::string value = v.getValue();
						if (value == getResourceID()) {
							// This stream is stored in us...
							std::shared_ptr<aff4::stream::ImageStream> stream = //
									std::make_shared<aff4::stream::ImageStream>(resource, this);
							return stream;
						}
					}
				}
			} else {
				// No stored property, instead look for resource index file in underlying zip container.
				std::string res = sanitizeResource(resource + "/00000000.index");
				if (parent->hasEntry(res)) {
					std::shared_ptr<aff4::stream::ImageStream> stream = //
							std::make_shared<aff4::stream::ImageStream>(resource, this);
					return stream;
				}
			}
		}
	}
	return nullptr;
}

std::string AFF4ZipContainer::sanitizeResource(const std::string& resource) noexcept {
	std::string res = resource;
#if DEBUG
	fprintf( aff4::getDebugOutput(), "%s[%d] : sanitize Resource: %s\n", __FILE__, __LINE__, res.c_str());
#endif

	// Strip leading '/'
	while (aff4::util::hasPrefix(res, "/")) {
		res = res.substr(1);
	}
	if (aff4::util::hasPrefix(res, getResourceID())) {
		res = res.substr(getResourceID().size());
	}
	// Convert any "aff4://" characters to "aff4%3A%2F%2F"
	res = aff4::util::nameCodecEncode(res);
	// strip any leading "/"
	while (aff4::util::hasPrefix(res, "/")) {
		res = res.substr(1);
	}
#if DEBUG
	fprintf( aff4::getDebugOutput(), "%s[%d] : sanitized Resource: %s\n", __FILE__, __LINE__, res.c_str());
#endif
	return res;
}

/*
 * IAFF4Container
 */

std::vector<std::shared_ptr<IAFF4Image>> AFF4ZipContainer::getImages() noexcept {
	// Scan for images on first call, and cache created objects.
	if (images.empty()) {
		// Look for all objects that have a RDFType of aff4:Image.
		std::vector<std::string> resources = model->getResourcesOfType(aff4::Lexicon::AFF4_IMAGE_TYPE);
		for (std::string resource : resources) {
			std::shared_ptr<aff4::image::AFF4Image> image = std::make_shared<aff4::image::AFF4Image>(resource, this);
			images.push_back(image);
		}
	}
	return images;
}

std::shared_ptr<IAFF4Image> AFF4ZipContainer::getImage(const std::string& resource) noexcept {
	std::vector<std::shared_ptr<IAFF4Image>> images = getImages();
	for (std::shared_ptr<IAFF4Image> image : images) {
		if (image->getResourceID().compare(resource) == 0) {
			return image;
		}
	}
	return nullptr;
}

std::shared_ptr<IAFF4Map> AFF4ZipContainer::getMap(const std::string& resource) noexcept {
	std::vector<std::string> objects = model->getResourcesOfType(aff4::Lexicon::AFF4_MAP_TYPE);
	for (std::string object : objects) {
		if (object.compare(resource) == 0) {
			return std::make_shared<aff4::map::AFF4Map>(resource, this);
		}
	}
	return nullptr;
}

void AFF4ZipContainer::setResolver(IAFF4Resolver* newResolver) noexcept {
	externalResolver = newResolver;
}

IAFF4Resolver* AFF4ZipContainer::getResolver() noexcept {
	return externalResolver;
}

void AFF4ZipContainer::close() noexcept {
	parent->close();
}

/*
 * From IAFF4Resolver
 */
std::shared_ptr<aff4::IAFF4Resource> AFF4ZipContainer::open(const std::string& resource) noexcept {
#if DEBUG
	fprintf( aff4::getDebugOutput(), "%s[%d] : open %s\n", __FILE__, __LINE__, resource.c_str());
#endif
	if (resource.empty()) {
		return nullptr;
	}

	// See if the request is for an aff4::image in us.
	std::shared_ptr<aff4::IAFF4Image> image = getImage(resource);
	if (image != nullptr) {
#if DEBUG
		fprintf( aff4::getDebugOutput(), "%s[%d] : Found Resource %s = %s\n", __FILE__, __LINE__, resource.c_str(),
				aff4::lexicon::getLexiconString(image->getBaseType()).c_str());
#endif
		return image;
	}

	// See if the request is for a aff4::map in us.
	std::shared_ptr<aff4::IAFF4Map> map = getMap(resource);
	if (map != nullptr) {
#if DEBUG
		fprintf( aff4::getDebugOutput(), "%s[%d] : Found Resource %s = %s\n", __FILE__, __LINE__, resource.c_str(),
				aff4::lexicon::getLexiconString(map->getBaseType()).c_str());
#endif
		return map;
	}

	// See if the request is for a aff4::map in us.
	std::shared_ptr<aff4::IAFF4Stream> stream = getImageStream(resource);
	if (stream != nullptr) {
#if DEBUG
		fprintf( aff4::getDebugOutput(), "%s[%d] : Found Resource %s = %s\n", __FILE__, __LINE__, resource.c_str(),
				aff4::lexicon::getLexiconString(stream->getBaseType()).c_str());
#endif
		return stream;
	}

	// See if a zip segment.
	stream = getSegment(resource);
	if (stream != nullptr) {
#if DEBUG
		fprintf( aff4::getDebugOutput(), "%s[%d] : Found Resource %s = %s\n", __FILE__, __LINE__, resource.c_str(),
				aff4::lexicon::getLexiconString(stream->getBaseType()).c_str());
#endif
		return stream;
	}

	return nullptr;
}

bool AFF4ZipContainer::hasResource(const std::string& resource) noexcept {
#if DEBUG
	fprintf( aff4::getDebugOutput(), "%s[%d] : hasResource %s\n", __FILE__, __LINE__, resource.c_str());
#endif
	std::shared_ptr<aff4::IAFF4Resource> res = open(resource);
	if (res == nullptr) {
		return false;
	}
#if DEBUG
	fprintf( aff4::getDebugOutput(), "%s[%d] : Found Resource %s = %s\n", __FILE__, __LINE__, resource.c_str(),
			aff4::lexicon::getLexiconString(res->getBaseType()).c_str());
#endif
	return true;
}

/*
 * AFF4 Resource
 */

std::string AFF4ZipContainer::getResourceID() const noexcept {
	return AFF4Resource::getResourceID();
}

aff4::Lexicon AFF4ZipContainer::getBaseType() noexcept {
	return aff4::Lexicon::AFF4_ZIP_TYPE;
}

std::map<aff4::Lexicon, std::vector<aff4::rdf::RDFValue>> AFF4ZipContainer::getProperties() noexcept {
	return AFF4Resource::getProperties();
}

std::vector<aff4::rdf::RDFValue> AFF4ZipContainer::getProperty(aff4::Lexicon resource) noexcept {
	return AFF4Resource::getProperty(resource);
}

} /* namespace container */
} /* namespace aff4 */
