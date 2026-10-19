#pragma once

#include <nghttp2/nghttp2.h>
#include <cstring>

#include "reqLifecycleCtx.hpp"
#include "../../utils/uringHandler.hpp"
#include "../sockCtx.hpp"


uint8_t sixtyFourKilobytes[65536];

int sendDataCb(nghttp2_session *session,
    nghttp2_frame *frame, 
    const uint8_t *framehd, 
    size_t length, 
    nghttp2_data_source *source, 
    void *user_data){

        if(frame->hd.type != NGHTTP2_DATA){
            //in all honesty dont expect this to happen
            
            printf("Non-DATA frame encountered\n");
        }

        printf("Processing DATA frame\n");

        auto* tmp = nghttp2_session_get_stream_user_data(session, frame->hd.stream_id);
        reqLifecycleCtx* ctx = (reqLifecycleCtx*)tmp;

        if(ctx == nullptr){
            return NGHTTP2_ERR_TEMPORAL_CALLBACK_FAILURE;
        }

       

        // nghttp2 emits one DATA frame per chunk, in order, subheads are added because uring can reorder them
        //memcpy((void*)framehd, ctx->write->frameHeader, sizeof(ctx->write->frameHeader));
        ctx->write->networkFd = static_cast<socketCtx*>(user_data)->outgoingFd;



        //there we go, took 15 whole ass minutes, been working on the uring trash for weeks
        //if i have time left over ill sort out the uring garbage but definitley mention why its a pain in the ass for the report



        lseek(ctx->openFd, ctx->write->offsetTrack, SEEK_SET);
        read(ctx->openFd, sixtyFourKilobytes, length);
        
        if(length == 0){
            
        }
        int headerRes = write(ctx->write->networkFd, framehd, sizeof(framehd));
        while(headerRes < sizeof(framehd)){
            headerRes += write(ctx->write->networkFd, framehd + headerRes, sizeof(framehd) - headerRes);
        }

        int subheaderRes = write(ctx->write->networkFd, &ctx->write->sh, sizeof(ctx->write->sh));
        while(subheaderRes < sizeof(ctx->write->sh)){
            subheaderRes += write(ctx->write->networkFd, &ctx->write->sh + subheaderRes, sizeof(ctx->write->sh) - subheaderRes);
        }
        
        int res = write(ctx->write->networkFd, sixtyFourKilobytes, length); //bulk data
        while(res < length){
            res += write(ctx->write->networkFd, sixtyFourKilobytes + res, length - res);
        }


        ctx->write->offsetTrack += length;
        //apperently the pipe reads short, the goddamn thing wont stop annoying me today with nonsense.
        //prepUring(ctx->write, length);


        return 0;
    }