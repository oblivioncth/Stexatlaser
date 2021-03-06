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
        static const ErrorCode NO_INPUT = 2;
        static const ErrorCode NO_OUTPUT = 3;
        static const ErrorCode INVALID_INPUT = 4;
        static const ErrorCode INVALID_OUTPUT = 5;
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

    static inline const QString CL_OPT_FORMATS_S_NAME = "f";
    static inline const QString CL_OPT_FORMATS_L_NAME = "formats";
    static inline const QString CL_OPT_FORMATS_DESC = "Prints the image formats this tool supports.";

    // Global command line options
    static inline const QCommandLineOption CL_OPTION_HELP{{CL_OPT_HELP_S_NAME, CL_OPT_HELP_L_NAME, CL_OPT_HELP_E_NAME}, CL_OPT_HELP_DESC}; // Boolean option
    static inline const QCommandLineOption CL_OPTION_VERSION{{CL_OPT_VERSION_S_NAME, CL_OPT_VERSION_L_NAME}, CL_OPT_VERSION_DESC}; // Boolean option
    static inline const QCommandLineOption CL_OPTION_FORMATS{{CL_OPT_FORMATS_S_NAME, CL_OPT_FORMATS_L_NAME}, CL_OPT_FORMATS_DESC}; // Boolean option

    static inline const QList<const QCommandLineOption*> CL_OPTIONS_ALL{&CL_OPTION_HELP, &CL_OPTION_VERSION, &CL_OPTION_FORMATS};

    // Help template
    static inline const QString HELP_TEMPL = "Usage:\n"
                                             "------\n"
                                             VER_INTERNALNAME_STR " <global options> [command] <command options>\n"
                                             "\n"
                                             "Global Options:\n"
                                             "---------------%1\n"
                                             "\n"
                                             "Commands:\n"
                                             "---------%2\n"
                                             "Use the '-h' switch after a command to see it's specific usage notes\n";
    static inline const QString HELP_OPT_TEMPL = "\n%1: %2";
    static inline const QString HELP_COMMAND_TEMPL = "\n[%1]: %2\n";

    // Messages
    static inline const QString MSG_VERSION = VER_PRODUCTNAME_STR " version " VER_FILEVERSION_STR "\n";
    static inline const QString MSG_FORMATS = VER_INTERNALNAME_STR " supports the following image formats:\n%1\n";

    // Stream
    static inline QTextStream qcout = QTextStream(stdout, QIODevice::WriteOnly);

    // Meta
    static inline const QString NAME = "stex";

//-Instance Variables------------------------------------------------------------------------------------------------------
private:
    QStringList mImageFormats;
    QStringList mImageFormatFilter;


//-Constructor----------------------------------------------------------------------------------------------------------
public:
    Stex();

//-Instance Functions------------------------------------------------------------------------------------------------------
private:
    void showHelp();
    void showVersion();
    void showFormats();

public:
    ErrorCode initialize(QStringList& commandLine);

    QStringList imageFormatFilter() const;
    QStringList supportedImageFormats() const;

    void printError(QString src, Qx::GenericError error);
    void printMessage(QString src, QString message);
    void printVerbatim(QString text);
};


#endif // STEX_H
