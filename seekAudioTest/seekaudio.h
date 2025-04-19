#ifndef SEEK_AUDIO_H
#define SEEK_AUDIO_H 1


#ifdef __cplusplus
extern "C" {
#endif

#ifndef SEEKAUDIO_EXPORT
# if defined(WIN32)
#   define SEEKAUDIO_EXPORT __declspec(dllexport)
# elif defined(__GNUC__)
#  define SEEKAUDIO_EXPORT __attribute__ ((visibility ("default")))
# else
#  define SEEKAUDIO_EXPORT
# endif
#else
#  define SEEKAUDIO_EXPORT
#endif

typedef void NoiseHandle;

typedef enum MODEL_TYPE_E
{
	MODEL_LITE = 0,
	MODEL_MAIN = 1,
}MODEL_TYPE;


/**
 * Return the handle of SeekAudioNs
 */
SEEKAUDIO_EXPORT NoiseHandle* SeekAudioNs_Create();

/*
 * This function frees the dynamic memory of a specified SeekAudio denoise
 * instance.
 *
 * Input:
 *      - NS_inst : Pointer to NS instance that should be freed
 */
SEEKAUDIO_EXPORT void SeekAudioNs_Free(NoiseHandle* NS_inst);

/*
 * This function initializes a NS instance and has to be called before any other
 * processing is made.
 *
 * Input:
 *      - NS_inst       : Instance that should be initialized
 *      - sampleRate    : sampling frequency
 *      - modelType     : model type,set a model for the NS,the default model is MODEL_LITE
 * Output:
 *      - NS_inst       : Initialized instance
 *
 * Return value         :  0 - Ok
 *                        -1 - Error
 */
SEEKAUDIO_EXPORT int SeekAudioNs_Init(NoiseHandle* NS_inst, int sampleRate, MODEL_TYPE modelType);


/*
 * This functions does Noise Suppression for the inserted speech frame. The
 * input and output signals should always be 10ms (160 samples).
 *
 * Input
 *      - NS_inst       : Noise suppression instance.
 *      - inframe       : Pointer to speech frame buffer
 *
 * Output:
 *      - outframe      : Pointer to output frame
 */
SEEKAUDIO_EXPORT void SeekAudioNs_Process(NoiseHandle* NS_inst,const float* inframe,float* outframe);

#ifdef __cplusplus
}
#endif

#endif
