#ifndef PANO_COMMAND_H
#define PANO_COMMAND_H

#include <boost/program_options.hpp>
#include "InputImageFactory.h"

namespace PanoProjector {

namespace po = boost::program_options;

/**
 * Base class for CLI commands
 */
class Command {
public:
	Command();

	/**
	 * Run the command. argv[0] will be the name of the command, and argv[argc-1]
	 * will be the last argument.
	 */
	int run(int argc, char **argv);

	/**
	 * Write a help message to stderr
	 */
	void usage();

	/**
	 * Get the name of the command
	 */
	virtual std::string getName() = 0;

	/**
	 * Get a long description of what the command does
	 */
	virtual std::string getDescription() = 0;
protected:
	/**
	 * The subclass should override this to populate m_visible, m_invisible and m_pos.
	 */
	virtual void initOptions() = 0;

	/**
	 * Get short example usage to go at the top of the usage output
	 */
	virtual std::string getSynopsis() = 0;

	/**
	 * Perform the action, after options have been initialized.
	 */
	virtual int doRun() = 0;

	/**
	 * Set the memory limit from the mem-limit command line option
	 */
	void setMemoryLimit();

	po::options_description m_visible;
	po::options_description m_invisible;
	po::positional_options_description m_pos;
	po::variables_map m_options;
	bool m_optionsDone;
};

} // namespace
#endif
