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
    
    *data_flags |= NGHTTP2_DATA_FLAG_NO_COPY;

    auto tmp = nghttp2_session_get_stream_user_data(session, stream_id);
    auto ctx = static_cast<reqLifecycleCtx *>(tmp);

    //how latency mindful
    struct stat st;
    stat(ctx->path.c_str(), &st);

    
    uint32_t fileOfsetTrack = ctx->fileOffset;
    if(st.st_size < chunkLimit){
        auto *ctxW = resumeCtxPool->get();        

        ctxW->targetWrite = st.st_size;
        ctxW->offsetTrack = fileOfsetTrack;
            
        fileOfsetTrack += ctxW->targetWrite;

    }else{
        auto *ctxW = resumeCtxPool->get();        

        ctxW->targetWrite = chunkLimit;
        ctxW->offsetTrack = fileOfsetTrack;
            
        fileOfsetTrack += ctxW->targetWrite;
    }

    return fileOfsetTrack;
    }