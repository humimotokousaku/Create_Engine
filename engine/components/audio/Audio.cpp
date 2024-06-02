#include "Audio.h"
#include <cassert>
#include <mfapi.h>
#include <mfidl.h>

Audio* Audio::GetInstance() {
	static Audio instance;

	return &instance;
}

void Audio::Initialize(uint32_t soundIndex) {
	// Xaudio2エンジンのインスタンスを生成
	XAudio2Create(&xAudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);
	// マスターボイスを生成
	xAudio2_->CreateMasteringVoice(&masterVoice_);
}

void Audio::Play(uint32_t soundIndex) {
	
}

void Audio::Stop(uint32_t soundIndex) {

}

void Audio::Finalize() {
	//xAudio2_.Reset();
}

uint32_t Audio::SoundLoadWave(const char* filename) {
	// .wavファイルを開く
	std::ifstream file;
	// .wavファイルをバイナリモードで開く
	file.open(filename, std::ios_base::binary);
	// ファイルオープン失敗を検出する
	assert(file.is_open());

	// .wavデータ読み込み
	// RIFFヘッダの読み込み
	RiffHeader riff;
	file.read((char*)&riff, sizeof(riff));
	// ファイルはRIFF?
	if (strncmp(riff.chunk.id, "RIFF", 4) != 0) {
		assert(0);
	}
	// タイプはWAVE?
	if (strncmp(riff.type, "WAVE", 4) != 0) {
		assert(0);
	}
	// Formatチャンクの読み込み
	FormatChunk format = {};
	// チャンクの読み込み
	file.read((char*)&format, sizeof(ChunkHeader));
	if (strncmp(format.chunk.id, "fmt ", 4) != 0) {
		assert(0);
	}

	// チャンク本体の読み込み
	assert(format.chunk.size <= sizeof(format.fmt));
	file.read((char*)&format.fmt, format.chunk.size);
	// Dataチャンクの読み込み
	ChunkHeader data;
	file.read((char*)&data, sizeof(data));
	// JUNKチャンクを検出した場合
	if (strncmp(data.id, "JUNK", 4) == 0) {
		// 読み取り位置をJUNKチャンクの終わりまで進める
		file.seekg(data.size, std::ios_base::cur);
		// 再読み込み
		file.read((char*)&data, sizeof(data));
	}

	if (strncmp(data.id, "data", 4) != 0) {
		assert(0);
	}

	// Dataチャンクのデータ部(波形データ)の読み込み
	char* pBuffer = new char[data.size];
	file.read(pBuffer, data.size);

	// Waveファイルを閉じる
	file.close();

	uint32_t index = index_;
	// returnするための音声データ
	SoundData soundData = {};
	soundData.wfex = format.fmt;
	soundData.pBuffer = reinterpret_cast<BYTE*>(pBuffer);
	soundData.bufferSize = data.size;
	soundData_.push_back(soundData);
	// ソースボイスの作成
	CreateSourceVoice(index);

	index_++;
	
	return index;
}

void Audio::SoundUnload(uint32_t soundIndex) {
	for (SoundData& soundData : soundData_) {
		// バッファのメモリを解放
		delete[] soundData.pBuffer;

		soundData.pBuffer = 0;
		soundData.bufferSize = 0;
		soundData.wfex = {};
	}
}

void Audio::SoundPlayWave(uint32_t soundIndex, float volume = 1.0f, float Semitones = 1.0f) {
	HRESULT result;

	// GetVoiceDetails メソッドを使用して詳細情報を取得
	// マスターボイス
	//XAUDIO2_VOICE_DETAILS m_voiceDetails;
	//masterVoice_->GetVoiceDetails(&m_voiceDetails);
	// 一定の周波数以上の音を変更(今回はこもったような音になる)
	XAUDIO2_FILTER_PARAMETERS FilterParameters = { LowPassFilter, 0.1f, 0.1f };
	soundData_[soundIndex].pSubmixVoice_->SetFilterParameters(&FilterParameters);
	// 再生する波形データの設定
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = soundData_[soundIndex].pBuffer;
	buf.AudioBytes = soundData_[soundIndex].bufferSize;
	buf.Flags = XAUDIO2_END_OF_STREAM;
	// 音量の設定
	result = soundData_[soundIndex].pSourceVoice_->SetVolume(volume);
	// 音階の設定
	result = soundData_[soundIndex].pSourceVoice_->SetFrequencyRatio(Semitones);

	// 波形データの再生
	result = soundData_[soundIndex].pSourceVoice_->SubmitSourceBuffer(&buf);
	// 再生開始
	result = soundData_[soundIndex].pSourceVoice_->Start();
}

