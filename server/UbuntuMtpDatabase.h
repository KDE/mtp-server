/*
 * Copyright (C) 2013 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3, as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef STUB_MTP_DATABASE_H_
#define STUB_MTP_DATABASE_H_

#include <mtp.h>
#include <MtpDatabase.h>
#include <MtpDataPacket.h>
#include <MtpStringBuffer.h>
#include <MtpObjectInfo.h>
#include <MtpProperty.h>

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <tuple>
#include <exception>

#include <boost/algorithm/string.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>
#include <boost/range/adaptors.hpp>
#include <boost/range/algorithm.hpp>

using namespace boost::filesystem;

namespace android
{
class UbuntuMtpDatabase : public android::MtpDatabase {
private:
    struct DbEntry
    {
        MtpStorageID storage_id;
        MtpObjectFormat object_format;
        MtpObjectHandle parent;
        size_t object_size;
        std::string display_name;
        std::string path;
    };

    uint32_t counter;
    std::map<MtpObjectHandle, DbEntry> db;
    std::map<std::string, MtpObjectFormat> formats = boost::assign::map_list_of
        (".gif", MTP_FORMAT_GIF)
        (".png", MTP_FORMAT_PNG)
        (".jpeg", MTP_FORMAT_JFIF)
        (".tiff", MTP_FORMAT_TIFF)
        (".ogg", MTP_FORMAT_OGG)
        (".mp3", MTP_FORMAT_MP3)
        (".wav", MTP_FORMAT_WAV)
        (".wma", MTP_FORMAT_WMA)
        (".aac", MTP_FORMAT_AAC)
        (".flac", MTP_FORMAT_FLAC);

    MtpObjectFormat guess_object_format(std::string extension)
    {
        std::map<std::string, MtpObjectFormat>::iterator it;

        it = formats.find(extension);
        if (it == formats.end()) {
            boost::to_upper(extension);
            it = formats.find(extension);
            if (it == formats.end()) {
                return MTP_FORMAT_UNDEFINED;
            }
	}

	return it->second;
    }

    void parse_directory(path p, MtpObjectHandle parent)
    {
	DbEntry entry;
        std::vector<path> v;

        copy(directory_iterator(p), directory_iterator(), std::back_inserter(v));

        for (std::vector<path>::const_iterator it(v.begin()), it_end(v.end()); it != it_end; ++it)
        {
            MtpObjectHandle handle = counter;

            counter++;

            entry.storage_id = MTP_STORAGE_FIXED_RAM;
            entry.parent = parent;
            entry.display_name = std::string(it->filename().string());
            entry.path = it->string();

            if (is_regular_file (*it)) {
                entry.object_format = guess_object_format(it->extension().string());
                entry.object_size = file_size(*it);

                db.insert( std::pair<MtpObjectHandle, DbEntry>(handle, entry) );
            } else if (is_directory (*it)) {
                entry.object_format = MTP_FORMAT_ASSOCIATION;
                entry.object_size = 0;

                db.insert( std::pair<MtpObjectHandle, DbEntry>(handle, entry) );

                parse_directory (*it, handle);
	    }
        }
    }

    void readFiles(const std::string& sourcedir)
    {
        path p (sourcedir);
	DbEntry entry;
	MtpObjectHandle handle = counter++;

        try {
            if (exists(p)) {
                if (is_directory(p)) {
                    entry.storage_id = MTP_STORAGE_FIXED_RAM;
                    entry.parent = MTP_PARENT_ROOT;
                    entry.display_name = std::string(p.filename().string());
                    entry.path = p.string();
                    entry.object_format = MTP_FORMAT_ASSOCIATION;
                    entry.object_size = 0;

                    db.insert( std::pair<MtpObjectHandle, DbEntry>(handle, entry) );

                    parse_directory (p, handle);
                }
            } else
                std::cout << p << " does not exist\n";
        }
        catch (const filesystem_error& ex) {
            std::cout << ex.what() << '\n';
        }

    }

public:
    UbuntuMtpDatabase(const char *dir) : counter(1)
    {
	std::string basedir (dir);

	db = std::map<MtpObjectHandle, DbEntry>();

	readFiles(basedir + "/Documents");
	readFiles(basedir + "/Music");
	readFiles(basedir + "/Videos");
	readFiles(basedir + "/Pictures");
	readFiles(basedir + "/Downloads");

	std::cout << "Added " << counter << " entries to the database." << std::endl;
    }

    virtual ~UbuntuMtpDatabase() {}

    // called from SendObjectInfo to reserve a database entry for the incoming file
    virtual MtpObjectHandle beginSendObject(
        const MtpString& path,
        MtpObjectFormat format,
        MtpObjectHandle parent,
        MtpStorageID storage,
        uint64_t size,
        time_t modified)
    {
	DbEntry entry;
	MtpObjectHandle handle = counter;

        if (parent == 0)
            return kInvalidObjectHandle;

        std::cout << __PRETTY_FUNCTION__ << ": " << path << " - " << parent << std::endl;

        entry.storage_id = storage;
        entry.parent = parent;
        entry.display_name = std::string(basename(path.c_str()));
        entry.path = path;
        entry.object_format = format;
        entry.object_size = size;

        db.insert( std::pair<MtpObjectHandle, DbEntry>(handle, entry) );

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

	if (!succeeded) {
            db.erase(handle);
        } else {
            boost::filesystem::path p (path);

            if (format != MTP_FORMAT_ASSOCIATION) {
                /* Resync file size, just in case this is actually an Edit. */
                db.at(handle).object_size = file_size(p);
            }
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
            std::vector<MtpObjectHandle> keys;

            BOOST_FOREACH(MtpObjectHandle i, db | boost::adaptors::map_keys) {
                if (db.at(i).parent == parent)
                    keys.push_back(i);
            }

            list = new MtpObjectHandleList(keys);
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
        static const MtpObjectFormatList list = {
            /* Generic files */
            MTP_FORMAT_UNDEFINED,

            /* Supported audio formats */
            MTP_FORMAT_OGG,
            MTP_FORMAT_MP3,
            MTP_FORMAT_WAV,
            MTP_FORMAT_WMA,
            MTP_FORMAT_AAC,
            MTP_FORMAT_FLAC,

            /* Supported video formats */
            // none listed yet, video apparently broken.

            /* Audio album, and album art */
            MTP_FORMAT_ABSTRACT_AUDIO_ALBUM,

            /* Playlists for audio and video */
            MTP_FORMAT_ABSTRACT_AV_PLAYLIST,
        };

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
	/*
        if (format != MTP_FORMAT_PNG)
            return nullptr;
        */
            
        static const MtpObjectPropertyList list = 
        {
            MTP_PROPERTY_STORAGE_ID,
            MTP_PROPERTY_PARENT_OBJECT,
            MTP_PROPERTY_OBJECT_FORMAT,
            MTP_PROPERTY_OBJECT_SIZE,
            MTP_PROPERTY_WIDTH,
            MTP_PROPERTY_HEIGHT,
            MTP_PROPERTY_IMAGE_BIT_DEPTH,
            MTP_PROPERTY_OBJECT_FILE_NAME,
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
            case MTP_PROPERTY_STORAGE_ID: packet.putUInt32(db.at(handle).storage_id); break;            
            case MTP_PROPERTY_PARENT_OBJECT: packet.putUInt32(db.at(handle).parent); break;            
            case MTP_PROPERTY_OBJECT_FORMAT: packet.putUInt32(db.at(handle).object_format); break;
            case MTP_PROPERTY_OBJECT_SIZE: packet.putUInt32(db.at(handle).object_size); break;
            case MTP_PROPERTY_DISPLAY_NAME: packet.putString(db.at(handle).display_name.c_str()); break;
            case MTP_PROPERTY_OBJECT_FILE_NAME: packet.putString(db.at(handle).display_name.c_str()); break;
            default: return MTP_RESPONSE_GENERAL_ERROR; break;                
        }
        
        return MTP_RESPONSE_OK;
    }

    virtual MtpResponseCode setObjectPropertyValue(
        MtpObjectHandle handle,
        MtpObjectProperty property,
        MtpDataPacket& packet)
    {
        DbEntry entry;
        MtpStringBuffer buffer;
        std::string oldname;
        std::string newname;
        path oldpath;
        path newpath;

        std::cout << __PRETTY_FUNCTION__ << std::endl;

        switch(property)
        {
            case MTP_PROPERTY_OBJECT_FILE_NAME:
                try {
                    entry = db.at(handle);

                    packet.getString(buffer);
                    newname = strdup(buffer);

                    oldpath /= entry.path;
                    newpath /= oldpath.branch_path() / "/" / newname;

                    boost::filesystem::rename(oldpath, newpath);

                    db.at(handle).display_name = newname;
                    db.at(handle).path = newpath.string();
                } catch (filesystem_error& fe) {
                    std::cout << "ERROR: " << fe.what() << std::endl;
                    return MTP_RESPONSE_DEVICE_BUSY;
                } catch (std::exception& e) {
                    std::cout << "ERROR: " << e.what() << std::endl;
                    return MTP_RESPONSE_GENERAL_ERROR;
                } catch (...) {
                    std::cout << "ERROR: autre exception" << std::endl;
                    return MTP_RESPONSE_GENERAL_ERROR;
		}

                break;
            default: return MTP_RESPONSE_OPERATION_NOT_SUPPORTED; break;
        }
        
        return MTP_RESPONSE_OK;
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
        info.mStorageID = db.at(handle).storage_id;
        info.mFormat = db.at(handle).object_format;
        info.mProtectionStatus = 0x0;
        info.mCompressedSize = db.at(handle).object_size;
        info.mImagePixWidth = 0;
        info.mImagePixHeight = 0;
        info.mImagePixDepth = 0;
        info.mParent = db.at(handle).parent;
        info.mAssociationType = 0;
        info.mAssociationDesc = 0;
        info.mSequenceNumber = 0;
        info.mName = ::strdup(db.at(handle).display_name.c_str());
        info.mDateCreated = 0;
        info.mDateModified = 0;
        info.mKeywords = ::strdup("ubuntu,touch");
        
        return MTP_RESPONSE_OK;
    }

    virtual void* getThumbnail(MtpObjectHandle handle, size_t& outThumbSize)
    {
        void* result;

	outThumbSize = 0;
	memset(result, 0, outThumbSize);

        return result;
    }

    virtual MtpResponseCode getObjectFilePath(
        MtpObjectHandle handle,
        MtpString& outFilePath,
        int64_t& outFileLength,
        MtpObjectFormat& outFormat)
    {
        DbEntry entry = db.at(handle);

        std::cout << __PRETTY_FUNCTION__ << std::endl;

        outFilePath = std::string(entry.path);
        outFileLength = entry.object_size;
        outFormat = entry.object_format;

        return MTP_RESPONSE_OK;
    }

    virtual MtpResponseCode deleteFile(MtpObjectHandle handle)
    {
        size_t orig_size = db.size();
        size_t new_size;

        std::cout << __PRETTY_FUNCTION__ << std::endl;

        new_size = db.erase(handle);

        if (orig_size > new_size) {
            /* Recursively remove children object from the DB as well.
             * we can safely ignore failures here, since the objects
             * would not be reachable anyway.
             */
            BOOST_FOREACH(MtpObjectHandle i, db | boost::adaptors::map_keys) {
                if (db.at(i).parent == handle)
                    db.erase(i);
            }
            return MTP_RESPONSE_OK;
        }
        else
            return MTP_RESPONSE_GENERAL_ERROR;
    }

    virtual MtpResponseCode moveFile(MtpObjectHandle handle, MtpObjectHandle new_parent)
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;

        // change parent
        db.at(handle).parent = new_parent;

        return MTP_RESPONSE_OK;
    }

    /*
    virtual MtpResponseCode copyFile(MtpObjectHandle handle, MtpObjectHandle new_parent)
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;

        // duplicate DbEntry
        // change parent

        return MTP_RESPONSE_OK
    }
    */

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
            case MTP_PROPERTY_DISPLAY_NAME: result = new MtpProperty(property, MTP_TYPE_STR, true); break;
            case MTP_PROPERTY_OBJECT_FILE_NAME: result = new MtpProperty(property, MTP_TYPE_STR, true); break;
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
