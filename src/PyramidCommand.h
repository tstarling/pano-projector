#ifndef PANO_FACE_PYRAMID_COMMAND_H
#define PANO_FACE_PYRAMID_COMMAND_H

#include "Command.h"

namespace PanoProjector {

class PyramidCommand : public Command {
public:
	std::string getName() override;
	std::string getDescription() override;
protected:
	void initOptions() override;
	std::string getSynopsis() override;
	int doRun() override;

};

} // namespace
#endif
