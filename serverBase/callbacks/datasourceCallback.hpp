#pragma once


#include <liburing.h>
#include <sys/stat.h>
#include <vector>

#include <nghttp2/nghttp2.h>

#include "reqLifecycleCtx.hpp"
#include "../../utils/uringHandler.hpp"
int chunkLimit = 16384;




ssize_t dataReadCb(nghttp2_session *session, 
    int32_t stream_id,
    uint8_t *buf, size_t length,
    uint32_t *data_flags,
    nghttp2_data_source *source,
    void *user_data){
    printf("dataReadCb called for stream_id: %d\n", stream_id);

    *data_flags |= NGHTTP2_DATA_FLAG_NO_COPY;

    auto tmp = nghttp2_session_get_stream_user_data(session, stream_id);
    auto ctx = static_cast<reqLifecycleCtx *>(tmp);

    //how latency mindful
    struct stat st;
    stat(ctx->path.c_str(), &st);
    printf("File size: %ld\n", st.st_size);
    
    uint32_t fileOfsetTrack = ctx->fileOffset;
    
    

    if(st.st_size < chunkLimit){
        printf("File size is less than chunk limit, using targetWrite=%ld\n", (long)st.st_size);
        auto *ctxW = resumeCtxPool->get();        

        ctxW->targetWrite = st.st_size;
        ctxW->offsetTrack = fileOfsetTrack;
        ctxW->fileFd = ctx->openFd;
        ctx->write = (resumeCtx*)ctxW;

        fileOfsetTrack += ctxW->targetWrite;

        *data_flags |= NGHTTP2_DATA_FLAG_NO_COPY | NGHTTP2_DATA_FLAG_EOF;
        return ctxW->targetWrite;

    }else{
        printf("File size is greater than or equal to chunk limit, using targetWrite=%d\n", chunkLimit);
        auto *ctxW = resumeCtxPool->get();        

        ctxW->targetWrite = chunkLimit;
        ctxW->offsetTrack = fileOfsetTrack;
        ctxW->fileFd = ctx->openFd;
        ctx->write = (resumeCtx*)ctxW;

        //wont work for now
        *data_flags |= NGHTTP2_DATA_FLAG_NO_COPY | NGHTTP2_DATA_FLAG_EOF;
        return ctxW->targetWrite;

        fileOfsetTrack += ctxW->targetWrite;
    }

    ctx->fileOffset = fileOfsetTrack;

    if(fileOfsetTrack <= st.st_size){
        *data_flags |= (NGHTTP2_DATA_FLAG_NO_COPY | NGHTTP2_DATA_FLAG_EOF);
    }


    return fileOfsetTrack;
}