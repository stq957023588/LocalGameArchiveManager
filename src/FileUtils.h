#pragma once

#include <QString>
#include <QFile>
#include <QIODevice>
#include <QCryptographicHash>
#include <zip/zip.h>

QString calculateFileMd5(QString filepath);

bool extractFile(zip_t *zip, const QString &fileName, const QString &outputDir);
bool unzipFile(const QString &zipFilePath, const QString &outputDir);
bool addFileToZip(zip_t *zip, const QString &filePath, const QString &baseDir);
bool addDirectoryToZip(zip_t *zip, const QString &dirPath, const QString &baseDir);


bool compressDirectory(const QString &dirToCompress, const QString &zipFilePath);