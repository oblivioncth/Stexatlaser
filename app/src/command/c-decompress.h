#ifndef CDECOMPRESS_H
#define CDECOMPRESS_H

// Qt Includes
#include <QFileInfo>

// Project Includes
#include "untex-command.h"

class QX_ERROR_TYPE(CDecompressError, "CDecompressError", 1214)
{
    friend class CDecompress;
//-Class Enums-------------------------------------------------------------
public:
    enum Type
    {
        NoError,
        InvalidInput,
        CantReadTex,
        TexUnsupported
    };

//-Class Variables-------------------------------------------------------------
private:
    static inline const QHash<Type, QString> ERR_STRINGS{
        {NoError, u""_s},
        {InvalidInput, u"The provided input TEX path is invalid."_s},
        {CantReadTex, u"Failed to read TEX."_s},
        {TexUnsupported, u"The input TEX is not supported."_s},
    };

//-Instance Variables-------------------------------------------------------------
private:
    Type mType;
    QString mSpecific;
    QString mDetails;

//-Constructor-------------------------------------------------------------
private:
    CDecompressError(Type t = NoError, const QString& s = {}, const QString& d = {});

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

class CDecompress : public UntexCommand
{
//-Class Variables------------------------------------------------------------------------------------------------------
private:
    // Messages
    static inline const QString MSG_SUCCESS = u"Successfully decompressed TEX"_s;

    // Command line option strings
    static inline const QString CL_OPT_INPUT_S_NAME = u"i"_s;
    static inline const QString CL_OPT_INPUT_L_NAME = u"input"_s;
    static inline const QString CL_OPT_INPUT_DESC = u"TEX to decompress."_s;

    static inline const QString CL_OPT_OUTPUT_S_NAME = u"o"_s;
    static inline const QString CL_OPT_OUTPUT_L_NAME = u"output"_s;
    static inline const QString CL_OPT_OUTPUT_DESC = u"Path to the resultant image. Defaults to input with PNG extension."_s;

    // Command line options
    static inline const QCommandLineOption CL_OPTION_INPUT{{CL_OPT_INPUT_S_NAME, CL_OPT_INPUT_L_NAME}, CL_OPT_INPUT_DESC, u"input"_s}; // Takes value
    static inline const QCommandLineOption CL_OPTION_OUTPUT{{CL_OPT_OUTPUT_S_NAME, CL_OPT_OUTPUT_L_NAME}, CL_OPT_OUTPUT_DESC, u"output"_s}; // Takes value
    static inline const QList<const QCommandLineOption*> CL_OPTIONS_SPECIFIC{&CL_OPTION_INPUT, &CL_OPTION_OUTPUT};
    static inline const QSet<const QCommandLineOption*> CL_OPTIONS_REQUIRED{&CL_OPTION_INPUT};

public:
    // Meta
    static inline const QString NAME = u"decompress"_s;
    static inline const QString DESCRIPTION = u"Converts a single TEX to a PNG image."_s;

//-Constructor----------------------------------------------------------------------------------------------------------
public:
    CDecompress(Stex& coreRef);

//-Instance Functions------------------------------------------------------------------------------------------------------
protected:
    QList<const QCommandLineOption*> options() const override;
    QSet<const QCommandLineOption*> requiredOptions() const override;
    QString name() const override;

public:
    Qx::Error perform() override;
};
REGISTER_COMMAND(CDecompress::NAME, CDecompress, CDecompress::DESCRIPTION);

#endif // CDecompress_H
