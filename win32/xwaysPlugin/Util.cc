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

#include "Util.h"

// INTERNAL API.
#include "stream/MapStream.h"
#include "stream/struct/MapEntryPoint.h"

const static std::string aff4Prefix(AFF4_BASE_URI);

std::wstring s2ws(const std::string& str) {
	using convert_typeX = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_typeX, wchar_t> converterX;
	return converterX.from_bytes(str);
}

std::string ws2s(const std::wstring& wstr) {
	using convert_typeX = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_typeX, wchar_t> converterX;
	return converterX.to_bytes(wstr);
}

LPWSTR createText(const std::wstring& value) {
	size_t length = (value.size() + 2) * 2;
	LPWSTR pointer = (LPWSTR) ::malloc(length);
	ZeroMemory(pointer, length);
	value.copy(pointer, value.size());
	return pointer;
}

bool hasPrefix(std::string const &value, std::string const &prefix) {
	if (value.length() >= prefix.length()) {
		return (!value.compare(0, prefix.length(), prefix));
	}
	else {
		return false;
	}
}

std::string setAFF4Prefix(std::string value) {
	if (hasPrefix(value, aff4Prefix)) {
		value = "aff4:" + value.substr(aff4Prefix.length());
	}
	return value;
}

LPWSTR getMetaData(std::shared_ptr<aff4::IAFF4Image> image) {
	std::string metadata;
	metadata += PACKAGE_STRING CRLF;
	std::map<aff4::Lexicon, std::vector<aff4::rdf::RDFValue>> properties = image->getProperties();
	for (auto it = properties.begin(); it != properties.end(); it++) {
		aff4::Lexicon t = it->first;

		if (t != aff4::Lexicon::AFF4_TYPE) {
			// if our value is a resource, then don't output.
			std::vector<aff4::rdf::RDFValue> values = it->second;
			bool isResource = false;
			if (!values.empty()) {
				aff4::rdf::RDFValue v = values[0];
				isResource = (v.getXSDType() == aff4::rdf::XSDType::Resource);
			}
			if (!isResource) {
				metadata += setAFF4Prefix(aff4::lexicon::getLexiconString(t)) + ": ";
				bool first = true;
				for (aff4::rdf::RDFValue value : values) {
					if (!first) {
						metadata += ", ";
					}
					else {
						first = false;
					}
					metadata += setAFF4Prefix(value.toString());
				}
				metadata += CRLF;
			}
		}
	}

	return createText(s2ws(metadata));
}

bool isSpareResource(const std::string& resource) {
	std::string r = aff4::lexicon::getLexiconString(aff4::Lexicon::AFF4_IMAGESTREAM_UNREADABLE);
	if (r.compare(resource) == 0) {
		return true;
	}
	r = aff4::lexicon::getLexiconString(aff4::Lexicon::AFF4_IMAGESTREAM_UNKNOWN);
	if (r.compare(resource) == 0) {
		return true;
	}
	r = aff4::lexicon::getLexiconString(aff4::Lexicon::AFF4_IMAGESTREAM_ZERO);
	if (r.compare(resource) == 0) {
		return true;
	}
	r = aff4::lexicon::getLexiconString(aff4::Lexicon::AFF4_IMAGESTREAM_SYMBOLIC_PREFIX) + "00";
	if (r.compare(resource) == 0) {
		return true;
	}
	return false;
}

bool isSparse(std::shared_ptr<aff4::IAFF4Stream> stream, INT64 nOfs, INT64 nSize) {
	aff4::stream::MapStream* _map = static_cast<aff4::stream::MapStream*>(stream.get());
	std::vector<std::shared_ptr<aff4::IAFF4Stream>>* streams = _map->getStreams();
	std::map<uint64_t, aff4::stream::structs::MapEntryPoint>* map = _map->getMap();

	uint64_t leftToRead = nSize;
	uint64_t offset = nOfs;
	aff4::stream::structs::MapEntryPoint entry;

	while (leftToRead > 0) {
		// Get the map entry for the current offset;
		/*
		* map.lower_bound() will match either exact entry (offset = key), or get the next entry greater than offset.
		* if offset != key, we need the entry prior, hence the check of entry offset.
		*/
		auto mapIt = map->lower_bound(offset);
		if (mapIt != map->end()) {
			entry = mapIt->second;
			if (entry.offset > offset) {
				// We have gone 1 entry too far, so get the prior map entry.
				mapIt--;
				entry = mapIt->second;
			}
		}
		else {
			// past last, get last entry.
			auto endIt = map->rbegin();
			entry = endIt->second;
		}

		std::shared_ptr<aff4::IAFF4Stream> stream = (*streams)[entry.streamID];
		// All we determine if the stream is sparse based on the resource ID...
		std::string resource = stream->getResourceID();
		if (!isSpareResource(resource)) {
			return false;
		}
		// Advance to the next region in the map.
		uint64_t streadReadLength = std::min<uint64_t>(leftToRead, (entry.length - (offset - entry.offset)));
		offset += streadReadLength;
		leftToRead -= streadReadLength;
	}
	// If we get here we have all sparse regions.
	return true;
}