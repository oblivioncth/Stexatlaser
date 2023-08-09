#ifndef STEX_H
#define STEX_H

// Qt Includes
#include <QString>
#include <QSet>
#include <QTextStream>
#include <QIODevice>
#include <QCommandLineOption>

// Qx Includes
#include <qx/core/qx-error.h>

// Project Includes
#include "project_vars.h"

// General Aliases
using ErrorCode = quint32;

class QX_ERROR_TYPE(StexError, "CoreError", 1200)
{
    friend class Stex;
//-Class Enums-------------------------------------------------------------
public:
    enum Type
    {
        NoError = 0,
        InvalidOptions = 1
    };

//-Class Variables-------------------------------------------------------------
private:
    static inline const QHash<Type, QString> ERR_STRINGS{
        {NoError, u""_s},
        {InvalidOptions, u"Invalid global options provided."_s}
    };

//-Instance Variables-------------------------------------------------------------
private:
    Type mType;
    QString mSpecific;
    Qx::Severity mSeverity;

//-Constructor-------------------------------------------------------------
private:
    StexError(Type t = NoError, const QString& s = {}, Qx::Severity sv = Qx::Critical);

//-Instance Functions-------------------------------------------------------------
public:
    bool isValid() const;
    Type type() const;
    QString specific() const;

private:
    Qx::Severity deriveSeverity() const override;
    quint32 deriveValue() const override;
    QString derivePrimary() const override;
    QString deriveSecondary() const override;
};

class Stex
{
//-Class Variables------------------------------------------------------------------------------------------------------
private:
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
                                             PROJECT_SHORT_NAME " <global options> [command] <command options>\n"
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
    static inline const QString MSG_VERSION = PROJECT_APP_NAME " version " PROJECT_VERSION_STR "\n";
    static inline const QString MSG_FORMATS = PROJECT_SHORT_NAME " supports the following image formats:\n%1\n";

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
    Qx::Error initialize(QStringList& commandLine);

    QStringList imageFormatFilter() const;
    QStringList supportedImageFormats() const;

    void printError(QString src, Qx::Error error);
    void printMessage(QString src, QString message);
    void printVerbatim(QString text);
};


#endif // STEX_H
