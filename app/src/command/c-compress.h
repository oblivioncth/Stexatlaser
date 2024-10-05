#ifndef CCOMPRESS_H
#define CCOMPRESS_H

// Qt Includes
#include <QFileInfo>

// Project Includes
#include "tex-command.h"

class QX_ERROR_TYPE(CCompressError, "CCompressError", 1213)
{
    friend class CCompress;
//-Class Enums-------------------------------------------------------------
public:
    enum Type
    {
        NoError,
        InvalidInput,
        CantWriteTex
    };

//-Class Variables-------------------------------------------------------------
private:
    static inline const QHash<Type, QString> ERR_STRINGS{
        {NoError, u""_s},
        {InvalidInput, u"The provided input image path is invalid."_s},
        {CantWriteTex, u"Failed to write output TEX."_s}
    };

//-Instance Variables-------------------------------------------------------------
private:
    Type mType;
    QString mSpecific;
    QString mDetails;

//-Constructor-------------------------------------------------------------
private:
    CCompressError(Type t = NoError, const QString& s = {}, const QString& d = {});

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

class CCompress : public TexCommand
{
//-Class Variables------------------------------------------------------------------------------------------------------
private:
    // Messages
    static inline const QString MSG_SUCCESS = u"Successfully compressed image"_s;

    // Command line option strings
    static inline const QString CL_OPT_INPUT_S_NAME = u"i"_s;
    static inline const QString CL_OPT_INPUT_L_NAME = u"input"_s;
    static inline const QString CL_OPT_INPUT_DESC = u"Image to compress."_s;

    static inline const QString CL_OPT_OUTPUT_S_NAME = u"o"_s;
    static inline const QString CL_OPT_OUTPUT_L_NAME = u"output"_s;
    static inline const QString CL_OPT_OUTPUT_DESC = u"Path to the resultant TEX. Defaults to input basename with TEX extension."_s;

    // Command line options
    static inline const QCommandLineOption CL_OPTION_INPUT{{CL_OPT_INPUT_S_NAME, CL_OPT_INPUT_L_NAME}, CL_OPT_INPUT_DESC, u"input"_s}; // Takes value
    static inline const QCommandLineOption CL_OPTION_OUTPUT{{CL_OPT_OUTPUT_S_NAME, CL_OPT_OUTPUT_L_NAME}, CL_OPT_OUTPUT_DESC, u"output"_s}; // Takes value
    static inline const QList<const QCommandLineOption*> CL_OPTIONS_SPECIFIC{&CL_OPTION_INPUT, &CL_OPTION_OUTPUT};
    static inline const QSet<const QCommandLineOption*> CL_OPTIONS_REQUIRED{&CL_OPTION_INPUT};

public:
    // Meta
    static inline const QString NAME = u"compress"_s;
    static inline const QString DESCRIPTION = u"Converts a single image to TEX. No key is created."_s;

//-Constructor----------------------------------------------------------------------------------------------------------
public:
    CCompress(Stex& coreRef);

//-Instance Functions------------------------------------------------------------------------------------------------------
protected:
    QList<const QCommandLineOption*> options() const override;
    QSet<const QCommandLineOption*> requiredOptions() const override;
    QString name() const override;

public:
    Qx::Error perform() override;
};
REGISTER_COMMAND(CCompress::NAME, CCompress, CCompress::DESCRIPTION);

#endif // CCOMPRESS_H
