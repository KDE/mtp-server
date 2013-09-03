#ifndef STUB_MTP_DATABASE_H_
#define STUB_MTP_DATABASE_H_

#include <mtp.h>
#include <MtpDatabase.h>
#include <MtpDataPacket.h>
#include <MtpObjectInfo.h>
#include <MtpProperty.h>

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <map>
#include <string>
#include <tuple>

#include <QMap>
#include <QVector>
#include <QDir>
#include <QString>

namespace android
{
class MtpImageDatabase : public android::MtpDatabase {
private:
    struct DbEntry
    {
        MtpStorageID storage_id;
        std::string *object_name;
        MtpObjectFormat object_format;
        size_t object_size;
        size_t width;
        size_t height;
        size_t bit_depth;
        std::string *display_name;
        std::string *path;
    };

    uint32_t counter;
    QMap<MtpObjectHandle, DbEntry*> db;
    
    void readFiles(const QString& path)
    {
        QDir dir(path);
	struct DbEntry *entry;

        dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);

        std::cout << __PRETTY_FUNCTION__ << ": " << path.toStdString() << std::endl;

        QFileInfoList list = dir.entryInfoList();
        std::cout << "     Bytes Filename" << std::endl;
        for (int i = 0; i < list.size(); ++i, counter++) {
            QFileInfo fileInfo = list.at(i);

            std::cout << qPrintable(QString("%1 %2").arg(fileInfo.size(), 10)
                                                    .arg(fileInfo.fileName()));
            std::cout << std::endl;

            entry = (DbEntry*) malloc(sizeof(*entry));
            memset (entry, 0, sizeof(*entry));
            entry->storage_id = MTP_STORAGE_REMOVABLE_RAM;
            entry->object_name = new std::string(fileInfo.fileName().toStdString());
            entry->display_name = new std::string(fileInfo.fileName().toStdString());
            entry->path = new std::string(fileInfo.canonicalFilePath().toStdString());
            entry->object_format = MTP_FORMAT_PNG;
            entry->object_size = fileInfo.size();
            entry->width = 0;
            entry->height = 0;
            entry->bit_depth = 0;

            db.insert(counter, entry);
        }
    }

public:
    MtpImageDatabase() : counter(1)
    {
	db = QMap<MtpObjectHandle, DbEntry*>();
	readFiles(QString("/home/phablet/Pictures"));
	
        std::cout << __PRETTY_FUNCTION__ << ": object count:" << db.count() << std::endl;
    }

    virtual ~MtpImageDatabase() {}

    // called from SendObjectInfo to reserve a database entry for the incoming file
    virtual MtpObjectHandle beginSendObject(
        const MtpString& path,
        MtpObjectFormat format,
        MtpObjectHandle parent,
        MtpStorageID storage,
        uint64_t size,
        time_t modified)
    {
	DbEntry *entry;
	MtpObjectHandle handle = counter;

        std::cout << __PRETTY_FUNCTION__ << ": " << path << std::endl;

        entry = (DbEntry*) malloc(sizeof(*entry));
        memset (entry, 0, sizeof(*entry));
        entry->storage_id = storage;
        entry->object_name = new std::string(basename(path.c_str()));
        entry->display_name = new std::string(basename(path.c_str()));
        entry->path = new std::string(path);
        entry->object_format = format;
        entry->object_size = size;

        db.insert(handle, entry);

	counter++;

        return handle; 
    }

    // called to report success or failure of the SendObject file transfer
    // success should signal a notification of the new object's creation,
    // failure should remove the database entry created in beginSendObject
    virtual void endSendObject(
        const MtpString& path,
        MtpObjectHandle handle,
        MtpObjectFormat format,
        bool succeeded)
    {
        std::cout << __PRETTY_FUNCTION__ << ": " << path << std::endl;

	if (!suceeded) {
		db.remove(handle);
	}
    }

    virtual MtpObjectHandleList* getObjectList(
        MtpStorageID storageID,
        MtpObjectFormat format,
        MtpObjectHandle parent)
    {
        std::cout << __PRETTY_FUNCTION__ << ": " << storageID << ", " << format << ", " << parent << std::endl;
        MtpObjectHandleList* list = nullptr;
        try
        {
            list = new MtpObjectHandleList(db.keys().toVector().toStdVector());
        } catch(...)
        {
            list = new MtpObjectHandleList();
        }
        
        return list;
    }

