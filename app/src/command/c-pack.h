#ifndef CPACK_H
#define CPACK_H

// Qt Includes
#include <QFileInfo>

// Project Includes
#include "command.h"
#include "klei/k-tex.h"

class QX_ERROR_TYPE(CPackError, "CPackError", 1211)
{
    friend class CPack;
//-Class Enums-------------------------------------------------------------
public:
    enum Type
    {
        NoError,
        InvalidInput,
        InvalidOutput,
        InvalidFormat,
        NoImages,
        DupeBasename,
        CantReadImage,
        CantWriteAtlas,
        CantWriteKey
    };

//-Class Variables-------------------------------------------------------------
private:
    static inline const QHash<Type, QString> ERR_STRINGS{
        {NoError, u""_s},
        {InvalidInput, u"The provided input directory is invalid."_s},
        {InvalidOutput, u"The provided output directory is invalid."_s},
        {InvalidFormat, u"The provided output pixel format is invalid."_s},
        {NoImages, u"The provided input directory contains no images."_s},
        {DupeBasename, u"The provided input directory contains images with the same basename (name without extension)."_s},
        {CantReadImage, u"Failed to read image."_s},
        {CantWriteAtlas, u"Failed to write output atlas."_s},
        {CantWriteKey, u"Failed to write output atlas key."_s}
    };

//-Instance Variables-------------------------------------------------------------
private:
    Type mType;
    QString mSpecific;
    QString mDetails;

//-Constructor-------------------------------------------------------------
private:
    CPackError(Type t = NoError, const QString& s = {}, const QString& d = {});

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

class CPack : public Command
{
//-Class Variables------------------------------------------------------------------------------------------------------
private:
    // Processing
    static inline const QMap<QString, KTex::Header::PixelFormat> PIXEL_FORMAT_MAP = {
        {"dxt1", KTex::Header::PixelFormat::DXT1},
        {"dxt3", KTex::Header::PixelFormat::DXT3},
        {"dxt5", KTex::Header::PixelFormat::DXT5},
        {"rgb", KTex::Header::PixelFormat::RGB},
        {"rgba", KTex::Header::PixelFormat::RGBA},
    };

    // Messages
    static inline const QString MSG_INPUT_VALIDATION = "Validating input...";
    static inline const QString MSG_READ_IMAGES = "Reading input images...";
    static inline const QString MSG_CREATE_ATLAS = "Creating atlas...";
    static inline const QString MSG_CREATE_KEY = "Creating atlas key...";
    static inline const QString MSG_CREATE_TEX = "Creating TEX...";
    static inline const QString MSG_WRITE_TEX = "Writing TEX...";
    static inline const QString MSG_WRITE_KEY = "Writing atlas key...";
    static inline const QString MSG_SUCCESS = "Successfully packed %1 images";

    // Command line option strings
    static inline const QString CL_OPT_STRAIGHT_S_NAME = "s";
    static inline const QString CL_OPT_STRAIGHT_L_NAME = "straight";
    static inline const QString CL_OPT_STRAIGHT_DESC = "Keep straight alpha channel, do not pre-multiply.";

    static inline const QString CL_OPT_UNOPT_S_NAME = "u";
    static inline const QString CL_OPT_UNOPT_L_NAME = "unoptimized";
    static inline const QString CL_OPT_UNOPT_DESC = "Do not generate smoothed mipmaps.";

    static inline const QString CL_OPT_FORMAT_S_NAME = "f";
    static inline const QString CL_OPT_FORMAT_L_NAME = "format";
    static inline const QString CL_OPT_FORMAT_DESC = "Pixel format to use when encoding to TEX. <" +
                                                     PIXEL_FORMAT_MAP.keys().join(" | ") + ">. " +
                                                     "Defaults to DXT5.";

    static inline const QString CL_OPT_MARGIN_S_NAME = "m";
    static inline const QString CL_OPT_MARGIN_L_NAME = "margin";
    static inline const QString CL_OPT_MARGIN_DESC = "Add a 1-px transparent margin to each input image (when more than one). Useful for rare cases of element bleed-over.";

    static inline const QString CL_OPT_INPUT_S_NAME = "i";
    static inline const QString CL_OPT_INPUT_L_NAME = "input";
    static inline const QString CL_OPT_INPUT_DESC = "Directory containing images to pack.";

    static inline const QString CL_OPT_OUTPUT_S_NAME = "o";
    static inline const QString CL_OPT_OUTPUT_L_NAME = "output";
    static inline const QString CL_OPT_OUTPUT_DESC = "Directory in which to place the resultant atlas and key.";

    // Command line options
    static inline const QCommandLineOption CL_OPTION_STRAIGHT{{CL_OPT_STRAIGHT_S_NAME, CL_OPT_STRAIGHT_L_NAME}, CL_OPT_STRAIGHT_DESC}; // Boolean option
    static inline const QCommandLineOption CL_OPTION_UNOPT{{CL_OPT_UNOPT_S_NAME, CL_OPT_UNOPT_L_NAME}, CL_OPT_UNOPT_DESC}; // Boolean option
    static inline const QCommandLineOption CL_OPTION_FORMAT{{CL_OPT_FORMAT_S_NAME, CL_OPT_FORMAT_L_NAME}, CL_OPT_FORMAT_DESC, "format"}; // Takes value
    static inline const QCommandLineOption CL_OPTION_MARGIN{{CL_OPT_MARGIN_S_NAME, CL_OPT_MARGIN_L_NAME}, CL_OPT_MARGIN_DESC}; // Boolean option
    static inline const QCommandLineOption CL_OPTION_INPUT{{CL_OPT_INPUT_S_NAME, CL_OPT_INPUT_L_NAME}, CL_OPT_INPUT_DESC, "input"}; // Takes value
    static inline const QCommandLineOption CL_OPTION_OUTPUT{{CL_OPT_OUTPUT_S_NAME, CL_OPT_OUTPUT_L_NAME}, CL_OPT_OUTPUT_DESC, "output"}; // Takes value
    static inline const QList<const QCommandLineOption*> CL_OPTIONS_SPECIFIC{&CL_OPTION_INPUT, &CL_OPTION_OUTPUT, &CL_OPTION_STRAIGHT,
                                                                             &CL_OPTION_UNOPT, &CL_OPTION_FORMAT, &CL_OPTION_MARGIN};
    static inline const QSet<const QCommandLineOption*> CL_OPTIONS_REQUIRED{&CL_OPTION_INPUT, &CL_OPTION_OUTPUT};

public:
    // Meta
    static inline const QString NAME = "pack";
    static inline const QString DESCRIPTION = "Pack a folder of images (PNG) into a TEX atlas. The input directory will be used as the name for the atlas/key, "
                                              "while the image names will be used as the element names";

//-Constructor----------------------------------------------------------------------------------------------------------
public:
    CPack(Stex& coreRef);

//-Class Functions-------------------------------------------------------------------------------------------------------
private:
    static bool hasBasenameCollision(const QFileInfoList& imageFiles);

//-Instance Functions------------------------------------------------------------------------------------------------------
protected:
    QList<const QCommandLineOption*> options();
    QSet<const QCommandLineOption*> requiredOptions();
    QString name();

public:
    Qx::Error perform();
};
REGISTER_COMMAND(CPack::NAME, CPack, CPack::DESCRIPTION);

#endif // CPACK_H
