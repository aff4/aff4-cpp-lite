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

#include "RDFValue.h"
#include <time.h>

namespace aff4 {

namespace rdf {

#define TIMESTAMP_STRING_BUFFER 1024

/*
 * Define all mappings.
 */
/**
 * XSD Resource type to enum mappings
 */
static const std::map<std::string, XSDType> xsdMappings = { //
		{ (AFF4_XSD_PREFIX "string"), XSDType::String }, //
				{ (AFF4_XSD_PREFIX "int"), XSDType::Int }, //
				{ (AFF4_XSD_PREFIX "integer"), XSDType::Int }, //
				{ (AFF4_XSD_PREFIX "long"), XSDType::Long }, //
				{ (AFF4_XSD_PREFIX "dateTime"), XSDType::XSDDateTime }, //
				{ (AFF4_XSD_PREFIX "boolean"), XSDType::Boolean }, //
				{ (AFF4_XSD_PREFIX "float"), XSDType::Float }, //
		};

/**
 * AFF4:hash XSD datatypes to aff4::Lexicon mappings.
 */
static const std::map<std::string, aff4::Lexicon> aff4Mappings = { //
		{ (AFF4_BASE_URI "MD5"), aff4::Lexicon::AFF4_DIGEST_MD5 }, //
				{ (AFF4_BASE_URI "SHA1"), aff4::Lexicon::AFF4_DIGEST_SHA1 }, //
				{ (AFF4_BASE_URI "SHA256"), aff4::Lexicon::AFF4_DIGEST_SHA256 }, //
				{ (AFF4_BASE_URI "SHA512"), aff4::Lexicon::AFF4_DIGEST_SHA512 }, //
				{ (AFF4_BASE_URI "BLAKE2B"), aff4::Lexicon::AFF4_DIGEST_BLAKE2B }, // Evimetry Legacy
				{ (AFF4_BASE_URI "Blake2b"), aff4::Lexicon::AFF4_DIGEST_BLAKE2B }, // Evimetry Legacy
				{ (AFF4_BASE_URI "blake2b"), aff4::Lexicon::AFF4_DIGEST_BLAKE2B }, // AFF4 v1.0 std
				{ (AFF4_BASE_URI "blockMapHashSHA512"), aff4::Lexicon::AFF4_DIGEST_BLOCK_MAP_HASH_SHA512 }, //
		};

XSDType getType(const std::string& type) {
#if DEBUG
	fprintf( aff4::getDebugOutput(), "%s[%d] : Determine XSD type: %s\n", __FILE__, __LINE__, type.c_str());
#endif
	if (type.empty()) {
		return XSDType::String;
	}
	auto it = xsdMappings.find(type);
	if (it != xsdMappings.end()) {
		return it->second;
	}
	// Default as typed literal?
	return XSDType::Literal;
}

aff4::Lexicon getAFF4Type(const std::string& type) {
#if DEBUG
	fprintf( aff4::getDebugOutput(), "%s[%d] : Determine AFF4 Data type: %s\n", __FILE__, __LINE__, type.c_str());
#endif
	if (type.empty()) {
		return aff4::Lexicon::UNKNOWN;
	}
	auto it = aff4Mappings.find(type);
	if (it != aff4Mappings.end()) {
		return it->second;
	}
#if DEBUG
	fprintf( aff4::getDebugOutput(), "%s[%d] : Unknown AFF4 type? %s\n", __FILE__, __LINE__, type.c_str());
#endif
	// Default as unknown.
	return aff4::Lexicon::UNKNOWN;
}

/*
 * The following is from:
 * https://stackoverflow.com/questions/530519/stdmktime-and-timezone-info
 * and
 * http://www.catb.org/esr/time-programming/
 */

/**
 * Convert the tm struct to time_t, ignoring local timezone.
 * @param t The tm struct.
 * @return time_t for UTC.
 */
time_t my_timegm(register struct tm * t)
/* struct tm to seconds since Unix epoch */
{
	long year;
	time_t result;
#define MONTHSPERYEAR   12      /* months per calendar year */
	static const int cumdays[MONTHSPERYEAR] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };

	/*@ +matchanyintegral @*/
	year = 1900 + t->tm_year + t->tm_mon / MONTHSPERYEAR;
	result = (year - 1970) * 365 + cumdays[t->tm_mon % MONTHSPERYEAR];
	result += (year - 1968) / 4;
	result -= (year - 1900) / 100;
	result += (year - 1600) / 400;
	if ((year % 4) == 0 && ((year % 100) != 0 || (year % 400) == 0) && (t->tm_mon % MONTHSPERYEAR) < 2)
		result--;
	result += t->tm_mday - 1;
	result *= 24;
	result += t->tm_hour;
	result *= 60;
	result += t->tm_min;
	result *= 60;
	result += t->tm_sec;
	if (t->tm_isdst == 1)
		result -= 3600;
	/*@ -matchanyintegral @*/
	return (result);
}

std::chrono::system_clock::time_point getTime(const std::string& value) {
#if DEBUG
	fprintf( aff4::getDebugOutput(), "%s[%d] : Convert XSDDateTime: %s\n", __FILE__, __LINE__, value.c_str());
#endif
	// The time format should be RFC3339
	std::tm timeinfo;
	std::time_t time_epoch;
	::memset(&timeinfo, 0, sizeof(struct tm));
	std::istringstream ss(value);
	ss >> std::get_time(&timeinfo, "%Y-%m-%dT%T%z");
	if (ss.fail()) {
		ss = std::istringstream(value);
		ss >> std::get_time(&timeinfo, "%Y-%m-%dT%T%Ez%z");
	}
	time_epoch = my_timegm(&timeinfo);
	return std::chrono::system_clock::from_time_t(time_epoch);
}

std::string RDFValue::toString() noexcept {

	switch (xsdType) {
	case UNKNOWN:
		break;
	case String:
		return value_literal;
	case Boolean:
		return (value_bool == true) ? "true" : "false";
	case Float:
		return std::to_string(value_float);
	case Int:
		return std::to_string(value_int);
	case Long:
		return std::to_string(value_long);
	case Literal:
		return value_literal + "^^" + aff4::lexicon::getLexiconString(rdfType);
	case Resource:
		if (!value_literal.empty()) {
			return value_literal;
		} else {
			return aff4::lexicon::getLexiconString(rdfType);
		}
	case XSDDateTime:
		std::time_t now_c = std::chrono::system_clock::to_time_t(timestamp);
#ifdef _WIN32
		std::tm now_tm;
		gmtime_s(&now_tm, &now_c);
#else
		std::tm now_tm = *gmtime(&now_c);
#endif
		std::unique_ptr<char[]> buffer(new char[TIMESTAMP_STRING_BUFFER]);
		if (buffer == nullptr) {
			break;
		}
		// The time format should be RFC3339
		size_t res = std::strftime(buffer.get(), TIMESTAMP_STRING_BUFFER, "%FT%TZ", &now_tm);
		std::string timestamp(buffer.get(), res);
		return timestamp;

	}
// ???
	return value_literal;
}

} /* namespace rdf */
} /* namespace aff4 */