    virtual int getNumObjects(
        MtpStorageID storageID,
        MtpObjectFormat format,
        MtpObjectHandle parent)
    {
        std::cout << __PRETTY_FUNCTION__ << ": " << storageID << ", " << format << ", " << parent << std::endl;
        try
        {
            return db.size();
        } catch(...)
        {
        }
        
        return 0;
    }

    // callee should delete[] the results from these
    // results can be NULL
    virtual MtpObjectFormatList* getSupportedPlaybackFormats()
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        static const MtpObjectFormatList list = {MTP_FORMAT_PNG};
        return new MtpObjectFormatList{list};
    }
    
    virtual MtpObjectFormatList* getSupportedCaptureFormats()
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        static const MtpObjectFormatList list = {MTP_FORMAT_ASSOCIATION, MTP_FORMAT_PNG};
        return new MtpObjectFormatList{list};
    }
    
    virtual MtpObjectPropertyList* getSupportedObjectProperties(MtpObjectFormat format)
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        if (format != MTP_FORMAT_PNG)
            return nullptr;
            
        static const MtpObjectPropertyList list = 
        {
            MTP_PROPERTY_STORAGE_ID,
            MTP_PROPERTY_OBJECT_FORMAT,
            MTP_PROPERTY_OBJECT_SIZE,
            MTP_PROPERTY_WIDTH,
            MTP_PROPERTY_HEIGHT,
            MTP_PROPERTY_IMAGE_BIT_DEPTH,
            MTP_PROPERTY_DISPLAY_NAME            
        };
         
        return new MtpObjectPropertyList{list};
    }
    
    virtual MtpDevicePropertyList* getSupportedDeviceProperties()
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        static const MtpDevicePropertyList list = { MTP_DEVICE_PROPERTY_UNDEFINED };
        return new MtpDevicePropertyList{list};
    }

    virtual MtpResponseCode getObjectPropertyValue(
        MtpObjectHandle handle,
        MtpObjectProperty property,
        MtpDataPacket& packet)
    {        
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        switch(property)
        {
            case MTP_PROPERTY_STORAGE_ID: packet.putUInt32(db.value(handle)->storage_id); break;            
            case MTP_PROPERTY_OBJECT_FORMAT: packet.putUInt32(db.value(handle)->object_format); break;
            case MTP_PROPERTY_OBJECT_SIZE: packet.putUInt32(db.value(handle)->object_size); break;
            case MTP_PROPERTY_WIDTH: packet.putUInt32(db.value(handle)->width); break;
            case MTP_PROPERTY_HEIGHT: packet.putUInt32(db.value(handle)->height); break;
            case MTP_PROPERTY_IMAGE_BIT_DEPTH: packet.putUInt32(db.value(handle)->bit_depth); break;
            case MTP_PROPERTY_DISPLAY_NAME: packet.putString(db.value(handle)->display_name->c_str()); break;
            default: return MTP_RESPONSE_GENERAL_ERROR; break;                
        }
        
        return MTP_RESPONSE_OK;
    }

    virtual MtpResponseCode setObjectPropertyValue(
        MtpObjectHandle handle,
        MtpObjectProperty property,
        MtpDataPacket& packet)
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        return MTP_RESPONSE_OPERATION_NOT_SUPPORTED;
    }

    virtual MtpResponseCode getDevicePropertyValue(
        MtpDeviceProperty property,
        MtpDataPacket& packet)
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        return MTP_RESPONSE_GENERAL_ERROR;
    }

    virtual MtpResponseCode setDevicePropertyValue(
        MtpDeviceProperty property,
        MtpDataPacket& packet)
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        return MTP_RESPONSE_OPERATION_NOT_SUPPORTED;
    }

    virtual MtpResponseCode resetDeviceProperty(
        MtpDeviceProperty property)
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        return MTP_RESPONSE_OPERATION_NOT_SUPPORTED;
    }

    virtual MtpResponseCode getObjectPropertyList(
        MtpObjectHandle handle,
        uint32_t format, 
        uint32_t property,
        int groupCode, 
        int depth,
        MtpDataPacket& packet)
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        return MTP_RESPONSE_OPERATION_NOT_SUPPORTED;
    }

    virtual MtpResponseCode getObjectInfo(
        MtpObjectHandle handle,
        MtpObjectInfo& info)
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        info.mHandle = handle;
        info.mStorageID = db.value(handle)->storage_id;
        info.mFormat = db.value(handle)->object_format;
        info.mProtectionStatus = 0x0;
        info.mCompressedSize = 0;
        info.mThumbFormat = db.value(handle)->object_format;
        info.mThumbCompressedSize = 20*20*4;
        info.mThumbPixWidth = 20;
        info.mThumbPixHeight  =20;
        info.mImagePixWidth = 20;
        info.mImagePixHeight = 20;
        info.mImagePixDepth = 4;
        info.mParent = MTP_PARENT_ROOT;
        info.mAssociationType = 0;
        info.mAssociationDesc = 0;
        info.mSequenceNumber = 0;
        info.mName = ::strdup(db.value(handle)->object_name->c_str());
        info.mDateCreated = 0;
        info.mDateModified = 0;
        info.mKeywords = ::strdup("ubuntu,touch");
        
        return MTP_RESPONSE_OK;
    }

    virtual void* getThumbnail(MtpObjectHandle handle, size_t& outThumbSize)
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        outThumbSize = 20*20*4;
        void* result = malloc(20*20*4);
        memset(result, 0, 20*20*4);
        return result;
    }

    virtual MtpResponseCode getObjectFilePath(
        MtpObjectHandle handle,
        MtpString& outFilePath,
        int64_t& outFileLength,
        MtpObjectFormat& outFormat)
    {
        DbEntry *entry = db.value(handle);

        std::cout << __PRETTY_FUNCTION__ << std::endl;

        outFilePath = std::string(entry->path->c_str());
        outFileLength = entry->object_size;
        outFormat = entry->object_format;

        return MTP_RESPONSE_OK;
    }

    virtual MtpResponseCode deleteFile(MtpObjectHandle handle)
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        return MTP_RESPONSE_OPERATION_NOT_SUPPORTED;
    }

    virtual MtpObjectHandleList* getObjectReferences(MtpObjectHandle handle)
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        return nullptr;
    }

    virtual MtpResponseCode setObjectReferences(
        MtpObjectHandle handle,
        MtpObjectHandleList* references)
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        return MTP_RESPONSE_OPERATION_NOT_SUPPORTED;    
    }

    virtual MtpProperty* getObjectPropertyDesc(
        MtpObjectProperty property,
        MtpObjectFormat format)
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        MtpProperty* result = nullptr;
        switch(property)
        {
            case MTP_PROPERTY_STORAGE_ID: result = new MtpProperty(property, MTP_TYPE_UINT32); break;
            case MTP_PROPERTY_OBJECT_FORMAT: result = new MtpProperty(property, MTP_TYPE_UINT32); break;
            case MTP_PROPERTY_OBJECT_SIZE: result = new MtpProperty(property, MTP_TYPE_UINT32); break;
            case MTP_PROPERTY_WIDTH: result = new MtpProperty(property, MTP_TYPE_UINT32); break;
            case MTP_PROPERTY_HEIGHT: result = new MtpProperty(property, MTP_TYPE_UINT32); break;
            case MTP_PROPERTY_IMAGE_BIT_DEPTH: result = new MtpProperty(property, MTP_TYPE_UINT32); break;
            case MTP_PROPERTY_DISPLAY_NAME: result = new MtpProperty(property, MTP_TYPE_STR); break;
            default: break;                
        }
        
        return result;
    }

    virtual MtpProperty* getDevicePropertyDesc(MtpDeviceProperty property)
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        return new MtpProperty(MTP_DEVICE_PROPERTY_UNDEFINED, MTP_TYPE_UNDEFINED);
    }
    
    virtual void sessionStarted()
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
    }

    virtual void sessionEnded()
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
    }
};
}

#endif // STUB_MTP_DATABASE_H_
