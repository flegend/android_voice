//
// Created by zsy on 2019/5/20.
//

#include "my_voice_engine_impl.h"
#include "native_debug.h"
#include "XUtil/OperationMgr.h"
#include "VoiceMgr.h"

MyVoiceEngineInterface *g_pVoiceEngineInterface = NULL;

int VoiceEngineTrace::SetTraceCallback(TraceCallback *cb) {
    return 0;
}

void VoiceEngineTrace::SetTraceFilter(int filter) {

}

int VoiceEngineTrace::SetTraceFile(const char *filename) {
    return 0;
}

void MyVoiceEngineInterface::SetAndroidObjects(void *javaVM, void *context) {

}

MyVoiceEngineInterface *MyVoiceEngineInterface::GetVoiceEngine() {
    if (NULL == g_pVoiceEngineInterface) {
        g_pVoiceEngineInterface = new VOICEENGINEAPI::CVoiceEngineInterfaceImpl();
    }
    return g_pVoiceEngineInterface;
}

void MyVoiceEngineInterface::DestroyVoiceEngine(MyVoiceEngineInterface *myVoiceEngineInterface) {
    if (NULL != myVoiceEngineInterface) {
        ASSERT(g_pVoiceEngineInterface == myVoiceEngineInterface,
               "DestroyVoiceEngine VoiceEngineInterface not create one");
        delete myVoiceEngineInterface;
        myVoiceEngineInterface = NULL;
        g_pVoiceEngineInterface = NULL;
    }
}

namespace VOICEENGINEAPI {
    CVoiceEngineInterfaceImpl::CVoiceEngineInterfaceImpl() {
        COperationMgr::Ins().StartUp();
    }

    CVoiceEngineInterfaceImpl::~CVoiceEngineInterfaceImpl() {
        Unint();
        COperationMgr::Ins().ShutDown();
    }

    int CVoiceEngineInterfaceImpl::Unint() {
        COperationMgr::Ins().StartSyncOperation(
                [=]() {
                    CVoiceMgr::Ins().VoiceUnint();
                }
        );
        return 0;
    }

    int CVoiceEngineInterfaceImpl::Init() {
        int ret = 0;
        COperationMgr::Ins().StartSyncOperation(
                [&]() {
                    ret = CVoiceMgr::Ins().VoiceInit();
                }
        );
        return ret;
    }

    int CVoiceEngineInterfaceImpl::SetSendCodec(VOICE_ENGINE_CODEC_ID codec_id) {
        int ret = 0;
        COperationMgr::Ins().StartSyncOperation(
                [&]() {
                    ret = CVoiceMgr::Ins().SetSendCodec(codec_id);
                }
        );
        return 0;
    }

    int CVoiceEngineInterfaceImpl::GetCodecInfo(VOICE_ENGINE_CODEC_ID codec_id, CodecInst &inst) {
        CVoiceMgr::Ins().GetCodecInfo(codec_id, inst);
        return 0;
    }

    int CVoiceEngineInterfaceImpl::RegisterAudioCallback(Transport *cb) {
        COperationMgr::Ins().StartSyncOperation(
                [&]() {
                    CVoiceMgr::Ins().RegisterAudioCallback(cb);
                }
        );
        return 0;
    }

    int CVoiceEngineInterfaceImpl::DeRegisterAudioCallback() {
        COperationMgr::Ins().StartSyncOperation(
                [=]() {
                    CVoiceMgr::Ins().DeRegisterAudioCallback();
                }
        );
        return 0;
    }

    int CVoiceEngineInterfaceImpl::StartRecording() {
        COperationMgr::Ins().StartAsynOperation(
                [=]() {
                    CVoiceMgr::Ins().StartRecording();
                }
        );
        return 0;
    }

    int CVoiceEngineInterfaceImpl::StopRecording() {
        COperationMgr::Ins().StartAsynOperation(
                [=]() {
                    CVoiceMgr::Ins().StopRecording();
                }
        );
        return 0;
    }

    int CVoiceEngineInterfaceImpl::CreateDecodeChannel() {
        int32_t nChannelId = -1;
        COperationMgr::Ins().StartSyncOperation(
                [&]() {
                    nChannelId = CVoiceMgr::Ins().CreateDecodeChannel();
                }
        );
        return nChannelId;
    }

    int CVoiceEngineInterfaceImpl::DestroyDecodeChannel(int decodeChannelID) {
        COperationMgr::Ins().StartSyncOperation(
                [=]() {
                    CVoiceMgr::Ins().DestroyDecodeChannel(decodeChannelID);
                }
        );
        return 0;
    }

    int CVoiceEngineInterfaceImpl::Decode(int channelID, const void *data, int nlen) {
        return CVoiceMgr::Ins().Decode(channelID, (const int8_t *) data, nlen);
    }

    int CVoiceEngineInterfaceImpl::StartPlayout(int channelID) {
        COperationMgr::Ins().StartAsynOperation(
                [=]() {
                    CVoiceMgr::Ins().StartPlayout(channelID);
                }
        );
        return 0;
    }

    int CVoiceEngineInterfaceImpl::StopPlayout(int channelID) {
        COperationMgr::Ins().StartAsynOperation(
                [=]() {
                    CVoiceMgr::Ins().StopPlayout(channelID);
                }
        );
        return 0;
    }
}