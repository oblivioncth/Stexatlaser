#ifndef CPACK_H
#define CPACK_H

// Qt Includes
#include <QFileInfo>

// Project Includes
#include "tex-command.h"

class QX_ERROR_TYPE(CPackError, "CPackError", 1215)
{
    friend class CPack;
//-Class Enums-------------------------------------------------------------
public:
    enum Type
    {
        NoError,
        InvalidInput,
        InvalidOutput,
        NoImages,
        DupeBasename,
        CantWriteAtlas,
        CantWriteKey
    };

//-Class Variables-------------------------------------------------------------
private:
    static inline const QHash<Type, QString> ERR_STRINGS{
        {NoError, u""_s},
        {InvalidInput, u"The provided input directory is invalid."_s},
        {InvalidOutput, u"The provided output directory is invalid."_s},
        {NoImages, u"The provided input directory contains no images."_s},
        {DupeBasename, u"The provided input directory contains images with the same basename (name without extension)."_s},
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

class CPack : public TexCommand
{
//-Class Variables------------------------------------------------------------------------------------------------------
private:
    // Messages
    static inline const QString MSG_READ_IMAGES = u"Reading input images..."_s;
    static inline const QString MSG_CREATE_ATLAS = u"Creating atlas..."_s;
    static inline const QString MSG_CREATE_KEY = u"Creating atlas key..."_s;
    static inline const QString MSG_WRITE_KEY = u"Writing atlas key..."_s;
    static inline const QString MSG_SUCCESS = u"Successfully packed %1 images"_s;

    // Command line option strings
    static inline const QString CL_OPT_MARGIN_S_NAME = u"m"_s;
    static inline const QString CL_OPT_MARGIN_L_NAME = u"margin"_s;
    static inline const QString CL_OPT_MARGIN_DESC = u"Add a 1-px transparent margin to each input image (when more than one). Useful for rare cases of element bleed-over."_s;

    static inline const QString CL_OPT_INPUT_S_NAME = u"i"_s;
    static inline const QString CL_OPT_INPUT_L_NAME = u"input"_s;
    static inline const QString CL_OPT_INPUT_DESC = u"Directory containing images to pack."_s;

    static inline const QString CL_OPT_OUTPUT_S_NAME = u"o"_s;
    static inline const QString CL_OPT_OUTPUT_L_NAME = u"output"_s;
    static inline const QString CL_OPT_OUTPUT_DESC = u"Directory in which to place the resultant atlas and key."_s;

    // Command line options
    static inline const QCommandLineOption CL_OPTION_MARGIN{{CL_OPT_MARGIN_S_NAME, CL_OPT_MARGIN_L_NAME}, CL_OPT_MARGIN_DESC}; // Boolean option
    static inline const QCommandLineOption CL_OPTION_INPUT{{CL_OPT_INPUT_S_NAME, CL_OPT_INPUT_L_NAME}, CL_OPT_INPUT_DESC, u"input"_s}; // Takes value
    static inline const QCommandLineOption CL_OPTION_OUTPUT{{CL_OPT_OUTPUT_S_NAME, CL_OPT_OUTPUT_L_NAME}, CL_OPT_OUTPUT_DESC, u"output"_s}; // Takes value
    static inline const QList<const QCommandLineOption*> CL_OPTIONS_SPECIFIC{&CL_OPTION_MARGIN, &CL_OPTION_INPUT, &CL_OPTION_OUTPUT};
    static inline const QSet<const QCommandLineOption*> CL_OPTIONS_REQUIRED{&CL_OPTION_INPUT, &CL_OPTION_OUTPUT};

public:
    // Meta
    static inline const QString NAME = u"pack"_s;
    static inline const QString DESCRIPTION = u"Pack a folder of images (PNG) into a TEX atlas. The input directory will be used as the name for the atlas/key, "
                                              "while the image names will be used as the element names"_s;

//-Constructor----------------------------------------------------------------------------------------------------------
public:
    CPack(Stex& coreRef);

//-Class Functions-------------------------------------------------------------------------------------------------------
private:
    static bool hasBasenameCollision(const QFileInfoList& imageFiles);

//-Instance Functions------------------------------------------------------------------------------------------------------
protected:
    QList<const QCommandLineOption*> options() const override;
    QSet<const QCommandLineOption*> requiredOptions() const override;
    QString name() const override;

public:
    Qx::Error perform() override;
};
REGISTER_COMMAND(CPack::NAME, CPack, CPack::DESCRIPTION);

#endif // CPACK_H
