#ifndef CUNPACK_H
#define CUNPACK_H

#include "command.h"

class QX_ERROR_TYPE(CUnpackError, "CUnpackError", 1212)
{
    friend class CUnpack;
//-Class Enums-------------------------------------------------------------
public:
    enum Type
    {
        NoError = 0,
        InvalidInput = 1,
        InvalidOutput = 2,
        CantReadKey = 3,
        AtlasDoesntExist = 4,
        CantReadAtlas = 5,
        AtlasUnsupported = 6,
        CantCreateDir = 7,
        CantWriteImage = 8
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
        {CantCreateDir, u"Failed to create unpack folder."_s},
        {CantWriteImage, u"Failed to write output image."_s},
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

class CUnpack : public Command
{
//-Class Variables------------------------------------------------------------------------------------------------------
private:
    // Messages
    static inline const QString MSG_INPUT_VALIDATION = "Validating input...";
    static inline const QString MSG_READ_KEY = "Reading input atlas key...";
    static inline const QString MSG_READ_TEX = "Reading TEX...";
    static inline const QString MSG_EXTRACT_IMAGE = "Extracting primary TEX image...";
    static inline const QString MSG_FORM_ATLAS = "Forming atlas...";
    static inline const QString MSG_DEATLAS = "Deatlasing...";
    static inline const QString MSG_WRITE_IMAGES = "Writing output images...";
    static inline const QString MSG_SUCCESS = "Successfully unpacked %1 images";

    // Command line option strings
    static inline const QString CL_OPT_STRAIGHT_S_NAME = "s";
    static inline const QString CL_OPT_STRAIGHT_L_NAME = "straight";
    static inline const QString CL_OPT_STRAIGHT_DESC = "Specify that the alpha information within the input TEX is straight, do not de-multiply.";

    static inline const QString CL_OPT_INPUT_S_NAME = "i";
    static inline const QString CL_OPT_INPUT_L_NAME = "input";
    static inline const QString CL_OPT_INPUT_DESC = "Key of the atlas to unpack. Must be in the same directory as its atlas.";

    static inline const QString CL_OPT_OUTPUT_S_NAME = "o";
    static inline const QString CL_OPT_OUTPUT_L_NAME = "output";
    static inline const QString CL_OPT_OUTPUT_DESC = "Directory in which to place the resultant folder of unpacked images.";

    // Command line options
    static inline const QCommandLineOption CL_OPTION_STRAIGHT{{CL_OPT_STRAIGHT_S_NAME, CL_OPT_STRAIGHT_L_NAME}, CL_OPT_STRAIGHT_DESC}; // Boolean option
    static inline const QCommandLineOption CL_OPTION_INPUT{{CL_OPT_INPUT_S_NAME, CL_OPT_INPUT_L_NAME}, CL_OPT_INPUT_DESC, "input"}; // Takes value
    static inline const QCommandLineOption CL_OPTION_OUTPUT{{CL_OPT_OUTPUT_S_NAME, CL_OPT_OUTPUT_L_NAME}, CL_OPT_OUTPUT_DESC, "output"}; // Takes value
    static inline const QList<const QCommandLineOption*> CL_OPTIONS_SPECIFIC{&CL_OPTION_INPUT, &CL_OPTION_OUTPUT, &CL_OPTION_STRAIGHT};
    static inline const QSet<const QCommandLineOption*> CL_OPTIONS_REQUIRED{&CL_OPTION_INPUT, &CL_OPTION_OUTPUT};

public:
    // Meta
    static inline const QString NAME = "unpack";
    static inline const QString DESCRIPTION = "Unpack a TEX atlas into its component images";

//-Constructor----------------------------------------------------------------------------------------------------------
public:
    CUnpack(Stex& coreRef);

//-Instance Functions------------------------------------------------------------------------------------------------------
protected:
    QList<const QCommandLineOption*> options();
    QSet<const QCommandLineOption*> requiredOptions();
    QString name();

public:
    Qx::Error perform();
};
REGISTER_COMMAND(CUnpack::NAME, CUnpack, CUnpack::DESCRIPTION);

#endif // CUNPACK_H
