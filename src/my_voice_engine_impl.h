//
// Created by zsy on 2019/5/20.
//
#include "../include/my_voice_engine_interface.h"

namespace VOICEENGINEAPI {
    class CVoiceEngineInterfaceImpl : public MyVoiceEngineInterface {
    public:
        CVoiceEngineInterfaceImpl();

        ~CVoiceEngineInterfaceImpl();

        int Unint();

        virtual int Init();

        //设置音频编码格式
        virtual int SetSendCodec(VOICE_ENGINE_CODEC_ID codec_id);

        //通过codec_id获取编码格式的详细信息
        virtual int GetCodecInfo(VOICE_ENGINE_CODEC_ID codec_id, CodecInst &inst);

        //注册音频编码数据回调函数
        virtual int RegisterAudioCallback(Transport *cb);

        //取消注册音频编码数据回调
        virtual int DeRegisterAudioCallback();

        //开始录制/编码
        virtual int StartRecording();

        //停止录制/编码
        virtual int StopRecording();

        //创建一路音频解码通道
        virtual int CreateDecodeChannel();

        //销毁一路音频解码通道
        virtual int DestroyDecodeChannel(int decodeChannelID);

        //向解码通道写入音频数据
        virtual int Decode(int channelID, const void *data, int nlen);

        //开始播放/解码
        virtual int StartPlayout(int channelID);

        //停止播放/解码
        virtual int StopPlayout(int channelID);

        //设置麦克风音量 0~255  安卓平台暂不支持
        virtual int SetMicVolume(int level) { return 0; }

        //设置扬声器音量 0~255
        virtual int SetSpeakerVolume(int level) { return 0; }

        //获取麦克风音量 0~255  安卓平台暂不支持
        virtual int GetMicVolume() { return 0; }

        //获取扬声器音量 0~255
        virtual int GetSpeakerVolume() { return 0; }

        //设置麦克风静音   //安卓平台暂不支持
        virtual int SetMicMute(bool mute) { return 0; }

        //设置扬声器静音
        virtual int SetSpeakerMute(bool mute) { return 0; }

        //获取麦克风输入DB -96db ~ 0db
        virtual float GetMicSpeechDBValue() { return 0; }

        //获取扬声器输出DB -96db ~ 0db
        virtual float GetSpeakerSpeechDBValue() { return 0; }

        //设置回音消除模式
        virtual int SetEcStatus(bool enable, EcModes ecMode) { return 0; }

        //设置AECM模式 仅在回音消除模式设置为AECM时有效
        virtual int SetAecmMode(AecmModes aecmMode, bool cng) { return 0; }

        //设置AGC模式
        virtual int SetAgcStatus(bool enable, AgcModes mode) { return 0; }

        //设置AGC参数
        virtual int SetAgcConfig(AgcConfig config) { return 0; }

        //设置噪音抑制模式
        virtual int SetNsStatus(bool enable, NsModes mode) { return 0; }

        //设置噪音抑制参数，暂时无用，不要调用此接口
        virtual int SetNsConfig(float overdive, float denoiseband, int gainmap) { return 0; }

        //设置静音检测是否开启
        virtual int SetVAD(bool enable) { return 0; }
    };
}

