#ifndef CUNPACK_H
#define CUNPACK_H

// Project Includes
#include "untex-command.h"

class QX_ERROR_TYPE(CUnpackError, "CUnpackError", 1216)
{
    friend class CUnpack;
//-Class Enums-------------------------------------------------------------
public:
    enum Type
    {
        NoError,
        InvalidInput,
        InvalidOutput,
        CantReadKey,
        AtlasDoesntExist,
        CantReadAtlas,
        AtlasUnsupported,
        CantCreateDir
    };

//-Class Variables-------------------------------------------------------------
private:
    static inline const QHash<Type, QString> ERR_STRINGS{
        {NoError, u""_s},
        {InvalidInput, u"The provided input atlas key is invalid."_s},
        {InvalidOutput, u"The provided output directory is invalid."_s},
        {CantReadKey, u"Failed to read atlas key."_s},
        {AtlasDoesntExist, u"The atlas specified by the provided atlas key does not exist."_s},
        {CantReadAtlas, u"Failed to read atlas."_s},
        {AtlasUnsupported, u"The input atlas is not supported."_s},
        {CantCreateDir, u"Failed to create unpack folder."_s}
    };

//-Instance Variables-------------------------------------------------------------
private:
    Type mType;
    QString mSpecific;
    QString mDetails;

//-Constructor-------------------------------------------------------------
private:
    CUnpackError(Type t = NoError, const QString& s = {}, const QString& d = {});

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

class CUnpack : public UntexCommand
{
//-Class Variables------------------------------------------------------------------------------------------------------
private:
    // Messages
    static inline const QString MSG_READ_KEY = u"Reading input atlas key..."_s;
    static inline const QString MSG_FORM_ATLAS = u"Forming atlas..."_s;
    static inline const QString MSG_DEATLAS = u"Deatlasing..."_s;
    static inline const QString MSG_WRITE_IMAGES = u"Writing output images..."_s;
    static inline const QString MSG_SUCCESS = u"Successfully unpacked %1 images"_s;

    // Command line option strings
    static inline const QString CL_OPT_INPUT_S_NAME = u"i"_s;
    static inline const QString CL_OPT_INPUT_L_NAME = u"input"_s;
    static inline const QString CL_OPT_INPUT_DESC = u"Key of the atlas to unpack. Must be in the same directory as its atlas."_s;

    static inline const QString CL_OPT_OUTPUT_S_NAME = u"o"_s;
    static inline const QString CL_OPT_OUTPUT_L_NAME = u"output"_s;
    static inline const QString CL_OPT_OUTPUT_DESC = u"Directory in which to place the resultant folder of unpacked images."_s;

    // Command line options
    static inline const QCommandLineOption CL_OPTION_INPUT{{CL_OPT_INPUT_S_NAME, CL_OPT_INPUT_L_NAME}, CL_OPT_INPUT_DESC, u"input"_s}; // Takes value
    static inline const QCommandLineOption CL_OPTION_OUTPUT{{CL_OPT_OUTPUT_S_NAME, CL_OPT_OUTPUT_L_NAME}, CL_OPT_OUTPUT_DESC, u"output"_s}; // Takes value

    static inline const QList<const QCommandLineOption*> CL_OPTIONS_SPECIFIC{&CL_OPTION_INPUT, &CL_OPTION_OUTPUT};
    static inline const QSet<const QCommandLineOption*> CL_OPTIONS_REQUIRED{&CL_OPTION_INPUT, &CL_OPTION_OUTPUT};

public:
    // Meta
    static inline const QString NAME = u"unpack"_s;
    static inline const QString DESCRIPTION = u"Unpack a TEX atlas into its component images"_s;

//-Constructor----------------------------------------------------------------------------------------------------------
public:
    CUnpack(Stex& coreRef);

//-Instance Functions------------------------------------------------------------------------------------------------------
protected:
    QList<const QCommandLineOption*> options() const override;
    QSet<const QCommandLineOption*> requiredOptions() const override;
    QString name() const override;

public:
    Qx::Error perform() override;
};
REGISTER_COMMAND(CUnpack::NAME, CUnpack, CUnpack::DESCRIPTION);

#endif // CUNPACK_H
