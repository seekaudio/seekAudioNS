###                            seekAudioNS  

这是降噪技术seekaudio的测试程序，由寻声科技团队研发，入口网址seekaudio.cn

seekaudio具备出色的音频降噪功能，它是基于自然界中常见噪音的大量音频数据训练出来的AI降噪模型，它不但可以消除常见的稳定噪声，而且还能够轻松消除传统降噪算法很难处理的非稳定噪声，甚至还可以消除人为制造的噪声。尽管如此，这些还不是seekaudio最出色的性能，它最出色的性能是可以在很低端的移动设备上以很低cpu占用率轻松的实时运行,而且还可以根据用户的具体需求把运算量压的更低。  

seekaudio模型为了满足不同群体的用户需求，在降噪性能与运算量之间取舍，分为两种，分别是model_lite和model_main。基于这两种模型我们封装成了降噪库供用户使用，可以支持windows,android，linux等各种平台。由于webrtc自带的降噪模块（称为webrtcNS）是目前市场上可免费获取的性能较好的降噪模块，因此我们在详细的[测试报告](https://docs.qq.com/doc/DZXFSVk5peEZwUFB0) 中，主要和webrtcNS做对比。经过详细的对比测试，基于model_lite模型的推理进行降噪的运算量是webrtcNS两三倍左右，由于webrtcNS的运算量在webrtc的音频处理线程总运算量(包括capture,agc,ns,aec等)的占比很低，即使model_lite降噪模型运算量大了两三倍，也一样能够轻松的实现在低端移动设备实时处理的指标，而且经过更干净的降噪处理后，再交给aec处理，aec处理的效果也会更好。model_main模型推理的运算量又比model_lite大了四倍以上，但是降噪效果会更好，而且在中端移动设备中依然能够轻松(cpu占比很低)的实现实时处理的指标。该降噪库不但适用于voip音视频通话应用场景，也非常适用于其它任何需要语音降噪的场景，比如人机交互前的语音降噪处理，噪音消除越干净，语音识别就会更准确。  

seekaudio库的接口简单，使用起来很方便。可以使用visual studio 2017或者2017以上的visual studio版本打开程序，如果没有visual studio，也可以使用windows的命令行程序cmd直接跑文件夹release中已经编译好的seekAudioTest.exe进行测试。使用方法如下：  

seekAudioTest.exe  input_file  output_file  mode

 seekAudioTest.exe目前只能处理16000采样率的pcm原始裸数据（seekaudio目前只支持16000采样率）。 input_file是输入的pcm格式音频文件路径，output_file是输出的pcm格式音频文件路径,mode是表示降噪的模式，可以取数值0,1和2，0表示使用model_lite模型进行降噪，1表示使model_main模型进行降噪，2表示使用webrtcNS进行降噪。使用命令行示范如下：

seekAudioTest.exe  d://test.pcm  d://out.pcm  0


    
