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
 * @file Model.h
 * @author Schatz Forensic, Ptd Ltd.
 * @version 1.0
 * @date 12-Sep-2017
 * @copyright Copyright Schatz Forensic, Ptd Ltd. 2017. All Rights Reserved. This project is released under the LGPL 3.0+.
 *
 * @brief A generic RDF Model interface for Raptor2
 */

#ifndef SRC_RDF_MODEL_H_
#define SRC_RDF_MODEL_H_

#include "aff4config.h"
#include "aff4.h"

#ifdef _WIN32
#include <raptor2.h>
#else 
#include <raptor2/raptor2.h>
#endif

namespace aff4 {
namespace rdf {

/**
 * @brief RDF Model implementation with basic resource lookup functionality.
 */
class Model {
public:
	/**
	 * Create a new model instance
	 */
	LIBAFF4_API_LOCAL Model();
	/**
	 * Parse the buffer into the model.
	 * <p>
	 * To flush any held elements, call with nullptr, 0.
	 * @param buffer The buffer
	 * @param size The size of the buffer.
	 * @return Non-zero on error.
	 */
	LIBAFF4_API_LOCAL int parse(unsigned char* buffer, uint64_t size);
	virtual ~Model();

	/**
	 * Get a vector of all resources that have rdf:type
	 * @param type The object type to enquire
	 * @return A vector of all resources that have the given rdf:type.
	 */
	LIBAFF4_API_LOCAL std::vector<std::string> getResourcesOfType(aff4::Lexicon type);

	/**
	 * Get the object properties for the given object
	 * @param resource The resource URI
	 * @return A map of elements. (empty map for unknown object).
	 */
	LIBAFF4_API_LOCAL std::map<aff4::Lexicon, std::vector<RDFValue>> getObjectInformation(const std::string& resource);

	/**
	 * Raptor 2 statement handler.
	 * <p>
	 * This will be called, when data is parsed in via parse(). DO NOT CALL DIRECTLY.
	 * @param statement The raptor statement.
	 */
	LIBAFF4_API_LOCAL void statementHandler(raptor_statement* statement);

private:
	/**
	 * Raptor world
	 */
	raptor_world* world;
	/**
	 * Base parser.
	 */
	raptor_parser* parser;

	/**
	 * Object model.
	 */
	std::map<std::string, std::map<aff4::Lexicon, std::vector<RDFValue>>> model;

	/**
	 * Convert the raptor term into a RDFValue.
	 * @param property The RDF property.
	 * @param term The raptor term
	 * @return The RDF Value or nullptr if unable to convert.
	 */
	std::unique_ptr<aff4::rdf::RDFValue> getValueFromRaptorTerm(aff4::Lexicon property, raptor_term* term) noexcept;

};

} /* namespace rdf */
} /* namespace aff4 */

#endif /* SRC_RDF_MODEL_H_ */
