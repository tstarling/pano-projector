#ifndef FACE_DIAGRAM_COMMAND_H
#define FACE_DIAGRAM_COMMAND_H

#include "Command.h"

namespace PanoProjector {

class FaceDiagramCommand : public Command {
public:
	virtual std::string getName() override;
	virtual std::string getDescription() override;

protected:
	virtual void initOptions() override;
	virtual std::string getSynopsis() override;
	virtual int doRun() override;

private:
	void makeFaceDiagramNumerical(std::string outputPath, int width);
	void makeFaceDiagramAnalytic(std::string outputPath, int width);
};

} // namespace
#endif
