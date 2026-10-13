#pragma once
#include "BackupPlatform.hpp"
#include "BackupDocument.hpp"
#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <fcntl.h>
namespace metrobackup {
#ifdef _WIN32
inline std::wstring wide(const std::string& path){int size=MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,path.c_str(),-1,nullptr,0);if(!size)return {};std::wstring result(size,0);MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,path.c_str(),-1,&result[0],size);result.pop_back();return result;}
inline FILE* openRead(const std::string& path){auto p=wide(path);return p.empty()?nullptr:_wfopen(p.c_str(),L"rb");}
inline int createExclusive(const std::string& path){auto p=wide(path);return p.empty()?-1:_wopen(p.c_str(),_O_BINARY|_O_WRONLY|_O_CREAT|_O_EXCL,_S_IREAD|_S_IWRITE);}
inline FILE* stream(int fd){return _fdopen(fd,"wb");}
inline void closeFd(int fd){_close(fd);}
inline bool sync(FILE* file){return std::fflush(file)==0 && _commit(_fileno(file))==0;}
inline void discard(const std::string& path){_wremove(wide(path).c_str());}
inline bool replace(const std::string& source,const std::string& destination){return MoveFileExW(wide(source).c_str(),wide(destination).c_str(),MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH)!=0;}
#else
inline FILE* openRead(const std::string& path){return std::fopen(path.c_str(),"rb");}
inline int createExclusive(const std::string& path){return ::open(path.c_str(),O_WRONLY|O_CREAT|O_EXCL,0600);}
inline FILE* stream(int fd){return fdopen(fd,"wb");}
inline void closeFd(int fd){::close(fd);}
inline bool sync(FILE* file){return std::fflush(file)==0 && fsync(fileno(file))==0;}
inline void discard(const std::string& path){std::remove(path.c_str());}
inline bool replace(const std::string& source,const std::string& destination){return std::rename(source.c_str(),destination.c_str())==0;}
#endif
inline bool write(const std::string& path,json_t* document,std::string& error){
    std::unique_ptr<char,decltype(&std::free)> bytes(json_dumps(document,JSON_INDENT(2)|JSON_REAL_PRECISION(9)),std::free);
    if(!bytes){error="Could not encode the backup.";return false;}
    size_t count=std::strlen(bytes.get());if(count>MaxBytes){error="The backup exceeds the file size limit.";return false;}
    std::string temporary;int fd=-1;
    for(int n=0;n<8 && fd<0;++n){temporary=path+".tmp-"+std::to_string(rack::random::u64());fd=createExclusive(temporary);if(fd<0 && errno!=EEXIST)break;}
    if(fd<0){error="Could not create a backup in that location.";return false;}
    FILE* file=stream(fd);if(!file){closeFd(fd);discard(temporary);error="Could not open the backup for writing.";return false;}
    bool good=std::fwrite(bytes.get(),1,count,file)==count && sync(file);if(std::fclose(file)!=0)good=false;
    if(!good || !replace(temporary,path)){discard(temporary);error="Could not save the backup. The previous file was preserved.";return false;}
    error.clear();return true;
}
inline json_t* read(const std::string& path,std::string& error){
    FILE* file=openRead(path);if(!file){error="Could not open the backup file.";return nullptr;}
    std::string bytes;char block[16384];bool good=true;
    for(;;){size_t count=std::fread(block,1,sizeof(block),file);if(bytes.size()+count>MaxBytes){good=false;error="The backup exceeds the 4 MiB limit.";break;}bytes.append(block,count);if(count<sizeof(block)){if(std::ferror(file)){good=false;error="Could not read the backup file.";}break;}}
    std::fclose(file);if(!good)return nullptr;
    json_error_t parser{};auto* document=json_loadb(bytes.data(),bytes.size(),JSON_REJECT_DUPLICATES,&parser);
    if(!document){error=rack::string::f("Invalid backup JSON at line %d: %s",parser.line,parser.text);return nullptr;}
    error.clear();return document;
}
}
