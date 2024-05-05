#ifndef PANO_FACE_PYRAMID_COMMAND_H
#define PANO_FACE_PYRAMID_COMMAND_H

#include "Command.h"

namespace PanoProjector {

class PyramidCommand : public Command {
public:
	virtual std::string getName() override;
	virtual std::string getDescription() override;
protected:
	virtual void initOptions() override;
	virtual std::string getSynopsis() override;
	virtual int doRun() override;

};

} // namespace
#endif
