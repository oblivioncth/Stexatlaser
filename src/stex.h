#ifndef STEX_H
#define STEX_H

#include <QString>
#include <QSet>
#include <QTextStream>
#include <QIODevice>
#include <QCommandLineOption>

#include "qx.h"

#include "version.h"


//-Typedef---------------------------------------------------------------------
typedef int ErrorCode;

class Stex
{
//-Inner Classes--------------------------------------------------------------------------------------------------------
public:
    class ErrorCodes
    {
    //-Class Variables--------------------------------------------------------------------------------------------------
    public:
        static const ErrorCode NO_ERR = 0;
        static const ErrorCode INVALID_ARGS = 1;
    };

//-Class Variables------------------------------------------------------------------------------------------------------
public:
    // Error Messages
    static inline const QString ERR_INVALID_PARAM = "Invalid arguments. Use " VER_INTERNALNAME_STR " -h for help";

    // Global command line option strings
    static inline const QString CL_OPT_HELP_S_NAME = "h";
    static inline const QString CL_OPT_HELP_L_NAME = "help";
    static inline const QString CL_OPT_HELP_E_NAME = "?";
    static inline const QString CL_OPT_HELP_DESC = "Prints this help message.";

    static inline const QString CL_OPT_VERSION_S_NAME = "v";
    static inline const QString CL_OPT_VERSION_L_NAME = "version";
    static inline const QString CL_OPT_VERSION_DESC = "Prints the current version of this tool.";

    // Global command line options
    static inline const QCommandLineOption CL_OPTION_HELP{{CL_OPT_HELP_S_NAME, CL_OPT_HELP_L_NAME, CL_OPT_HELP_E_NAME}, CL_OPT_HELP_DESC}; // Boolean option
    static inline const QCommandLineOption CL_OPTION_VERSION{{CL_OPT_VERSION_S_NAME, CL_OPT_VERSION_L_NAME}, CL_OPT_VERSION_DESC}; // Boolean option

    static inline const QList<const QCommandLineOption*> CL_OPTIONS_ALL{&CL_OPTION_HELP, &CL_OPTION_VERSION};

    // Help template
    static inline const QString HELP_TEMPL = "<u>Usage:</u><br>"
                                             VER_INTERNALNAME_STR "&lt;global options&gt; <i>command</i> &lt;command options&gt;<br>"
                                             "<br>"
                                             "<u>Global Options:</u>%1<br>"
                                             "<br>"
                                             "<u>Commands:</u>%2<br>"
                                             "<br>"
                                             "Use the <b>-h</b> switch after a command to it's specific usage notes";
    static inline const QString HELP_OPT_TEMPL = "<br><b>%1:</b> &nbsp;%2";
    static inline const QString HELP_COMMAND_TEMPL = "<br><b>%1:</b> &nbsp;%2";

    // Command line messages
    static inline const QString CL_VERSION_MESSAGE = VER_PRODUCTNAME_STR " version " VER_FILEVERSION_STR;

    // Stream
    static inline QTextStream qcout = QTextStream(stdout, QIODevice::WriteOnly);

    // Meta
    static inline const QString NAME = "core";

//-Instance Variables------------------------------------------------------------------------------------------------------
private:

//-Constructor----------------------------------------------------------------------------------------------------------
public:
    Stex();

//-Instance Functions------------------------------------------------------------------------------------------------------
private:
    void showHelp();
    void showVersion();

public:
    ErrorCode initialize(QStringList& commandLine);

    void printError(QString src, Qx::GenericError error);
    void printMessage(QString src, QString message);
};


#endif // STEX_H
