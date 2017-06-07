//
// File ConfigFileLine.C
//
/*! \class ConfigFileLine
 * \brief A class representing a (continued) line in the configuration
 * file, that is a pair of an option name and a (list of) value(s)
 *
 *
 * $Date: 2012/02/12 20:16:10 $
 * $Author: rgerosa $
 *
 * The class ConfigFileLine is used by the config_parser during
 * parsing of the analyis configuration file. Every parsed line is put
 * into a ConfigFile line object. Lines may be continued by the \
 * line continuation character. The name of the configuration option
 * is stored as a string in the "values" class member, while options
 * contains a list<string> of configuration options. The latter field
 * might also be empty if the option is boolean. As soon as the name
 * field contains a non-zero string, the respective option is defined.
 *
 * The class is used during the parsing proceedure itself and must
 * therefore also be able to contain the data during the parsing
 * step. This includes ConfigFileLine objects which have no option
 * name because they are on the right hand side of the '=' assignment
 * operator.
 *
 * This class is not usable by root itself, no dictionary is generated
 * for it.
*/

#include "Calibration/EOverPCalibration/interface/ConfigFileLine.h"
#include <iostream>

using namespace std;

ConfigFileLine::ConfigFileLine(const string &o, const string &v)
{
	option = o;
	if( v.length())
		values.push_back(v);
}


ConfigFileLine::~ConfigFileLine()
{
	// The "option" string and the list are "self deleting"
	// we do not need to do anything therefore, here.
}

void ConfigFileLine::append(const ConfigFileLine &other)
{
	values.push_back(other.getOptionName());
}

void ConfigFileLine::appendList(const ConfigFileLine &other)
{
	ValueIterator I = other.values.begin();
	while (I != other.values.end()) {
		values.push_back(*I);
		I++;
	}
}

void ConfigFileLine::setScope(const string &scope)
{
	string s = scope;
	s.append("::");
	s.append(option);
	option = s;
}

void ConfigFileLine::print() const
{
	std::cout << "Option '" << option << "' ";
	if (!values.empty() ) {
		ValueIterator I = values.begin();
		std::cout << "set to '";
		while (I != values.end()) {
			std::cout << *I ;
			I++;
			if(I != values.end()) std::cout << ", ";
		}
		std::cout << "'" << std::endl;
	} else {
		std::cout << "is defined!" << std::endl;
	}
}


//! stream operator
std::ostream & operator<< (std::ostream & out, const ConfigFileLine & line)
{
	out << "Option '" << line.option << "' ";
	if (!line.values.empty() ) {
//      line.ValueIterator I=line.values.begin();
		std::list<std::string>::const_iterator I = line.values.begin();
		out << "set to '";
		while (I != line.values.end()) {
			out << *I ;
			I++;
			if(I != line.values.end()) out << ", ";
		}
		out << "'" << std::endl;
	} else {
		out << "is defined!" << std::endl;
	}
	return out ;
}


