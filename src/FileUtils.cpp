#include "FileUtils.h"
#include <QDebug>
#include <QDir>

QString calculateFileMd5(QString filepath)
{
	QFile file(filepath);
	if (!file.open(QIODevice::ReadOnly))
	{
		qDebug() << "Failed to open file:" << filepath;
		return QString();
	}

	// 使用 QCryptographicHash 计算 MD5
	QCryptographicHash hash(QCryptographicHash::Md5);
	if (hash.addData(&file))
	{
		QByteArray md5Result = hash.result();
		return md5Result.toHex(); // 转换为16进制字符串
	}
	else
	{
		qDebug() << "Failed to add file data to hash.";
		return QString();
	}
}




// 解压单个文件
bool extractFile(zip_t *zip, const QString &fileName, const QString &outputDir)
{
    // 获取文件索引
    struct zip_stat stat;
    if (zip_stat(zip, fileName.toUtf8().constData(), 0, &stat) != 0)
    {
        qWarning() << "Failed to stat file:" << fileName;
        return false;
    }

    // 创建输出文件路径
    QString outputFilePath = outputDir + "/" + fileName;
    QFile outputFile(outputFilePath);
    if (!outputFile.open(QIODevice::WriteOnly))
    {
        qWarning() << "Failed to open output file:" << outputFilePath;
        return false;
    }

    // 获取文件的源数据
    zip_file_t *zfile = zip_fopen(zip, fileName.toUtf8().constData(), 0);
    if (!zfile)
    {
        qWarning() << "Failed to open file in zip:" << fileName;
        return false;
    }

    // 读取压缩包中的文件数据并写入目标文件
    char buffer[1024];
    int bytesRead = 0;
    while ((bytesRead = zip_fread(zfile, buffer, sizeof(buffer))) > 0)
    {
        outputFile.write(buffer, bytesRead);
    }

    // 关闭文件和输出流
    zip_fclose(zfile);
    outputFile.close();
    return true;
}

bool unzipFile(const QString &zipFilePath, const QString &outputDir)
{
    // 打开zip文件
    int err = 0;
    zip_t *zip = zip_open(zipFilePath.toUtf8().constData(), 0, &err);
    if (!zip)
    {
        qWarning() << "Failed to open zip file:" << zipFilePath;
        return false;
    }

    // 创建输出目录（如果不存在）
    QDir dir(outputDir);
    if (!dir.exists())
    {
        dir.mkpath(".");
    }

    // 获取zip文件中的文件数量
    zip_int64_t fileCount = zip_get_num_entries(zip, 0);
    for (zip_int64_t i = 0; i < fileCount; ++i)
    {
        // 获取每个文件的名字
        const char *fileName = zip_get_name(zip, i, 0);
        if (fileName)
        {
            // 解压每个文件
            if (!extractFile(zip, QString::fromUtf8(fileName), outputDir))
            {
                zip_close(zip);
                return false;
            }
        }
    }

    // 关闭zip文件
    zip_close(zip);
    return true;
}

// 添加单个文件到zip包
bool addFileToZip(zip_t *zip, const QString &filePath, const QString &baseDir)
{
    // Open the file to compress
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
    {
        qWarning() << "Failed to open file:" << filePath;
        return false;
    }

    // relative path inside the zip，文件绝对路径去掉被压缩的文件夹的绝对路径得到压缩包内的相对路径
    QString relativePath = filePath.mid(baseDir.length() + 1);
    qDebug() << relativePath << "\t" << relativePath.toUtf8().constData();

    // Create and open file inside the zip
    zip_source_t *source = zip_source_file(zip, filePath.toUtf8().constData(), 0, 0);
    if (source == nullptr)
    {
        qWarning() << "Failed to create zip source for file:" << filePath;
        return false;
    }

    if (zip_file_add(zip, relativePath.toUtf8().constData(), source, ZIP_FL_OVERWRITE) < 0)
    {
        qWarning() << "Failed to add file to zip:" << filePath;
        zip_source_free(source); // Free the source object in case of failure
        return false;
    }

    return true;
}

// 递归添加目录到zip文件
bool addDirectoryToZip(zip_t *zip, const QString &dirPath, const QString &baseDir)
{
    QDir dir(dirPath);
    if (!dir.exists())
    {
        qWarning() << "Directory doesn't exist:" << dirPath;
        return false;
    }

    // Add all files in the directory to the zip
    QStringList files = dir.entryList(QDir::Files | QDir::NoDotAndDotDot);
    foreach (const QString &file, files)
    {
        if (!addFileToZip(zip, dir.absoluteFilePath(file), baseDir))
        {
            return false;
        }
    }

    // Recurse into subdirectories
    QStringList dirs = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    foreach (const QString &subdir, dirs)
    {
        if (!addDirectoryToZip(zip, dir.absoluteFilePath(subdir), baseDir))
        {
            return false;
        }
    }

    return true;
}

// 压缩整个目录
bool compressDirectory(const QString &dirToCompress, const QString &zipFilePath)
{
    QFileInfo zipFileInfo(zipFilePath);
    QDir dir = zipFileInfo.absoluteDir();
    if (!dir.exists())
    {
        dir.mkpath(dir.absolutePath());
    }

    // Open the zip file
    int error = 0;
    zip_t *zip = zip_open(zipFilePath.toUtf8().constData(), ZIP_CREATE | ZIP_TRUNCATE, &error);
    if (zip == nullptr)
    {
        qWarning() << "Failed to create zip file:" << zipFilePath;
        return false;
    }

    // Recursively add the directory to the zip
    if (!addDirectoryToZip(zip, dirToCompress, dirToCompress))
    {
        zip_close(zip);
        return false;
    }

    zip_close(zip);
    return true;
}
