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

#include "Model.h"

namespace aff4 {
namespace rdf {

/**
 * Global static statement handler
 * @param user_data The pointer to the Model object
 * @param statement The provided statement
 */
static void statement_Handler(void* user_data, raptor_statement* statement) {
	Model* model = reinterpret_cast<Model*>(user_data);
	model->statementHandler(statement);
}

Model::Model() :
		world(nullptr), parser(nullptr) {
	world = raptor_new_world();
	parser = raptor_new_parser(world, "turtle");

	/*
	 * Initialise raptor.
	 */
	raptor_parser_set_statement_handler(parser, this, statement_Handler);

	// Dont talk to the internet
	raptor_parser_set_option(parser, RAPTOR_OPTION_NO_NET, nullptr, 1);
	raptor_parser_set_option(parser, RAPTOR_OPTION_ALLOW_RDF_TYPE_RDF_LIST, nullptr, 1);

	raptor_uri* uri = raptor_new_uri(world, (const unsigned char*) ".");
	raptor_parser_parse_start(parser, uri);
	raptor_free_uri(uri);

}

Model::~Model() {
	if (parser != nullptr) {
		raptor_free_parser(parser);
	}
	if (world != nullptr)
		raptor_free_world(world);
}

int Model::parse(unsigned char* buffer, uint64_t size) {
	return raptor_parser_parse_chunk(parser, buffer, (size_t) size, 1);
}

std::map<aff4::Lexicon, std::vector<RDFValue>> Model::getObjectInformation(const std::string& resource) {
	auto obj = model.find(resource);
	if (obj != model.end()) {
		return obj->second;
	}
	std::map<aff4::Lexicon, std::vector<RDFValue>> empty;
	return empty;
}

LIBAFF4_API_LOCAL std::vector<std::string> Model::getResourcesOfType(aff4::Lexicon type) {
	std::vector<std::string> results;
	for (auto it = model.begin(); it != model.end(); it++) {
		std::map<aff4::Lexicon, std::vector<aff4::rdf::RDFValue>> properties = it->second;
		for (auto prop = properties.begin(); prop != properties.end(); prop++) {
			if (prop->first == aff4::Lexicon::AFF4_TYPE) {
				std::vector<RDFValue> values = prop->second;
				for (RDFValue v : values) {
					if (v.getType() == type) {
						results.push_back(it->first);
						break;
					}
				}
			}
		}
	}
	return results;
}

std::unique_ptr<aff4::rdf::RDFValue> Model::getValueFromRaptorTerm(aff4::Lexicon property, raptor_term* term) noexcept {
	if (term->type == RAPTOR_TERM_TYPE_URI) {
		// Need to use URI to string, as internals of URI are hidden...
		char* uri = reinterpret_cast<char*>(raptor_uri_to_string(term->value.uri));
		std::string uriStr(uri);
		raptor_free_memory(uri);
		// see if the URI maps to an AFF4 property.
		aff4::Lexicon p = aff4::lexicon::getLexicon(uriStr);
		if (p != aff4::Lexicon::UNKNOWN) {
			return std::unique_ptr<RDFValue>(new RDFValue(p));
		} else {
			return std::unique_ptr<RDFValue>(new RDFValue(XSDType::Resource, property, uriStr));
		}
	}
	if (term->type == RAPTOR_TERM_TYPE_LITERAL) {
		// Does it have a special data type?
		if (term->value.literal.datatype) {
			char* uri = reinterpret_cast<char*>(raptor_uri_to_string(term->value.literal.datatype));
			std::string uriStr(uri);
			XSDType type = getType(uriStr);
			raptor_free_memory(uri);
			if (type == XSDType::UNKNOWN) {
				return nullptr;
			}
			// Raptor gives us everything as strings...
			std::string value_string(reinterpret_cast<char*>(term->value.literal.string),
					term->value.literal.string_len);
			try {
				switch (type) {
				case String:
					return std::unique_ptr<RDFValue>(new RDFValue(value_string));
				case Int:
					try {
						return std::unique_ptr<RDFValue>(new RDFValue((int32_t)std::stoi(value_string)));
					} catch (...){
						// expected overflow.
					}
					return std::unique_ptr<RDFValue>(new RDFValue((int64_t)std::stoll(value_string)));
				case Long:
					// Use std::stoll() for conversion, as this should be 64bit Long on all platforms.
					return std::unique_ptr<RDFValue>(new RDFValue((int64_t)std::stoll(value_string)));
				case Float:
					return std::unique_ptr<RDFValue>(new RDFValue(std::stof(value_string)));
				case Literal:
					return std::unique_ptr<RDFValue>(new RDFValue(getAFF4Type(uriStr), value_string));
				case Resource:
					return std::unique_ptr<RDFValue>(new RDFValue(XSDType::Resource, property, value_string));
				case Boolean:
					return std::unique_ptr<RDFValue>(new RDFValue(!value_string.compare("true")));
				case XSDDateTime:
					return std::unique_ptr<RDFValue>(new RDFValue(getTime(value_string)));
					break;
				case UNKNOWN:
					break;
				}
			} catch (...) {
#if DEBUG
				fprintf( aff4::getDebugOutput(), "%s[%d] : RDFValue construction failed? %d : %s\n", __FILE__, __LINE__, type,
						value_string.c_str());
#endif
			}
			return nullptr;

		} else {
			// Basic String type - nothing special needed.
			std::string value_string(reinterpret_cast<char*>(term->value.literal.string),
					term->value.literal.string_len);

			return std::unique_ptr<RDFValue>(new RDFValue(value_string));
		}
	}
	return nullptr;
}

void Model::statementHandler(raptor_statement* statement) {

	if (statement->subject->type == RAPTOR_TERM_TYPE_URI && statement->predicate->type == RAPTOR_TERM_TYPE_URI) {
		char* subject = reinterpret_cast<char*>(raptor_uri_to_string(statement->subject->value.uri));
		char* predicate = reinterpret_cast<char*>(raptor_uri_to_string(statement->predicate->value.uri));
		std::string subjectURN(subject);
		std::string propertryURN(predicate);
		aff4::Lexicon property = aff4::lexicon::getLexicon(propertryURN);
		if (property != aff4::Lexicon::UNKNOWN) {
			// Get the value.
			std::unique_ptr<RDFValue> v = getValueFromRaptorTerm(property, statement->object);
			if (v != nullptr) {
#if DEBUG
				raptor_statement_print_as_ntriples(statement, aff4::getDebugOutput());
				fprintf(aff4::getDebugOutput(), "\n%s[%d] :%s : %s : %s\n", __FILE__, __LINE__, subjectURN.c_str(),
						propertryURN.c_str(), v->toString().c_str());
#endif
				// Add into the map.
				std::map<aff4::Lexicon, std::vector<RDFValue>> properties = getObjectInformation(subjectURN);
				std::vector<RDFValue> values;
				auto it = properties.find(property);
				if (it != properties.end()) {
					values = it->second;
				}
				values.push_back(*v);
				properties[property] = values;
				model[subjectURN] = properties;
			}
		}
		raptor_free_memory(subject);
		raptor_free_memory(predicate);
	}
}

} /* namespace rdf */
} /* namespace aff4 */
