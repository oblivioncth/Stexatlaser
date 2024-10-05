#ifndef TEX_COMMAND_H
#define TEX_COMMAND_H

// Qx Includes
#include <qx/io/qx-ioopreport.h>

// Project Includes
#include "command.h"
#include "klei/k-tex.h"

class QX_ERROR_TYPE(TexCommandError, "TexCommandError", 1211)
{
    friend class TexCommand;
//-Class Enums-------------------------------------------------------------
public:
    enum Type
    {
        NoError,
        InvalidFormat,
        CantReadImage
    };

//-Class Variables-------------------------------------------------------------
private:
    static inline const QHash<Type, QString> ERR_STRINGS{
        {NoError, u""_s},
        {InvalidFormat, u"The provided output pixel format is invalid."_s},
        {CantReadImage, u"Failed to read image."_s}
    };

//-Instance Variables-------------------------------------------------------------
private:
    Type mType;
    QString mSpecific;
    QString mDetails;

//-Constructor-------------------------------------------------------------
private:
    TexCommandError(Type t = NoError, const QString& s = {}, const QString& d = {});

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

class TexCommand : public Command
{
//-Class Variables------------------------------------------------------------------------------------------------------
private:
    // Processing
    static inline const QMap<QString, KTex::Header::PixelFormat> PIXEL_FORMAT_MAP = {
        {u"dxt1"_s, KTex::Header::PixelFormat::DXT1},
        {u"dxt3"_s, KTex::Header::PixelFormat::DXT3},
        {u"dxt5"_s, KTex::Header::PixelFormat::DXT5},
        {u"rgb"_s, KTex::Header::PixelFormat::RGB},
        {u"rgba"_s, KTex::Header::PixelFormat::RGBA},
    };

    // Messages
    static inline const QString MSG_CREATE_TEX = u"Creating TEX..."_s;
    static inline const QString MSG_TEX_INFO =  u"TEX Info:\n%1"_s;
    static inline const QString MSG_WRITE_TEX = u"Writing TEX..."_s;

    // Command line option strings
    static inline const QString CL_OPT_STRAIGHT_S_NAME = u"s"_s;
    static inline const QString CL_OPT_STRAIGHT_L_NAME = u"straight"_s;
    static inline const QString CL_OPT_STRAIGHT_DESC = u"Keep straight alpha channel, do not pre-multiply."_s;

    static inline const QString CL_OPT_UNOPT_S_NAME = u"u"_s;
    static inline const QString CL_OPT_UNOPT_L_NAME = u"unoptimized"_s;
    static inline const QString CL_OPT_UNOPT_DESC = u"Do not generate smoothed mipmaps."_s;

    static inline const QString CL_OPT_FORMAT_S_NAME = u"f"_s;
    static inline const QString CL_OPT_FORMAT_L_NAME = u"format"_s;
    static inline const QString CL_OPT_FORMAT_DESC = u"Pixel format to use when encoding to TEX. <"_s +
                                                     PIXEL_FORMAT_MAP.keys().join(u" | "_s) + u">. "_s +
                                                     u"Defaults to DXT5."_s;

protected:
    // Messages
    static inline const QString MSG_INPUT_VALIDATION = u"Validating input..."_s;

    // Command line options
    static inline const QCommandLineOption CL_OPTION_STRAIGHT{{CL_OPT_STRAIGHT_S_NAME, CL_OPT_STRAIGHT_L_NAME}, CL_OPT_STRAIGHT_DESC}; // Boolean option
    static inline const QCommandLineOption CL_OPTION_UNOPT{{CL_OPT_UNOPT_S_NAME, CL_OPT_UNOPT_L_NAME}, CL_OPT_UNOPT_DESC}; // Boolean option
    static inline const QCommandLineOption CL_OPTION_FORMAT{{CL_OPT_FORMAT_S_NAME, CL_OPT_FORMAT_L_NAME}, CL_OPT_FORMAT_DESC, u"format"_s}; // Takes value

    static inline const QList<const QCommandLineOption*> CL_OPTIONS_SPECIFIC{&CL_OPTION_STRAIGHT, &CL_OPTION_UNOPT, &CL_OPTION_FORMAT};

public:
    // Meta
    static inline const QString NAME = u"tex-command"_s;

//-Constructor----------------------------------------------------------------------------------------------------------
public:
    TexCommand(Stex& coreRef);

//-Destructor----------------------------------------------------------------------------------------------------------
public:
    virtual ~TexCommand() = default;

//-Instance Functions------------------------------------------------------------------------------------------------------
protected:
    virtual QList<const QCommandLineOption*> options() const override;
    TexCommandError getFormat(KTex::Header::PixelFormat& format) const;
    TexCommandError readImage(QImage& image, const QString& path) const;
    KTex createTex(const QImage& image, KTex::Header::PixelFormat format) const;
    Qx::IoOpReport writeTex(const KTex& tex, const QString& path) const;

};

#endif // TEX_COMMAND_H
