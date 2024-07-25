#ifndef FACE_DIAGRAM_COMMAND_H
#define FACE_DIAGRAM_COMMAND_H

#include "Command.h"

namespace PanoProjector {

class FaceDiagramCommand : public Command {
public:
	std::string getName() override;
	std::string getDescription() override;

protected:
	void initOptions() override;
	std::string getSynopsis() override;
	int doRun() override;

private:
	static void makeFaceDiagramNumerical(const std::string & outputPath, int width);
	static void makeFaceDiagramAnalytic(const std::string & outputPath, int width);
};

} // namespace
#endif