void Audio::SoundPlayLoopingWave(uint32_t soundIndex, float volume) {
	HRESULT result;

	//波形フォーマットもとにSourceVoiceの生成
	IXAudio2SourceVoice* pSourceVoice = nullptr;
	result = xAudio2_->CreateSourceVoice(&pSourceVoice, &soundData_[soundIndex].wfex);
	assert(SUCCEEDED(result));

	// 再生する波形データの設定
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = soundData_[soundIndex].pBuffer;
	buf.AudioBytes = soundData_[soundIndex].bufferSize;
	buf.Flags = XAUDIO2_END_OF_STREAM;
	buf.LoopCount = XAUDIO2_LOOP_INFINITE;

	// 波形データの再生
	result = pSourceVoice->SubmitSourceBuffer(&buf);
	// 音量の設定
	result = pSourceVoice->SetVolume(volume); // 1.0fが通常の音量
	// 再生開始
	result = pSourceVoice->Start();
}

void Audio::SoundPlayWithMuffledEffect(const SoundData& soundData, float volume, float Semitones) {
//	HRESULT result;
//#pragma region サブミックスボイスの作成
//	IXAudio2SubmixVoice* pSubmixVoice;
//	xAudio2_->CreateSubmixVoice(&pSubmixVoice, 1, 44100, XAUDIO2_VOICE_USEFILTER, 0, 0, 0);
//#pragma region 音の編集
//	//// 一定の周波数以上の音を変更(今回はこもったような音になる)
//	//XAUDIO2_FILTER_PARAMETERS FilterParameters = { LowPassFilter, 0.2f, 1.0f };
//	//pSubmixVoice->SetFilterParameters(&FilterParameters);
//	//// 音量
//	//pSubmixVoice->SetVolume(volume);
//
//	// サブミックスを送る設定
//	XAUDIO2_SEND_DESCRIPTOR submixSend = { 0, pSubmixVoice };
//	XAUDIO2_VOICE_SENDS submixSendList = { 1, &submixSend };
//#pragma endregion
//
//#pragma endregion
//
//	// 波形フォーマットもとにSourceVoiceの生成
//	IXAudio2SourceVoice* pSourceVoice = nullptr;
//	result = xAudio2_->CreateSourceVoice(&pSourceVoice, &soundData.wfex, XAUDIO2_VOICE_USEFILTER, XAUDIO2_DEFAULT_FREQ_RATIO, nullptr, &submixSendList, nullptr);
//	assert(SUCCEEDED(result));
//
//#pragma region 実装途中
//
//	// GetVoiceDetails メソッドを使用して詳細情報を取得
//	XAUDIO2_VOICE_DETAILS voiceDetails;
//	pSourceVoice->GetVoiceDetails(&voiceDetails);
//
//	X3DAUDIO_EMITTER Emitter = {};
//	Emitter.ChannelCount = 1;
//	Emitter.CurveDistanceScaler = Emitter.DopplerScaler = 1.0f;
//	Emitter.OrientFront = { 0,0,1 };
//	Emitter.OrientTop = { 0,1,0 };
//	Emitter.Position = { -10,0,0 };
//	Emitter.Velocity = { 0,0,0 };
//	X3DAUDIO_LISTENER Listener = {};
//	Listener.OrientFront = { 0.0f, 0.0f, 0.0f };
//	Listener.OrientTop = { 0.0f, 0.0f, 1.0f };
//	Listener.Position = { 0.0f, 0.0f, 0.0f };
//	Listener.Velocity = { 0.0f, 0.0f, 0.0f };
//	X3DAUDIO_CONE innerCone = {};
//	innerCone.InnerAngle = X3DAUDIO_PI / 4.0f;  // 内部コーンの角度（45度）
//	innerCone.OuterAngle = X3DAUDIO_PI / 3.0f;  // 外部コーンの角度（60度）
//	innerCone.InnerVolume = 1.0f;  // 内部コーン内の音量（最大音量）
//	innerCone.OuterVolume = 0.5f;  // 外部コーン内の音量（半分の音量）
//	// エミッターにコーンの設定を適用
//	Listener.pCone = &innerCone;
//	// ドップラー効果の設定
//	X3DAUDIO_DSP_SETTINGS DSPSettings = {};
//	FLOAT32* matrix = new FLOAT32[voiceDetails.InputChannels];
//	DSPSettings.SrcChannelCount = 1;
//	DSPSettings.DstChannelCount = voiceDetails.InputChannels;
//	DSPSettings.pMatrixCoefficients = matrix;
//	// ミキサーアウトプットマトリックスの計算（例: モノラルソースをステレオ出力にマッピング）
//	for (UINT32 i = 0; i < voiceDetails.InputChannels; ++i) {
//		//DSPSettings.pMatrixCoefficients[i] = 1.0f / voiceDetails.InputChannels;
//	}
//	// ドップラーエフェクトの設定
//	DSPSettings.DopplerFactor = 10.0f;
//	// レバーデルエフェクトの設定
//	DSPSettings.ReverbLevel = 1.0f;  // レバーデルの効果を無効にする場合
//
//	X3DAudioCalculate(X3DInstance_, &Listener, &Emitter,
//		X3DAUDIO_CALCULATE_MATRIX | X3DAUDIO_CALCULATE_DOPPLER | X3DAUDIO_CALCULATE_LPF_DIRECT | X3DAUDIO_CALCULATE_REVERB,
//		&DSPSettings);
//
//
//	// 
//	IXAudio2SourceVoice* p3DSourceVoice = nullptr;
//	result = xAudio2_->CreateSourceVoice(&p3DSourceVoice, &soundData.wfex, 0, XAUDIO2_DEFAULT_FREQ_RATIO, nullptr, nullptr, nullptr);
//	assert(SUCCEEDED(result));
//	CreateXAPO(p3DSourceVoice);
//	result = pSourceVoice->SetOutputMatrix(p3DSourceVoice, 1, voiceDetails.InputChannels, DSPSettings.pMatrixCoefficients);
//	assert(SUCCEEDED(result));
//	//pSourceVoice->SetFrequencyRatio(DSPSettings.DopplerFactor);
//	result = p3DSourceVoice->SetOutputMatrix(pSubmixVoice, 1, voiceDetails.InputChannels, &DSPSettings.ReverbLevel);
//	assert(SUCCEEDED(result));
//
//
//	XAUDIO2_FILTER_PARAMETERS FilterParameters3D = { LowPassFilter, 2.0f * sinf(X3DAUDIO_PI / 6.0f * DSPSettings.LPFDirectCoefficient), 1.0f };
//	result = p3DSourceVoice->SetFilterParameters(&FilterParameters3D);
//	assert(SUCCEEDED(result));
//#pragma endregion
//
//	//xAudio2_->CommitChanges(XAUDIO2_COMMIT_ALL);
//
//	// 再生する波形データの設定
//	XAUDIO2_BUFFER buf{};
//	buf.pAudioData = soundData.pBuffer;
//	buf.AudioBytes = soundData.bufferSize;
//	buf.Flags = XAUDIO2_END_OF_STREAM;
//
//	// 波形データの再生
//	pSourceVoice->SubmitSourceBuffer(&buf);
//	// 音量の設定
//	//pSourceVoice->SetFrequencyRatio(XAudio2SemitonesToFrequencyRatio(Semitones));
//	// 再生開始
//	pSourceVoice->Start();
}

