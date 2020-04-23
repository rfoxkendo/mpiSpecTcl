#ifndef VIEWER_CMDLINEOPTIONS_H
#define VIEWER_CMDLINEOPTIONS_H

// generated by gengetopt
#include "cmdline.h"

#include <QString>

#include <vector>
#include <string>

namespace Viewer {

class CmdLineOptions
{
private:
    std::vector<std::string>  m_args;
    gengetopt_args_info       m_info;

public:
    CmdLineOptions();

    void parse(int argc, char* argv[]);

    int getPort();

    QString getHost();

    /*! \returns whether the user specified the --disable-truetype or -t options */
    bool disableTrueTypeFonts() const;
};

} // namespace Viewer

#endif // VIEWER_CMDLINEOPTIONS_H