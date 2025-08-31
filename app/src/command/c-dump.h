#ifndef CDUMP_H
#define CDUMP_H

// Qx Includes
#include <qx/io/qx-ioopreport.h>

// Project Includes
#include "command.h"

class KTex;

class QX_ERROR_TYPE(CDumpError, "CDumpError", 1214)
{
    friend class CDump;
//-Class Enums-------------------------------------------------------------
public:
    enum Type
    {
        NoError,
        InvalidInput,
        CantReadTex,
        TexEmpty,
        CantMakeDir
    };

//-Class Variables-------------------------------------------------------------
private:
    static inline const QHash<Type, QString> ERR_STRINGS{
        {NoError, u""_s},
        {InvalidInput, u"The provided input TEX path is invalid."_s},
        {CantReadTex, u"Failed to read TEX."_s},
        {TexEmpty, u"The TEX contained no mip-maps."_s},
        {CantMakeDir, u"Failed to create output directory."_s}
    };

//-Instance Variables-------------------------------------------------------------
private:
    Type mType;
    QString mSpecific;
    QString mDetails;

//-Constructor-------------------------------------------------------------
private:
    CDumpError(Type t = NoError, const QString& s = {}, const QString& d = {});

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
    QString deriveDetails() const override;
};

class CDump : public Command
{
//-Class Variables------------------------------------------------------------------------------------------------------
private:
    // Messages
    static inline const QString MSG_READ_TEX = u"Reading TEX..."_s;
    static inline const QString MSG_TEX_INFO =  u"TEX Info:\n%1"_s;
    static inline const QString MSG_EXTRACT_IMAGE = u"Extracting primary TEX image..."_s;

    // Command line option strings
    static inline const QString CL_OPT_INPUT_S_NAME = u"i"_s;
    static inline const QString CL_OPT_INPUT_L_NAME = u"input"_s;
    static inline const QString CL_OPT_INPUT_DESC = u"TEX to dump."_s;

    static inline const QString CL_OPT_OUTPUT_S_NAME = u"o"_s;
    static inline const QString CL_OPT_OUTPUT_L_NAME = u"output"_s;
    static inline const QString CL_OPT_OUTPUT_DESC = u"Path to a directory for the resultant data. Defaults to the basename of the input."_s;

protected:
    // Messages
    static inline const QString MSG_INPUT_VALIDATION = u"Validating input..."_s;
    static inline const QString MSG_SUCCESS = u"Successfully dumped TEX"_s;

    // Output
    static inline const QString META_OUTPUT_TEMPLATE = u"%1-meta.json"_s;
    static inline const QString DATA_OUTPUT_TEMPLATE = u"%1-data.bin"_s;

    // Command line options
    static inline const QCommandLineOption CL_OPTION_INPUT{{CL_OPT_INPUT_S_NAME, CL_OPT_INPUT_L_NAME}, CL_OPT_INPUT_DESC, u"input"_s}; // Takes value
    static inline const QCommandLineOption CL_OPTION_OUTPUT{{CL_OPT_OUTPUT_S_NAME, CL_OPT_OUTPUT_L_NAME}, CL_OPT_OUTPUT_DESC, u"output"_s}; // Takes value
    static inline const QList<const QCommandLineOption*> CL_OPTIONS_SPECIFIC{&CL_OPTION_INPUT, &CL_OPTION_OUTPUT};
    static inline const QSet<const QCommandLineOption*> CL_OPTIONS_REQUIRED{&CL_OPTION_INPUT};

public:
    // Meta
    static inline const QString NAME = u"dump"_s;
    static inline const QString DESCRIPTION = u"Dumps all image data of a TEX without converting it."_s;

//-Constructor----------------------------------------------------------------------------------------------------------
public:
    CDump(Stex& coreRef);

//-Destructor----------------------------------------------------------------------------------------------------------
public:
    virtual ~CDump() = default;

//-Instance Functions------------------------------------------------------------------------------------------------------
private:
    QList<const QCommandLineOption*> options() const override;
    QSet<const QCommandLineOption*> requiredOptions() const override;
    QString name() const override;

    Qx::IoOpReport readTex(KTex& tex, const QString& path) const;

public:
    Qx::Error perform() override;
};
REGISTER_COMMAND(CDump::NAME, CDump, CDump::DESCRIPTION);

#endif // CDUMP_H