void Audio::CreateXAPO(IXAudio2SourceVoice* pSourceVoice) {
	// リバーブ効果の作成
	IUnknown* pXAPO;
	XAudio2CreateReverb(&pXAPO);

	// チェーンの設定
	XAUDIO2_EFFECT_DESCRIPTOR descriptor;
	descriptor.InitialState = true;
	descriptor.OutputChannels = 1;
	descriptor.pEffect = pXAPO;
	XAUDIO2_EFFECT_CHAIN chain;
	chain.EffectCount = 1;
	chain.pEffectDescriptors = &descriptor;
	pSourceVoice->SetEffectChain(&chain);


	XAUDIO2FX_REVERB_PARAMETERS reverbParameters;
	reverbParameters.ReflectionsDelay = XAUDIO2FX_REVERB_DEFAULT_REFLECTIONS_DELAY;
	reverbParameters.ReverbDelay = XAUDIO2FX_REVERB_DEFAULT_REVERB_DELAY;
	reverbParameters.RearDelay = XAUDIO2FX_REVERB_DEFAULT_REAR_DELAY;
	reverbParameters.PositionLeft = XAUDIO2FX_REVERB_DEFAULT_POSITION;
	reverbParameters.PositionRight = XAUDIO2FX_REVERB_DEFAULT_POSITION;
	reverbParameters.PositionMatrixLeft = XAUDIO2FX_REVERB_DEFAULT_POSITION_MATRIX;
	reverbParameters.PositionMatrixRight = XAUDIO2FX_REVERB_DEFAULT_POSITION_MATRIX;
	reverbParameters.EarlyDiffusion = XAUDIO2FX_REVERB_DEFAULT_EARLY_DIFFUSION;
	reverbParameters.LateDiffusion = XAUDIO2FX_REVERB_DEFAULT_LATE_DIFFUSION;
	reverbParameters.LowEQGain = XAUDIO2FX_REVERB_DEFAULT_LOW_EQ_GAIN;
	reverbParameters.LowEQCutoff = XAUDIO2FX_REVERB_DEFAULT_LOW_EQ_CUTOFF;
	reverbParameters.HighEQGain = XAUDIO2FX_REVERB_DEFAULT_HIGH_EQ_GAIN;
	reverbParameters.HighEQCutoff = XAUDIO2FX_REVERB_DEFAULT_HIGH_EQ_CUTOFF;
	reverbParameters.RoomFilterFreq = XAUDIO2FX_REVERB_DEFAULT_ROOM_FILTER_FREQ;
	reverbParameters.RoomFilterMain = XAUDIO2FX_REVERB_DEFAULT_ROOM_FILTER_MAIN;
	reverbParameters.RoomFilterHF = XAUDIO2FX_REVERB_DEFAULT_ROOM_FILTER_HF;
	reverbParameters.ReflectionsGain = XAUDIO2FX_REVERB_DEFAULT_REFLECTIONS_GAIN;
	reverbParameters.ReverbGain = XAUDIO2FX_REVERB_DEFAULT_REVERB_GAIN;
	reverbParameters.DecayTime = XAUDIO2FX_REVERB_DEFAULT_DECAY_TIME;
	reverbParameters.Density = XAUDIO2FX_REVERB_DEFAULT_DENSITY;
	reverbParameters.RoomSize = XAUDIO2FX_REVERB_DEFAULT_ROOM_SIZE;
	reverbParameters.WetDryMix = XAUDIO2FX_REVERB_DEFAULT_WET_DRY_MIX;
	// パラメータを設定
	pSourceVoice->SetEffectParameters(1, &reverbParameters, sizeof(reverbParameters));
	pSourceVoice->EnableEffect(1);
}

void Audio::CreateSourceVoice(uint32_t soundIndex) {
	HRESULT result;
#pragma region サブミックスボイスの作成
	xAudio2_->CreateSubmixVoice(&soundData_[soundIndex].pSubmixVoice_, 2, 44100, XAUDIO2_VOICE_USEFILTER);
#pragma region 音の編集
	//// 一定の周波数以上の音を変更(今回はこもったような音になる)
	//XAUDIO2_FILTER_PARAMETERS FilterParameters = { LowPassFilter, 1.0f, 1.0f };
	//soundData_[soundIndex].pSubmixVoice_->SetFilterParameters(&FilterParameters);
#pragma endregion

#pragma endregion

	// サブミックスを送る設定
	XAUDIO2_SEND_DESCRIPTOR submixSend = { 0, soundData_[soundIndex].pSubmixVoice_ };
	XAUDIO2_VOICE_SENDS submixSendList = { 1, &submixSend };

	// 波形フォーマットもとにSourceVoiceの生成
	result = xAudio2_->CreateSourceVoice(&soundData_[soundIndex].pSourceVoice_, &soundData_[soundIndex].wfex, XAUDIO2_VOICE_USEFILTER, 2.0f, 0, &submixSendList);
	assert(SUCCEEDED(result));
}
