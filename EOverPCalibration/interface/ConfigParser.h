// ConfigParser.h
//
// A class representing the configuration of the analysis program,
// gathered from the configuration file and the command line
//
// last change : $Date: 2012/02/12 20:14:50 $
// by          : $Author: rgerosa $
//

#ifndef CONFIGPARSER_H
#define CONFIGPARSER_H

#include "ConfigFileLine.h"
#include "TROOT.h"
#include "TObject.h"

#include "stdio.h"
#include <stdexcept>
#include <vector>

class ConfigParser
{
public:


	FILE *config_file;

	// Constructor which does basically nothing
	ConfigParser();

	// Initialize configuration using the command line arguments
	bool init(const char *fileName = 0);

	// Destructor: Deletes the entries in the list of configuration file lines
	virtual ~ConfigParser();

	// Append the option name of another ConfigFileLine as value
	void appendLine(ConfigFileLine *line);

	// Deletes a line from the already parsed configuration file lines
	bool deleteLine(const char *name, const char *scope = 0);

	// Override an option
	bool overrideOption(const char *name, std::list<std::string> &values,
	                    const char *scope = 0);

	// Is this option defined?
	bool isDefined(const char *name) const;

	// Read Integer Option
	int readIntOption(const char *name) const throw(const char *);

	// Read Double Option
	double readDoubleOption(const char *name) const throw(const char *);

	// Read Float Option
	float readFloatOption(const char *name) const throw(const char *);

	// Read bool Option
	const bool readBoolOption(const char *name) const throw(const char *);

	// Read Integer Option
	const char *readStringOption(const char *name) const throw(const char *);

	// Read Integer List Option
	std::vector<int> readIntListOption(const char *name) const throw(const char *);

	// Read Double List Option
	std::vector<double> readDoubleListOption(const char *name)
	const throw(const char *);

	// Read String List Option
	std::vector<std::string> readStringListOption(const char *name)
	const throw(const char *);

	// Print all options
	void print() const;

	//! stream operator
	friend std::ostream & operator<< (std::ostream & out, const ConfigParser & conf) ;

private:

	std::list<ConfigFileLine *> configLines; //!< A list of configuration file lines

	//! For convinience a ValueIterator type is defined which walks along
	//! the list of option strings
	typedef std::list<ConfigFileLine *>::const_iterator LineIterator;

};

// Do not forget to define the global variables so that rootcint
// produces the necessary stub
R__EXTERN ConfigParser *gConfigParser;

#endif // #ifndef CONFIGPARSER_H
